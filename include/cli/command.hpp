#pragma once

#include "cli/environment.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace cli {

/**
 * Abstract interface for all commands (built-in and external).
 *
 * Commands receive the argument list (name plus arguments), standard I/O
 * streams, and the current environment. Built-ins typically only read the
 * environment; external commands receive it for the child process.
 *
 * @see CatCommand
 * @see EchoCommand
 * @see PwdCommand
 * @see WcCommand
 * @see ExitCommand
 * @see ExternalCommand
 */
class Command {
public:
  virtual ~Command() = default;

  /**
   * Run the command with the given arguments and streams.
   *
   * The first element of `args` is the command name; the rest are arguments.
   * The command may read from `in`, write to `out` and `err`, and use `env`
   * for variable lookup or for spawning external processes.
   *
   * @param[in] args Command name (args[0]) and arguments (args[1..]).
   * @param[in,out] in Standard input stream (e.g. for cat or external
   * programs).
   * @param[in,out] out Standard output stream.
   * @param[in,out] err Standard error stream.
   * @param[in] env Environment (read-only for built-ins; passed to external).
   *
   * @returns Exit code: 0 for success, non-zero for failure (e.g. 1 for error,
   *     127 for command not found in external execution).
   *
   * @exceptsafe Depends on the concrete command; built-ins aim for strong
   * guarantee.
   */
  virtual int execute(const std::vector<std::string> &args, std::istream &in,
                      std::ostream &out, std::ostream &err,
                      const Environment &env) = 0;
};

} // namespace cli
