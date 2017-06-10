
#ifndef SIK_GAMESERVER_H
#define SIK_GAMESERVER_H


#include <cstdint>
#include <vector>
#include <deque>
#include <queue>
#include <unordered_set>
#include "../Messages/ServerMessage.h"
#include "../Network/UdpSocket.h"
#include "../Network/PollSocketsServer.h"
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <functional>
#include <utility>
#include <libnet.h>

const uint16_t TIMEOUT_LIMIT = 200;
const double PI = 3.14159265359;

class Head {
public:
    Head() {}

    Head(double x, double y, double direction, int64_t turn_speed) : x_(x),
                                                                     y_(y),
                                                                     direction_(direction),
                                                                     turn_speed_(turn_speed) {}

    std::pair<int64_t, int64_t> get_position() {
        return {std::floor(x_), std::floor(y_)};
    };

    void move_forward(double direction) {
        direction_ += turn_speed_ * direction;
        x_ += cos(direction_ * PI / 180);
        y_ += sin(direction_ * PI / 180);
    }

private:
    double x_;
    double y_;
    double direction_;

    int64_t turn_speed_;
};

class Client {
public:
    Client(int64_t session_id,
           NetworkAddress address,
           uint64_t timestamp) : session_id_(session_id),
                                 address_(address),
                                 is_connected_(true),
                                 last_active_(timestamp) {}

    int64_t get_session_id() {
        return session_id_;
    }

    NetworkAddress get_address() {
        return address_;
    }

    void set_connected_status(bool status) {
        is_connected_ = status;
    }

    bool is_connected() {
        return is_connected_;
    }

    void set_last_active(uint64_t timestamp) {
        last_active_ = timestamp;
    }

    uint64_t get_last_active() {
        return last_active_;
    }

    void set_expected_event_no(uint32_t event_no) {
        expected_event_no = event_no;
    }

    uint32_t get_expected_event_no() {
        return expected_event_no;
    }

protected:
    int64_t session_id_;
    NetworkAddress address_;
    bool is_connected_;
    uint64_t last_active_;
    uint32_t expected_event_no;
};

class Observer : public Client {
public:
    Observer(int64_t session_id,
             NetworkAddress address,
             uint64_t timestamp) : Client(session_id, address, timestamp) {

    }
};

class Player : public Client {
public:
    Player(std::string player_name,
           int64_t session_id,
           NetworkAddress address,
           uint64_t timestamp) : Client(session_id, address, timestamp),
                                 player_name_(player_name),
                                 is_playing_(false),
                                 direction_(0) {

    }

    std::string get_player_name() {
        return player_name_;
    }

    void set_last_direction(char direction) {
        direction_ = direction;
    }

    char get_last_direction() {
        return direction_;
    }

    void set_playing_status(bool status) {
        is_playing_ = status;
    }

    bool is_playing() {
        return is_playing_;
    }

    bool operator==(const Player &other) const {
        return player_name_ == other.player_name_;
    }

    bool operator!=(const Player &other) const {
        return !(*this == other);
    }

    bool operator<(const Player &other) const {
        return player_name_ < other.player_name_;
    }

    void set_head(std::shared_ptr<Head> head) {
        head_ = head;
    }

    std::shared_ptr<Head> get_head() {
        return head_;
    }

    void set_player_number(char number) {
        player_number_ = number;
    }

    char get_player_number() {
        return player_number_;
    }

    void set_alive() {
        is_alive_ = true;
    }

    void kill() {
        is_alive_ = false;
    }

    bool is_alive() {
        return is_alive_;
    }

private:
    std::shared_ptr<Head> head_;
    std::string player_name_;
    bool is_playing_;
    bool is_alive_;
    char direction_;
    char player_number_;
};

class GameBoard {
public:
    GameBoard(int64_t maxx, int64_t maxy) : maxx_(maxx), maxy_(maxy), taken_positions_() {}

    void take_position(std::pair<int64_t, int64_t> pos) {
        taken_positions_.insert(pos);
    }

    bool is_valid(std::pair<int64_t, int64_t> pos) {
        bool res = taken_positions_.find(pos) == taken_positions_.end();
        res = res && pos.first >= 0 && pos.first < maxx_;
        res = res && pos.second >= 0 && pos.second < maxy_;
        return res;
    }

private:
    int64_t maxx_;
    int64_t maxy_;
    std::unordered_set<std::pair<int64_t, int64_t>, boost::hash<std::pair<int, int>>> taken_positions_;
};

class GameServer {
public:
    GameServer(int64_t width,
               int64_t height,
               int64_t port,
               int64_t game_speed,
               int64_t turn_speed,
               int64_t random_seed) : width_(width), height_(height), port_(port), game_speed_(game_speed),
                                      turn_speed_(turn_speed), random_state_(random_seed),
                                      board_(width, height),
                                      sockets_(1, (uint16_t) port, game_speed),
                                      is_game_active_(false) {}

