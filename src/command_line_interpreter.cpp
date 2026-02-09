#include "cli/command_line_interpreter.hpp"
#include "cli/commands/cat_command.hpp"
#include "cli/commands/echo_command.hpp"
#include "cli/commands/exit_command.hpp"
#include "cli/commands/pwd_command.hpp"
#include "cli/commands/wc_command.hpp"
#include <memory>

namespace cli {

CommandLineInterpreter::CommandLineInterpreter() : executor_(registry_) {
    env_.init_from_current();
    register_builtins();
}

void CommandLineInterpreter::register_builtins() {
    registry_.register_command("cat", std::make_unique<CatCommand>());
    registry_.register_command("echo", std::make_unique<EchoCommand>());
    registry_.register_command("wc", std::make_unique<WcCommand>());
    registry_.register_command("pwd", std::make_unique<PwdCommand>());
    registry_.register_command("exit", std::make_unique<ExitCommand>());
}

void CommandLineInterpreter::run(std::istream& in, std::ostream& out, std::ostream& err) {
    std::string line;
    while (true) {
        if (&in == &std::cin) {
            out << "> " << std::flush;
        }
        if (!std::getline(in, line))
            break;
        auto node = parser_.parse(line);
        if (!node)  // empty line
            continue;
        ExecutorResult result = executor_.execute(*node, in, out, err, env_);
        if (result.should_exit)
            break;
    }
}

}  // namespace cli
