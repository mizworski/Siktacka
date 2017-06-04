
#ifndef SIK_GAMECLIENT_H
#define SIK_GAMECLIENT_H

#include <string>
//#include <stdint-gcc.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <cstring>
#include <sstream>

#define TCP 21
#define UDP 37

const int32_t FAILURE = 1;
const int32_t MAX_DATAGRAM_SIZE = 512;

class GameClient {
public:
    GameClient(std::string &player_name, std::string &game_server_host, uint64_t game_server_port,
               std::string ui_server_host, uint64_t ui_server_port);

    void send_message_to_server(char turn_direction, uint32_t next_expected_event_no) {
        std::string message = serialize_message(turn_direction, next_expected_event_no);
        size_t msg_len = message.size();

        socklen_t rcv_address_len = (socklen_t) sizeof(my_address);
        ssize_t snd_len = sendto(game_sock_, message.c_str(), msg_len, 0, (struct sockaddr *) &my_address,
                                 rcv_address_len);

        if (snd_len != (ssize_t) msg_len) {
            std::cerr << "Failed to send message." << std::endl;
            return;
        }

        std::cout << message << std::endl;
    }

    void receive_message_from_server() {
        struct sockaddr_in server_address;
        char raw_msg[MAX_DATAGRAM_SIZE];

        size_t rcv_msg_len = MAX_DATAGRAM_SIZE;
        socklen_t rcv_address_len = (socklen_t) sizeof(server_address);
        ssize_t rcv_len = recvfrom(game_sock_, &raw_msg, rcv_msg_len, 0,
                                   (struct sockaddr *) &server_address, &rcv_address_len);

        if (rcv_len < 0) {
            std::cerr << "read." << std::endl;
            return;
        }

//        msg = deserialize_message(raw_msg, rcv_len, msg);

    }

private:
    void get_new_session_id();

    void init_addr_hints(addrinfo &addr_hints, int32_t type);

    void initialize_connection(addrinfo &addr_hints, addrinfo *&addr_result, const std::string &host_str,
                               int64_t port, sockaddr_in &my_address, int32_t &sock);

    std::string serialize_message(char turn_direction, uint32_t next_expected_event_no);

    std::string player_name_;
    std::string game_server_host_;
    std::string ui_server_host_;
    uint64_t game_server_port_;
    uint64_t ui_server_port_;

    uint64_t session_id_;
//    std::vector<unsigned char> session_id_bytes_;

    struct sockaddr_in my_address;

    struct addrinfo addr_hints_game_;
    struct addrinfo *addr_result_game_;
    int32_t game_sock_;

    struct addrinfo addr_hints_ui_;
    struct addrinfo *addr_result_ui_;
    int32_t sock_ui_;
};


#endif //SIK_GAMECLIENT_H
