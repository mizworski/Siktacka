#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <iostream>

class Game {

};

void parse_arguments(int argc, char *const *argv, int64_t &width, int64_t &height, int64_t &port, int64_t &game_speed,
                     int64_t &turn_speed, int64_t &random_seed);

int main(int argc, char **argv) {
    int64_t width = 800;
    int64_t height = 600;
    int64_t port = 12345;
    int64_t game_speed = 50;
    int64_t turn_speed = 6;
    int64_t random_seed;

    try {
        parse_arguments(argc, argv, width, height, port, game_speed, turn_speed,
                        random_seed);
    } catch (std::runtime_error e) {
        fprintf(stderr, e.what());
        exit(EXIT_FAILURE);
    }


    std::cout << random_seed << turn_speed << game_speed << port << height << width;

    return 42;
}

void parse_arguments(int argc, char *const *argv, int64_t &width, int64_t &height, int64_t &port, int64_t &game_speed,
                     int64_t &turn_speed, int64_t &random_seed) {
    while (true) {
        int option_index = 0;
        static struct option long_options[] = {
                {"width",       required_argument, 0, 0},
                {"height",      required_argument, 0, 0},
                {"port",        required_argument, 0, 0},
                {"game_speed",  required_argument, 0, 0},
                {"turn_speed",  required_argument, 0, 0},
                {"random_seed", required_argument, 0, 0},
                {0, 0,                             0, 0}
        };

        int c = getopt_long(argc, argv, "W:H:p:s:t:r:",
                            long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'W':
                width = atoi(optarg);
                break;
            case 'H':
                height = atoi(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 's':
                game_speed = atoi(optarg);
                break;
            case 't':
                turn_speed = atoi(optarg);
                break;
            case 'r':
                random_seed = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-W n] [-H n] [-p n] [-s n] [-t n] [-r n]\n", argv[0]);
                throw std::runtime_error("Parse failed");
        }
    }
}