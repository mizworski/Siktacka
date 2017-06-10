
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

    void send_message_to_server(char turn_direction, uint32_t next_expected_event_no) {
        ClientMessage message(session_id_, turn_direction, next_expected_event_no, player_name_);

        sockets_.send_messages_server(message);

    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    void start() {;
        ServerDatagram datagram_from_server;
        std::string message_from_gui;

        while (true) {
//            sleep(1);
//            send_message_to_server(1, 0);
            auto response = sockets_.poll_sockets(datagram_from_server, message_from_gui);
            if (response.second) {
                //todo process gui
            }

            if (response.first) {

                std::cout << "Odebralem jedna wiadomosc" <<std::endl;
                //todo process server
                uint32_t received_game_id = datagram_from_server.get_game_id();

                if (!is_game_active_ || received_game_id == game_id_) {
                    auto events = datagram_from_server.get_events();
                    for (auto &event : events) {
                        int8_t type = event->get_type();

                        if (!is_game_active_ && type == 0) {
                            std::shared_ptr<NewGame> ng_ptr = std::dynamic_pointer_cast<NewGame>(event);
                            if (ng_ptr) {
                                game_id_ = received_game_id;
                                is_game_active_ = true;
                                maxx_ = ng_ptr->get_maxy();
                                maxy_ = ng_ptr->get_maxy();
                                player_names_ = ng_ptr->get_players();

                                for (auto &player : player_names_) {
                                    std::cout << player << std::endl;
                                }
                            }

                        } else if (is_game_active_) {
                            switch (type) {
                                case 1:

                                    break;
                                case 2:
                                    break;
                                case 3:
                                    is_game_active_ = false;
                                    player_names_.clear();
                                    break;
                                default:
                                    break;
                            }
                        }
                    }

                }

            }

        }
        // todo poll sockets
        // todo send action to server
        // todo wait for new game
        // todo initialize players list
    }

#pragma clang diagnostic pop

    std::shared_ptr<Event> convert_datagram(ServerDatagram &datagram) {

        auto events = datagram.get_events();
        try {
//            auto receive = game_socket_.receive();
//            std::string message(receive.first);
//
//            ServerDatagram datagram(message);

            std::cout << "game_id=" << datagram.get_game_id() << std::endl;
            for (auto &event : events) {
                int8_t type = event->get_type();
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
                    std::cout << "player=" << +pixel->get_player_number() << " x="
                              << pixel->get_x() << " y=" << pixel->get_y() << std::endl;;

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
        } catch (std::runtime_error e) {
            std::cerr << e.what() << std::endl;
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
    PollSocketsClient sockets_;

    bool is_game_active_;
    uint32_t game_id_;
    int8_t left_arrow_down_;
    int8_t right_arrow_down_;
    std::vector<std::string> player_names_;

    uint32_t maxx_;
    uint32_t maxy_;
};


#endif //SIK_GAMECLIENT_H
