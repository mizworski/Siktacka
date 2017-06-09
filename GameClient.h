
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
#include "ServerMessage.h"

class GameClient {
public:
    GameClient(std::string &player_name, std::string &game_server_host, uint64_t game_server_port,
               std::string ui_server_host, uint64_t ui_server_port);

    void send_message_to_server(char turn_direction, uint32_t next_expected_event_no) {
        ClientMessage cm(session_id_, turn_direction, next_expected_event_no, player_name_);

        std::string message(cm.serialize());
        game_socket_.send(message, game_server_address_);

    }

    void receive_message_from_server() {
        std::string message(game_socket_.recv());

        ServerDatagram datagram(message);

        std::cout << "game_id=" << datagram.get_game_id() << std::endl;
        auto events = datagram.get_events();
        for (auto &event : events) {
            int8_t type = event->get_type();
//            std::cout << "event_type=" << +type << std::endl;
            std::shared_ptr<NewGame> ng_ptr = std::dynamic_pointer_cast<NewGame>(event);
            if (ng_ptr) {
                NewGame ng(*ng_ptr.get());
                std::cout << "x=" << ng.get_maxx() << " y=" << ng.get_maxy() << std::endl;;

                auto players = ng.get_players();
                for (auto &player : players) {
                    std::cout << "player=" << player << std::endl;
                }
            }
            std::shared_ptr<Pixel> pixel = std::dynamic_pointer_cast<Pixel>(event);
            if (pixel) {
                std::cout << "player=" << +pixel->get_player_number()<< " x="
                          << pixel->get_x() <<" y=" << pixel->get_y() << std::endl;;

            }
            std::shared_ptr<PlayerEliminated> pe = std::dynamic_pointer_cast<PlayerEliminated>(event);
            if (pe) {
                std::cout << "player_eliminated=" << +pe->get_player_number() << std::endl;

            }
            std::shared_ptr<GameOver> go = std::dynamic_pointer_cast<GameOver>(event);
            if (go) {
                std::cout << "game_over" << std::endl;

            }
        }
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
