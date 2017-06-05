#include <iostream>
#include "../ServerMessage.h"

int main() {
    std::cout << "Pixel: " << std::endl;
    Pixel px(10, 1, 1, 21, 37);

    std::string px_str = px.serialize();

    for (auto byte : px_str) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    Pixel px2(px_str);

    for (auto byte : px2.serialize()) {
        printf("%d#", byte);
    }
    std::cout << std::endl;
    for (auto byte : px2.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    std::cout << "New game: " << std::endl;

    NewGame ng(15, 33, 44, 14,88, {"Michal", "ABCD", "abcd"});
    for (auto byte : ng.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    NewGame ng2(ng.serialize());
    for (auto byte : ng2.get_message()) {
        printf("%d#", byte);
    }

    return 42;
}