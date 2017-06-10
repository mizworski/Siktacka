
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

    void start() {
        ServerDatagram datagram_from_server;
        std::string message_from_gui;

        while (true) {
            auto response = sockets_.poll_sockets(datagram_from_server, message_from_gui);
            if (response.second) {
                if (message_from_gui == "LEFT_KEY_DOWN\n") {
                    left_arrow_down_ = 1;
                } else if (message_from_gui == "LEFT_KEY_UP\n") {
                    left_arrow_down_ = 0;
                } else if (message_from_gui == "RIGHT_KEY_DOWN\n") {
                    right_arrow_down_ = 1;
                } else if (message_from_gui == "RIGHT_KEY_UP\n") {
                    right_arrow_down_ = 0;
                }
            }

            if (response.first) {
//                std::cout << "my next expected no=" << next_expected_event_no_ << std::endl;
                uint32_t received_game_id = datagram_from_server.get_game_id();
//                last_dir_ = right_arrow_down_ - left_arrow_down_;//todo
                auto events = datagram_from_server.get_events();
                for (auto &event : events) {
                    int8_t type = event->get_type();
                    std::cout << "my_next_eventno=" << next_expected_event_no_ << std::endl;
                    std::cout << "eventno=" << event->get_event_no()<< std::endl;

                    if (type == 0) {
                        std::shared_ptr<NewGame> ng_ptr = std::dynamic_pointer_cast<NewGame>(event);
                        if (ng_ptr) {
                            std::cout << "ng_eventno=" << ng_ptr->get_event_no() << std::endl;
                            if (ng_ptr->get_event_no() != 0) {
                                break;
                            }
                            next_expected_event_no_ = 1;
                            game_id_ = received_game_id;
                            is_game_active_ = true;
                            maxx_ = ng_ptr->get_maxy();
                            maxy_ = ng_ptr->get_maxy();
                            player_names_ = ng_ptr->get_players();
//                                last_dir_ = 0;//todo

                            for (auto &player : player_names_) {
                                std::cout << player << std::endl;
                            }

                            sockets_.drop_messages();
                            auto message = ng_ptr->get_string();
                            sockets_.send_messages_gui(message);
                        }
                    } else if (is_game_active_ && received_game_id == game_id_) {
                        if (type == 1) { /// can't use switch due to initialization of ptrs
                            std::shared_ptr<Pixel> pixel = std::dynamic_pointer_cast<Pixel>(event);
                            if (pixel) {
//                                std::cout << "px_eventno=" << pixel->get_event_no() << std::endl;
//                                std::cout << "my_next_eventno=" << next_expected_event_no_ << std::endl;
                                char player_id = pixel->get_player_number();
                                if (player_id < 0 || player_id >= player_names_.size() ||
                                    pixel->get_x() < 0 || pixel->get_y() < 0 ||
                                    pixel->get_x() > maxx_ || pixel->get_y() > maxy_ ||
                                    pixel->get_event_no() != next_expected_event_no_) {
                                    break;
                                }
//                                std::cout << "player=" << +pixel->get_player_number() << " x="
//                                          << pixel->get_x() << " y=" << pixel->get_y() << std::endl;;
                                ++next_expected_event_no_;
                                std::string player_name = player_names_[player_id];
                                auto message = pixel->get_string();
                                message += player_name;
                                message += 10;
                                sockets_.send_messages_gui(message);
                            }
                        } else if (type == 2) {
                            std::shared_ptr<PlayerEliminated> pe = std::dynamic_pointer_cast<PlayerEliminated>(
                                    event);
                            if (pe) {

                                std::cout << "player_eliminated=" << +pe->get_player_number() << std::endl;
                                char player_id = pe->get_player_number();
                                if (player_id < 0 || player_id >= player_names_.size() ||
                                    pe->get_event_no() != next_expected_event_no_) {
                                    break;
                                }
                                ++next_expected_event_no_;
                                std::string player_name = player_names_[player_id];
                                auto message = pe->get_string();
                                message += player_name;
                                message += 10;
                                sockets_.send_messages_gui(message);
                            }
                        } else if (type == 3) {
                            std::shared_ptr<GameOver> go = std::dynamic_pointer_cast<GameOver>(event);
                            if (go) {
                                std::cout << "game over!" <<std::endl;
                                if (go->get_event_no() != next_expected_event_no_) {
                                    break;
                                }
                                next_expected_event_no_ = 0;
                                is_game_active_ = false;
                                player_names_.clear();
                            }
                        }
                    } else {
                        break; //todo
                    }
                }

            }
            struct timeval tp;
            gettimeofday(&tp, NULL);
            int64_t current_time = (int64_t) (tp.tv_sec * 1000 + tp.tv_usec / 1000);
            if (current_time - last_send_ > TIMEOUT_LIMIT) {
                last_send_ = current_time;
                last_dir_ = right_arrow_down_ - left_arrow_down_;
//                std::cout << "dir=" << +last_dir_ << std::endl;
                send_message_to_server(last_dir_, next_expected_event_no_);
            }
        }
    }

#pragma clang diagnostic pop


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
    int8_t last_dir_;
    std::vector<std::string> player_names_;

    uint32_t maxx_;
    uint32_t maxy_;
    uint32_t next_expected_event_no_;

    int64_t last_send_;
};


#endif //SIK_GAMECLIENT_H
