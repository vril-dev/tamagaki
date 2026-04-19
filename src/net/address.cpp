#include "src/net/address.hpp"

#include <arpa/inet.h>

#include <cstdio>
#include <cstring>
#include <sstream>

namespace tamagaki::net {

std::optional<MacAddress> parseMacAddress(const std::string &value)
{
    MacAddress address{};
    unsigned int octets[6];
    if (std::sscanf(
            value.c_str(),
            "%2x:%2x:%2x:%2x:%2x:%2x",
            &octets[0],
            &octets[1],
            &octets[2],
            &octets[3],
            &octets[4],
            &octets[5]) != 6) {
        return std::nullopt;
    }

    for (std::size_t i = 0; i < address.size(); ++i) {
        address[i] = static_cast<std::uint8_t>(octets[i]);
    }

    return address;
}

std::string formatMacAddress(const MacAddress &value)
{
    char buffer[18];
    std::snprintf(
        buffer,
        sizeof(buffer),
        "%02x:%02x:%02x:%02x:%02x:%02x",
        value[0],
        value[1],
        value[2],
        value[3],
        value[4],
        value[5]);
    return buffer;
}

std::optional<std::uint32_t> parseIPv4(const std::string &value)
{
    struct in_addr address {};
    if (inet_pton(AF_INET, value.c_str(), &address) != 1) {
        return std::nullopt;
    }

    return ntohl(address.s_addr);
}

std::string formatIPv4(std::uint32_t value)
{
    struct in_addr address {};
    address.s_addr = htonl(value);

    char buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &address, buffer, sizeof(buffer)) == nullptr) {
        return {};
    }

    return buffer;
}

}  // namespace tamagaki::net
