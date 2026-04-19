#include "src/ebpf/loader.hpp"

#include <fstream>
#include <string>

namespace tamagaki::ebpf {
namespace {

std::string trim(const std::string &value)
{
    const auto first = value.find_first_not_of(" \t\r\n\"");
    if (first == std::string::npos) {
        return {};
    }

    const auto last = value.find_last_not_of(" \t\r\n\"");
    return value.substr(first, last - first + 1);
}

}  // namespace

LoadOptions Loader::loadOptions(const std::string &config_path) const
{
    LoadOptions options;
    options.config_path = config_path;

    std::ifstream input(config_path);
    std::string line;
    while (std::getline(input, line)) {
        const auto pos = line.find("interface:");
        if (pos == std::string::npos) {
            continue;
        }

        const auto value = trim(line.substr(pos + std::string("interface:").size()));
        if (!value.empty()) {
            options.interface_name = value;
        }
    }

    return options;
}

AttachResult Loader::attach(const LoadOptions &options) const
{
    return AttachResult{
        .interface_name = options.interface_name,
        .maps = {"blocklist", "events", "flow_stats"},
    };
}

}  // namespace tamagaki::ebpf
