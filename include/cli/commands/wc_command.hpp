#pragma once

#include "cli/command.hpp"

namespace cli {

/**
 * Built-in command: wc — print line, word, and byte counts.
 *
 * For each file path in arguments (or stdin if no arguments), prints the
 * number of lines, words, and bytes. Words are separated by whitespace.
 *
 * @see Command
 * @see CatCommand
 */
class WcCommand : public Command {
public:
  /**
   * Execute wc: count lines, words, and bytes for files or stdin.
   *
   * If args[1..] are present, each is treated as a file path and counted.
   * If only args[0] ("wc") is given, counts from `in`. Output is written
   * to `out` in a format like "lines words bytes [path]"; errors to `err`.
   *
   * @param[in] args args[0] is "wc"; args[1..] are optional file paths.
   * @param[in,out] in Used when no file arguments are given.
   * @param[in,out] out Where the count line(s) are written.
   * @param[in,out] err Where error messages are written.
   * @param[in] env Not used by this command.
   *
   * @returns 0 on success; non-zero if any file could not be read.
   *
   * @exceptsafe Basic guarantee; may throw on I/O or allocation.
   */
  int execute(const std::vector<std::string> &args, std::istream &in,
              std::ostream &out, std::ostream &err,
              const Environment &env) override;
};

} // namespace cli
