#pragma once

#include <string>
#include <vector>

namespace cli {

/**
 * Whether a token should undergo variable substitution ($VAR) when executing.
 * Single-quoted: false. Double-quoted and unquoted: true.
 */
enum class Substitute { No, Yes };

/**
 * AST node representing a single command.
 * Command name and list of arguments after tokenization and quote handling.
 * substitute_name and substitute_arg[i] indicate if $VAR expansion applies.
 */
struct CommandNode {
    std::string name;
    std::vector<std::string> args;
    Substitute substitute_name{Substitute::Yes};
    std::vector<Substitute> substitute_arg;
};

/**
 * Pipeline: sequence of commands connected by |.
 * Data flows: stdin -> cmd0 -> cmd1 -> ... -> stdout; stderr goes to shell stderr.
 */
using Pipeline = std::vector<CommandNode>;

}  // namespace cli
