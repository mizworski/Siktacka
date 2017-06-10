
#ifndef SIK_POLLSOCKETSCLIENT_H
#define SIK_POLLSOCKETSCLIENT_H


#include <cstdint>
#include <poll.h>
#include <vector>
#include <queue>
#include "TcpSocket.h"
#include "UdpSocket.h"
#include "../Messages/ClientMessage.h"
#include "../Messages/ServerMessage.h"

const uint16_t TIMEOUT_LIMIT = 20;

class PollSocketsClient {
public:
    PollSocketsClient(std::string server_hostname,
                      uint16_t server_port,
                      std::string gui_hostname,
                      uint16_t gui_port);

    ~PollSocketsClient();

    std::pair<bool, bool> poll_sockets(ServerDatagram &datagram, std::string &message_gui);

    void send_messages_server(std::vector<ClientMessage> &messages);

    void send_messages_server(ClientMessage &message);

    void send_messages_gui(std::string message);

    void drop_messages();


private:
    struct pollfd *sockets_fds_;
    NetworkAddress server_address_;
    UdpSocket server_socket_;
    TcpSocket gui_socket_;
    std::queue<std::string> messages_server_;
    std::queue<std::string> messages_gui_;
};


#endif //SIK_POLLSOCKETSCLIENT_H
