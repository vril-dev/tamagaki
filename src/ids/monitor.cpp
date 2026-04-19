#include "src/ids/monitor.hpp"

#include <chrono>
#include <thread>

namespace tamagaki::ids {

Monitor::Monitor(ebpf::XDPController &xdp, Baseline &baseline)
    : xdp_(xdp), baseline_(baseline)
{
}

void Monitor::watch(std::atomic_bool &stop_requested)
{
    while (!stop_requested.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void Monitor::block(std::uint32_t ip)
{
    xdp_.block(ip);
}

std::optional<Alert> Monitor::observeDestination(const std::string &destination)
{
    if (baseline_.allows(destination)) {
        return std::nullopt;
    }

    baseline_.remember(destination);
    return Alert{
        .severity = Severity::warning,
        .message = "new outbound destination detected: " + destination,
    };
}

}  // namespace tamagaki::ids
