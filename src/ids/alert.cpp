#include "src/ids/alert.hpp"

namespace tamagaki::ids {

std::string toString(Severity severity)
{
    switch (severity) {
    case Severity::info:
        return "info";
    case Severity::warning:
        return "warning";
    case Severity::critical:
        return "critical";
    }

    return "unknown";
}

}  // namespace tamagaki::ids
