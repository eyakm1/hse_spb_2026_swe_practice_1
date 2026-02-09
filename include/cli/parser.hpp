#pragma once

#include "cli/ast.hpp"
#include <optional>
#include <string>

namespace cli {

/**
 * Parses a single input line into a CommandNode.
 * - Single quote: weak quoting — backslash escapes (\', \\, \n, \t, \r).
 * - Double quote: full quoting — no escapes, every character literal.
 */
class Parser {
public:
    /**
     * Parses line into command name and arguments.
     * Returns nullopt if line is empty or only whitespace.
     */
    std::optional<CommandNode> parse(const std::string& line) const;
};

}  // namespace cli
