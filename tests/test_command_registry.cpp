#include <doctest/doctest.h>
#include "cli/command_registry.hpp"
#include "cli/command.hpp"
#include "cli/environment.hpp"
#include <memory>

using namespace cli;

namespace {

class DummyCommand : public Command {
public:
    int execute(const std::vector<std::string>& /*args*/,
                std::istream& /*in*/,
                std::ostream& out,
                std::ostream& /*err*/,
                const Environment& /*env*/) override {
        out << "dummy";
        return 0;
    }
};

}  // namespace

TEST_CASE("CommandRegistry has returns false for unregistered name") {
    CommandRegistry reg;
    CHECK(reg.has("nosuch") == false);
    CHECK(reg.find("nosuch") == nullptr);
}

TEST_CASE("CommandRegistry register and find") {
    CommandRegistry reg;
    reg.register_command("dummy", std::make_unique<DummyCommand>());
    CHECK(reg.has("dummy") == true);
    Command* cmd = reg.find("dummy");
    REQUIRE(cmd != nullptr);
}

TEST_CASE("CommandRegistry find returns nullptr for unknown") {
    CommandRegistry reg;
    reg.register_command("a", std::make_unique<DummyCommand>());
    CHECK(reg.find("b") == nullptr);
    CHECK(reg.has("b") == false);
}

TEST_CASE("CommandRegistry register overwrites existing") {
    CommandRegistry reg;
    reg.register_command("x", std::make_unique<DummyCommand>());
    reg.register_command("x", std::make_unique<DummyCommand>());
    CHECK(reg.has("x") == true);
    CHECK(reg.find("x") != nullptr);
}

TEST_CASE("CommandRegistry register with nullptr does not store") {
    CommandRegistry reg;
    reg.register_command("empty", nullptr);
    CHECK(reg.has("empty") == false);
    CHECK(reg.find("empty") == nullptr);
}

TEST_CASE("CommandRegistry multiple commands") {
    CommandRegistry reg;
    reg.register_command("cmd1", std::make_unique<DummyCommand>());
    reg.register_command("cmd2", std::make_unique<DummyCommand>());
    CHECK(reg.has("cmd1"));
    CHECK(reg.has("cmd2"));
    CHECK(reg.find("cmd1") != reg.find("cmd2"));
}
