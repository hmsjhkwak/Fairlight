#ifndef SCENE_TRANSFORM_SYSTEM_H
#define SCENE_TRANSFORM_SYSTEM_H

#include <EASTL/hash_map.h>
#include "Core/SoaVector.h"
#include "Core/Features.h"
#include "Math/Vector2i.h"
#include "Util/Archives.h"
#include "Entity.h"
#include "EInstance.h"

namespace asset { class PackFile; }
using namespace asset;
using namespace math;
using namespace util;

namespace scene
{
    class EntityManager;

    class TransformSystem
    {
    private:
        eastl::hash_map<Entity, EInstance> _map;

        CLASS_SOA_VECTOR7(Storage,
            Entity, entities,
            Vector2i, localPos,
            Vector2i, worldPos,
            EInstance, parent,
            EInstance, firstChild,
            EInstance, nextSib,
            EInstance, prevSib);
        Storage _data;

    public:
        TransformSystem();

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
            }

            //Serialize fields
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.entities, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.localPos, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.worldPos, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.parent, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.firstChild, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.nextSib, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.prevSib, length);

            //Add entities to map
            if (ar.IsReading)
            {
                for (uint32_t i = 0; i < length; i++)
                {
                    _map[_data.entities[i]] = i;
                }
            }
        }

        inline bool exists(Entity e) { return _map.find(e) != _map.end(); }
        EInstance create(Entity e);
        inline EInstance createOrGetInstance(Entity e)
        {
            if (exists(e)) { return getInstance(e); }
            else { return create(e); }
        }
        void destroy(Entity e);
        const uint8_t* instantiate(Entity e, const uint8_t* data);

        void handleDestroyed(EntityManager& entityManager);
        void handleDestroyedChildren(EntityManager& entityManager, EInstance ei);

        inline EInstance getInstance(Entity e)
        {
            NW_ASSERT(exists(e));
            return _map[e];
        }
        inline Entity getEntity(EInstance ei) { return _data.entities[ei.index]; }
        inline Vector2i getLocalPos(EInstance ei) { return _data.localPos[ei.index]; }
        inline Vector2i getWorldPos(EInstance ei) { return _data.worldPos[ei.index]; }
        inline EInstance getParent(EInstance ei) { return _data.parent[ei.index]; }
        inline EInstance getFirstChild(EInstance ei) { return _data.firstChild[ei.index]; }
        inline EInstance getNextSib(EInstance ei) { return _data.nextSib[ei.index]; }
        inline EInstance getPrevSib(EInstance ei) { return _data.prevSib[ei.index]; }

        void setLocalPos(EInstance ei, const Vector2i& localPos);
        void setWorldPos(EInstance ei, const Vector2i& worldPos);
        void setParent(EInstance ei, EInstance parent);

    private:
        void removeChild(EInstance ei);
        void moveInstance(EInstance dst, EInstance src);

        void updateWorldPos(EInstance ei, const Vector2i& parPos);
    };
}

#endif
