#pragma once

#include "cli/command_registry.hpp"
#include "cli/environment.hpp"
#include "cli/executor.hpp"
#include "cli/parser.hpp"
#include <iostream>
#include <string>

namespace cli {

/**
 * Main REPL: read a line, parse it, execute the pipeline, repeat until exit.
 *
 * Combines a Parser, Environment, CommandRegistry, and Executor to implement
 * a read-eval-print loop. Built-in commands (cat, echo, pwd, wc, exit) are
 * registered at construction; unknown names are executed as external programs.
 *
 * @see Parser
 * @see Executor
 * @see CommandRegistry
 */
class CommandLineInterpreter {
public:
  /**
   * Construct an interpreter with default built-ins and current environment.
   *
   * Registers the built-in commands (cat, echo, pwd, wc, exit) and
   * initializes the environment from the current process (e.g. getenv).
   *
   * @exceptsafe May throw on allocation or during register_builtins.
   */
  CommandLineInterpreter();

  /**
   * Run the read-execute-print loop until exit or EOF.
   *
   * For each iteration: read a line from `in`, parse it into a pipeline,
   * execute the pipeline with the current environment, and continue. The
   * loop stops when the user runs the "exit" command or when `in` reaches EOF.
   *
   * @param[in,out] in Input stream for user lines (default: std::cin).
   * @param[in,out] out Output stream for command stdout (default: std::cout).
   * @param[in,out] err Output stream for errors and stderr (default:
   * std::cerr).
   *
   * @returns Exit code when stopped by the "exit" command, or 0 when EOF is
   * reached.
   *
   * @exceptsafe Basic guarantee; streams and internal state may change on
   * failure.
   */
  int run(std::istream &in = std::cin, std::ostream &out = std::cout,
          std::ostream &err = std::cerr);

private:
  /// Register built-in commands (cat, echo, pwd, wc, exit) in the registry.
  void register_builtins();

  Parser parser_;
  Environment env_;
  CommandRegistry registry_;
  Executor executor_;
};

} // namespace cli
