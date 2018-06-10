#ifndef SCENE_TIMER_H
#define SCENE_TIMER_H

#include <stdint.h>

class Timer
{
public:
    static const int FIXED_UPDATE = 16; //milliseconds

private:
    uint32_t _realStartTime;
    uint32_t _realPreviousTime;
    uint32_t _realCurrentTime;

    uint32_t _gamePreviousTime;
    uint32_t _gameCurrentTime;

public:
    bool updateGameTimer;

    Timer();

    void reset();
    void update();
    uint32_t elapsedTicks();
    float time();
};

#endif
