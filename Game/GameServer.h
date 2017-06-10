
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

    Head(double x, double y, double direction, int64_t turn_speed);

    std::pair<int64_t, int64_t> get_position();;

    void move_forward(double direction);

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
           uint64_t timestamp);

    int64_t get_session_id();

    void set_connected_status(bool status);

    bool is_connected();

    void set_last_active(uint64_t timestamp);

    uint64_t get_last_active();

    void set_expected_event_no(uint32_t event_no);

    uint32_t get_expected_event_no();

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
             uint64_t timestamp);
};

class Player : public Client {
public:
    Player(std::string player_name,
           int64_t session_id,
           NetworkAddress address,
           uint64_t timestamp);

    std::string get_player_name();

    void set_last_direction(char direction);

    char get_last_direction();

    void set_playing_status(bool status);

    bool is_playing();

    bool operator==(const Player &other) const;

    bool operator!=(const Player &other) const;

    bool operator<(const Player &other) const;

    void set_head(std::shared_ptr<Head> head);

    std::shared_ptr<Head> get_head();

    void set_player_number(char number);

    char get_player_number();

    void set_alive();

    void kill();

    bool is_alive();

    void set_pressed_key(bool val);

    bool has_pressed_key();

private:
    std::shared_ptr<Head> head_;
    std::string player_name_;
    bool is_playing_;
    bool is_alive_;
    char direction_;
    char player_number_;
    bool pressed_key_;
};

class GameBoard {
public:
    GameBoard(int64_t maxx, int64_t maxy);

    void take_position(std::pair<int64_t, int64_t> pos);

    bool is_valid(std::pair<int64_t, int64_t> pos);

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
               int64_t random_seed);

    void start();


private:
    void process_message(const std::pair<bool, ClientMessage> &response);

    void check_game_over();

    void tick_one_round();

    int64_t rand();

    void check_if_start();

    void send_message(std::pair<NetworkAddress, std::shared_ptr<Client>> player);

    void start_game(std::vector<std::string> &ready_players);

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
    int64_t last_tick_time_;

    bool is_game_active_;
    uint32_t event_no_;
};


#endif //SIK_GAMESERVER_H
