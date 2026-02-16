#include "cli/environment.hpp"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <cstdlib>
extern char** environ;
#endif

namespace cli {

Environment::Environment() = default;

void Environment::init_from_current() {
#ifdef _WIN32
    LPSTR env = GetEnvironmentStringsA();
    if (!env)
        return;
    for (LPSTR p = env; *p; ) {
        std::string line;
        while (*p)
            line += *p++;
        ++p;
        std::size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string key = line.substr(0, eq);
            std::string val = line.substr(eq + 1);
            vars_[key] = val;
        }
    }
    FreeEnvironmentStringsA(env);
#else
    for (char** p = ::environ; p && *p; ++p) {
        std::string line(*p);
        std::size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string key = line.substr(0, eq);
            std::string val = line.substr(eq + 1);
            vars_[key] = val;
        }
    }
#endif
}

std::string Environment::get(const std::string& name) const {
    auto it = vars_.find(name);
    if (it == vars_.end())
        return "";
    return it->second;
}

namespace {

bool is_var_char(char c, bool first) {
    if (first)
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

}  // namespace

std::string Environment::substitute(const std::string& s) const {
    std::string out;
    out.reserve(s.size());
    const std::size_t n = s.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (s[i] != '$') {
            out += s[i];
            continue;
        }
        if (i + 1 >= n) {
            out += '$';
            continue;
        }
        if (s[i + 1] == '$') {
            out += '$';
            ++i;
            continue;
        }
        if (s[i + 1] == '{') {
            std::size_t j = i + 2;
            while (j < n && s[j] != '}')
                ++j;
            std::string name = s.substr(i + 2, j - (i + 2));
            if (j < n) {
                out += get(name);
                i = j;
            } else {
                out += s.substr(i);
                break;
            }
            continue;
        }
        if (is_var_char(s[i + 1], true)) {
            std::size_t j = i + 1;
            while (j < n && is_var_char(s[j], j == i + 1))
                ++j;
            std::string name = s.substr(i + 1, j - (i + 1));
            out += get(name);
            i = j - 1;
            continue;
        }
        out += '$';
    }
    return out;
}

void Environment::set(const std::string& name, const std::string& value) {
    vars_[name] = value;
}

void Environment::unset(const std::string& name) {
    vars_.erase(name);
}

std::vector<std::string> Environment::to_env_vector() const {
    std::vector<std::string> out;
    out.reserve(vars_.size());
    for (const auto& [k, v] : vars_)
        out.push_back(k + "=" + v);
    return out;
}

}  // namespace cli
