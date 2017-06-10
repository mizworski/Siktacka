#include <iostream>
#include "../Network/TcpSocket.h"
#include "../Network/PollSocketsClient.h"

void test1();

int main() {
//    test1();

    std::string hostname("localhost");
//    std::string port_server("12345");
//    std::string port_gui("12346");
    uint16_t port_server = 12345;
    uint16_t port_gui = 12346;
    PollSocketsClient sockets(hostname, port_server, hostname, port_gui);
    sockets.send_messages_gui("dupa");
    sockets.send_messages_gui("dupa");
    ClientMessage cm(2, 1, 0, "micha");
    sockets.send_messages_server(cm);

    ServerDatagram datagram;
    std::string tcp_message;
    for (int i = 0; i < 6; ++i) {
        auto res = sockets.poll_sockets(datagram, tcp_message);
        std::cout << res.first << " " << res.second << std::endl;
        auto events = datagram.get_events();

        for (auto & el : events) {
            std::cout << el->get_string() << std::endl;
            break;
        }
    }
    return 42;
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