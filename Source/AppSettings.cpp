#include "Core/Core.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "AppSettings.h"
#include "Input/KeyBindings.h"
#include "Util/File.h"

using namespace rapidjson;
using namespace util;
using namespace input;

void loadKeyBinding(KeyBindings& binds, Value& jBinds,
    const char* name, InputKey input, SDL_Keycode defaultBind)
{
    //Just reassign to the defaultBind
    if (jBinds.HasMember(name))
    {
        defaultBind = SDL_GetKeyFromName(jBinds[name].GetString());
    }
    binds.setKeycode(input, defaultBind);
}

void loadKeyBindings(KeyBindings& binds, Value& jBinds)
{
    loadKeyBinding(binds, jBinds, "left", InputKey::MoveLeft, SDLK_LEFT);
    loadKeyBinding(binds, jBinds, "right", InputKey::MoveRight, SDLK_RIGHT);
    loadKeyBinding(binds, jBinds, "up", InputKey::MoveUp, SDLK_UP);
    loadKeyBinding(binds, jBinds, "down", InputKey::MoveDown, SDLK_DOWN);
    loadKeyBinding(binds, jBinds, "jump", InputKey::Jump, SDLK_z);
    loadKeyBinding(binds, jBinds, "attack", InputKey::Attack, SDLK_x);
}

void AppSettings::load(AppSettings& settings, const char* file)
{
    std::string json = file::readAllText(file);
    Document jRoot;
    jRoot.Parse(json.c_str());
    if (jRoot.GetParseError() != kParseErrorNone)
    {
        jRoot.Parse("{}");
    }

    settings.fullscreen = jRoot.HasMember("fullscreen") ?
        jRoot["fullscreen"].GetBool() : false;
    settings.width = jRoot.HasMember("width") ?
        jRoot["width"].GetDouble() : 640;
    settings.height = jRoot.HasMember("height") ?
        jRoot["height"].GetDouble() : 360;

    if (jRoot.HasMember("keys") && jRoot["keys"].IsObject())
    {
        loadKeyBindings(settings.bindings, jRoot["keys"]);
    }
    else
    {
        //Keys does not exist so we parse nothing to get the defaults
        Value temp;
        temp.SetObject();
        loadKeyBindings(settings.bindings, temp);
    }
}



void saveKeyBindings(PrettyWriter<StringBuffer>& writer, const KeyBindings& binds)
{
    writer.String("keys");
    writer.StartObject();

    writer.String("left"); writer.String(SDL_GetKeyName(binds.getKeycode(InputKey::MoveLeft)));
    writer.String("right"); writer.String(SDL_GetKeyName(binds.getKeycode(InputKey::MoveRight)));
    writer.String("up"); writer.String(SDL_GetKeyName(binds.getKeycode(InputKey::MoveUp)));
    writer.String("down"); writer.String(SDL_GetKeyName(binds.getKeycode(InputKey::MoveDown)));
    writer.String("jump"); writer.String(SDL_GetKeyName(binds.getKeycode(InputKey::Jump)));
    writer.String("attack"); writer.String(SDL_GetKeyName(binds.getKeycode(InputKey::Attack)));

    writer.EndObject();
}

void AppSettings::save(const AppSettings& settings, const char* file)
{
    StringBuffer buf;
    PrettyWriter<StringBuffer> writer(buf);

    writer.StartObject();
    writer.String("fullscreen"); writer.Bool(settings.fullscreen);
    writer.String("width"); writer.Int(settings.width);
    writer.String("height"); writer.Int(settings.height);
    saveKeyBindings(writer, settings.bindings);
    writer.EndObject();

    //Write to file
    file::writeAllText(file, buf.GetString());
}
