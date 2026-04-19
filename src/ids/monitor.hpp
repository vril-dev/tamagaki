#pragma once

#include <atomic>
#include <cstdint>
#include <optional>
#include <string>

#include "src/ebpf/xdp.hpp"
#include "src/ids/alert.hpp"
#include "src/ids/baseline.hpp"

namespace tamagaki::ids {

class Monitor {
  public:
    Monitor(ebpf::XDPController &xdp, Baseline &baseline);

    void watch(std::atomic_bool &stop_requested);
    void block(std::uint32_t ip);
    std::optional<Alert> observeDestination(const std::string &destination);

  private:
    ebpf::XDPController &xdp_;
    Baseline &baseline_;
};

}  // namespace tamagaki::ids
