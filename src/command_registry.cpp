#include "cli/command_registry.hpp"

namespace cli {

void CommandRegistry::register_command(const std::string &name,
                                       std::unique_ptr<Command> cmd) {
  if (cmd)
    commands_[name] = std::move(cmd);
}

Command *CommandRegistry::find(const std::string &name) const {
  auto it = commands_.find(name);
  if (it == commands_.end())
    return nullptr;
  return it->second.get();
}

bool CommandRegistry::has(const std::string &name) const {
  return commands_.find(name) != commands_.end();
}

} // namespace cli
