#include "src/ebpf/xdp.hpp"

#include <chrono>

namespace tamagaki::ebpf {

void XDPController::block(std::uint32_t ip)
{
    std::scoped_lock lock(mutex_);
    blocked_ips_.insert(ip);
}

void XDPController::unblock(std::uint32_t ip)
{
    std::scoped_lock lock(mutex_);
    blocked_ips_.erase(ip);
}

std::vector<std::uint32_t> XDPController::blockedIPs() const
{
    std::scoped_lock lock(mutex_);
    return {blocked_ips_.begin(), blocked_ips_.end()};
}

bool XDPController::blockMac(const net::MacAddress &mac)
{
    std::scoped_lock lock(mutex_);
    const auto [_, inserted] = blocked_macs_.insert(mac);
    return inserted;
}

bool XDPController::unblockMac(const net::MacAddress &mac)
{
    std::scoped_lock lock(mutex_);
    return blocked_macs_.erase(mac) > 0;
}

void XDPController::observeMac(const net::MacAddress &mac)
{
    std::scoped_lock lock(mutex_);

    auto &entry = seen_macs_[mac];
    entry.mac = mac;
    entry.seen_count += 1;
    entry.last_seen_unix = std::chrono::duration_cast<std::chrono::seconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count();
}

std::vector<net::MacAddress> XDPController::blockedMacs() const
{
    std::scoped_lock lock(mutex_);
    return {blocked_macs_.begin(), blocked_macs_.end()};
}

std::vector<SeenMacEntry> XDPController::seenMacs() const
{
    std::scoped_lock lock(mutex_);

    std::vector<SeenMacEntry> entries;
    entries.reserve(seen_macs_.size());
    for (const auto &[_, entry] : seen_macs_) {
        entries.push_back(entry);
    }

    return entries;
}

void XDPController::onEvent(std::function<void(const PacketEvent &)> callback)
{
    std::scoped_lock lock(mutex_);
    callbacks_.push_back(std::move(callback));
}

}  // namespace tamagaki::ebpf
