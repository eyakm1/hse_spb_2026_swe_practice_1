#pragma once

#include <string>
#include <vector>

namespace cli {

/**
 * Indicate whether a token undergoes variable substitution when executing.
 *
 * Single-quoted segments do not substitute (`No`). Double-quoted and unquoted
 * segments do substitute `$VAR` and `${VAR}` (`Yes`).
 */
enum class Substitute { No, Yes };

/**
 * AST node representing a single command in the pipeline.
 *
 * Holds the command name and argument list after tokenization and quote
 * handling. The `substitute_name` and `substitute_arg` flags control whether
 * `$VAR` expansion is applied to each token during execution.
 */
struct CommandNode {
  /// Command name (e.g. "echo", "cat"); subject to substitution if
  /// substitute_name is Yes.
  std::string name;
  /// Argument list after tokenization; each element may be substituted per
  /// substitute_arg.
  std::vector<std::string> args;
  /// Whether to substitute variables in the command name.
  Substitute substitute_name{Substitute::Yes};
  /// Per-argument substitution flags; size should match args.
  std::vector<Substitute> substitute_arg;
};

/**
 * Pipeline: sequence of commands connected by pipe.
 *
 * Data flows from stdin through the first command, then to the next, and so on
 * to stdout. stderr from each command is forwarded to the shell stderr.
 */
using Pipeline = std::vector<CommandNode>;

} // namespace cli
