#ifndef MATH_MATH_H
#define MATH_MATH_H

#include <cmath>
#include <stdint.h>

namespace math
{
    const float PI = (float)3.14159265358979323846;
    const float DEG2RAD = 2 * PI / 360;
    const float RAD2DEG = 360 / (2 * PI);

    inline float lerp(float a, float b, float t)
    {
        return a * (1 - t) + b * t;
    }

    inline int32_t sign(int32_t x)
    {
        if (x > 0) { return 1; }
        else if (x < 0) { return -1; }
        return 0;
    }

    inline int32_t sign(float x)
    {
        if (x > 0) { return 1; }
        else if (x < 0) { return -1; }
        return 0;
    }

    inline int32_t roundToZero(float x)
    {
        return (int32_t)(sign(x) * std::floor(std::abs(x)));
    }

    inline int32_t min(int32_t value1, int32_t value2)
    {
        return (value1 < value2) ? value1 : value2;
    }

    inline int32_t max(int32_t value1, int32_t value2)
    {
        return (value1 > value2) ? value1 : value2;
    }

    inline int32_t clamp(int32_t value, int32_t low, int32_t high)
    {
        NW_ASSERT(high > low);
        return min(max(value, low), high);
    }

    inline float min(float value1, float value2)
    {
        return (value1 < value2) ? value1 : value2;
    }

    inline float max(float value1, float value2)
    {
        return (value1 > value2) ? value1 : value2;
    }

    inline float clamp(float value, float low, float high)
    {
        NW_ASSERT(high > low);
        return min(max(value, low), high);
    }

    inline float degToRad(float theta)
    {
        return theta * DEG2RAD;
    }

    inline float radToDeg(float theta)
    {
        return theta * RAD2DEG;
    }

    inline float sinDeg(float theta)
    {
        return std::sinf(theta * DEG2RAD);
    }

    inline float cosDeg(float theta)
    {
        return std::cosf(theta * DEG2RAD);
    }

    inline float tanDeg(float theta)
    {
        return std::tanf(theta * DEG2RAD);
    }

    inline bool floatEq(float lhs, float rhs)
    {
        const float epsilon = 0.000001f;
        return std::abs(lhs - rhs) < epsilon;
    }
}

#endif