#include "Core/Core.h"
#include "KeyBindings.h"

namespace input
{
    SDL_Keycode KeyBindings::getKeycode(InputKey key) const
    {
        return keys[(int)key];
    }

    SDL_Scancode KeyBindings::getScancode(InputKey key) const
    {
        return SDL_GetScancodeFromKey(keys[(int)key]);
    }

    void KeyBindings::setKeycode(InputKey key, SDL_Keycode keycode)
    {
        keys[(int)key] = keycode;
    }
}
