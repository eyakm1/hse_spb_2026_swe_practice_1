#include "cli/foo.hpp"
#include <utility>

namespace cli {
    std::string greet(std::string name) {
        if (name.empty()) name = "world";
        return "Hello, " + std::move(name) + "!";
    }
}
