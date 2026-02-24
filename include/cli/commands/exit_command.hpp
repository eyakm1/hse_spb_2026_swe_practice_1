#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: exit — request the interpreter to exit with a given code.
 *
 * Optional argument `n` (0–255) is the exit code; default is 0. This command
 * returns a special value (e.g. -(n+1)) so the Executor sets should_exit and
 * exit_code instead of treating it as a normal exit code.
 *
 * @see Command
 * @see Executor
 * @see ExecutorResult
 */
class ExitCommand : public Command {
public:
  /**
   * Execute exit: signal the REPL to stop with the given exit code.
   *
   * If args[1] is present and numeric in 0–255, that value is used as the
   * exit code; otherwise 0 is used. The return value is a sentinel so the
   * Executor recognizes an exit request and sets ExecutorResult::should_exit.
   *
   * @param[in] args args[0] is "exit"; optional args[1] is exit code (0–255).
   * @param[in,out] in Not used.
   * @param[in,out] out Not used.
   * @param[in,out] err Where error messages may be written for invalid code.
   * @param[in] env Not used by this command.
   *
   * @returns A sentinel value (e.g. negative) so Executor sets should_exit
   *     and exit_code; not a normal process exit code.
   *
   * @exceptsafe Shall not throw exceptions for valid usage.
   */
  int execute(const std::vector<std::string> &args, std::istream &in,
              std::ostream &out, std::ostream &err,
              const Environment &env) override;
};

} // namespace cli
