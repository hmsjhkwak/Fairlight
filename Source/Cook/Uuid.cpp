#include "Core/Core.h"
#include "UUID.h"
#include <cstdlib>

namespace cook
{
    Uuid::Uuid() : a(0), b(0), c(0), d(0) { }
    Uuid::Uuid(const char* str) { parse(str); }
    Uuid::Uuid(uint32_t a, uint32_t b, uint32_t c, uint32_t d) : a(a), b(b), c(c), d(d) { }

    void Uuid::parse(const char* str)
    {
        char buffer[9];
        buffer[8] = '\0';

        const char* pStr = str;
        uint32_t* data = &a;
        int validCount = 0;
        for (int i = 0; i < 4; i++)
        {
            //Copy portion of string into buffer
            validCount = 0;
            while (validCount < 8)
            {
                //Ignore invalid characters
                if ((*pStr >= '0' && *pStr <= '9') ||
                    (*pStr >= 'a' && *pStr <= 'f') ||
                    (*pStr >= 'A' && *pStr <= 'F'))
                {
                    buffer[validCount] = *pStr;
                    pStr++;
                    validCount++;
                }
            }

            //Convert hex to decimal
            *data = strtoul(buffer, nullptr, 16);
            data++;
        }
    }

    bool Uuid::operator==(const Uuid& other) const
    {
        return a == other.a &&
            b == other.b &&
            c == other.c &&
            d == other.d;
    }
}
