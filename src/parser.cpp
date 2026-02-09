#include "cli/parser.hpp"
#include <cctype>
#include <sstream>

namespace cli {

namespace {

bool is_space(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

/** Weak quoting: append one character, handling backslash escapes. Returns next index. */
std::size_t parse_weak_quoted_escape(const std::string& line, std::size_t i, std::string& token) {
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
        case '\'': token += '\''; return i + 1;
        case '\\': token += '\\'; return i + 1;
        case 'n':  token += '\n'; return i + 1;
        case 't':  token += '\t'; return i + 1;
        case 'r':  token += '\r'; return i + 1;
        default:   token += '\\'; token += line[i]; return i + 1;
    }
}

}  // namespace

std::optional<CommandNode> Parser::parse(const std::string& line) const {
    std::vector<std::string> tokens;
    std::string token;
    const std::size_t n = line.size();
    std::size_t i = 0;

    auto flush_token = [&] {
        if (!token.empty()) {
            tokens.push_back(std::move(token));
            token.clear();
        }
    };

    while (i < n) {
        while (i < n && is_space(line[i]))
            ++i;
        if (i >= n)
            break;

        // Weak quoting (single quote): backslash escapes like \', \\, \n, \t.
        if (line[i] == '\'') {
            flush_token();
            ++i;
            while (i < n && line[i] != '\'') {
                i = parse_weak_quoted_escape(line, i, token);
            }
            if (i < n)
                ++i;  // consume closing '
            tokens.push_back(std::move(token));
            token.clear();
            continue;
        }
        // Full quoting (double quote): no escapes, every character literal.
        if (line[i] == '"') {
            flush_token();
            ++i;
            while (i < n && line[i] != '"') {
                token += line[i];
                ++i;
            }
            if (i < n)
                ++i;  // consume closing "
            tokens.push_back(std::move(token));
            token.clear();
            continue;
        }

        while (i < n && !is_space(line[i]) && line[i] != '\'' && line[i] != '"') {
            token += line[i];
            ++i;
        }
        flush_token();
    }
    flush_token();

    if (tokens.empty())
        return std::nullopt;

    CommandNode node;
    node.name = std::move(tokens[0]);
    node.args.reserve(tokens.size() - 1);
    for (std::size_t j = 1; j < tokens.size(); ++j)
        node.args.push_back(std::move(tokens[j]));
    return node;
}

}  // namespace cli
