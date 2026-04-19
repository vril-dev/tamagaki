#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <vector>

#include "src/net/address.hpp"

namespace tamagaki::ebpf {

struct PacketEvent {
    std::uint32_t ip = 0;
};

struct SeenMacEntry {
    net::MacAddress mac{};
    std::uint64_t seen_count = 0;
    std::int64_t last_seen_unix = 0;
};

class XDPController {
  public:
    void block(std::uint32_t ip);
    void unblock(std::uint32_t ip);
    std::vector<std::uint32_t> blockedIPs() const;

    bool blockMac(const net::MacAddress &mac);
    bool unblockMac(const net::MacAddress &mac);
    void observeMac(const net::MacAddress &mac);
    std::vector<net::MacAddress> blockedMacs() const;
    std::vector<SeenMacEntry> seenMacs() const;

    void onEvent(std::function<void(const PacketEvent &)> callback);

  private:
    mutable std::mutex mutex_;
    std::set<std::uint32_t> blocked_ips_;
    std::set<net::MacAddress> blocked_macs_;
    std::map<net::MacAddress, SeenMacEntry> seen_macs_;
    std::vector<std::function<void(const PacketEvent &)>> callbacks_;
};

}  // namespace tamagaki::ebpf
