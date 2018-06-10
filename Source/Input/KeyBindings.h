#ifndef INPUT_KEY_BINDINGS_H
#define INPUT_KEY_BINDINGS_H

#include <SDL.h>

namespace input
{
    enum class InputKey
    {
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        Jump,
        Attack,

        COUNT
    };

    struct KeyBindings
    {
        SDL_Keycode keys[(int)InputKey::COUNT];

        SDL_Keycode getKeycode(InputKey key) const;
        SDL_Scancode getScancode(InputKey key) const;
        void setKeycode(InputKey key, SDL_Keycode keycode);
    };
}

#endif
