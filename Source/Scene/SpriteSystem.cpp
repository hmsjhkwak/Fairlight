#include "Core/Core.h"
#include "SpriteSystem.h"
#include "TransformSystem.h"
#include "Asset/PackFile.h"
#include "Asset/AssetManager.h"
#include "Render/RenderCommon.h"
#include "Render/Renderer2d.h"

using namespace asset;

namespace scene
{
    SpriteSystem::SpriteSystem()
    {
    }

    void SpriteSystem::prepare(AssetManager& assetMan)
    {
        //We need to get the actual texture from the hash
        for (uint32_t i = 0; i < _data.getSize(); i++)
        {
            _data.texture[i] = assetMan.getTexture(_data.textureRef[i]);
        }
    }

#ifdef NW_ASSET_COOK
    uint8_t* SpriteSystem::convertToPrefab(Entity e, uint8_t* buffer)
    {
        EInstance ei = getInstance(e);

        Vector2i size = getSize(ei);
        Vector2i offset = getOffset(ei);
        uint8_t depth = getDepth(ei);
        uint8_t alpha = getAlpha(ei);
        AssetRef textureRef = getTextureRef(ei);
        Vector2i texOffset = getTexOffset(ei);

        memcpy(buffer, &size, sizeof(size)); buffer += sizeof(size);
        memcpy(buffer, &offset, sizeof(offset)); buffer += sizeof(offset);
        memcpy(buffer, &depth, sizeof(depth)); buffer += sizeof(depth);
        memcpy(buffer, &alpha, sizeof(alpha)); buffer += sizeof(alpha);
        memcpy(buffer, &textureRef.hash, sizeof(textureRef.hash)); buffer += sizeof(textureRef.hash);   //HACK UNTIL PROPER SERIALIZATION
        memcpy(buffer, &texOffset, sizeof(texOffset)); buffer += sizeof(texOffset);

        return buffer;
    }
#endif

    void SpriteSystem::handleInstantiated(AssetManager& assetMan)
    {
        //Just like in prepare(), we need to map hash -> texture
        for (Entity e : _instantiated)
        {
            EInstance ei = _map[e];
            _data.texture[ei.index] = assetMan.getTexture(_data.textureRef[ei.index]);
        }
        _instantiated.clear();
    }

    void SpriteSystem::render(TransformSystem& trSystem, Renderer2d& renderer)
    {
        SCOPED_CPU_EVENT(event)(PROF_COLOR_GRAPHICS, "SpriteSystem::render");
        for (size_t i = 0; i < _data.getSize(); i++)
        {
            Vector2i texFlip(
                _data.misc[i].horTexFlip ? -1 : 1,
                _data.misc[i].verTexFlip ? -1 : 1);
            float rotation = _data.misc[i].rotation * 90.0f;
            renderer.submitSprite(
                trSystem.getWorldPos(trSystem.getInstance(_data.entities[i])) + _data.offset[i],
                _data.size[i], _data.misc[i].depth, _data.misc[i].alpha,
                _data.texture[i], _data.texOffset[i], texFlip, rotation);
        }
    }

    EInstance SpriteSystem::create(Entity e)
    {
        EInstance ei = EInstance(_data.getSize());
        _map.insert(eastl::make_pair(e, ei));

        Misc misc;
        misc.depth = 0;
        misc.alpha = 255;
        misc._flags = 0;

        _data.push(e,
            Vector2i(0, 0), Vector2i(0, 0), Vector2i(0, 0),
            AssetRef(), BGFX_INVALID_HANDLE,
            misc);

        return ei;
    }

    void SpriteSystem::destroy(Entity e)
    {
        NW_ASSERT(exists(e));

        const EInstance ei = _map[e];
        const EInstance lastInst(_data.getSize() - 1);
        const Entity lastEntity = getEntity(lastInst);

        //Copy the last component to the removed position
        moveInstance(ei, lastInst);

        //Remove last
        _data.pop();

        //Update the keys in the map
        _map[lastEntity] = ei;
        _map.erase(e);
    }

    const uint8_t* SpriteSystem::instantiate(Entity e, const uint8_t* data)
    {
        EInstance ei = create(e);
        Vector2i size, offset, texOffset;
        uint8_t depth, alpha;
        AssetRef texRef;

        memcpy(&size, data, sizeof(size)); data += sizeof(size);
        memcpy(&offset, data, sizeof(offset)); data += sizeof(offset);
        memcpy(&depth, data, sizeof(depth)); data += sizeof(depth);
        memcpy(&alpha, data, sizeof(alpha)); data += sizeof(alpha);
        memcpy(&texRef.hash, data, sizeof(texRef.hash)); data += sizeof(texRef.hash);   //HACK UNTIL PROPER SERIALIZATION
        memcpy(&texOffset, data, sizeof(texOffset)); data += sizeof(texOffset);

        setSize(ei, size);
        setOffset(ei, offset);
        setDepth(ei, depth);
        setAlpha(ei, alpha);
        setTextureRef(ei, texRef);  //This will be resolved in handleInstantiated
        setTexOffset(ei, texOffset);

        _instantiated.push_back(e);

        return data;
    }

    void SpriteSystem::moveInstance(EInstance dst, EInstance src)
    {
        _data.move(dst.index, src.index);
    }

    void SpriteSystem::handleDestroyed(const Entity* destroyed, size_t destroyedLen)
    {
        for (size_t i = 0; i < destroyedLen; i++)
        {
            auto result = _map.find(destroyed[i]);
            if (result != _map.end())
            {
                destroy(result->first);
            }
        }
    }
}
