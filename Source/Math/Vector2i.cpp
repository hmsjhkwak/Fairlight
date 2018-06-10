#include "Core/Core.h"
#include "Vector2i.h"

namespace math
{
    const Vector2i Vector2i::zero = Vector2i(0, 0);
    const Vector2i Vector2i::right = Vector2i(1, 0);
    const Vector2i Vector2i::left = Vector2i(-1, 0);
    const Vector2i Vector2i::up = Vector2i(0, -1);
    const Vector2i Vector2i::down = Vector2i(0, 1);
}
