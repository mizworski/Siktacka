
#ifndef SIK_GAMECLIENT_H
#define SIK_GAMECLIENT_H

#include <string>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <cstring>
#include <sstream>
#include "../Messages/ClientMessage.h"
#include "../Network/UdpSocket.h"
#include "../Messages/ServerMessage.h"
#include "../Network/PollSocketsClient.h"

class GameClient {
public:
    GameClient(std::string &player_name, std::string &game_server_host, uint64_t game_server_port,
               std::string ui_server_host, uint64_t ui_server_port);

    void send_message_to_server(char turn_direction, uint32_t next_expected_event_no);

    void start();


private:
    void get_new_session_id();

    std::string player_name_;
    std::string game_server_host_;
    std::string ui_server_host_;

    uint64_t session_id_;
    PollSocketsClient sockets_;

    bool is_game_active_;
    uint32_t game_id_;
    int8_t left_arrow_down_;
    int8_t right_arrow_down_;
    int8_t last_dir_;
    std::vector<std::string> player_names_;

    uint32_t maxx_;
    uint32_t maxy_;
    uint32_t next_expected_event_no_;

    int64_t last_send_;
};


#endif //SIK_GAMECLIENT_H
