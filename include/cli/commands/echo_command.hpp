#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: echo — print arguments to stdout, space-separated.
 *
 * Writes each argument (args[1], args[2], ...) to standard output with a
 * single space between them, followed by a newline. Does not read from stdin.
 *
 * @see Command
 */
class EchoCommand : public Command {
public:
  /**
   * Execute echo: print arguments to stdout.
   *
   * @param[in] args args[0] is "echo"; args[1..] are printed, space-separated.
   * @param[in,out] in Not used.
   * @param[in,out] out Where the arguments and a trailing newline are written.
   * @param[in,out] err Not used for normal operation.
   * @param[in] env Not used by this command.
   *
   * @returns 0 on success.
   *
   * @exceptsafe Basic guarantee; may throw on stream write failure.
   */
  int execute(const std::vector<std::string> &args, std::istream &in,
              std::ostream &out, std::ostream &err,
              const Environment &env) override;
};

} // namespace cli
