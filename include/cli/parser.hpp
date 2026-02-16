#pragma once

#include "cli/ast.hpp"
#include <optional>
#include <string>

namespace cli {

/**
 * Parses a single input line into a pipeline (one or more commands separated by |).
 * - Single quote: weak quoting — backslash escapes (\', \\, \n, \t, \r); no variable substitution.
 * - Double quote: full quoting — no escapes, literal; variable substitution applies.
 * - Unquoted: variable substitution applies.
 * - Pipe | separates commands; literal inside quotes.
 */
class Parser {
public:
    /**
     * Parses line into a pipeline of command nodes.
     * Returns nullopt if line is empty or only whitespace.
     */
    std::optional<Pipeline> parse(const std::string& line) const;
};

}  // namespace cli
