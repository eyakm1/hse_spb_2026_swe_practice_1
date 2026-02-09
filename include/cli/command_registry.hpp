#pragma once

#include "cli/command.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace cli {

/**
 * Maps command names to command instances (or factories).
 * Used by Executor to resolve built-in commands; unknown names → external.
 */
class CommandRegistry {
public:
    /**
     * Registers a command under the given name (e.g. "echo", "cat").
     * Overwrites any existing registration for that name.
     */
    void register_command(const std::string& name, std::unique_ptr<Command> cmd);

    /**
     * Returns the command for name, or nullptr if not registered.
     */
    Command* find(const std::string& name) const;

    /**
     * Returns true if name is a built-in registered command.
     */
    bool has(const std::string& name) const;

private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
};

}  // namespace cli
