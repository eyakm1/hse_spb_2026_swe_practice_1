#include "cli/commands/echo_command.hpp"

namespace cli {

int EchoCommand::execute(const std::vector<std::string> &args,
                         std::istream & /*in*/, std::ostream &out,
                         std::ostream & /*err*/, const Environment & /*env*/) {
  for (std::size_t i = 1; i < args.size(); ++i) {
    if (i > 1)
      out << ' ';
    out << args[i];
  }
  out << '\n';
  return 0;
}

} // namespace cli
