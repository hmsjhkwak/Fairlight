#ifndef COOK_UUID_H
#define COOK_UUID_H

#include <stdint.h>
#include <EASTL/functional.h>
#include "Core/xxhash/xxhash.h"

namespace cook
{
    struct Uuid
    {
    public:
        uint32_t a, b, c, d;

        Uuid();
        Uuid(const char* str);
        Uuid(uint32_t a, uint32_t b, uint32_t c, uint32_t d);

        void parse(const char* str);

        bool operator==(const Uuid& other) const;
    };
}

template <>
struct eastl::hash<cook::Uuid>
{
    std::size_t operator()(const cook::Uuid& k) const
    {
        return XXH32(&k, sizeof(cook::Uuid), 0);
    }
};

#endif
