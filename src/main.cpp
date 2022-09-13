#include <iostream>
#include "defines.h"

int main() {
    if constexpr (os() == Os::windows) {
        std::cout << "Hello windows!" << std::endl;
    } else {
        std::cout << "Hello linux!" << std::endl;
    }
}