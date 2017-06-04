
#include "HelperFunctions.h"

std::vector<unsigned char> itob(int64_t val, uint64_t size) {
    std::vector<unsigned char> bytes(size);
    for (int i = 0; i < size; ++i)
        bytes[(size - 1) - i] = (unsigned char) (val >> (i * 8));
    return bytes;
}
