#include "Core/Core.h"
#include <SDL_syswm.h>
#include <SDL_mixer.h>
#include <bgfx/platform.h>
#include "Application.h"
#include "AngelApplication.h"
#include "Render/RenderCommon.h"

const char* SETTINGS_FILE = "app.json";

Application::Application() :
    _isRunning(true),
    _isLoading(false)
{
    //Load settings from json file
    AppSettings::load(_settings, SETTINGS_FILE);

    //Init SDL
    NW_ASSERT(SDL_Init(SDL_INIT_EVERYTHING) == 0);

    //Create window and context
    uint32_t flags = 0;
    if (_settings.fullscreen) { flags |= SDL_WINDOW_FULLSCREEN; }
    _window = SDL_CreateWindow("Edge of No World",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        _settings.width, _settings.height, flags);

    //Initialize rendering
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(_window, &wmInfo);

        bgfx::PlatformData bgfxPlatformData;
        memset(&bgfxPlatformData, 0, sizeof(bgfxPlatformData));
#if _WIN32
        bgfxPlatformData.nwh = wmInfo.info.win.window;
#endif

        render::initRendering(bgfxPlatformData);
        bgfx::reset(_settings.width, _settings.height, BGFX_RESET_VSYNC);
    }

    //Initialize SDL_mixer
    Mix_Init(MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

#if USE_ASSET_REF_NAMES
    AssetNameTable::loadAssetNames("Assets.cpknames");
#endif
    _assetManager.loadPackFile("Assets.cpk");


    //TODO: Material system
    AssetRef shaderRefs[4];
    shaderRefs[0] = _assetManager.getAssetRefFromName("Shaders/Base.cvs");
    shaderRefs[1] = _assetManager.getAssetRefFromName("Shaders/Base.cps");
    shaderRefs[2] = _assetManager.getAssetRefFromName("Shaders/Saturate.cvs");
    shaderRefs[3] = _assetManager.getAssetRefFromName("Shaders/Saturate.cps");
    _assetManager.loadShaders(shaderRefs, _countof(shaderRefs));

    bgfx::ProgramHandle baseProgram = bgfx::createProgram(
        _assetManager.getShader(shaderRefs[0]),
        _assetManager.getShader(shaderRefs[1]));
    bgfx::ProgramHandle saturateProgram = bgfx::createProgram(
        _assetManager.getShader(shaderRefs[2]),
        _assetManager.getShader(shaderRefs[3]));



    //TODO: Move away from fixed buffer size for 2D rendering
    _renderManager.init(640, 360);
    _renderManager.getRenderer2d().init(baseProgram);

    _renderManager.getPostProcessingManager().init(saturateProgram, _settings.width, _settings.height);

    //Initialize angelscript and load all scripts
    _angelState.init();
    _angelState.setAssetManager(_assetManager);
    _angelState.setInput(_input);
    _angelState.setPostProcessingManager(_renderManager.getPostProcessingManager());
    _angelState.setScene(_scene);
    static Application* angelApp = this;
    angelApplication_RegisterTypes(_angelState.getScriptEngine(), &angelApp);
    _angelState.startCompiling();
    eastl::string sectionName, sectionCode;
    for (auto iter = _assetManager.getPackFile().fileSpanBegin();
        iter != _assetManager.getPackFile().fileSpanEnd(); iter++)
    {
        //TODO: We should have a separate list of only scripts
        if (iter->second.assetType != AssetType::AngelScript) { continue; }

        _assetManager.loadScript(iter->first, sectionName, sectionCode);
        _angelState.addScriptSection(sectionName.c_str(), sectionCode.c_str());
    }
    _angelState.endCompiling();


    //Load initial scene
    _sceneRef = _assetManager.getAssetRefFromName("Scenes/Level01.scene");

    PackFile& packFile = _assetManager.getPackFile();
    _scene.load(_assetManager, packFile, packFile.getFileSpan(_sceneRef), _angelState);

    _timer.reset();
    _input.init(_settings.bindings);
}

Application::~Application()
{
    Mix_HaltMusic();
    Mix_Quit();
    SDL_Quit();

    AppSettings::save(_settings, SETTINGS_FILE);
}



void Application::update()
{
    SCOPED_CPU_EVENT(updateEvent)(0xFFFFFFFF, "Application::update");

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EventType::SDL_QUIT)
        {
            exit();
        }
        else if (event.type == SDL_EventType::SDL_WINDOWEVENT)
        {
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                _timer.updateGameTimer = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                _timer.updateGameTimer = false;
                break;
            }
        }
    }

    //Check for scene change
    if (_isLoading)
    {
        SCOPED_CPU_EVENT(loadEvent)(0xFFFFFFFF, "Loading scene");

        //Stop all sounds
        Mix_HaltChannel(-1);

        _scene.~Scene();
        new (&_scene) Scene();
        auto& packFile = _assetManager.getPackFile();
        _scene.load(_assetManager, packFile, packFile.getFileSpan(_sceneRef), _angelState);
        _angelState.setScene(_scene);
        _input.update();
        _isLoading = false;

        _timer.reset();
    }

    _scene.handleInstantiated(_assetManager);

    //Update every 16ms
    static uint32_t totalElapsed = 0;
    _timer.update();
    totalElapsed += _timer.elapsedTicks();
    while (totalElapsed > Timer::FIXED_UPDATE)
    {
        _input.update();
        _scene.update(_assetManager, Timer::FIXED_UPDATE);
        totalElapsed -= Timer::FIXED_UPDATE;
    }

    _scene.render(_renderManager);
    _renderManager.renderPostProcessing();
    bgfx::frame();
}

bool Application::isRunning() { return _isRunning; }
void Application::exit() { _isRunning = false; }

void Application::restartScene()
{
    _isLoading = true;
}

void Application::loadScene(AssetRef ref)
{
    _isLoading = true;
    _sceneRef = ref;
}
