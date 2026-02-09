#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace cli {

/**
 * Stores and provides environment variables.
 * Used to pass environment to external processes.
 */
class Environment {
public:
    Environment();

    /**
     * Initializes from current process environment (e.g. getenv on each platform).
     */
    void init_from_current();

    /**
     * Gets value of variable name, or empty string if not set.
     */
    std::string get(const std::string& name) const;

    /**
     * Sets variable name to value (overwrites if exists).
     */
    void set(const std::string& name, const std::string& value);

    /**
     * Unsets variable if present.
     */
    void unset(const std::string& name);

    /**
     * Builds platform-specific environment block for external process.
     * Windows: pointer to "key=val\0key2=val2\0\0" block.
     * POSIX: vector of "key=val" strings for execve.
     */
    std::vector<std::string> to_env_vector() const;

private:
    std::unordered_map<std::string, std::string> vars_;
};

}  // namespace cli
