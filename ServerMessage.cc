
#include "ServerMessage.h"

NewGame::NewGame(uint32_t maxx,
                 uint32_t maxy,
                 std::vector<std::string> players) : maxx_(maxx),
                                                     maxy_(maxy),
                                                     players_(players) {}
