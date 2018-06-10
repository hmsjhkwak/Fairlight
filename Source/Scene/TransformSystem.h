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

        struct TransformData
        {
            Vector2i localPos;
            Vector2i worldPos;

            template <typename Archive>
            void serialize(Archive& ar)
            {
                ar.serializeCustom(localPos);
                ar.serializeCustom(worldPos);
            }
        };

        struct HierarchyData
        {
            EInstance parent;
            EInstance firstChild;
            EInstance nextSib;
            EInstance prevSib;

            template <typename Archive>
            void serialize(Archive& ar)
            {
                ar.serializeCustom(parent);
                ar.serializeCustom(firstChild);
                ar.serializeCustom(nextSib);
                ar.serializeCustom(prevSib);
            }
        };

        CLASS_SOA_VECTOR3(Storage,
            Entity, entities,
            TransformData, trData,
            HierarchyData, hierData);
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
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.trData, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.hierData, length);

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
        inline Vector2i getLocalPos(EInstance ei) { return _data.trData[ei.index].localPos; }
        inline Vector2i getWorldPos(EInstance ei) { return _data.trData[ei.index].worldPos; }
        inline EInstance getParent(EInstance ei) { return _data.hierData[ei.index].parent; }
        inline EInstance getFirstChild(EInstance ei) { return _data.hierData[ei.index].firstChild; }
        inline EInstance getNextSib(EInstance ei) { return _data.hierData[ei.index].nextSib; }
        inline EInstance getPrevSib(EInstance ei) { return _data.hierData[ei.index].prevSib; }

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
