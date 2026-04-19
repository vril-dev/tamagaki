#include "src/control/server.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "src/net/address.hpp"

namespace tamagaki::control {
namespace {

std::string jsonEscape(const std::string &value)
{
    std::string escaped;
    escaped.reserve(value.size() + 8);

    for (const char ch : value) {
        switch (ch) {
        case '\\':
            escaped += "\\\\";
            break;
        case '"':
            escaped += "\\\"";
            break;
        case '\n':
            escaped += "\\n";
            break;
        default:
            escaped += ch;
            break;
        }
    }

    return escaped;
}

std::vector<std::string> split(const std::string &value)
{
    std::istringstream stream(value);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string ok(const std::string &payload)
{
    return "{\"ok\":true," + payload + "}\n";
}

std::string error(const std::string &message)
{
    return "{\"ok\":false,\"error\":\"" + jsonEscape(message) + "\"}\n";
}

}  // namespace

Server::Server(Runtime &runtime, ebpf::XDPController &xdp, std::string socket_path)
    : runtime_(runtime), xdp_(xdp), socket_path_(std::move(socket_path))
{
}

void Server::run(std::atomic_bool &stop_requested)
{
    namespace fs = std::filesystem;

    const fs::path socket_file(socket_path_);
    if (socket_file.has_parent_path()) {
        fs::create_directories(socket_file.parent_path());
    }

    ::unlink(socket_path_.c_str());

    const int server_fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        return;
    }

    ::fcntl(server_fd, F_SETFL, ::fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);

    struct sockaddr_un address {};
    address.sun_family = AF_UNIX;
    std::snprintf(address.sun_path, sizeof(address.sun_path), "%s", socket_path_.c_str());

    if (::bind(server_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) != 0 ||
        ::listen(server_fd, 16) != 0) {
        ::close(server_fd);
        ::unlink(socket_path_.c_str());
        return;
    }

    while (!stop_requested.load()) {
        const int client_fd = ::accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            break;
        }

        char buffer[1024];
        const ssize_t received = ::read(client_fd, buffer, sizeof(buffer) - 1);
        if (received > 0) {
            buffer[received] = '\0';
            const std::string response = handleCommand(buffer);
            (void)::write(client_fd, response.data(), response.size());
        }

        ::close(client_fd);
    }

    ::close(server_fd);
    ::unlink(socket_path_.c_str());
}

std::string Server::handleCommand(const std::string &command_line)
{
    const auto tokens = split(command_line);
    if (tokens.empty()) {
        return error("empty command");
    }

    const auto &command = tokens[0];

    if (command == "health") {
        const auto attach_result = runtime_.attachResult();
        std::ostringstream response;
        response << "\"healthy\":" << (runtime_.healthy() ? "true" : "false")
                 << ",\"interface\":\"" << jsonEscape(attach_result.interface_name) << "\""
                 << ",\"maps\":[";

        for (std::size_t i = 0; i < attach_result.maps.size(); ++i) {
            if (i != 0) {
                response << ",";
            }
            response << "\"" << jsonEscape(attach_result.maps[i]) << "\"";
        }

        response << "]";
        return ok(response.str());
    }

    if (command == "reload_config") {
        const bool reloaded = runtime_.reload();
        return ok(std::string("\"reloaded\":") + (reloaded ? "true" : "false"));
    }

    if (command == "list_seen_macs") {
        const auto seen_macs = xdp_.seenMacs();
        const auto blocked_macs = xdp_.blockedMacs();
        std::set<net::MacAddress> blocked(blocked_macs.begin(), blocked_macs.end());

        std::ostringstream response;
        response << "\"seen_macs\":[";
        for (std::size_t i = 0; i < seen_macs.size(); ++i) {
            const auto &entry = seen_macs[i];
            if (i != 0) {
                response << ",";
            }

            response << "{"
                     << "\"mac\":\"" << net::formatMacAddress(entry.mac) << "\","
                     << "\"seen_count\":" << entry.seen_count << ","
                     << "\"last_seen_unix\":" << entry.last_seen_unix << ","
                     << "\"blocked\":" << (blocked.contains(entry.mac) ? "true" : "false")
                     << "}";
        }
        response << "]";
        return ok(response.str());
    }

    if (command == "list_blocked_macs") {
        const auto blocked_macs = xdp_.blockedMacs();
        std::ostringstream response;
        response << "\"blocked_macs\":[";
        for (std::size_t i = 0; i < blocked_macs.size(); ++i) {
            if (i != 0) {
                response << ",";
            }
            response << "\"" << net::formatMacAddress(blocked_macs[i]) << "\"";
        }
        response << "]";
        return ok(response.str());
    }

    if (command == "list_blocked_ips") {
        const auto blocked_ips = xdp_.blockedIPs();
        std::ostringstream response;
        response << "\"blocked_ips\":[";
        for (std::size_t i = 0; i < blocked_ips.size(); ++i) {
            if (i != 0) {
                response << ",";
            }
            response << "\"" << net::formatIPv4(blocked_ips[i]) << "\"";
        }
        response << "]";
        return ok(response.str());
    }

    if (tokens.size() != 2) {
        return error("expected exactly one argument");
    }

    if (command == "observe_mac" || command == "block_mac" || command == "unblock_mac") {
        const auto mac = net::parseMacAddress(tokens[1]);
        if (!mac.has_value()) {
            return error("invalid mac address");
        }

        if (command == "observe_mac") {
            xdp_.observeMac(*mac);
            return ok("\"observed_mac\":\"" + net::formatMacAddress(*mac) + "\"");
        }

        const bool changed =
            (command == "block_mac") ? xdp_.blockMac(*mac) : xdp_.unblockMac(*mac);
        return ok(std::string("\"changed\":") + (changed ? "true" : "false"));
    }

    if (command == "block_ip" || command == "unblock_ip") {
        const auto ip = net::parseIPv4(tokens[1]);
        if (!ip.has_value()) {
            return error("invalid ipv4 address");
        }

        if (command == "block_ip") {
            xdp_.block(*ip);
        } else {
            xdp_.unblock(*ip);
        }

        return ok("\"ip\":\"" + net::formatIPv4(*ip) + "\"");
    }

    return error("unknown command");
}

}  // namespace tamagaki::control
