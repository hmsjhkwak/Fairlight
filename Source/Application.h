#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL.h>
#include "AppSettings.h"
#include "Timer.h"
#include "Input/Input.h"
#include "Asset/AssetManager.h"
#include "Scene/Scene.h"
#include "Render/RenderManager.h"
#include "Script/AngelState.h"

class Application
{
private:
    bool _isRunning;
    bool _isLoading;
    AssetRef _sceneRef;

    AppSettings _settings;

    SDL_Window* _window;

    Timer _timer;
    input::Input _input;
    asset::AssetManager _assetManager;
    scene::Scene _scene;
    render::RenderManager _renderManager;
    script::AngelState _angelState;

public:
    Application();
    ~Application();

    void update();

    bool isRunning();
    void exit();

    void restartScene();
    void loadScene(AssetRef ref);
};

#endif
