#include "Core/Core.h"
#include "Application.h"

#ifdef NW_ASSET_COOK
#include <iostream>
#include "Cook/Cook.h"
#include "Cook/Pack.h"

void cookMain()
{
    NW_ASSERT(SDL_Init(SDL_INIT_EVERYTHING) == 0);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    cook::CookSettings settings;
    cook::readCookSettings("cook.json", settings);

    std::cout << "Asset Folder: " << settings.assetFolder.c_str() << std::endl;
    std::cout << "Cache Folder: " << settings.cacheFolder.c_str() << std::endl;
    cook::cookAssets(settings);
    cook::packAssets(settings.cacheFolder.c_str());
}
#endif


int main(int argc, char** argv)
{
#ifdef NW_ASSET_COOK
    if (argc > 1 && strncmp(argv[1], "cook", strlen("cook")) == 0)
    {
        cookMain();
        exit(0);
    }
#endif
    NW_UNUSED(argc);
    NW_UNUSED(argv);

    Application app;
    while (app.isRunning())
    {
        app.update();
    }

    return 0;
}
