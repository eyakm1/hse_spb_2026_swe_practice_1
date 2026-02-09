#include "cli/executor.hpp"

namespace cli {

Executor::Executor(CommandRegistry& registry) : registry_(registry) {}

ExecutorResult Executor::execute(const CommandNode& node,
                                 std::istream& in,
                                 std::ostream& out,
                                 std::ostream& err,
                                 const Environment& env) {
    std::vector<std::string> args;
    args.push_back(node.name);
    std::copy(node.args.begin(), node.args.end(), std::back_inserter(args));

    Command* cmd = registry_.find(node.name);
    if (cmd) {
        const int exit_sentinel = -1;
        int code = cmd->execute(args, in, out, err, env);
        if (code == exit_sentinel)
            return ExecutorResult{true, 0};
        return ExecutorResult{false, code};
    }

    int code = external_.execute(args, in, out, err, env);
    return ExecutorResult{false, code};
}

}  // namespace cli
