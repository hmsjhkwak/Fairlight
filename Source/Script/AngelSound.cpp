#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include <SDL_mixer.h>
#include "Math/Math.h"
#include "Asset/AssetManager.h"

using namespace asset;
using namespace math;

namespace script
{
    int convertVolumef(float vol)
    {
        return (int)(max(min(vol, 1.0f), 0.0f) * MIX_MAX_VOLUME);
    }

    int angelSound_playSound(AssetRef asset, float volume)
    {
        AssetManager* assetManager = AngelState::getCurrent()->getAssetManager();
        int channel = Mix_PlayChannel(-1, assetManager->getSound(asset), 0);
        Mix_Volume(channel, convertVolumef(volume));

        //Return channel so that the volume can be changed anytime
        return channel;
    }

    void angelSound_setSoundVolume(int channel, float volume)
    {
        Mix_Volume(channel, convertVolumef(volume));
    }

    void angelSound_playMusic(AssetRef asset, float volume, bool loops)
    {
        AssetManager* assetManager = AngelState::getCurrent()->getAssetManager();
        Mix_Music* music = assetManager->streamMusic(asset);
        Mix_PlayMusic(music, loops ? -1 : 1);
        Mix_VolumeMusic(convertVolumef(volume));
    }

    bool angelSound_isMusicPlaying()
    {
        return (Mix_PlayingMusic() != 0);
    }

    void angelSound_setMusicVolume(float volume)
    {
        Mix_VolumeMusic(convertVolumef(volume));
    }

    void angelSound_RegisterTypes(asIScriptEngine* engine)
    {
        AS_VERIFY(engine->RegisterGlobalFunction("int playSound(AssetRef asset, float volume = 1.0f)", asFUNCTION(angelSound_playSound), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("void setSoundVolume(int channel, float volume)", asFUNCTION(angelSound_setSoundVolume), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("void playMusic(AssetRef asset, float volume, bool loops)", asFUNCTION(angelSound_playMusic), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("bool isMusicPlaying()", asFUNCTION(angelSound_isMusicPlaying), asCALL_CDECL));
        AS_VERIFY(engine->RegisterGlobalFunction("void setMusicVolume(float volume)", asFUNCTION(angelSound_setMusicVolume), asCALL_CDECL));
    }
}
