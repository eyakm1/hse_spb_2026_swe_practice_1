#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: cat — output file contents or stdin to stdout.
 *
 * With file path arguments, concatenates and prints each file. With no
 * arguments, copies standard input to standard output (e.g. for use in pipes).
 *
 * @see Command
 * @see WcCommand
 */
class CatCommand : public Command {
public:
  /**
   * Execute cat: print files or stdin to stdout.
   *
   * For each path in args[1..], opens the file and writes its contents to
   * `out`. If there are no file arguments, reads from `in` and writes to `out`.
   * Errors (e.g. file not found) are written to `err` and reflected in the exit
   * code.
   *
   * @param[in] args args[0] is "cat"; args[1..] are optional file paths.
   * @param[in,out] in Used when no file arguments are given.
   * @param[in,out] out Where file or stdin contents are written.
   * @param[in,out] err Where error messages are written.
   * @param[in] env Not used by this command.
   *
   * @returns 0 on success; non-zero if any file could not be read.
   *
   * @exceptsafe Basic guarantee; may throw on I/O or allocation.
   */
  int execute(const std::vector<std::string> &args, std::istream &in,
              std::ostream &out, std::ostream &err,
              const Environment &env) override;
};

} // namespace cli
