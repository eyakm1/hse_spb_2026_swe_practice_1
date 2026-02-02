#include <doctest/doctest.h>
#include "cli/foo.hpp"

TEST_CASE("greet formats correctly") {
    CHECK(cli::greet("eyakm1") == "Hello, eyakm1!");
}

TEST_CASE("greet handles empty") {
    CHECK(cli::greet("") == "Hello, world!");
}
