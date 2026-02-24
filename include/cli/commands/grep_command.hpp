#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: grep — search for regular expression matches in input.
 *
 * Reads from files or stdin, prints lines matching the given pattern.
 * Supports options: -w (whole word), -i (ignore case), -A N (N lines of
 * context after each match). Overlapping context regions are merged so each
 * line is printed at most once.
 *
 * @see Command
 * @see CatCommand
 * @see WcCommand
 */
class GrepCommand : public Command {
public:
  /**
   * Execute grep: search for pattern in files or stdin.
   *
   * Parses options with a CLI library (CLI11). Pattern is the first
   * positional argument; remaining positionals are file paths (or stdin if
   * none). -w enables whole-word match, -i case-insensitive, -A N prints
   * N lines after each match.
   *
   * @param[in] args args[0] is "grep"; args[1..] are options and operands.
   * @param[in,out] in Used when no file arguments are given.
   * @param[in,out] out Where matching lines (and context) are written.
   * @param[in,out] err Where error messages are written.
   * @param[in] env Not used by this command.
   *
   * @returns 0 if at least one match; 1 if no match or no input; 2 on
   * invalid usage or invalid regex.
   *
   * @exceptsafe Basic guarantee; may throw on I/O or allocation.
   */
  int execute(const std::vector<std::string> &args, std::istream &in,
              std::ostream &out, std::ostream &err,
              const Environment &env) override;
};

} // namespace cli
