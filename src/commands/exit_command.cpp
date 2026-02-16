#include "cli/commands/exit_command.hpp"
#include <cstdlib>
#include <string>

namespace cli {

int ExitCommand::execute(const std::vector<std::string> &args,
                         std::istream & /*in*/, std::ostream & /*out*/,
                         std::ostream &err, const Environment & /*env*/) {
  int code = 0;
  if (args.size() >= 2) {
    const std::string &arg = args[1];
    char *end = nullptr;
    long val = std::strtol(arg.c_str(), &end, 10);
    if (end != arg.c_str() && *end == '\0' && val >= 0 && val <= 255) {
      code = static_cast<int>(val);
    } else if (end != arg.c_str() && *end == '\0') {
      code = (val < 0) ? 0 : 255;
    } else {
      err << "exit: " << arg << ": numeric argument required\n";
      code = 255;
    }
  }
  return -(code + 1);
}

} // namespace cli
