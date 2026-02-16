#include "cli/parser.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace cli {

namespace {

bool is_space(char c) { return std::isspace(static_cast<unsigned char>(c)); }

/** Weak quoting: append one character, handling backslash escapes. Returns next
 * index. */
std::size_t parse_weak_quoted_escape(const std::string &line, std::size_t i,
                                     std::string &token) {
  const std::size_t n = line.size();
  if (i >= n)
    return i;
  if (line[i] != '\\') {
    token += line[i];
    return i + 1;
  }
  ++i;
  if (i >= n) {
    token += '\\';
    return i;
  }
  switch (line[i]) {
  case '\'':
    token += '\'';
    return i + 1;
  case '\\':
    token += '\\';
    return i + 1;
  case 'n':
    token += '\n';
    return i + 1;
  case 't':
    token += '\t';
    return i + 1;
  case 'r':
    token += '\r';
    return i + 1;
  default:
    token += '\\';
    token += line[i];
    return i + 1;
  }
}

/** Split line by |, respecting single and double quotes. */
std::vector<std::string> split_by_pipe(const std::string &line) {
  std::vector<std::string> segments;
  std::string seg;
  bool in_single = false;
  bool in_double = false;
  const std::size_t n = line.size();
  for (std::size_t i = 0; i < n; ++i) {
    char c = line[i];
    if (in_single) {
      if (c == '\'') {
        in_single = false;
        seg += c;
      } else if (c == '\\') {
        if (i + 1 < n) {
          seg += c;
          seg += line[i + 1];
          ++i;
        } else {
          seg += c;
        }
      } else {
        seg += c;
      }
      continue;
    }
    if (in_double) {
      if (c == '"') {
        in_double = false;
        seg += c;
      } else {
        seg += c;
      }
      continue;
    }
    if (c == '\'') {
      in_single = true;
      seg += c;
    } else if (c == '"') {
      in_double = true;
      seg += c;
    } else if (c == '|') {
      segments.push_back(std::move(seg));
      seg.clear();
    } else {
      seg += c;
    }
  }
  segments.push_back(std::move(seg));
  return segments;
}

/** Tokenize one pipeline segment; fill tokens and substitute flags (Single=No,
 * Double/Unquoted=Yes). */
void tokenize_segment(const std::string &segment,
                      std::vector<std::string> &tokens,
                      std::vector<Substitute> &substitute_flags) {
  tokens.clear();
  substitute_flags.clear();
  std::string token;
  const std::size_t n = segment.size();
  std::size_t i = 0;

  auto flush_token = [&](Substitute sub) {
    if (!token.empty()) {
      tokens.push_back(std::move(token));
      substitute_flags.push_back(sub);
      token.clear();
    }
  };

  while (i < n) {
    while (i < n && is_space(segment[i]))
      ++i;
    if (i >= n)
      break;

    if (segment[i] == '\'') {
      flush_token(Substitute::Yes); // flush any previous
      ++i;
      Substitute seg_sub = Substitute::No; // single-quoted: no substitution
      while (i < n && segment[i] != '\'') {
        i = parse_weak_quoted_escape(segment, i, token);
      }
      if (i < n)
        ++i;
      tokens.push_back(std::move(token));
      substitute_flags.push_back(seg_sub);
      token.clear();
      continue;
    }
    if (segment[i] == '"') {
      flush_token(Substitute::Yes);
      ++i;
      Substitute seg_sub = Substitute::Yes; // double-quoted: substitution
      while (i < n && segment[i] != '"') {
        token += segment[i];
        ++i;
      }
      if (i < n)
        ++i;
      tokens.push_back(std::move(token));
      substitute_flags.push_back(seg_sub);
      token.clear();
      continue;
    }

    Substitute seg_sub = Substitute::Yes; // unquoted
    while (i < n && !is_space(segment[i]) && segment[i] != '\'' &&
           segment[i] != '"') {
      token += segment[i];
      ++i;
    }
    tokens.push_back(std::move(token));
    substitute_flags.push_back(seg_sub);
    token.clear();
  }
  flush_token(Substitute::Yes);
}

bool segment_empty_or_whitespace(const std::string &s) {
  return std::all_of(
      s.begin(), s.end(),
      [](char c) { return is_space(static_cast<unsigned char>(c)); });
}

} // namespace

std::optional<Pipeline> Parser::parse(const std::string &line) {
  std::vector<std::string> segments = split_by_pipe(line);
  Pipeline pipeline;
  pipeline.reserve(segments.size());

  std::vector<std::string> tokens;
  std::vector<Substitute> substitute_flags;

  for (const auto &seg : segments) {
    if (segment_empty_or_whitespace(seg)) {
      CommandNode empty_node;
      empty_node.name = "";
      empty_node.args = {};
      empty_node.substitute_name = Substitute::Yes;
      empty_node.substitute_arg = {};
      pipeline.push_back(std::move(empty_node));
      continue;
    }
    tokenize_segment(seg, tokens, substitute_flags);
    if (tokens.empty()) {
      CommandNode empty_node;
      empty_node.name = "";
      empty_node.args = {};
      empty_node.substitute_name = Substitute::Yes;
      empty_node.substitute_arg = {};
      pipeline.push_back(std::move(empty_node));
      continue;
    }
    CommandNode node;
    node.name = std::move(tokens[0]);
    node.substitute_name =
        substitute_flags.empty() ? Substitute::Yes : substitute_flags[0];
    for (std::size_t j = 1; j < tokens.size(); ++j) {
      node.args.push_back(std::move(tokens[j]));
      node.substitute_arg.push_back(
          j < substitute_flags.size() ? substitute_flags[j] : Substitute::Yes);
    }
    pipeline.push_back(std::move(node));
  }

  if (pipeline.empty())
    return std::nullopt;

  bool all_empty =
      std::all_of(pipeline.begin(), pipeline.end(),
                  [](const CommandNode &node) {
                    return node.name.empty() && node.args.empty();
                  });
  if (all_empty && pipeline.size() == 1)
    return std::nullopt;

  return pipeline;
}

} // namespace cli
