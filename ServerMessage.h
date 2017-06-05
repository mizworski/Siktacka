
#ifndef SIK_SERVERMESSAGE_H
#define SIK_SERVERMESSAGE_H

#include <string>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include <iostream>
#include "HelperFunctions.h"

#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <algorithm>
#include <cassert>

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

protected:
    uint32_t len_;
    uint32_t event_no_;
    char event_type_;
};

class NewGame : public Event {
public:
    NewGame(uint32_t len,
            uint32_t event_no,
            char event_type,
            uint32_t maxx,
            uint32_t maxy,
            std::vector<std::string> &players) : Event(len, event_no, event_type),
                                                 maxx_(maxx),
                                                 maxy_(maxy),
                                                 players_(players) {}

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
            throw std::runtime_error("Wrong control sum.");
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

private:
    uint32_t maxx_;
    uint32_t maxy_;
    std::vector<std::string> players_;
};

class Pixel : public Event {
public:
    Pixel(uint32_t len, uint32_t event_no, char event_type, uint32_t x, uint32_t y) : Event(len, event_no, event_type),
                                                                                      x_(x),
                                                                                      y_(y) {}

    Pixel(std::string const &serialized_message) {
        if (serialized_message.length() != 21) {
            throw std::runtime_error("Pixel message of wrong size. Should be exactly 21 bytes.");
        }

        len_ = (uint32_t) bton(serialized_message.substr(0, 4));
        if (len_ != 17) {
            throw std::runtime_error("Pixel len value is wrong. Should be 17.");
        }

        event_no_ = (uint32_t) bton(serialized_message.substr(4, 4));
        event_type_ = (char) bton(serialized_message.substr(8, 1));

        x_ = (uint32_t) bton(serialized_message.substr(9, 4));
        y_ = (uint32_t) bton(serialized_message.substr(13, 4));
        std::string checksum_str = serialized_message.substr(serialized_message.length() - 4, 4);
        uint32_t checksum = (uint32_t) bton(checksum_str);
        if (!check_control_sum(checksum)) {
            throw std::runtime_error("Wrong control sum.");
        }
        assert(check_control_sum(checksum));
    }

    std::string serialize() override {
        std::ostringstream os;
        std::string message(Event::serialize());

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

private:
    uint32_t x_;
    uint32_t y_;
};

class PlayerEliminated : public Event {
public:
    PlayerEliminated(uint32_t len,
                     uint32_t event_no,
                     char event_type,
                     char player_number) : Event(len, event_no, event_type),
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
            throw std::runtime_error("Wrong control sum.");
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

private:
    char player_number_;
};

class GameOver : public Event {
public:
    GameOver(uint32_t len,
             uint32_t event_no,
             char event_type) : Event(len, event_no, event_type) {}

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
            throw std::runtime_error("Wrong control sum.");
        }
        assert(check_control_sum(checksum));
    }
};

class ServerMessage {
public:
    ServerMessage() {

    }

    ServerMessage(std::string serialized_message) {

    }

private:
    uint32_t game_id_;
    std::vector<Event> events_;


};


#endif //SIK_SERVERMESSAGE_H
