#include "cli/executor.hpp"

namespace cli {

Executor::Executor(CommandRegistry& registry) : registry_(registry) {}

void Executor::expand_node(const CommandNode& node, const Environment& env,
                           std::vector<std::string>& args_out) const {
    args_out.clear();
    std::string name = (node.substitute_name == Substitute::Yes)
                       ? env.substitute(node.name) : node.name;
    args_out.push_back(std::move(name));
    for (std::size_t i = 0; i < node.args.size(); ++i) {
        bool sub = (i < node.substitute_arg.size())
                   ? (node.substitute_arg[i] == Substitute::Yes)
                   : true;
        std::string arg = sub ? env.substitute(node.args[i]) : node.args[i];
        args_out.push_back(std::move(arg));
    }
}

ExecutorResult Executor::execute_one(const std::vector<std::string>& args,
                                     std::istream& in,
                                     std::ostream& out,
                                     std::ostream& err,
                                     const Environment& env) {
    if (args.empty()) {
        err << "cli: empty command\n";
        return ExecutorResult{false, 127};
    }
    const std::string& name = args[0];
    Command* cmd = registry_.find(name);
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

ExecutorResult Executor::execute(const Pipeline& pipeline,
                                 std::istream& in,
                                 std::ostream& out,
                                 std::ostream& err,
                                 const Environment& env) {
    if (pipeline.empty()) {
        return ExecutorResult{false, 0};
    }
    if (pipeline.size() == 1) {
        std::vector<std::string> args;
        expand_node(pipeline[0], env, args);
        if (args.empty() || args[0].empty()) {
            err << "cli: command not found\n";
            return ExecutorResult{false, 127};
        }
        return execute_one(args, in, out, err, env);
    }

    std::vector<std::vector<std::string>> expanded;
    expanded.reserve(pipeline.size());
    for (const auto& node : pipeline) {
        std::vector<std::string> args;
        expand_node(node, env, args);
        if (args.empty() || args[0].empty()) {
            err << "cli: empty command in pipeline\n";
            return ExecutorResult{false, 127};
        }
        expanded.push_back(std::move(args));
    }

    std::istream* current_in = &in;
    std::stringstream pipe_read;
    std::stringstream pipe_write;
    int last_code = 0;

    for (std::size_t i = 0; i < expanded.size(); ++i) {
        const bool is_last = (i == expanded.size() - 1);
        std::ostream* current_out = is_last ? &out : &pipe_write;
        ExecutorResult result = execute_one(expanded[i], *current_in, *current_out, err, env);
        if (result.should_exit)
            return result;
        last_code = result.exit_code;
        if (!is_last) {
            pipe_write.seekg(0);
            pipe_read.str(pipe_write.str());
            pipe_read.clear();
            pipe_write.str("");
            pipe_write.clear();
            current_in = &pipe_read;
        }
    }
    return ExecutorResult{false, last_code};
}

}  // namespace cli