    void start() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
        while (true) {
            std::pair<bool, ClientMessage> response;
            response.first = false;

            struct timeval tp;
            gettimeofday(&tp, NULL);
            int64_t timestamp_before = (int64_t) (tp.tv_sec * 1000 + tp.tv_usec / 1000);

            try {
                auto poll_res = sockets_.poll_sockets();
                response.first = poll_res.first;
                response.second = poll_res.second;
            } catch (std::runtime_error e) {
                std::cerr << e.what() << std::endl; //todo continue?
            }

            if (response.first) {
                process_message(response);
            }

            if (!is_game_active_) {
                check_if_start();
            }

            if (is_game_active_) {
                tick_one_round();
                check_game_over();
            }

            gettimeofday(&tp, NULL);
            int64_t timestamp_after = (int64_t) (tp.tv_sec * 1000 + tp.tv_usec / 1000);
            if (1000 / game_speed_ - (timestamp_after - timestamp_before) > 0) {
                struct timespec time;
                time.tv_sec = 0;
                time.tv_nsec = (1000 / game_speed_ - (timestamp_after - timestamp_before)) * 1000000;
                nanosleep(&time, nullptr);
            }

        }
#pragma clang diagnostic pop
    }


private:
    void process_message(const std::pair<bool, ClientMessage> &response) {
        uint64_t timestamp = (uint64_t) time(NULL);
        auto message = response.second;
        auto player_address = message.get_sender();
        auto session_id = message.get_session_id();
        auto player_name = message.get_player_name_();
        auto direction = message.get_turn_direction();
        auto expected_event_no = message.get_next_expected_event_no_();

        if (direction < -1 || direction > 1) {
            return;
        }

        auto player_node = players_.find(player_address);
        if (player_node == players_.end()) {

            if (player_names_.find(player_name) != player_names_.end()) {
                return;
            }
            if (!player_name.empty()) {
                Player new_player(player_name, (int64_t) session_id, player_address, timestamp);
                new_player.set_last_direction(direction);
                new_player.set_expected_event_no(0); //todo or 0?
                std::shared_ptr<Player> player_ptr = std::make_shared<Player>(new_player);
                players_.insert({player_address, player_ptr});

                send_message({player_address, player_ptr});
            } else if (observers_.find(player_address) == observers_.end()) {
                Observer new_obs((int64_t) session_id, player_address, timestamp);
                new_obs.set_expected_event_no(0); //todo or 0?
                observers_.insert({player_address, new_obs});
            }
        } else {
            if (session_id < player_node->second->get_session_id()) {
                return;
            }
            if (timestamp - player_node->second->get_last_active() > TIMEOUT_LIMIT) {
                player_node->second->set_playing_status(false);
            }

            if (player_name.empty()) {
                players_.erase(player_address);
                Observer new_obs((int64_t) session_id, player_address, timestamp);
                new_obs.set_expected_event_no(expected_event_no);
                observers_.insert({player_address, new_obs});
            } else if (session_id > player_node->second->get_session_id()) {
                player_node->second->set_playing_status(false);
                // todo resend everything?
            } else if (player_node->second->is_playing() &&
                       player_node->second->get_player_name() == player_name) { //todo what if different names
                player_node->second->set_last_active(timestamp);
                player_node->second->set_last_direction(direction);
                player_node->second->set_expected_event_no(expected_event_no);
                send_message({player_address, player_node->second});
            } else if (player_node->second->get_player_name() == player_name) {
                send_message({player_address, player_node->second});
            } else {
                return;
            }
        }

        if (!is_game_active_) {
            player_node = players_.find(player_address);
            player_node->second->set_connected_status(true);
        }
    }

    void check_game_over() {
        int32_t players_alive = 0;
        for (auto &el : players_) {
            if (el.second->is_alive()) {
                ++players_alive;
            }
        }

        if (players_alive <= 1) {
            is_game_active_ = false;
            GameOver ng(event_no_);
            event_no_ = 0;

            auto go_ptr = std::make_shared<GameOver>(ng);
            events_.push_back(go_ptr);

            for (auto &el : players_) {
                el.second->set_playing_status(false);
                el.second->kill();
                el.second->set_last_direction(0);
            }
        }
    }

    void tick_one_round() {
        for (auto &el : players_sorted_) {
            auto player = el.second;

            if (player->is_alive()) {
                auto head = player->get_head();
                auto head_pos_before = head->get_position();
                head->move_forward(player->get_last_direction());
                auto head_pos_after = head->get_position();
                if (board_.is_valid(head_pos_after)) {
                    board_.take_position(head_pos_after);
                    Pixel new_pixel(event_no_,
                                    el.second->get_player_number(),
                                    (uint32_t) head_pos_after.first,
                                    (uint32_t) head_pos_after.second);
                    ++event_no_;
                    auto px_ptr = std::make_shared<Pixel>(new_pixel);
                    events_.push_back(px_ptr);
                } else if (head_pos_before != head_pos_after) {
                    el.second->kill();
                    std::cout << "player=" << el.second->get_player_number() << " x=" << head_pos_after.first
                              << " y=" << head_pos_after.second << std::endl;
                    PlayerEliminated pe(event_no_, el.second->get_player_number());
                    ++event_no_;
                    auto pe_ptr = std::make_shared<PlayerEliminated>(pe);
                    events_.push_back(pe_ptr);
                }
            }
        }
    }

    int64_t rand() {
        int64_t res = random_state_;
        random_state_ *= 279470273;
        random_state_ %= 4294967291;
        return res;
    }

    void check_if_start() {
        uint64_t timestamp = (uint64_t) time(NULL);

        uint16_t players_ready = 0;

        std::vector<std::string> ready_players;

        std::vector<NetworkAddress> to_remove;
        for (auto &el : players_) {
            auto player = el.second;

            if (timestamp - player->get_last_active() > TIMEOUT_LIMIT) {
                player->set_connected_status(false);
                to_remove.push_back(el.first);
            } else if (player->is_connected() && player->get_last_direction() != 0) {
                ++players_ready;
                ready_players.push_back(player->get_player_name());
            } else { // connected player with direction == 0
                return;
            }
        }
        for (auto &player_address : to_remove) {
            players_.erase(player_address);
        }

        if (players_ready > 1) {
            is_game_active_ = true;
            game_id_ = (uint32_t) rand();

            start_game(ready_players);
        }
    }

    void send_message(std::pair<NetworkAddress, std::shared_ptr<Player>> player) {
        uint32_t expected_event_no = player.second->get_expected_event_no();

        std::vector<std::shared_ptr<Event>> events_to_send(events_.begin() + expected_event_no,
                                                           events_.end());

        ServerMessage message(game_id_, events_to_send);

        if (!events_to_send.empty()) {
            sockets_.add_message_to_queue(player.first, message);
        }
    }

    void start_game(std::vector<std::string> &ready_players) {
        is_game_active_ = true;

        std::cout << "Zaczynamy!" << std::endl; //todo to remove

        std::vector<std::pair<std::shared_ptr<Player>, NetworkAddress>> players_to_sort;
        for (auto &el : players_) {
            players_to_sort.push_back({el.second, el.first});
        }
        std::sort(players_to_sort.begin(), players_to_sort.end()); // todo validate

        std::vector<std::string> player_names_sorted;
        players_sorted_.clear();
        uint8_t player_number = 0;
        for (auto &el : players_to_sort) {
            el.first->set_player_number(player_number);
            el.first->set_playing_status(true);
            el.first->set_alive();
            players_sorted_.push_back({el.second, el.first});
            player_names_sorted.push_back({el.first->get_player_name()});
            ++player_number;
        }

        event_no_ = 0;
        NewGame ng(event_no_, (uint32_t) width_, (uint32_t) height_, player_names_sorted);
        event_no_++;
        auto ng_ptr = std::make_shared<NewGame>(ng);
        events_.push_back(ng_ptr);

        for (auto &el : players_sorted_) {

            double head_x = rand() % width_ + 0.5; //todo po co te 0.5 skoro zaokraglanie w dol?
            double head_y = rand() % height_ + 0.5;
            double direction = rand() % 360;

            Head player_head(head_x, head_y, direction, turn_speed_);
            el.second->set_head(std::make_shared<Head>(player_head));

            auto head_pos = el.second->get_head()->get_position();
            if (board_.is_valid(head_pos)) {
                board_.take_position(head_pos);
                Pixel new_pixel(event_no_, el.second->get_player_number(), (uint32_t) head_pos.first,
                                (uint32_t) head_pos.second);
                ++event_no_;
                auto px_ptr = std::make_shared<Pixel>(new_pixel);
                events_.push_back(px_ptr);
            } else {
                el.second->kill();
                std::cout << "player=" << el.second->get_player_number() << " x=" << head_pos.first
                          << " y=" << head_pos.second << std::endl; //todo to remove
                PlayerEliminated pe(event_no_, el.second->get_player_number());
                ++event_no_;
                auto pe_ptr = std::make_shared<PlayerEliminated>(pe);
                events_.push_back(pe_ptr);
            }
        }
    }

    int64_t width_;
    int64_t height_;
    int64_t port_;
    int64_t game_speed_;
    int64_t turn_speed_;
    int64_t random_state_;

    uint32_t game_id_;

    std::deque<std::shared_ptr<Event>> events_;
    GameBoard board_;

    std::set<std::string> player_names_;
    std::map<NetworkAddress, std::shared_ptr<Player>> players_;
    std::vector<std::pair<NetworkAddress, std::shared_ptr<Player>>> players_sorted_;
    std::map<NetworkAddress, Observer> observers_;
    PollSocketsServer sockets_;

    bool is_game_active_;
    uint32_t event_no_;
};


#endif //SIK_GAMESERVER_H
