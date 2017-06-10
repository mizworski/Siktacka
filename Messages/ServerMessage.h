
#ifndef SIK_SERVERMESSAGE_H
#define SIK_SERVERMESSAGE_H

#include <string>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include <iostream>
#include "../Network/HelperFunctions.h"

#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <algorithm>
#include <cassert>
#include <memory>

class Event {
public:
    Event() {}

    Event(uint32_t len, uint32_t event_no, char event_type) : len_(len),
                                                              event_no_(event_no),
                                                              event_type_(event_type) {}

    virtual std::string serialize() {
        std::ostringstream os;
        std::string message;

        std::vector<unsigned char> lenght_bytes = ntob(htonl(len_), 4);
        for (auto byte : lenght_bytes) {
            os << byte;
        }

        std::vector<unsigned char> event_no_bytes = ntob(htonl(event_no_), 4);
        for (auto byte : event_no_bytes) {
            os << byte;
        }

        os << event_type_;

        message = os.str();
        return message;
    }

    virtual std::string get_string() {
        return "";
    }

    std::string get_message() {
        std::string message(serialize());
        uint32_t crc32 = get_control_sum(message);
        std::vector<unsigned char> crc32_str = ntob(htonl(crc32), 4); // todo htonl(crc32) vs crc32?
        std::ostringstream os;
        for (auto ch : crc32_str) {
            os << ch;
        }
        message += os.str();

        return message;
    }

    uint32_t get_control_sum(std::string const &message) {
        boost::crc_32_type result;
        result.process_bytes(message.data(), message.length());
        return (uint32_t) result.checksum();
    }

    bool check_control_sum(uint32_t other_sum) {
        std::string message(serialize());
        uint32_t valid_sum = get_control_sum(message);

        return other_sum == valid_sum;
    }

    uint32_t get_size() {
        return len_ + 4;
    }

    virtual int8_t get_type() {
        return -1;
    }

    uint32_t get_event_no() {
        return event_no_;
    }

protected:
    uint32_t len_;
    uint32_t event_no_;
    char event_type_;
};

class NewGame : public Event {
public:
    NewGame(uint32_t event_no,
            uint32_t maxx,
            uint32_t maxy,
            std::vector<std::string> &players) : Event(17, event_no, 0),
                                                 maxx_(maxx),
                                                 maxy_(maxy),
                                                 players_(players) {
        for (auto &player : players) {
            len_ += player.length() + 1;
        }
    }

    NewGame(std::string const &serialized_message) {
        if (serialized_message.length() < 21) {
            throw std::runtime_error("New game message too short. Should be at least 21 bytes long.");
        }
        assert(serialized_message.length() >= 21);

        len_ = (uint32_t) bton(serialized_message.substr(0, 4));
        if (len_ + 4 != serialized_message.length()) {
            throw std::runtime_error("Message's length variable does't match actual length of message.");
        }
        assert(len_ + 4 == serialized_message.length());


        uint64_t split_point = serialized_message.length() - 4;
        std::string checksum_str = serialized_message.substr(split_point, 4);
        std::string message_without_checksum(serialized_message.substr(0, split_point));

        event_no_ = (uint32_t) bton(message_without_checksum.substr(4, 4));
        event_type_ = (char) bton(message_without_checksum.substr(8, 1));
        maxx_ = (uint32_t) bton(message_without_checksum.substr(9, 4));
        maxy_ = (uint32_t) bton(message_without_checksum.substr(13, 4));

        std::string players_buffer = message_without_checksum.substr(17);

        std::ostringstream os;

        if (players_buffer[players_buffer.length() - 1] != 0) {
            throw std::runtime_error("Players list is not valid (missing null at the end).");
        }
        assert(players_buffer[players_buffer.length() - 1] == 0);

        for (auto ch : players_buffer) { // todo what if two zeros next to each other?
            if (ch) {
                if (ch < 33 || ch > 126) {
                    throw new std::runtime_error("Player name contains invalid character.");
                }
                os << ch;
            } else {
                std::string player_name = os.str();
                if (player_name.length() > 64) {
                    throw std::runtime_error("Player name too long");
                }
                players_.push_back(std::move(player_name));
                os.clear();
                os.str("");
            }
        }

        uint32_t checksum = (uint32_t) bton(checksum_str);
        if (!check_control_sum(checksum)) {
            throw std::invalid_argument("Wrong control sum.");
        }
        assert(check_control_sum(checksum));
    }

    std::string serialize() override {
        std::ostringstream os;
        std::string message(Event::serialize());

        std::vector<unsigned char> x_bytes = ntob(htonl(maxx_), 4);
        for (auto byte : x_bytes) {
            os << byte;
        }

        std::vector<unsigned char> y_bytes = ntob(htonl(maxy_), 4);
        for (auto byte : y_bytes) {
            os << byte;
        }

        for (auto &player : players_) {
            for (auto ch : player) {
                os << ch;
            }
            os << '\0';
        }

        message += os.str();

        return message;

    }

