#include <doctest/doctest.h>
#include "cli/environment.hpp"
#include <algorithm>
#include <string>

using namespace cli;

TEST_CASE("Environment get returns empty string for unset variable") {
    Environment env;
    CHECK(env.get("MISSING_VAR") == "");
    CHECK(env.get("") == "");
}

TEST_CASE("Environment set and get") {
    Environment env;
    env.set("FOO", "bar");
    CHECK(env.get("FOO") == "bar");
    env.set("EMPTY", "");
    CHECK(env.get("EMPTY") == "");
}

TEST_CASE("Environment set overwrites existing value") {
    Environment env;
    env.set("X", "first");
    CHECK(env.get("X") == "first");
    env.set("X", "second");
    CHECK(env.get("X") == "second");
}

TEST_CASE("Environment unset removes variable") {
    Environment env;
    env.set("TO_REMOVE", "value");
    CHECK(env.get("TO_REMOVE") == "value");
    env.unset("TO_REMOVE");
    CHECK(env.get("TO_REMOVE") == "");
}

TEST_CASE("Environment unset on missing variable is safe") {
    Environment env;
    env.unset("NEVER_SET");
    CHECK(env.get("NEVER_SET") == "");
}

TEST_CASE("Environment to_env_vector format") {
    Environment env;
    env.set("A", "1");
    env.set("B", "2");
    auto vec = env.to_env_vector();  // cppcheck-suppress unreadVariable
    REQUIRE(vec.size() == 2);
    // Order not specified (unordered_map)
    CHECK(std::find(vec.begin(), vec.end(), "A=1") != vec.end());
    CHECK(std::find(vec.begin(), vec.end(), "B=2") != vec.end());
}

TEST_CASE("Environment to_env_vector empty when no vars") {
    Environment env;
    auto vec = env.to_env_vector();
    CHECK(vec.empty());
}

TEST_CASE("Environment init_from_current yields non-empty or empty") {
    Environment env;
    env.init_from_current();
    // At least PATH or similar often exists; if not, vector can be empty
    auto vec = env.to_env_vector();  // cppcheck-suppress unreadVariable
    // Just ensure it doesn't crash and get/set still work after
    env.set("CLI_TEST", "1");
    CHECK(env.get("CLI_TEST") == "1");
}
