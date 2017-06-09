#include <iostream>
#include "../Messages/ClientMessage.h"

int main() {
    ClientMessage cm(10, -5, 14, "abcd");

    std::string s(cm.serialize());

    for (unsigned char byte : s) {
        printf("%d#", byte);
    }

    std::cout << std::endl;

    return 42;
}