#include "src/ebpf/map.hpp"

namespace tamagaki::ebpf {

void MapRegistry::registerMap(const std::string &name, int fd)
{
    std::scoped_lock lock(mutex_);
    maps_[name] = fd;
}

int MapRegistry::lookup(const std::string &name) const
{
    std::scoped_lock lock(mutex_);
    const auto it = maps_.find(name);
    return it == maps_.end() ? -1 : it->second;
}

}  // namespace tamagaki::ebpf
