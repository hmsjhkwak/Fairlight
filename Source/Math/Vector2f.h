#ifndef MATH_VECTOR2F_H
#define MATH_VECTOR2F_H

#include "Math.h"

namespace math
{
    class Vector2f;

    Vector2f operator*(float scalar, const Vector2f& vec);

    class Vector2f
    {
    public:
        float x, y;

        static const Vector2f zero;
        static const Vector2f right;
        static const Vector2f left;
        static const Vector2f up;
        static const Vector2f down;

    public:
        inline Vector2f() : x(0), y(0)
        {
        }

        inline explicit Vector2f(float s) : x(s), y(s)
        {
        }

        inline Vector2f(float x, float y) : x(x), y(y)
        {
        }

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar.serializeF32(x);
            ar.serializeF32(y);
        }



        inline float operator[](const size_t i) const
        {
            NW_ASSERT(i >= 0 && i < 2);
            return *(&x + i);
        }

        inline float& operator[](const size_t i)
        {
            NW_ASSERT(i >= 0 && i < 2);
            return *(&x + i);
        }



        inline Vector2f operator-() const
        {
            return Vector2f(-x, -y);
        }

        inline Vector2f operator+(const Vector2f& other) const
        {
            return Vector2f(x + other.x, y + other.y);
        }

        inline Vector2f operator-(const Vector2f& other) const
        {
            return Vector2f(x - other.x, y - other.y);
        }

        inline Vector2f operator*(float scalar) const
        {
            return Vector2f(x * scalar, y * scalar);
        }

        inline Vector2f operator/(float divisor) const
        {
            return Vector2f(x / divisor, y / divisor);
        }

        inline Vector2f& operator+=(const Vector2f& rhs)
        {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        inline Vector2f& operator-=(const Vector2f& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;

            return *this;
        }

        inline Vector2f& operator*=(float rhs)
        {
            x *= rhs;
            y *= rhs;

            return *this;
        }

        inline Vector2f& operator/=(float rhs)
        {
            x /= rhs;
            y /= rhs;

            return *this;
        }

        inline bool operator==(const Vector2f& other) const
        {
            return x == other.x && y == other.y;
        }

        inline bool operator!=(const Vector2f& other) const
        {
            return !(*this == other);
        }



        inline static float dot(const Vector2f& lhs, const Vector2f& rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y;
        }



        inline float magnitude() const
        {
            return std::sqrt(x * x + y * y);
        }

        inline float magnitudeSqr() const
        {
            return x * x + y * y;
        }

        inline void normalize()
        {
            *this /= magnitude();
        }

        inline Vector2f normalized() const
        {
            return (*this) / magnitude();
        }



        inline static float distance(const Vector2f& point1, const Vector2f& point2)
        {
            return (point1 - point2).magnitude();
        }

        inline static Vector2f lerp(const Vector2f& start, const Vector2f& end, float t)
        {
            return start + t * (end - start);
        }

        inline static Vector2f slerp(const Vector2f& start, const Vector2f& end, float t)
        {
            float dot = Vector2f::dot(start, end);
            clamp(dot, -1.0f, 1.0f);

            float theta = std::acos(dot) * t;
            Vector2f rel = end - start * dot;
            rel.normalize();
            return (start * std::cos(theta)) + (rel * std::sin(theta));
        }

        inline static Vector2f nlerp(const Vector2f& start, const Vector2f& end, float t)
        {
            return lerp(start, end, t).normalized();
        }
    };

    inline Vector2f operator*(float scalar, const Vector2f& vec)
    {
        return Vector2f(vec.x * scalar, vec.y * scalar);
    }
}

#endif
