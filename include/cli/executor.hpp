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
     * Executes a pipeline (one or more commands). Applies variable substitution,
     * then runs single command or chains via streams. Exit code is the last command's;
     * if any command requests exit (built-in exit), returns should_exit.
     */
    ExecutorResult execute(const Pipeline& pipeline,
                           std::istream& in,
                           std::ostream& out,
                           std::ostream& err,
                           const Environment& env);

private:
    /** Expands node name/args using env according to substitute flags; fills args_out. */
    void expand_node(const CommandNode& node, const Environment& env,
                     std::vector<std::string>& args_out) const;

    /** Runs one command (already expanded args); used by execute(Pipeline). */
    ExecutorResult execute_one(const std::vector<std::string>& args,
                               std::istream& in,
                               std::ostream& out,
                               std::ostream& err,
                               const Environment& env);

    CommandRegistry& registry_;
    ExternalCommand external_;
};

}  // namespace cli
