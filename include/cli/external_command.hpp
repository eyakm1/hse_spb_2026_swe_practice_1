#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Run an external program by name with given arguments and environment.
 *
 * Spawns a child process: on Windows uses CreateProcess; on POSIX uses
 * fork/exec. Standard input is fed from the given stream; stdout and stderr
 * of the child are captured to the given streams. The exit code is that of
 * the child process (e.g. 127 if the program was not found).
 *
 * @see Command
 * @see Environment::to_env_vector
 */
class ExternalCommand : public Command {
public:
  /**
   * Execute the external program identified by args[0].
   *
   * Forks (or creates) a process with args[0] as the program name and
   * args[1..] as arguments. The environment is built from `env` and passed
   * to the child. Child stdin is connected to `in`; child stdout and stderr
   * are written to `out` and `err`.
   *
   * @param[in] args Program name (args[0]) and arguments (args[1..]).
   * @param[in,out] in Standard input for the child process.
   * @param[in,out] out Standard output from the child process.
   * @param[in,out] err Standard error from the child process.
   * @param[in] env Environment for the child (e.g. via
   * Environment::to_env_vector).
   *
   * @returns Exit code of the child process (0 on success; 127 if command
   *     not found; other values as returned by the program).
   *
   * @exceptsafe Basic guarantee; may throw on fork/exec or stream failure.
   */
  int execute(const std::vector<std::string> &args, std::istream &in,
              std::ostream &out, std::ostream &err,
              const Environment &env) override;
};

} // namespace cli
