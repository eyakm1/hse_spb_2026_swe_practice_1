#include "cli/commands/pwd_command.hpp"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <direct.h>
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#include <array>
#include <string>

namespace cli {

int PwdCommand::execute(const std::vector<std::string> & /*args*/,
                        std::istream & /*in*/, std::ostream &out,
                        std::ostream &err, const Environment & /*env*/) {
#ifdef _WIN32
  std::array<char, 4096> buf;
  if (_getcwd(buf.data(), static_cast<int>(buf.size())) == nullptr) {
    err << "pwd: cannot get current directory\n";
    return 1;
  }
  out << buf.data() << '\n';
#else
  std::array<char, PATH_MAX> buf;
  if (getcwd(buf.data(), buf.size()) == nullptr) {
    err << "pwd: cannot get current directory\n";
    return 1;
  }
  out << buf.data() << '\n';
#endif
  return 0;
}

} // namespace cli
