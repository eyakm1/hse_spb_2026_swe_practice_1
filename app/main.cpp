#include "cli/foo.hpp"
#include <iostream>


int main() {
    std::string name;
    std::cin >> name;
    std::cout << cli::greet(name) << std::endl;
    return 0;
}