    std::string get_string() override {
        std::string res("NEW_GAME " + std::to_string(maxx_) + " " + std::to_string(maxy_));

        for (auto &player : players_) {
            res += " " + player;
        }

        res += 10;

        return res;
    }

    int8_t get_type() override {
        return 0;
    }

    uint32_t get_maxx() {
        return maxx_;
    }

    uint32_t get_maxy() {
        return maxy_;
    }

    std::vector<std::string> get_players() {
        return players_;
    }

private:
    uint32_t maxx_;
    uint32_t maxy_;
    std::vector<std::string> players_;
};

class Pixel : public Event {
public:
    Pixel(uint32_t event_no, char player_number, uint32_t x, uint32_t y) : Event(18, event_no, 1),
                                                                           player_number_(player_number),
                                                                           x_(x),
                                                                           y_(y) {}

    Pixel(std::string const &serialized_message) {
        if (serialized_message.length() != 22) {
            throw std::runtime_error("Pixel message of wrong size. Should be exactly 22 bytes.");
        }

        len_ = (uint32_t) bton(serialized_message.substr(0, 4));
        if (len_ != 18) {
            throw std::runtime_error("Pixel len value is wrong. Should be 18.");
        }

        event_no_ = (uint32_t) bton(serialized_message.substr(4, 4));
        event_type_ = (char) bton(serialized_message.substr(8, 1));

        player_number_ = (char) bton(serialized_message.substr(9, 1));
        x_ = (uint32_t) bton(serialized_message.substr(10, 4));
        y_ = (uint32_t) bton(serialized_message.substr(14, 4));
        std::string checksum_str = serialized_message.substr(serialized_message.length() - 4, 4);
        uint32_t checksum = (uint32_t) bton(checksum_str);
        if (!check_control_sum(checksum)) {
            throw std::invalid_argument("Wrong control sum.");
        }
        assert(check_control_sum(checksum));
    }

    std::string serialize() override {
        std::ostringstream os;
        std::string message(Event::serialize());

        os << player_number_;

        std::vector<unsigned char> x_bytes = ntob(htonl(x_), 4);
        for (auto byte : x_bytes) {
            os << byte;
        }

        std::vector<unsigned char> y_bytes = ntob(htonl(y_), 4);
        for (auto byte : y_bytes) {
            os << byte;
        }

        message += os.str();

        return message;
    }

    int8_t get_type() override {
        return 1;
    }

    char get_player_number() {
        return player_number_;
    }

    uint32_t get_x() {
        return x_;
    }

    uint32_t get_y() {
        return y_;
    }

    std::string get_string() override {
        std::string res("PIXEL " + std::to_string(x_) + " " + std::to_string(y_) + " ");

        return res;
    }

private:
    char player_number_;
    uint32_t x_;
    uint32_t y_;
};

class PlayerEliminated : public Event {
public:
    PlayerEliminated(uint32_t event_no, char player_number) : Event(10, event_no, 2),
                                                              player_number_(player_number) {}

    PlayerEliminated(std::string const &serialized_message) {
        if (serialized_message.length() != 14) {
            throw std::runtime_error("Player eliminated message of wrong size. Should be exactly 14 bytes.");
        }

        len_ = (uint32_t) bton(serialized_message.substr(0, 4));
        if (len_ != 10) {
            throw std::runtime_error("Player eliminated len is wrong. Should be 10.");
        }

        event_no_ = (uint32_t) bton(serialized_message.substr(4, 4));
        event_type_ = (char) bton(serialized_message.substr(8, 1));
        player_number_ = (char) bton(serialized_message.substr(9, 1));

        std::string checksum_str = serialized_message.substr(serialized_message.length() - 4, 4);
        uint32_t checksum = (uint32_t) bton(checksum_str);

        if (!check_control_sum(checksum)) {
            throw std::invalid_argument("Wrong control sum.");
        }
        assert(check_control_sum(checksum));
    }

    std::string serialize() override {
        std::ostringstream os;
        std::string message(Event::serialize());

        os << player_number_;

        message += os.str();

        return message;
    }

    std::string get_string() override {
        std::string res("PLAYER_ELIMINATED ");

        return res;
    }

    int8_t get_type() override {
        return 2;
    }

    char get_player_number() {
        return player_number_;
    }

private:
    char player_number_;
};

class GameOver : public Event {
public:
    GameOver(uint32_t event_no) : Event(9, event_no, 3) {}

    GameOver(std::string const &serialized_message) {
        if (serialized_message.length() != 13) {
            throw std::runtime_error("Game over message of wrong size. Should be exactly 13 bytes.");
        }

        len_ = (uint32_t) bton(serialized_message.substr(0, 4));
        if (len_ != 9) {
            throw std::runtime_error("Game over len is wrong. Should be 9.");
        }

        event_no_ = (uint32_t) bton(serialized_message.substr(4, 4));
        event_type_ = (char) bton(serialized_message.substr(8, 1));

        std::string checksum_str = serialized_message.substr(serialized_message.length() - 4, 4);
        uint32_t checksum = (uint32_t) bton(checksum_str);
        if (!check_control_sum(checksum)) {
            throw std::invalid_argument("Wrong control sum.");
        }
        assert(check_control_sum(checksum));
    }

