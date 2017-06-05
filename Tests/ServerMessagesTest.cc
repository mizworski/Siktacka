#include <iostream>
#include "../ServerMessage.h"

void test_px(Pixel &e) {
    std::string e_str(e.get_message());

    for (unsigned char byte : e_str) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    Pixel e2(e_str);

    std::string e2_str(e2.get_message());

    for (unsigned char byte : e2_str) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

}

void test_ng(NewGame &ng) {
    std::string s1 = ng.get_message();
    for (unsigned char byte : s1) {
        printf("%d#", byte);
    }

    std::cout << std::endl;


    NewGame ng2(s1);
    auto s2(ng2.get_message());
    for (unsigned char byte : s2) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
}

void test_pe(PlayerEliminated &pe) {
    for (unsigned char byte : pe.get_message()) {
        printf("%d#", byte);
    }
    std::cout << std::endl;
    std::string s1 = pe.get_message();
    PlayerEliminated pe2(s1);
    for (unsigned char byte : pe2.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
}

void test_go(GameOver &go) {
    for (unsigned char byte : go.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    std::string s1 = go.get_message();
    GameOver go2(s1);
    for (unsigned char byte : go2.get_message()) {
        printf("%d#", byte);
    }

    std::cout << std::endl;
}

int main() {
    std::cout << "Pixel: " << std::endl;
    Pixel px(17, 1, 1, 21, 37);
    test_px(px);

    std::cout << "New game: " << std::endl;
    std::vector<std::string> players({"Michal", "ABCD", "abcd"});
    NewGame ng(34, 33, 44, 14, 88, players);
    test_ng(ng);

    std::cout << "Player eliminated" << std::endl;
    PlayerEliminated pe(10, 33, 55, 112);
    test_pe(pe);

    std::cout << "Game over" << std::endl;
    GameOver go(9, 37, 14);
    test_go(go);


    return 42;
}