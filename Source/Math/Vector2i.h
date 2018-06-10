#ifndef MATH_VECTOR2I_H
#define MATH_VECTOR2I_H

#include "Math.h"
#include <stdint.h>

namespace math
{
    class Vector2i;

    Vector2i operator*(int32_t scalar, const Vector2i& vec);

    class Vector2i
    {
    public:
        int32_t x, y;

        static const Vector2i zero;
        static const Vector2i right;
        static const Vector2i left;
        static const Vector2i up;
        static const Vector2i down;

    public:
        inline Vector2i() : x(0), y(0)
        {
        }

        inline explicit Vector2i(int32_t s) : x(s), y(s)
        {
        }

        inline Vector2i(int32_t x, int32_t y) : x(x), y(y)
        {
        }

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar.serializeI32(x);
            ar.serializeI32(y);
        }



        inline int32_t operator[](const size_t i) const
        {
            NW_ASSERT(i >= 0 && i < 2);
            return *(&x + i);
        }

        inline int32_t& operator[](const size_t i)
        {
            NW_ASSERT(i >= 0 && i < 2);
            return *(&x + i);
        }



        inline Vector2i operator-() const
        {
            return Vector2i(-x, -y);
        }

        inline Vector2i operator+(const Vector2i& other) const
        {
            return Vector2i(x + other.x, y + other.y);
        }

        inline Vector2i operator-(const Vector2i& other) const
        {
            return Vector2i(x - other.x, y - other.y);
        }

        inline Vector2i operator*(int32_t scalar) const
        {
            return Vector2i(x * scalar, y * scalar);
        }

        inline Vector2i operator/(int32_t divisor) const
        {
            return Vector2i(x / divisor, y / divisor);
        }

        inline Vector2i& operator+=(const Vector2i& rhs)
        {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        inline Vector2i& operator-=(const Vector2i& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;

            return *this;
        }

        inline Vector2i& operator*=(int32_t rhs)
        {
            x *= rhs;
            y *= rhs;

            return *this;
        }

        inline Vector2i& operator/=(int32_t rhs)
        {
            x /= rhs;
            y /= rhs;

            return *this;
        }

        inline bool operator==(const Vector2i& other) const
        {
            return x == other.x && y == other.y;
        }

        inline bool operator!=(const Vector2i& other) const
        {
            return !(*this == other);
        }



        inline float magnitude() const
        {
            return (float)std::sqrt(x * x + y * y);
        }

        inline int32_t magnitudeSqr() const
        {
            return x * x + y * y;
        }



        inline static float distance(const Vector2i& point1, const Vector2i& point2)
        {
            return (point1 - point2).magnitude();
        }
    };

    inline Vector2i operator*(int32_t scalar, const Vector2i& vec)
    {
        return Vector2i(vec.x * scalar, vec.y * scalar);
    }
}

#endif
