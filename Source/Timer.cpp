#include "Core/Core.h"
#include "Timer.h"
#include <SDL.h>

Timer::Timer() :
    _realStartTime(0),
    _realPreviousTime(0),
    _realCurrentTime(0),
    _gamePreviousTime(0),
    _gameCurrentTime(0),
    updateGameTimer(true)
{
}

void Timer::reset()
{
    uint32_t ticks = SDL_GetTicks();
    _realStartTime = ticks;
    _realPreviousTime = ticks;
    _realCurrentTime = ticks;

    _gamePreviousTime = 0;
    _gameCurrentTime = 0;
}

void Timer::update()
{
    _realPreviousTime = _realCurrentTime;
    _realCurrentTime = SDL_GetTicks();

    _gamePreviousTime = _gameCurrentTime;
    if (updateGameTimer)
    {
        _gameCurrentTime += (_realCurrentTime - _realPreviousTime);
    }
}

uint32_t Timer::elapsedTicks()
{
    return _gameCurrentTime - _gamePreviousTime;
}

float Timer::time()
{
    return (float)_gameCurrentTime / 1000.0f;
}
