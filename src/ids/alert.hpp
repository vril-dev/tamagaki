#pragma once

#include <string>

namespace tamagaki::ids {

enum class Severity {
    info,
    warning,
    critical,
};

struct Alert {
    Severity severity = Severity::info;
    std::string message;
};

std::string toString(Severity severity);

}  // namespace tamagaki::ids
