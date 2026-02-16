#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace cli {

/**
 * Store and provide environment variables for the shell.
 *
 * Holds a key-value map of variable names to values. Used for variable
 * substitution in command lines and for building the environment block
 * passed to external processes.
 */
class Environment {
public:
  Environment();

  /**
   * Initialize environment from the current process environment.
   *
   * Copies all environment variables from the running process (e.g. via
   * platform-specific APIs such as `getenv` iteration on POSIX or
   * `GetEnvironmentStrings` on Windows).
   *
   * @exceptsafe May throw on allocation or platform API failure.
   */
  void init_from_current();

  /**
   * Get the value of a variable by name.
   *
   * @param[in] name Variable name (case-sensitive).
   *
   * @returns The value of the variable, or an empty string if not set.
   *
   * @exceptsafe Shall not throw exceptions.
   */
  std::string get(const std::string &name) const;

  /**
   * Expand variable references in a string using the current environment.
   *
   * Replaces `$VAR` and `${VAR}` with the corresponding value. `$$` becomes
   * a literal `$`. A lone `$` at end of string or an invalid name is left
   * unchanged. Used for double-quoted and unquoted segments; single-quoted
   * segments do not call substitute.
   *
   * @param[in] s String that may contain `$VAR` or `${VAR}` patterns.
   *
   * @returns A new string with variable references expanded.
   *
   * @exceptsafe Shall not throw exceptions.
   */
  std::string substitute(const std::string &s) const;

  /**
   * Set a variable to a value.
   *
   * Overwrites the previous value if the variable already exists.
   *
   * @param[in] name Variable name.
   * @param[in] value Value to assign.
   *
   * @exceptsafe May throw on allocation.
   */
  void set(const std::string &name, const std::string &value);

  /**
   * Remove a variable from the environment.
   *
   * Has no effect if the variable is not set.
   *
   * @param[in] name Variable name to unset.
   *
   * @exceptsafe Shall not throw exceptions.
   */
  void unset(const std::string &name);

  /**
   * Build a platform-specific environment block for an external process.
   *
   * On Windows, the result is a vector of "key=value" strings that can be
   * converted to the null-terminated block format expected by CreateProcess.
   * On POSIX, the vector is in the form expected by `execve` (e.g. "key=val").
   *
   * @returns Vector of "key=value" strings representing the current
   * environment.
   *
   * @exceptsafe May throw on allocation.
   */
  std::vector<std::string> to_env_vector() const;

private:
  std::unordered_map<std::string, std::string> vars_;
};

} // namespace cli