    int8_t get_type() {
        return 3;
    }
};

class ServerDatagram {
public:
    ServerDatagram() {}

    ServerDatagram(uint32_t game_id, std::vector<std::shared_ptr<Event>> &events) : game_id_(game_id),
                                                                                    events_(events) {}

    ServerDatagram(std::string const &serialized_message) {
        if (serialized_message.length() < 4) {
            throw std::runtime_error("Message too short. Should be at least 4 bytes long.");
        }
        assert(serialized_message.length() >= 4);

        game_id_ = (uint32_t) bton(serialized_message.substr(0, 4));

        std::string events_str(serialized_message.substr(4));

        while (!events_str.empty()) {
            if (events_str.length() < 13) {
                std::cout << std::endl;
                throw std::runtime_error("Message is incomplete. Suffix of message is too short to be event.");
            }
            uint32_t event_type_ = (char) bton(events_str.substr(8, 1));
            uint32_t event_len = (uint32_t) bton(events_str.substr(0, 4));
            if (events_str.length() < event_len + 4) {
                throw std::runtime_error("Event is too long to fit into message.");
            }

            std::shared_ptr<Event> event = nullptr;
            try {
                switch (event_type_) {
                    case 0:
                        event = std::make_shared<NewGame>(NewGame(events_str.substr(0, event_len + 4)));
                        break;
                    case 1:
                        event = std::make_shared<Pixel>(Pixel(events_str.substr(0, event_len + 4)));
                        break;
                    case 2:
                        event = std::make_shared<PlayerEliminated>(PlayerEliminated(events_str.substr(0, event_len + 4)));
                        break;
                    case 3:
                        event = std::make_shared<GameOver>(GameOver(events_str.substr(0, event_len + 4)));
                        break;
                    default:
                        std::cerr << "Wrong event type in message." << std::endl;
                        break;
                }
            } catch (std::invalid_argument e) {
                std::cerr << std::endl << e.what() << std::endl;
                break;
            }

            events_str = events_str.substr(event_len + 4);
            if (event) {
                events_.push_back(event);
            }
        }
    }

    std::string serialize() {
        std::ostringstream os;

        std::vector<unsigned char> x_bytes = ntob(htonl(game_id_), 4);
        for (auto byte : x_bytes) {
            os << byte;
        }

        std::string message(os.str());
        for (auto &event : events_) {
            std::string serialized_event(event->get_message());
            message += serialized_event;
        }
        return message;
    }

    uint32_t get_game_id() {
        return game_id_;
    }

    std::vector<std::shared_ptr<Event>> get_events() {
        return events_;
    }

private:
    uint32_t game_id_;
    std::vector<std::shared_ptr<Event>> events_;
};

class ServerMessage {
public:
    ServerMessage(uint32_t game_id) : game_id_(game_id) {}

    ServerMessage(uint32_t game_id,
                  std::vector<std::shared_ptr<Event>> const &events) : game_id_(game_id), events_(events) {}

    ServerMessage(uint32_t game_id,
                  std::vector<std::shared_ptr<Event>> &&events) : game_id_(game_id), events_(std::move(events)) {}

    ServerMessage(uint32_t game_id,
                  std::shared_ptr<Event> &event) : game_id_(game_id), events_() {
        events_.push_back(event);
    }

    ServerMessage(uint32_t game_id,
                  std::shared_ptr<Event> &&event) : game_id_(game_id), events_() {
        events_.push_back(event);
    }

    void push_back(std::shared_ptr<Event> const &event) {
        events_.push_back(event);
    }

    void push_back(std::shared_ptr<Event> &&event) {
        events_.push_back(std::move(event));
    }

    std::vector<ServerDatagram> get_datagrams() {
        std::vector<ServerDatagram> result;
        std::vector<std::shared_ptr<Event>> current_events;
        uint32_t datagram_size_left = 512 - 4;
        for (auto &event : events_) {
            if (datagram_size_left >= event->get_size()) {
                current_events.push_back(event);
                datagram_size_left -= event->get_size();
            } else {
                ServerDatagram dg(game_id_, current_events);
                result.push_back(dg);
                current_events = {event};
                datagram_size_left = 512 - (4 + event->get_size());
            }
        }

        if (!current_events.empty()) {
            ServerDatagram dg(game_id_, current_events);
            result.push_back(dg);
            current_events.clear();
        }

        return result;
    }

private:
    uint32_t game_id_;
    std::vector<std::shared_ptr<Event>> events_;
};


#endif //SIK_SERVERMESSAGE_H
