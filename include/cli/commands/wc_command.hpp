#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: wc <FILE> — print line count, word count, byte count for file.
 */
class WcCommand : public Command {
public:
    int execute(const std::vector<std::string>& args,
                std::istream& in,
                std::ostream& out,
                std::ostream& err,
                const Environment& env) override;
};

}  // namespace cli
