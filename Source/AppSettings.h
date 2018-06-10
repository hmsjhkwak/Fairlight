#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <stdint.h>
#include "Input/KeyBindings.h"

class AppSettings
{
public:
    bool fullscreen;
    int32_t width;
    int32_t height;
    input::KeyBindings bindings;

    static void load(AppSettings& settings, const char* file);
    static void save(const AppSettings& settings, const char* file);
};

#endif
