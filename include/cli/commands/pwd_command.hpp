#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: pwd — print the current working directory.
 *
 * Writes the absolute path of the current working directory to stdout,
 * followed by a newline. Ignores arguments. May fail if the working
 * directory is unavailable (e.g. removed).
 *
 * @see Command
 */
class PwdCommand : public Command {
public:
  /**
   * Execute pwd: print current working directory to stdout.
   *
   * @param[in] args Ignored (args[0] is "pwd").
   * @param[in,out] in Not used.
   * @param[in,out] out Where the current working directory path is written.
   * @param[in,out] err Where error messages are written on failure.
   * @param[in] env Not used by this command.
   *
   * @returns 0 on success; non-zero if the current directory cannot be
   * determined.
   *
   * @exceptsafe Basic guarantee; may throw on I/O or platform API failure.
   */
  int execute(const std::vector<std::string> &args, std::istream &in,
              std::ostream &out, std::ostream &err,
              const Environment &env) override;
};

} // namespace cli
