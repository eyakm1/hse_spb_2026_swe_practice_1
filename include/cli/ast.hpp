#pragma once

#include <string>
#include <vector>

namespace cli {

/**
 * AST node representing a single command.
 * Command name and list of arguments after tokenization and quote handling.
 */
struct CommandNode {
    std::string name;
    std::vector<std::string> args;
};

}  // namespace cli
