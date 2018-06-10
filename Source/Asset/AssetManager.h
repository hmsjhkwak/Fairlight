#ifndef ASSET_ASSET_MANAGER_H
#define ASSET_ASSET_MANAGER_H

#include "../Core/Features.h"
#include <stdint.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>
#include <bgfx/bgfx.h>
#include <SDL_mixer.h>
#include "PackFile.h"
#include "MusicStream.h"

namespace scene { class Scene; }

using scene::Scene;



namespace asset
{
    class AssetManager
    {
    private:
        PackFile _packFile;
        eastl::hash_map<AssetRef, bgfx::TextureHandle> _textures;
        eastl::hash_map<AssetRef, bgfx::ShaderHandle> _shaders;
        eastl::hash_map<AssetRef, Mix_Chunk> _sounds;
        eastl::vector<uint8_t> _soundData;
        MusicStream _music;

    public:
        void loadPackFile(const char* fileName);
        AssetRef getAssetRefFromName(const char* fileName);
        PackFile& getPackFile() { return _packFile; }

        void loadTextures(AssetRef* refs, uint32_t count);
        void loadShaders(AssetRef* refs, uint32_t count);
        void loadSounds(AssetRef* refs, uint32_t count);

        void loadScript(AssetRef ref, eastl::string& chunkName, eastl::string& code);

        bgfx::TextureHandle getTexture(AssetRef ref);
        bgfx::ShaderHandle getShader(AssetRef ref);
        Mix_Chunk* getSound(AssetRef ref);

        Mix_Music* streamMusic(AssetRef ref);

#ifdef NW_EDITOR
        void saveScene(const char* fileName, const Scene& scene);
#endif
    };
}

#endif
