#pragma once

#include "cli/environment.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace cli {

/**
 * Abstract interface for all commands (built-in and external).
 * Executes with given arguments and I/O streams; may read environment.
 */
class Command {
public:
    virtual ~Command() = default;

    /**
     * Runs the command.
     * @param args Command name (args[0]) and arguments (args[1..]).
     * @param in Standard input stream (e.g. for external commands).
     * @param out Standard output stream.
     * @param err Standard error stream.
     * @param env Environment (read-only for built-ins; passed to external).
     * @return Exit code (0 = success; 1 = error; 127 = command not found for external).
     */
    virtual int execute(const std::vector<std::string>& args,
                        std::istream& in,
                        std::ostream& out,
                        std::ostream& err,
                        const Environment& env) = 0;
};

}  // namespace cli
