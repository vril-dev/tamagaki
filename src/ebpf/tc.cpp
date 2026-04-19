#include "src/ebpf/tc.hpp"

namespace tamagaki::ebpf {

void TCController::recordFlow(std::uint32_t source_ip, std::uint64_t destinations)
{
    std::scoped_lock lock(mutex_);
    flow_stats_[source_ip] = destinations;
}

std::uint64_t TCController::destinationsFor(std::uint32_t source_ip) const
{
    std::scoped_lock lock(mutex_);
    const auto it = flow_stats_.find(source_ip);
    return it == flow_stats_.end() ? 0 : it->second;
}

}  // namespace tamagaki::ebpf
