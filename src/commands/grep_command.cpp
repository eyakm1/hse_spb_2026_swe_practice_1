#include "cli/commands/grep_command.hpp"
#include <CLI/CLI.hpp>
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace cli {

namespace {

/// Merges overlapping [start, end] (inclusive) intervals; returns sorted
/// non-overlapping ranges.
std::vector<std::pair<std::size_t, std::size_t>>
merge_ranges(std::vector<std::pair<std::size_t, std::size_t>> ranges) {
  if (ranges.empty())
    return {};
  std::sort(ranges.begin(), ranges.end());
  std::vector<std::pair<std::size_t, std::size_t>> out;
  out.push_back(ranges[0]);
  for (std::size_t i = 1; i < ranges.size(); ++i) {
    if (ranges[i].first <= out.back().second + 1) {
      out.back().second =
          std::max(out.back().second, ranges[i].second);
    } else {
      out.push_back(ranges[i]);
    }
  }
  return out;
}

/// Reads lines from stream into a vector (each element is one line without
/// trailing newline; empty stream => empty vector).
std::vector<std::string> read_lines(std::istream &in) {
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(in, line))
    lines.push_back(std::move(line));
  return lines;
}

/// Runs grep on a single input: lines, regex, after_context. Fills
/// line_ranges with [start, end] (inclusive) of lines to print; each line
/// index is only in one range (merged).
void find_matching_ranges(const std::vector<std::string> &lines,
                         const std::regex &re,
                         std::size_t after_context,
                         std::vector<std::pair<std::size_t, std::size_t>> &line_ranges) {
  line_ranges.clear();
  for (std::size_t i = 0; i < lines.size(); ++i) {
    if (std::regex_search(lines[i], re)) {
      std::size_t end = std::min(i + after_context, lines.size() - 1);
      line_ranges.push_back({i, end});
    }
  }
  line_ranges = merge_ranges(std::move(line_ranges));
}

/// Prints lines in the given ranges to out. If show_filename is non-empty,
/// each line is prefixed with "show_filename:".
void print_ranges(const std::vector<std::string> &lines,
                 const std::vector<std::pair<std::size_t, std::size_t>> &ranges,
                 const std::string &show_filename,
                 std::ostream &out) {
  for (const auto &[start, end] : ranges) {
    for (std::size_t i = start; i <= end; ++i) {
      if (!show_filename.empty())
        out << show_filename << ":";
      out << lines[i] << "\n";
    }
  }
}

/// Runs grep on one stream (file or stdin). Returns number of matching
/// lines (for exit code: 0 if > 0, else 1).
int grep_stream(std::istream &in, const std::regex &re,
                std::size_t after_context, const std::string &filename,
                std::ostream &out) {
  std::vector<std::string> lines = read_lines(in);
  std::vector<std::pair<std::size_t, std::size_t>> ranges;
  find_matching_ranges(lines, re, after_context, ranges);
  bool multiple = !filename.empty();
  print_ranges(lines, ranges, multiple ? filename : "", out);
  return ranges.empty() ? 0 : 1;
}

} // namespace

int GrepCommand::execute(const std::vector<std::string> &args,
                         std::istream &in, std::ostream &out,
                         std::ostream &err, const Environment &) {
  if (args.size() < 2) {
    err << "grep: missing pattern\n";
    return 2;
  }

  CLI::App app("grep");
  std::string pattern;
  std::vector<std::string> files;
  bool word_boundary = false;
  bool ignore_case = false;
  int after_context = 0;

  app.add_option("pattern", pattern, "Regular expression to search for")
      ->required();
  app.add_option("files", files, "Input files (stdin if none)")
      ->expected(-1);
  app.add_flag("-w,--word-regexp", word_boundary,
               "Match only whole words");
  app.add_flag("-i,--ignore-case", ignore_case,
               "Case-insensitive search");
  app.add_option("-A,--after-context", after_context,
                 "Print N lines after each match")
      ->default_val(0)
      ->check(CLI::NonNegativeNumber);

  // CLI11 expects argv[0] to be the program name; pass full args so first
  // positional is pattern, not the command name.
  std::vector<std::string> argv_str(args.begin(), args.end());
  std::vector<char *> argv_ptrs;
  argv_ptrs.reserve(argv_str.size());
  for (std::string &s : argv_str)
    argv_ptrs.push_back(&s[0]);

  try {
    app.parse(static_cast<int>(argv_ptrs.size()), argv_ptrs.data());
  } catch (const CLI::ParseError &e) {
    err << "grep: " << e.what() << "\n";
    return 2;
  }

  std::string regex_pattern = pattern;
  if (word_boundary)
    regex_pattern = "\\b(" + pattern + ")\\b";

  std::regex::flag_type flags = std::regex::ECMAScript;
  if (ignore_case)
    flags |= std::regex::icase;

  std::regex re;
  try {
    re.assign(regex_pattern, flags);
  } catch (const std::regex_error &e) {
    err << "grep: invalid regular expression: " << e.what() << "\n";
    return 2;
  }

  const std::size_t after_n =
      static_cast<std::size_t>(std::max(0, after_context));
  int had_match = 0;

  if (files.empty()) {
    had_match = grep_stream(in, re, after_n, "", out);
  } else {
    for (const std::string &path : files) {
      std::ifstream f(path);
      if (!f) {
        err << "grep: cannot open '" << path << "'\n";
        return 2;
      }
      int m = grep_stream(f, re, after_n, files.size() > 1 ? path : "", out);
      if (m)
        had_match = 1;
    }
  }

  return had_match ? 0 : 1;
}

} // namespace cli
