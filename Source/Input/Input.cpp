#include "Core/Core.h"
#include <cstdlib>
#include "Input.h"
#include "KeyBindings.h"

namespace input
{
    Input::Input() : _prevKeys(nullptr), _binds(nullptr)
    {
    }

    Input::~Input()
    {
        free(_prevKeys);
    }

    void Input::init(KeyBindings& binds)
    {
        SDL_GetKeyboardState(&_keysLen);
        _prevKeys = (bool*)malloc(2 * _keysLen);
        _nextKeys = _prevKeys + _keysLen;

        _binds = &binds;
    }

    void Input::update()
    {
        const uint8_t* keys = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < _keysLen; i++)
        {
            _prevKeys[i] = _nextKeys[i];
            _nextKeys[i] = (keys[i] != 0);
        }
    }

    bool Input::moveLeft()
    {
        return _nextKeys[_binds->getScancode(InputKey::MoveLeft)];
    }

    bool Input::moveRight()
    {
        return _nextKeys[_binds->getScancode(InputKey::MoveRight)];
    }

    bool Input::moveUp()
    {
        return _nextKeys[_binds->getScancode(InputKey::MoveUp)];
    }

    bool Input::moveDown()
    {
        return _nextKeys[_binds->getScancode(InputKey::MoveDown)];
    }

    bool Input::jump()
    {
        SDL_Scancode code = _binds->getScancode(InputKey::Jump);
        return _nextKeys[code] && !_prevKeys[code];
    }

    bool Input::jumpHeld()
    {
        return _nextKeys[_binds->getScancode(InputKey::Jump)];
    }

    bool Input::attack()
    {
        return _nextKeys[_binds->getScancode(InputKey::Attack)];
    }
}
