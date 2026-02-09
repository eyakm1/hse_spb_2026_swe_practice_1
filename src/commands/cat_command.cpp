#include "cli/commands/cat_command.hpp"
#include <fstream>

namespace cli {

int CatCommand::execute(const std::vector<std::string>& args,
                        std::istream& /*in*/,
                        std::ostream& out,
                        std::ostream& err,
                        const Environment& /*env*/) {
    if (args.size() < 2) {
        err << "cat: missing file operand\n";
        return 1;
    }
    for (std::size_t i = 1; i < args.size(); ++i) {
        std::ifstream f(args[i], std::ios::binary);
        if (!f) {
            err << "cat: cannot open '" << args[i] << "'\n";
            return 1;
        }
        out << f.rdbuf();
        if (!f) {
            err << "cat: read error '" << args[i] << "'\n";
            return 1;
        }
    }
    return 0;
}

}  // namespace cli
