#include "cli/command_line_interpreter.hpp"

int main() {
    cli::CommandLineInterpreter interpreter;
    interpreter.run(std::cin, std::cout, std::cerr);
    return 0;
}
