#include "Core/Core.h"
#include "Vector2f.h"

namespace math
{
    const Vector2f Vector2f::zero = Vector2f(0, 0);
    const Vector2f Vector2f::right = Vector2f(1, 0);
    const Vector2f Vector2f::left = Vector2f(-1, 0);
    const Vector2f Vector2f::up = Vector2f(0, -1);
    const Vector2f Vector2f::down = Vector2f(0, 1);
}
