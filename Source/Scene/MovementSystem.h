#ifndef SCENE_MOVEMENT_SYSTEM_H
#define SCENE_MOVEMENT_SYSTEM_H

#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include "Core/SoaVector.h"
#include "Core/Features.h"
#include "Math/Vector2i.h"
#include "Math/Vector2f.h"
#include "Math/IntRect.h"
#include "Entity.h"
#include "EInstance.h"

namespace asset { class PackFile; class AssetManager; }
using namespace asset;
using namespace math;

namespace scene
{
    class TransformSystem;
    class TileSystem;

    struct CollisionPair { Entity e1; Entity e2; };

    class MovementSystem
    {
    private:
        eastl::hash_map<Entity, EInstance> _map;

        //Number of entities that have world collision enabled
        //All WC enabled entities are grouped at the beginning of the array
        uint32_t _worldCollLen;

        CLASS_SOA_VECTOR5(Storage,
            Entity, entities,
            Vector2i, size,
            Vector2i, offset,
            Vector2f, velocity,
            Vector2f, partialPos);
        Storage _data;

        eastl::vector<CollisionPair> _collisionPairs;

    public:
        MovementSystem();

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

                memset(_data.partialPos, 0, sizeof(_data.partialPos[0]) * length);
            }

            ar.serializeU32(_worldCollLen);

            //Serialize fields
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.entities, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.size, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.offset, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.velocity, length);

            //Add entities to map
            if (ar.IsReading)
            {
                for (uint32_t i = 0; i < length; i++)
                {
                    _map[_data.entities[i]] = i;
                }
            }
        }

#ifdef NW_ASSET_COOK
        uint8_t* convertToPrefab(Entity e, uint8_t* buffer);
#endif

        void update(float dt, TransformSystem& trSystem, const TileSystem& tileSystem);
        void updateWorldColl(float dt, TransformSystem& trSystem, const TileSystem& tileSystem);
        void updateNonWorldColl(float dt, TransformSystem& trSystem, const TileSystem& tileSystem);
        void recordCollisions(TransformSystem& trSystem);

        bool exists(Entity e);
        EInstance create(Entity e);
        EInstance createOrGetInstance(Entity e);
        void destroy(Entity e);
        const uint8_t* instantiate(Entity e, const uint8_t* data);

        void handleDestroyed(const Entity* destroyed, size_t destroyedLen);

        EInstance getInstance(Entity e)
        {
            NW_ASSERT(exists(e));
            return _map[e];
        }
        inline bool getWorldCollision(EInstance ei) { return ei.index < _worldCollLen; }
        inline Vector2i getSize(EInstance ei) { return _data.size[ei.index]; }
        inline Vector2i getOffset(EInstance ei) { return _data.offset[ei.index]; }
        inline Vector2f getVelocity(EInstance ei) { return _data.velocity[ei.index]; }

        void setWorldCollision(EInstance ei, bool worldColl);
        inline void setSize(EInstance ei, const Vector2i& size) { _data.size[ei.index] = size; }
        inline void setOffset(EInstance ei, const Vector2i& offset) { _data.offset[ei.index] = offset; }
        inline void setVelocity(EInstance ei, const Vector2f& velocity) { _data.velocity[ei.index] = velocity; }

        //Collision detection stuff
        bool intersectsWorld(TransformSystem& tr, TileSystem& tile, EInstance ei);
        uint32_t getFirstCollisionIndex(Entity e);
        CollisionPair getCollision(uint32_t index);
        IntRect getCollRect(TransformSystem& tr, EInstance ei);

    private:
        void moveInstance(EInstance dst, EInstance src);
        void swapInstances(EInstance inst1, EInstance inst2);

        Entity getEntity(EInstance ei) { return _data.entities[ei.index]; }
    };
}

#endif
