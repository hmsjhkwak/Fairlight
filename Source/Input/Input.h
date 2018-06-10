#ifndef INPUT_INPUT_H
#define INPUT_INPUT_H

#include <SDL_keyboard.h>

namespace input
{
    struct KeyBindings;

    class Input
    {
    private:
        int _keysLen;
        bool* _prevKeys;
        bool* _nextKeys;
        KeyBindings* _binds;

    public:
        Input();
        ~Input();

        void Input::init(KeyBindings& binds);
        void update();

        bool moveLeft();
        bool moveRight();
        bool moveUp();
        bool moveDown();
        bool jump();
        bool jumpHeld();
        bool attack();
    };
}

#endif
