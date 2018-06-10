#ifndef SCENE_SPRITE_SYSTEM_H
#define SCENE_SPRITE_SYSTEM_H

#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include <bgfx/bgfx.h>
#include "Core/SoaVector.h"
#include "Core/Features.h"
#include "Asset/AssetManager.h"
#include "Math/Vector2i.h"
#include "Entity.h"
#include "EInstance.h"

namespace asset { class PackFile; }
namespace render { class Renderer2d; }
using namespace math;
using namespace render;

namespace scene
{
    class TransformSystem;

    class SpriteSystem
    {
    private:
        eastl::hash_map<Entity, EInstance> _map;
        struct Misc
        {
            uint8_t depth;
            uint8_t alpha;
            union
            {
                struct
                {
                    uint8_t horTexFlip : 1;
                    uint8_t verTexFlip : 1;
                    uint8_t rotation : 2;
                };
                uint8_t _flags;
            };
            template <typename Archive>
            void serialize(Archive& ar)
            {
                ar.serializeU8(depth);
                ar.serializeU8(alpha);
                ar.serializeU8(_flags);
            }
        };
        CLASS_SOA_VECTOR7(Storage,
            Entity, entities,
            Vector2i, size,
            Vector2i, offset,
            Vector2i, texOffset,
            asset::AssetRef, textureRef,
            bgfx::TextureHandle, texture,
            Misc, misc);
        Storage _data;

        //Keeps track of which components have been instantiated this frame
        //Used primarily so that we can get their texture references in one place
        eastl::vector<Entity> _instantiated;

    public:
        SpriteSystem();

        template <typename Archive>
        void serialize(Archive& ar)
        {
            uint32_t length = _data.getSize();
            ar.serializeU32(length);

            if (length == 0)
            {
                return;
            }

            //If we're reading, then preallocate some space
            if (ar.IsReading)
            {
                _data.resize(length + 128);
                _data.setSize(length);

                memset(_data.texture, 0, length * sizeof(_data.texture[0]));
            }

            //Serialize fields
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.entities, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.size, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.offset, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.textureRef, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.texOffset, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.misc, length);

            //Add entities to map
            if (ar.IsReading)
            {
                for (uint32_t i = 0; i < length; i++)
                {
                    _map[_data.entities[i]] = i;
                }
            }
        }

        void prepare(asset::AssetManager& assetMan);
#ifdef NW_ASSET_COOK
        uint8_t* convertToPrefab(Entity e, uint8_t* buffer);
#endif

        void handleInstantiated(asset::AssetManager& assetMan);
        void render(TransformSystem& trSystem, Renderer2d& renderer);

        bool exists(Entity e) { return _map.find(e) != _map.end(); }
        EInstance create(Entity e);
        EInstance createOrGetInstance(Entity e)
        {
            if (exists(e)) { return getInstance(e); }
            else { return create(e); }
        }
        void destroy(Entity e);
        const uint8_t* instantiate(Entity e, const uint8_t* data);

        void handleDestroyed(const Entity* destroyed, size_t destroyedLen);

        inline EInstance getInstance(Entity e)
        {
            NW_ASSERT(exists(e));
            return _map[e];
        }
        inline Vector2i getSize(EInstance ei) { return _data.size[ei.index]; }
        inline Vector2i getOffset(EInstance ei) { return _data.offset[ei.index]; }
        inline uint8_t getDepth(EInstance ei) { return _data.misc[ei.index].depth; }
        inline uint8_t getAlpha(EInstance ei) { return _data.misc[ei.index].alpha; }
        inline asset::AssetRef getTextureRef(EInstance ei) { return _data.textureRef[ei.index]; }
        inline bgfx::TextureHandle getTexture(EInstance ei) { return _data.texture[ei.index]; }
        inline Vector2i getTexOffset(EInstance ei) { return _data.texOffset[ei.index]; }

        inline void setSize(EInstance ei, const Vector2i& size) { _data.size[ei.index] = size; }
        inline void setOffset(EInstance ei, const Vector2i& offset) { _data.offset[ei.index] = offset; }
        inline void setDepth(EInstance ei, uint8_t depth) { _data.misc[ei.index].depth = depth; }
        inline void setAlpha(EInstance ei, uint8_t alpha) { _data.misc[ei.index].alpha = alpha; }
        inline void setTextureRef(EInstance ei, asset::AssetRef ref)
        {
            _data.textureRef[ei.index] = ref;
            _instantiated.push_back(getEntity(ei));
        }
        inline void setTexOffset(EInstance ei, Vector2i texOffset) { _data.texOffset[ei.index] = texOffset; }
        inline void setHorFlip(EInstance ei, bool flip) { _data.misc[ei.index].horTexFlip = flip; }
        inline void setVerFlip(EInstance ei, bool flip) { _data.misc[ei.index].verTexFlip = flip; }
        inline void setRotation(EInstance ei, int rotation) { _data.misc[ei.index].rotation = rotation / 90; }

    private:
        void moveInstance(EInstance dst, EInstance src);

        inline Entity getEntity(EInstance ei) { return _data.entities[ei.index]; }
    };
}

#endif
