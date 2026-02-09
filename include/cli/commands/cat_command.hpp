#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: cat <FILE> — output file contents to stdout.
 */
class CatCommand : public Command {
public:
    int execute(const std::vector<std::string>& args,
                std::istream& in,
                std::ostream& out,
                std::ostream& err,
                const Environment& env) override;
};

}  // namespace cli
