#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace tamagaki::net {

using MacAddress = std::array<std::uint8_t, 6>;

std::optional<MacAddress> parseMacAddress(const std::string &value);
std::string formatMacAddress(const MacAddress &value);

std::optional<std::uint32_t> parseIPv4(const std::string &value);
std::string formatIPv4(std::uint32_t value);

}  // namespace tamagaki::net
