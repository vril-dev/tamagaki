#pragma once

#include <map>
#include <mutex>
#include <string>

namespace tamagaki::ebpf {

struct MapHandle {
    std::string name;
    int fd = -1;
};

class MapRegistry {
  public:
    void registerMap(const std::string &name, int fd);
    int lookup(const std::string &name) const;

  private:
    mutable std::mutex mutex_;
    std::map<std::string, int> maps_;
};

}  // namespace tamagaki::ebpf
