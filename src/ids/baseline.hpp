#pragma once

#include <set>
#include <string>

namespace tamagaki::ids {

class Baseline {
  public:
    explicit Baseline(std::string path);

    void load();
    void save() const;
    bool allows(const std::string &destination) const;
    void remember(const std::string &destination);

  private:
    std::string path_;
    std::set<std::string> destinations_;
};

}  // namespace tamagaki::ids
