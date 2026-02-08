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
    extern char** environ;
    for (char** p = environ; p && *p; ++p) {
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
