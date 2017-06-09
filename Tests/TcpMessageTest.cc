#include <iostream>
#include "../Network/TcpSocket.h"

int main() {
    TcpSocket socket;
    std::string hostname("localhost");
    std::string port("12346");

    socket.open(hostname, port);

    socket.send("test123");
    auto message(socket.receive());

    std::cout << message << std::endl;

    socket.disconnect();

    return 42;
}