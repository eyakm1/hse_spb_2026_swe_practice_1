#pragma once

#include "cli/ast.hpp"
#include "cli/command_registry.hpp"
#include "cli/environment.hpp"
#include "cli/external_command.hpp"
#include <iostream>
#include <stdexcept>

namespace cli {

/**
 * Result of execution: either an exit code or a request to exit the REPL.
 */
struct ExecutorResult {
    bool should_exit{false};
    int exit_code{0};
};

/**
 * Thrown when the user runs the exit command (optional alternative to ExecutorResult).
 */
class ExitRequest : public std::exception {
public:
    explicit ExitRequest(int code) : code_(code) {}
    int code() const { return code_; }
    const char* what() const noexcept override { return "exit"; }
private:
    int code_;
};

/**
 * Executes AST.
 * Resolves command via CommandRegistry; if not found, runs ExternalCommand.
 */
class Executor {
public:
    explicit Executor(CommandRegistry& registry);

    /**
     * Executes the given command node.
     * @param node Parsed command (name + args).
     * @param in Standard input.
     * @param out Standard output.
     * @param err Standard error.
     * @param env Environment for the process.
     * @return should_exit true when exit command was run, exit_code for status.
     */
    ExecutorResult execute(const CommandNode& node,
                           std::istream& in,
                           std::ostream& out,
                           std::ostream& err,
                           const Environment& env);

private:
    CommandRegistry& registry_;
    ExternalCommand external_;
};

}  // namespace cli
