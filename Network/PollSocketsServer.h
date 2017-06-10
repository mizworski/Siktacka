
#ifndef SIK_POLLSOCKETS_H
#define SIK_POLLSOCKETS_H

#include <cstdint>
#include <poll.h>
#include <vector>
#include <queue>
#include "UdpSocket.h"
#include "TcpSocket.h"
#include "../Messages/ClientMessage.h"
#include "../Messages/ServerMessage.h"

class PollSocketsServer {
public:
    PollSocketsServer(uint16_t sockets, uint16_t port, int64_t rounds_per_sec);

    ~PollSocketsServer();

    std::pair<bool, ClientMessage> poll_sockets();

    void add_message_to_queue(std::vector<NetworkAddress> &addresses, std::vector<ServerMessage> &messages);

    void add_message_to_queue(NetworkAddress &address, ServerMessage &message);

private:
    int64_t rounds_per_sec_;
    struct sockaddr_in server;
    struct pollfd *sockets_fds_;
    std::vector<UdpSocket> sockets_;
    std::queue<std::pair<NetworkAddress, std::string>> messages_to_send;
};


#endif //SIK_POLLSOCKETS_H
