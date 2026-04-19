#include "src/ids/baseline.hpp"

#include <fstream>
#include <regex>

namespace tamagaki::ids {

Baseline::Baseline(std::string path) : path_(std::move(path)) {}

void Baseline::load()
{
    destinations_.clear();

    std::ifstream input(path_);
    std::string line;
    const std::regex ip_pattern(R"((\d{1,3}(?:\.\d{1,3}){3}))");

    while (std::getline(input, line)) {
        std::smatch match;
        if (std::regex_search(line, match, ip_pattern)) {
            destinations_.insert(match.str(1));
        }
    }
}

void Baseline::save() const
{
    std::ofstream output(path_);
    output << "{\n";
    output << "  \"destinations\": [\n";

    for (auto it = destinations_.begin(); it != destinations_.end(); ++it) {
        output << "    \"" << *it << "\"";
        if (std::next(it) != destinations_.end()) {
            output << ",";
        }
        output << "\n";
    }

    output << "  ]\n";
    output << "}\n";
}

bool Baseline::allows(const std::string &destination) const
{
    return destinations_.contains(destination);
}

void Baseline::remember(const std::string &destination)
{
    destinations_.insert(destination);
}

}  // namespace tamagaki::ids
