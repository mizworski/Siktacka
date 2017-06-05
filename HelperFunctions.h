
#ifndef SIK_HELPERFUNCTIONS_H
#define SIK_HELPERFUNCTIONS_H

#include <vector>
#include <cstdint>
#include <string>

std::vector<unsigned char> itob(int64_t val, uint64_t size);

uint64_t btoi(std::string bytes);
#endif //SIK_HELPERFUNCTIONS_H
