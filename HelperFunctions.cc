
#include <stdexcept>
#include <cstring>
#include "HelperFunctions.h"

std::vector<unsigned char> ntob(int64_t val, uint64_t size) {
    std::vector<unsigned char> bytes(size);
    for (int i = 0; i < size; ++i)
        bytes[(size - 1) - i] = (unsigned char) (val >> (i * 8));
    return bytes;
}

uint64_t bton(std::string const &bytes) {
    uint64_t res = 0;
    int64_t mult = 1;
    for (int i = 0; i < bytes.length(); ++i) {
        res += ((uint8_t) bytes[i]) * mult;
        mult *= 256;
    }

    return res;
}
