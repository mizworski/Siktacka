
#ifndef SIK_POLLSOCKETS_H
#define SIK_POLLSOCKETS_H


#include <cstdint>
#include <poll.h>
#include <vector>
#include <queue>
#include "UdpSocket.h"
#include "ClientMessage.h"
#include "ServerMessage.h"

const uint16_t MAX_UDP_PACKET_SIZE = 512;

class PollSockets {
public:
    PollSockets(uint16_t sockets, uint16_t port) : sockets_(sockets) {
        sockets_fds_ = (pollfd *) calloc(sockets, sizeof(struct pollfd));

        for (uint16_t i = 0; i < sockets; ++i) {
            sockets_fds_[i].events = POLLIN | POLLOUT;
            sockets_fds_[i].revents = 0;
        }

        for (uint16_t i = 0; i < sockets; ++i) {
            sockets_[i].open();
            sockets_fds_->fd = sockets_[i].get_descriptor();
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(port);
        if (bind(sockets_fds_[0].fd, (struct sockaddr *) &server, (socklen_t) sizeof(server)) < 0) {
            throw std::runtime_error("Binding stream socket");
        }
    }

    ~PollSockets() {
        free(sockets_fds_);
    }

    std::pair<bool, ClientMessage> poll_sockets() {
        int32_t ret = poll(sockets_fds_, 1, 500);
        std::pair<bool, ClientMessage> res;
        res.first = false;
        if (ret < 0) {
            throw std::runtime_error("Error while polling");
        } else if (ret > 0) {
            if (sockets_fds_[0].revents & POLLIN) {
                char raw_msg[MAX_UDP_PACKET_SIZE];
                struct sockaddr_in client_address;
                socklen_t rcva_len = (socklen_t) sizeof(client_address);
                ssize_t len = recvfrom(sockets_fds_[0].fd, &raw_msg, MAX_UDP_PACKET_SIZE, 0,
                                       (struct sockaddr *) &client_address, &rcva_len);
                if (len < 0) {
                    throw std::runtime_error("Error while reading.");
                }
                std::string message(raw_msg, (size_t) len);
                NetworkAddress sender(client_address);
                res.first = true;
                try {
                    res.second = ClientMessage(message);
                    res.second.set_sender(sender);
                } catch (std::runtime_error e) {
                    std::cerr << e.what() << std::endl;
                    res.first = false;
                }
//                res.second.print_msg();
            }

            if ((sockets_fds_[0].revents & POLLOUT) && !messages_to_send.empty()) {
                auto &message = messages_to_send.front();
                sockets_[0].send(message.second, message.first);
                messages_to_send.pop();
            }
            return res;

        } else {
            std::cerr << "dupa\n";
            return res;
        }
    }

    void add_message_to_queue(std::vector<NetworkAddress> &addresses, std::vector<ServerMessage> &messages) {
        for (auto &message : messages) {
            std::vector<ServerDatagram> datagrams(message.get_datagrams());
            for (auto &datagram : datagrams) {
                for (auto &address : addresses) {
                    messages_to_send.push({address, datagram.serialize()});
                }
            }
        }
    }

    void add_message_to_queue(NetworkAddress &address, std::vector<ServerMessage> &messages) {
        std::vector<NetworkAddress> addresses;
        addresses.push_back(address);
        add_message_to_queue(addresses, messages);
    }

    void add_message_to_queue(std::vector<NetworkAddress> &addresses, ServerMessage &message) {
        std::vector<ServerMessage> messages;
        messages.push_back(message);
        add_message_to_queue(addresses, messages);
    }

    void add_message_to_queue(NetworkAddress &address, ServerMessage &message) {
        std::vector<NetworkAddress> addresses;
        addresses.push_back(address);
        std::vector<ServerMessage> messages;
        messages.push_back(message);
        add_message_to_queue(addresses, messages);
    }

private:
    struct sockaddr_in server;
    struct pollfd *sockets_fds_;
    std::vector<UdpSocket> sockets_;
    std::queue<std::pair<NetworkAddress, std::string>> messages_to_send;
};


#endif //SIK_POLLSOCKETS_H
