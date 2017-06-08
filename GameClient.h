
#ifndef SIK_GAMECLIENT_H
#define SIK_GAMECLIENT_H

#include <string>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <cstring>
#include <sstream>
#include "ClientMessage.h"
#include "UdpSocket.h"

class GameClient {
public:
    GameClient(std::string &player_name, std::string &game_server_host, uint64_t game_server_port,
               std::string ui_server_host, uint64_t ui_server_port);

    void send_message_to_server(char turn_direction, uint32_t next_expected_event_no) {
        ClientMessage cm(session_id_, turn_direction, next_expected_event_no, player_name_);

        std::string message(cm.serialize());
        cm.print_msg();
        game_socket_.send(message, game_server_address_);

    }

    void receive_message_from_server() {
        std::string message(game_socket_.recv());

        std::cout << message << std::endl;

//        msg = deserialize_message(raw_msg, rcv_len, msg);

    }

private:
    void get_new_session_id();

    std::string player_name_;
    std::string game_server_host_;
    std::string ui_server_host_;
    uint64_t game_server_port_;
    uint64_t ui_server_port_;

    uint64_t session_id_;
//    std::vector<unsigned char> session_id_bytes_;

    NetworkAddress game_server_address_;
    UdpSocket game_socket_;

    struct addrinfo addr_hints_ui_;
    struct addrinfo *addr_result_ui_;
    int32_t sock_ui_;
};


#endif //SIK_GAMECLIENT_H
