#include "cli/commands/exit_command.hpp"

namespace cli {

int ExitCommand::execute(const std::vector<std::string>& /*args*/,
                         std::istream& /*in*/,
                         std::ostream& /*out*/,
                         std::ostream& /*err*/,
                         const Environment& /*env*/) {
    return -1;  // Special code: Executor interprets as "exit REPL"
}

}  // namespace cli
