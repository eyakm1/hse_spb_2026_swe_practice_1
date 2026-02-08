#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Runs an external program by name with given args and environment.
 * Uses CreateProcess on Windows and fork/exec on POSIX.
 * stdout/stderr/return code are those of the child process.
 */
class ExternalCommand : public Command {
public:
    int execute(const std::vector<std::string>& args,
                std::istream& in,
                std::ostream& out,
                std::ostream& err,
                const Environment& env) override;
};

}  // namespace cli
