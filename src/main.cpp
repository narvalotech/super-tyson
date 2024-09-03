#include <iostream>

auto main() -> int {
    std::string input;
    std::getline(std::cin, input);

    std::cout << "Read: " << input << std::endl;

    return 0;
}
