#pragma once

#include "cli/ast.hpp"
#include <optional>
#include <string>

namespace cli {

/**
 * Parse a single input line into a pipeline of commands.
 *
 * Converts raw user input into an AST (Pipeline of CommandNode). Supports
 * single-quoted (weak quoting with backslash escapes, no variable
 * substitution), double-quoted (literal with variable substitution), and
 * unquoted segments. The pipe character `|` separates commands and is literal
 * inside quotes.
 *
 * @see Pipeline
 * @see CommandNode
 */
class Parser {
public:
  /**
   * Parse a line into a pipeline of command nodes.
   *
   * Tokenizes the line respecting quotes and backslash escapes, then builds
   * a sequence of commands. Empty or whitespace-only lines yield no pipeline.
   *
   * @param[in] line Raw input line from the user (may contain spaces, quotes,
   * pipes).
   *
   * @returns The parsed pipeline, or `std::nullopt` if `line` is empty or
   *     contains only whitespace.
   *
   * @exceptsafe Shall not throw exceptions.
   */
  static std::optional<Pipeline> parse(const std::string &line);
};

} // namespace cli
