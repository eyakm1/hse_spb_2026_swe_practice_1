#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: exit — request interpreter to exit.
 * Returns -1 as special code so Executor sets should_exit.
 */
class ExitCommand : public Command {
public:
    int execute(const std::vector<std::string>& args,
                std::istream& in,
                std::ostream& out,
                std::ostream& err,
                const Environment& env) override;
};

}  // namespace cli
