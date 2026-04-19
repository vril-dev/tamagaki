#pragma once

#include <mutex>
#include <string>

#include "src/ebpf/loader.hpp"

namespace tamagaki::control {

class Runtime {
  public:
    Runtime(ebpf::Loader &loader, std::string config_path);

    bool initialize();
    bool reload();

    ebpf::AttachResult attachResult() const;
    std::string configPath() const;
    bool healthy() const;

  private:
    bool refreshLocked();

    ebpf::Loader &loader_;
    std::string config_path_;

    mutable std::mutex mutex_;
    ebpf::AttachResult attach_result_;
    bool healthy_ = false;
};

}  // namespace tamagaki::control
