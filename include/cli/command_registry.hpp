#pragma once

#include "cli/command.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace cli {

/**
 * Map command names to command instances for built-in resolution.
 *
 * The Executor uses the registry to resolve command names; if a name is not
 * registered, the command is executed as an external program. Typically
 * holds built-in commands such as echo, cat, pwd, wc, and exit.
 *
 * @see Executor
 * @see Command
 */
class CommandRegistry {
public:
  /**
   * Register a command under the given name.
   *
   * The registry takes ownership of the command. If the name was already
   * registered, the previous command is replaced.
   *
   * @param[in] name Command name (e.g. "echo", "cat"); used for lookup.
   * @param[in] cmd Unique pointer to the command instance; must not be null.
   *
   * @exceptsafe Strong guarantee; may throw on allocation.
   */
  void register_command(const std::string &name, std::unique_ptr<Command> cmd);

  /**
   * Find the command registered under the given name.
   *
   * @param[in] name Command name to look up.
   *
   * @returns Pointer to the registered command, or `nullptr` if not found.
   *
   * @exceptsafe Shall not throw exceptions.
   */
  Command *find(const std::string &name) const;

  /**
   * Check whether a name is registered as a built-in command.
   *
   * @param[in] name Command name to check.
   *
   * @returns True if `name` is registered; false otherwise.
   *
   * @exceptsafe Shall not throw exceptions.
   */
  bool has(const std::string &name) const;

private:
  std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
};

} // namespace cli
