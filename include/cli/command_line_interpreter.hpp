#pragma once

#include "cli/command_registry.hpp"
#include "cli/environment.hpp"
#include "cli/executor.hpp"
#include "cli/parser.hpp"
#include <iostream>
#include <string>

namespace cli {

/**
 * Main REPL loop: read line → parse → execute → repeat until exit.
 */
class CommandLineInterpreter {
public:
    /**
     * Constructs interpreter with default command registry (cat, echo, wc, pwd, exit)
     * and environment copied from current process.
     */
    CommandLineInterpreter();

    /**
     * Runs the read-execute-print loop until the user runs "exit" or EOF.
     * Uses in for input, out for normal output, err for errors.
     * @return Exit code when stopped by "exit" command, or 0 on EOF.
     */
    int run(std::istream& in = std::cin, std::ostream& out = std::cout, std::ostream& err = std::cerr);

private:
    void register_builtins();

    Parser parser_;
    Environment env_;
    CommandRegistry registry_;
    Executor executor_;
};

}  // namespace cli
