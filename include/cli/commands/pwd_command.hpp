#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: pwd — print current working directory.
 */
class PwdCommand : public Command {
public:
    int execute(const std::vector<std::string>& args,
                std::istream& in,
                std::ostream& out,
                std::ostream& err,
                const Environment& env) override;
};

}  // namespace cli
