#include "Core/Core.h"
#include "Scene.h"
#include "Asset/PackFile.h"
#include "Asset/AssetManager.h"
#include "Render/RenderCommon.h"
#include "Render/RenderManager.h"
#include "Util/Archives.h"

namespace scene
{
    Scene::Scene() :
        _deltaTime(0),
        _sceneTime(0)
    {
    }

    template <typename Archive>
    void loadAssets(AssetManager& assetMan, Archive& ar)
    {
        //Read lengths
        uint32_t texturesLen, soundsLen;
        ar.serializeU32(texturesLen);
        ar.serializeU32(soundsLen);

        //Read asset hashes
        eastl::vector<AssetRef> textures, sounds;
        if (texturesLen > 0)
        {
            textures.resize(texturesLen);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, textures.data(), texturesLen);
        }
        if (soundsLen > 0)
        {
            sounds.resize(soundsLen);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, sounds.data(), soundsLen);
        }

        //Load assets through asset manager
        //(We're done with the pack file here so it's okay)
        if (texturesLen > 0)
        {
            assetMan.loadTextures(&textures[0], texturesLen);
        }
        if (soundsLen > 0)
        {
            assetMan.loadSounds(&sounds[0], soundsLen);
        }
    }

    template <typename Archive>
    void Scene::serialize(Archive& ar)
    {
        AR_SERIALIZE_MAP(ar, _prefabMap, serializeCustom, serializeCustom);
        AR_SERIALIZE_VECTOR(ar, _prefabData, serializeU8);

        _tagSystem.serialize(ar);
        _trSystem.serialize(ar);
        _spriteSystem.serialize(ar);
        _moveSystem.serialize(ar);
        _scriptSystem.serialize(ar);
        _tileSystem.serialize(ar);
    }

    void Scene::load(AssetManager& assetMan, PackFile& pack, const FileSpan& span, script::AngelState& angelState)
    {
        void* memory = malloc(span.size);

        _tagSystem.init();
        _scriptSystem.init(angelState);

        pack.lock();
        pack.decompress(span, memory);
        pack.unlock();

        util::MemoryReadArchive ar;
        ar.init(memory, span.size);

        uint32_t entityCount;
        ar.serializeU32(entityCount);

        if (ar.IsReading)
        {
            for (uint32_t i = 0; i < entityCount; i++)
            {
                _entityManager.create();
            }
        }

        serialize(ar);

        //Load scene assets (reads remainder of scene file)
        pack.lock();
        loadAssets(assetMan, ar);
        pack.unlock();

        _spriteSystem.prepare(assetMan);
        _tileSystem.prepare(assetMan);
        _camSystem.prepare(_tileSystem);

        free(memory);
    }

#ifdef NW_ASSET_COOK
    void Scene::save(EndianVectorWriteArchive& ar, uint32_t entityCount)
    {
        ar.serializeU32(entityCount);

        serialize(ar);
    }

    void Scene::addPrefab(AssetRef ref, const uint8_t* buffer, uint32_t length)
    {
        //Store the offset
        _prefabMap.insert(eastl::make_pair(
            ref, PrefabData{ (uint32_t)_prefabData.size() }));

        for (uint32_t i = 0; i < length; i++)
        {
            _prefabData.push_back(buffer[i]);
        }
    }
#endif

    void Scene::handleInstantiated(AssetManager& assetMan)
    {
        _spriteSystem.handleInstantiated(assetMan);
    }

    void Scene::update(AssetManager& assetMan, uint32_t deltaTime)
    {
        SCOPED_CPU_EVENT(event)(0xFFFFFFFF, "Scene::update");

        _deltaTime = deltaTime;
        _sceneTime += deltaTime;

        _moveSystem.update(getDeltaTime(), _trSystem, _tileSystem);
        _scriptSystem.update();

        //Call again in case the script system has created entities
        handleInstantiated(assetMan);

        //Handle destroyed entities
        //Transform system is first since it dumps children on the destroyed list
        _trSystem.handleDestroyed(_entityManager);
        const auto& destroyed = _entityManager.pollDestroyed();
        if (destroyed.size() > 0)
        {
            _spriteSystem.handleDestroyed(destroyed.data(), destroyed.size());
            _moveSystem.handleDestroyed(destroyed.data(), destroyed.size());
            _scriptSystem.handleDestroyed(destroyed.data(), destroyed.size());
            _tagSystem.handleDestroyed(destroyed.data(), destroyed.size());
        }
        _entityManager.clearDestroyed();
    }

    void Scene::render(RenderManager& renderManager)
    {
        SCOPED_CPU_EVENT(event)(PROF_COLOR_GRAPHICS, "Scene::render");

        bgfx::setViewFrameBuffer(VIEW_ID_SCENE, renderManager.globalRes.sceneFrameBuffer);
        bgfx::setViewRect(VIEW_ID_SCENE, 0, 0, 640, 360);
        bgfx::setViewScissor(VIEW_ID_SCENE, 0, 0, 640, 360);
        bgfx::setViewClear(VIEW_ID_SCENE, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000FF, 1.0f);

        bgfx::touch(VIEW_ID_SCENE);

        Renderer2d renderer = renderManager.getRenderer2d();
        renderer.setView(_camSystem.getView());
        _tileSystem.render(renderer, _camSystem.getView());
        _spriteSystem.render(_trSystem, renderer);
    }



    Scene::PrefabData Scene::getPrefab(AssetRef ref)
    {
        return _prefabMap[ref];
    }

    Entity Scene::instantiate(PrefabData prefab)
    {
        Entity e = _entityManager.create();
        instantiate(e, prefab);
        return e;
    }

    void Scene::instantiate(Entity e, PrefabData prefab)
    {
        uint8_t components = _prefabData[prefab.offset];
        const uint8_t* data = &_prefabData[prefab.offset + 1];

        if (components >> 0 & 1)
        {
            data = _tagSystem.instantiate(e, data);
        }

        if (components >> 1 & 1)
        {
            data = _trSystem.instantiate(e, data);
        }

        if (components >> 2 & 1)
        {
            data = _spriteSystem.instantiate(e, data);
        }

        if (components >> 3 & 1)
        {
            data = _moveSystem.instantiate(e, data);
        }

        if (components >> 4 & 1)
        {
            data = _scriptSystem.instantiate(e, data);
        }
    }
}
