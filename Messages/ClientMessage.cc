
#include "ClientMessage.h"

ClientMessage::ClientMessage(uint64_t session_id, char turn_direction, uint32_t next_expected_event_no,
                             std::string const &player_name) : session_id_(session_id),
                                                               turn_direction_(turn_direction),
                                                               next_expected_event_no_(next_expected_event_no),
                                                               player_name_(player_name) {}

ClientMessage::ClientMessage(std::string const &serialized_message) {
    if (serialized_message.length() < 13 || serialized_message.length() > 77) {
        throw std::runtime_error("Client message size was wrong.");
    }

    session_id_ = bton(serialized_message.substr(0, 8));
    turn_direction_ = (char) bton(serialized_message.substr(8, 1));
    if (turn_direction_ < -1 || turn_direction_ > 1) {
        throw std::runtime_error("Wrong direction given.");
    }
    next_expected_event_no_ = (uint32_t) bton(serialized_message.substr(9, 4));
    if (serialized_message.length() == 13) {
        player_name_ = "";
    } else {
        std::ostringstream os;
        std::string player_name_buf = serialized_message.substr(13);
        for (auto ch : player_name_buf) {
            if (ch < 33 || ch > 126) {
                throw new std::runtime_error("Player name contains invalid character.");
            }
            os << ch;
        }

        player_name_ = os.str();
    }
}

std::string ClientMessage::serialize() {
    std::ostringstream os;
    std::string message;

    std::vector<unsigned char> session_id_bytes = ntob((int64_t) htobe64(session_id_), 8);
    for (auto byte : session_id_bytes) {
        os << byte;
    }
    os << turn_direction_;

    std::vector<unsigned char> next_expected_event_no_bytes = ntob(htonl(next_expected_event_no_), 4);

    for (auto byte : next_expected_event_no_bytes) {
        os << byte;
    }

    for (auto byte : player_name_) {
        os << byte;
    }

    message = os.str();
    return message;
}

void ClientMessage::set_sender(NetworkAddress &sender) {
    sender_ = sender;
}

NetworkAddress ClientMessage::get_sender() {
    return sender_;
}

uint64_t ClientMessage::get_session_id() {
    return session_id_;
}

char ClientMessage::get_turn_direction() {
    return turn_direction_;
}

uint32_t ClientMessage::get_next_expected_event_no_() {
    return next_expected_event_no_;
}

std::string ClientMessage::get_player_name_() {
    return player_name_;
}
