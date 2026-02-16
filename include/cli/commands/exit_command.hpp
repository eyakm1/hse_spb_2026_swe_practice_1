#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: exit [n] — request interpreter to exit with code n (0–255).
 * Returns -(n+1) so Executor sets should_exit and exit_code.
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
