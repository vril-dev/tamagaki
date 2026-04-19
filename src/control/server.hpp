#pragma once

#include <atomic>
#include <string>

#include "src/control/runtime.hpp"
#include "src/ebpf/xdp.hpp"

namespace tamagaki::control {

class Server {
  public:
    Server(Runtime &runtime, ebpf::XDPController &xdp, std::string socket_path);

    void run(std::atomic_bool &stop_requested);

  private:
    std::string handleCommand(const std::string &command_line);

    Runtime &runtime_;
    ebpf::XDPController &xdp_;
    std::string socket_path_;
};

}  // namespace tamagaki::control
