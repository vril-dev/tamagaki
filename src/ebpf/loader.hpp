#pragma once

#include <string>
#include <vector>

namespace tamagaki::ebpf {

struct LoadOptions {
    std::string config_path = "/config/tamagaki.yaml";
    std::string interface_name = "eth0";
};

struct AttachResult {
    std::string interface_name;
    std::vector<std::string> maps;
};

class Loader {
  public:
    LoadOptions loadOptions(const std::string &config_path) const;
    AttachResult attach(const LoadOptions &options) const;
};

}  // namespace tamagaki::ebpf
