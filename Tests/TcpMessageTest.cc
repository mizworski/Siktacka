#include <iostream>
#include "../Network/TcpSocket.h"
#include "../Network/PollSocketsClient.h"

void test1();

void test2();

int main() {
    test1();

    test2();

    std::vector<std::string> strs;
    strs.push_back("dupa");
    strs.push_back("dupa2");
    NewGame ng(1, 2, 3, strs);
    Pixel px(1, 2, 3, 4);
    PlayerEliminated pe(1, 2);
    GameOver go(2);

    auto m1 = ng.get_string();
    auto m2 = px.get_string();
    auto m3 = pe.get_string();
    auto m4 = go.get_string();

    std::cout << m1 << std::endl;
    std::cout << m2 << std::endl;
    std::cout << m3 << std::endl;
    std::cout << m4 << std::endl;

    return 42;
}

void test2() {
    std::__cxx11::string hostname("localhost");
    uint16_t port_server = 12345;
    uint16_t port_gui = 12346;
    PollSocketsClient sockets(hostname, port_server, hostname, port_gui);
    sockets.send_messages_gui("dupa");
    sockets.send_messages_gui("dupa2");
    ClientMessage cm(2, 1, 0, "micha");
    sockets.send_messages_server(cm);

    ServerDatagram datagram;
    std::__cxx11::string tcp_message;
    for (int i = 0; i < 6; ++i) {
        auto res = sockets.poll_sockets(datagram, tcp_message);
        std::cout << res.first << " " << res.second << std::endl;
        auto events = datagram.get_events();

        for (auto & el : events) {
            std::cout << el->get_string() << std::endl;
            break;
        }
    }
}

void test1() {
    TcpSocket socket;
    std::string hostname("localhost");
    std::string port("12346");

    socket.open(hostname, port);

    socket.send("test123");
    auto message(socket.receive());

    std::cout << message << std::endl;

    socket.disconnect();
}