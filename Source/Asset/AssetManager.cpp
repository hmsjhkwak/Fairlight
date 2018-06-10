#include "Core/Core.h"
#include "AssetManager.h"
#include "Render/RenderCommon.h"
#include "Scene/Scene.h"
#include "Util/Archives.h"
#include "Core/xxhash/xxhash.h"

namespace asset
{
    void AssetManager::loadPackFile(const char* fileName)
    {
        _packFile.load(fileName);
    }

    AssetRef AssetManager::getAssetRefFromName(const char* fileName)
    {
        return AssetRef(XXH32(fileName, strlen(fileName), _packFile.getHashSeed()));
    }

    void AssetManager::loadTextures(AssetRef* refs, uint32_t count)
    {
        eastl::vector<uint8_t> buffer;
        buffer.resize(1024);    //Let's just pick some arbitrary starting point
        for (uint32_t i = 0; i < count; i++)
        {
            //Skip it if it's already loaded
            auto search = _textures.find(refs[i]);
            if (search != _textures.end()) { continue; }

            auto span = _packFile.getFileSpan(refs[i]);

            bool isValid = (span.size > 0) && (span.compressedSize > 0) && (span.assetType != AssetType::Unknown);
            NW_REQUIRE(isValid);
            if (isValid)
            {
                buffer.reserve(span.size);

                //Read into buffer
                _packFile.decompress(span, &buffer[0]);

                //Create texture
                bgfx::TextureHandle tex = render::createTexture(buffer.data(), (uint32_t)buffer.size());
                _textures.insert(eastl::make_pair(refs[i], tex));
            }
        }
    }

    void AssetManager::loadShaders(AssetRef* refs, uint32_t count)
    {
        _packFile.lock();   //TEMP: Should have this loaded by the scene

        eastl::vector<uint8_t> buffer;
        buffer.resize(1024);    //Let's just pick some arbitrary starting point
        for (uint32_t i = 0; i < count; i++)
        {
            //Skip it if it's already loaded
            auto search = _shaders.find(refs[i]);
            if (search != _shaders.end()) { continue; }

            auto span = _packFile.getFileSpan(refs[i]);
            buffer.reserve(span.size);

            //Read into buffer
            _packFile.decompress(span, &buffer[0]);

            //Create and store shader
            bgfx::ShaderHandle shader = bgfx::createShader(bgfx::copy(buffer.data(), span.size));
            _shaders.insert(eastl::make_pair(refs[i], shader));
        }

        _packFile.unlock(); //TEMP: Should have this loaded by the scene
    }

    void AssetManager::loadSounds(AssetRef* refs, uint32_t count)
    {
        struct SoundData
        {
            AssetRef asset;
            FileSpan span;
        };

        eastl::vector<SoundData> tempSoundData;
        size_t len = 0;
        for (uint32_t i = 0; i < count; i++)
        {
            //Skip it if it's already loaded
            auto search = _sounds.find(refs[i]);
            if (search != _sounds.end()) { continue; }

            SoundData data;
            data.asset = refs[i];
            data.span = _packFile.getFileSpan(refs[i]);
            len += data.span.size;
            tempSoundData.push_back(data);
        }

        if (tempSoundData.size() < 1) return;

        //Start of pointer patching: turn pointers into offsets
        for (auto& chunk : _sounds)
        {
            chunk.second.abuf = (uint8_t*)(chunk.second.abuf - &_soundData[0]);
        }

        //Resize it just once
        size_t pos = _soundData.size();
        _soundData.resize(pos + len);

        //End of pointer patching: turn offset into pointers
        for (auto& chunk : _sounds)
        {
            chunk.second.abuf = (uint8_t*)((size_t)chunk.second.abuf + (size_t)&_soundData[0]);
        }

        //Now add all the new ones
        for (size_t i = 0; i < tempSoundData.size(); i++)
        {
            FileSpan& span = tempSoundData[i].span;

            //Read into buffer
            _packFile.decompress(span, &_soundData[pos]);

            //Create and store Mix_Chunk
            Mix_Chunk chunk;
            chunk.allocated = 0;
            chunk.volume = 128;
            chunk.alen = tempSoundData[i].span.size;
            chunk.abuf = &_soundData[pos];
            _sounds.insert(eastl::make_pair(tempSoundData[i].asset, chunk));

            pos += tempSoundData[i].span.size;
        }
    }

    void AssetManager::loadScript(AssetRef ref, eastl::string& chunkName, eastl::string& code)
    {
        _packFile.lock();

        auto span = _packFile.getFileSpan(ref);
        void* buffer = malloc(span.size);
        _packFile.decompress(span, buffer);

        uint32_t chunkLen = ((uint32_t*)buffer)[0];
        uint32_t codeLen = ((uint32_t*)buffer)[1];

        chunkName.reserve(chunkLen + 1);
        chunkName.force_size(chunkLen + 1);
        memcpy(&chunkName[0], (char*)buffer + 8, chunkLen);
        chunkName[chunkLen] = 0;

        code.reserve(codeLen + 1);
        code.force_size(codeLen + 1);
        memcpy(&code[0], (char*)buffer + chunkLen + 8, codeLen);
        code[codeLen] = 0;

        free(buffer);

        _packFile.unlock();
    }

    bgfx::TextureHandle AssetManager::getTexture(AssetRef ref)
    {
        auto search = _textures.find(ref);
        if (search != _textures.end())
        {
            return search->second;
        }
        else
        {
            //Default texture
            return BGFX_INVALID_HANDLE;
        }
    }

    bgfx::ShaderHandle AssetManager::getShader(AssetRef ref)
    {
        auto search = _shaders.find(ref);
        if (search != _shaders.end())
        {
            return search->second;
        }
        else
        {
            //Default shader
            return BGFX_INVALID_HANDLE;
        }
    }

    Mix_Chunk* AssetManager::getSound(AssetRef ref)
    {
        auto search = _sounds.find(ref);
        if (search != _sounds.end())
        {
            return &search->second;
        }
        else
        {
            return nullptr;
        }
    }

    Mix_Music* AssetManager::streamMusic(AssetRef ref)
    {
        _music.init(_packFile, ref);
        return _music.music;
    }

#ifdef NW_EDITOR
    void AssetManager::saveScene(const char* fileName, const Scene& scene)
    {
        NW_UNUSED(fileName);
        NW_UNUSED(scene);
    }
#endif
}
