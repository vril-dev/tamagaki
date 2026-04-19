#include "src/control/runtime.hpp"

namespace tamagaki::control {

Runtime::Runtime(ebpf::Loader &loader, std::string config_path)
    : loader_(loader), config_path_(std::move(config_path))
{
}

bool Runtime::initialize()
{
    std::scoped_lock lock(mutex_);
    return refreshLocked();
}

bool Runtime::reload()
{
    std::scoped_lock lock(mutex_);
    return refreshLocked();
}

ebpf::AttachResult Runtime::attachResult() const
{
    std::scoped_lock lock(mutex_);
    return attach_result_;
}

std::string Runtime::configPath() const
{
    std::scoped_lock lock(mutex_);
    return config_path_;
}

bool Runtime::healthy() const
{
    std::scoped_lock lock(mutex_);
    return healthy_;
}

bool Runtime::refreshLocked()
{
    const auto options = loader_.loadOptions(config_path_);
    attach_result_ = loader_.attach(options);
    healthy_ = !attach_result_.interface_name.empty() && !attach_result_.maps.empty();
    return healthy_;
}

}  // namespace tamagaki::control
