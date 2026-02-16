#pragma once

#include "cli/ast.hpp"
#include "cli/command_registry.hpp"
#include "cli/environment.hpp"
#include "cli/external_command.hpp"
#include <iostream>
#include <stdexcept>

namespace cli {

/**
 * Result of pipeline execution: exit code and whether the REPL should stop.
 *
 * When a built-in exit command runs, `should_exit` is true and `exit_code`
 * holds the requested code. Otherwise, `exit_code` is the exit code of the
 * last command in the pipeline.
 */
struct ExecutorResult {
  /// True if the interpreter should exit (e.g. user ran "exit").
  bool should_exit{false};
  /// Exit code to report (last command's code, or exit argument).
  int exit_code{0};
};

/**
 * Exception thrown when the user runs the exit command.
 *
 * Optional alternative to signaling exit via ExecutorResult; carries the
 * requested exit code. Executor may use this or the result struct.
 */
class ExitRequest : public std::exception {
public:
  /**
   * Construct exit request with the given exit code.
   *
   * @param[in] code Exit code (typically 0–255).
   */
  explicit ExitRequest(int code) : code_(code) {}

  /**
   * Get the requested exit code.
   *
   * @returns The exit code passed to the constructor.
   *
   * @exceptsafe Shall not throw exceptions.
   */
  int code() const { return code_; }

  /**
   * Return a short description of the exception.
   *
   * @returns The string "exit".
   *
   * @exceptsafe Shall not throw exceptions.
   */
  const char *what() const noexcept override { return "exit"; }

private:
  int code_;
};

/**
 * Execute a parsed pipeline (AST) using the command registry and environment.
 *
 * Resolves each command name via CommandRegistry; if not found, runs it as
 * an ExternalCommand. Applies variable substitution to names and arguments
 * according to the AST, then runs a single command or chains multiple
 * commands via streams.
 *
 * @see CommandRegistry
 * @see ExternalCommand
 * @see Pipeline
 */
class Executor {
public:
  /**
   * Construct an executor that uses the given command registry.
   *
   * @param[in] registry Registry for built-in commands; must outlive the
   * executor.
   */
  explicit Executor(CommandRegistry &registry);

  /**
   * Execute a pipeline of one or more commands.
   *
   * Expands variables in each CommandNode according to its substitute flags,
   * then runs the first command with the given streams. For pipelines of
   * multiple commands, chains stdout of one to stdin of the next. The exit
   * code is that of the last command; if any command requests exit (e.g.
   * built-in exit), the result has `should_exit` set.
   *
   * @param[in] pipeline Parsed sequence of commands to execute.
   * @param[in,out] in Standard input for the first command.
   * @param[in,out] out Standard output (and input for next command in a pipe).
   * @param[in,out] err Standard error stream.
   * @param[in] env Environment for variable substitution and external
   * processes.
   *
   * @returns Result with exit code and whether the REPL should exit.
   *
   * @throws ExitRequest Optionally, when the user runs the exit command.
   *
   * @exceptsafe Basic guarantee; streams and process state may change on
   * failure.
   */
  ExecutorResult execute(const Pipeline &pipeline, std::istream &in,
                         std::ostream &out, std::ostream &err,
                         const Environment &env);

private:
  /**
   * Expand a command node's name and arguments using the environment.
   *
   * Applies substitution only where Substitute::Yes is set; fills args_out
   * with the final list of strings for execute_one.
   *
   * @param[in] node AST node (name, args, substitute flags).
   * @param[in] env Environment for variable expansion.
   * @param[out] args_out Output vector of expanded command name and arguments.
   */
  static void expand_node(const CommandNode &node, const Environment &env,
                         std::vector<std::string> &args_out);

  /**
   * Run a single command with already-expanded arguments.
   *
   * Used internally by execute(Pipeline) after expand_node. Looks up the
   * command in the registry or runs it as external.
   *
   * @param[in] args Command name (args[0]) and arguments (args[1..]).
   * @param[in,out] in Standard input.
   * @param[in,out] out Standard output.
   * @param[in,out] err Standard error.
   * @param[in] env Environment for external commands.
   *
   * @returns Result with exit code and should_exit flag.
   */
  ExecutorResult execute_one(const std::vector<std::string> &args,
                             std::istream &in, std::ostream &out,
                             std::ostream &err, const Environment &env);

  CommandRegistry &registry_;
  ExternalCommand external_;
};

} // namespace cli
