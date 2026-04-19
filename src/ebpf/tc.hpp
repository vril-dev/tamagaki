#pragma once

#include <cstdint>
#include <map>
#include <mutex>

namespace tamagaki::ebpf {

class TCController {
  public:
    void recordFlow(std::uint32_t source_ip, std::uint64_t destinations);
    std::uint64_t destinationsFor(std::uint32_t source_ip) const;

  private:
    mutable std::mutex mutex_;
    std::map<std::uint32_t, std::uint64_t> flow_stats_;
};

}  // namespace tamagaki::ebpf
