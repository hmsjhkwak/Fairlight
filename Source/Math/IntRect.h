#ifndef MATH_INT_RECT_H
#define MATH_INT_RECT_H

#include <stdint.h>
#include "Math.h"
#include "Vector2i.h"

namespace math
{
    class IntRect
    {
    public:
        int32_t left, top, width, height;

    public:
        inline IntRect() : left(0), top(0), width(0), height(0)
        {
        }

        inline IntRect(int32_t left, int32_t top, int32_t width, int32_t height) :
            left(left), top(top), width(width), height(height)
        {
        }

        inline IntRect(Vector2i corner, Vector2i size) :
            left(corner.x), top(corner.y),
            width(size.x), height(size.y)
        {
        }

        inline bool intersects(const IntRect& other) const
        {
            return (left < other.left + other.width &&
                left + width > other.left &&
                top < other.top + other.height &&
                top + height > other.top);
        }

        inline void offset(Vector2i offset)
        {
            left += offset.x;
            top += offset.y;
        }
    };

    inline bool operator==(const IntRect& lhs, const IntRect& rhs)
    {
        return lhs.left == rhs.left &&
            lhs.top == rhs.top &&
            lhs.width == rhs.width &&
            lhs.height == rhs.height;
    }

    inline bool operator!=(const IntRect& lhs, const IntRect& rhs)
    {
        return !(lhs == rhs);
    }
}

#endif
