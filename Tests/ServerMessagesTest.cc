#include <iostream>
#include "../ServerMessage.h"

void test_px(Pixel e) {
    std::string e_str = e.serialize();

    for (auto byte : e_str) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    Pixel e2(e_str);

    for (auto byte : e2.serialize()) {
        printf("%d#", byte);
    }
    std::cout << std::endl;
    for (unsigned char byte : e2.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
}

void test_ng(NewGame ng) {
    for (unsigned char byte : ng.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    NewGame ng2(ng.get_message());
    for (auto byte : ng2.serialize()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
    for (unsigned char  byte : ng2.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
}

void test_pe(PlayerEliminated pe) {
    for (auto byte : pe.serialize()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    PlayerEliminated pe2(pe.serialize());
    for (auto byte : pe2.serialize()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
    for (unsigned char  byte : pe2.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
}

void test_go(GameOver go) {
    for (auto byte : go.serialize()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    GameOver go2(go.serialize());
    for (auto byte : go2.serialize()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
    for (unsigned char  byte : go2.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
}

int main() {
    std::cout << "Pixel: " << std::endl;
    Pixel px(17, 1, 1, 21, 37);
    test_px(px);

    std::cout << "New game: " << std::endl;
    NewGame ng(34, 33, 44, 14,88, {"Michal", "ABCD", "abcd"});
    test_ng(ng);

    std::cout << "Player eliminated" << std::endl;
    PlayerEliminated pe(10, 33, 55, 112);
    test_pe(pe);

    std::cout << "Game over" << std::endl;
    GameOver go(9,37,14);
    test_go(go);


    return 42;
}