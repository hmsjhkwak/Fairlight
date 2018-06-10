#ifndef UTIL_FILE_H
#define UTIL_FILE_H

#include <string>
#include <stdint.h>

namespace util
{
namespace file
{
    std::string readAllText(const char* fileName);
    void writeAllText(const char* fileName, const char* text);
}
}

#endif
