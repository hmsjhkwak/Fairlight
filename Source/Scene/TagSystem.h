#ifndef SCENE_TAG_SYSTEM_H
#define SCENE_TAG_SYSTEM_H

#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include "Core/BuddyAllocator.h"
#include "Core/SoaVector.h"
#include "Core/Features.h"
#include "Util/Archives.h"
#include "Entity.h"
#include "EInstance.h"

namespace asset { class PackFile; class AssetManager; }
using namespace asset;
using namespace memory;
using namespace util;

namespace scene
{
    class TagSystem
    {
    private:
        eastl::hash_map<Entity, EInstance> _map;
        CLASS_SOA_VECTOR2(Storage,
            Entity, entities,
            uint32_t, tagsOffset);   //Stored as byte offsets into the buddy allocator to reduce pointer patching
        Storage _data;

        BuddyAllocator _buddy;

    public:
        TagSystem();
        void init();

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

            //TODO: Come up with a faster way to save/load data in bulk?
            for (uint32_t i = 0; i < length; i++)
            {
                EInstance ei(i);
                uint32_t tagLen;

                //If writing, update the length
                if (ar.IsWriting) { tagLen = getLength(ei); }

                ar.serializeU32(tagLen);

                //If reading, allocate space for tags
                if (ar.IsReading)
                {
                    //Manually create
                    size_t allocSize = (tagLen + 2) * sizeof(uint32_t);
                    allocSize = _buddy.getActualAllocSize(allocSize);
                    uint32_t* mem = (uint32_t*)_buddy.alloc(allocSize);

                    _data.tagsOffset[ei.index] = getOffset(mem);
                    getLength(ei) = tagLen;
                    getCapacity(ei) = (uint32_t)(allocSize / sizeof(uint32_t)) - 2;
                }

                AR_SERIALIZE_ARRAY_U32(ar, getTags(ei), tagLen);
            }

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

        bool exists(Entity e) { return _map.find(e) != _map.end(); }
        EInstance create(Entity e, uint32_t tagCount = 0);
        EInstance createOrGetInstance(Entity e)
        {
            if (exists(e)) { return getInstance(e); }
            else { return create(e); }
        }
        void destroy(Entity e);
        const uint8_t* instantiate(Entity e, const uint8_t* data);

        void handleDestroyed(const Entity* destroyed, size_t destroyedLen);

        EInstance getInstance(Entity e)
        {
            NW_ASSERT(exists(e));
            return _map[e];
        }

        void addTag(EInstance ei, uint32_t tag);
        void removeTag(EInstance ei, uint32_t tag);
        bool hasTag(EInstance ei, uint32_t tag);

        uint32_t& getLength(EInstance ei) { return getPointer(ei)[0]; }
        uint32_t& getCapacity(EInstance ei) { return getPointer(ei)[1]; }
        uint32_t* getTags(EInstance ei) { return &getPointer(ei)[2]; }

    private:
        void moveInstance(EInstance dst, EInstance src);

        Entity getEntity(EInstance ei) { return _data.entities[ei.index]; }

        uint32_t* getPointer(EInstance ei)
        {
            return (uint32_t*)((uintptr_t)_buddy.getBaseAddress() + _data.tagsOffset[ei.index]);
        }

        uint32_t getOffset(uint32_t* ptr)
        {
            return (uint32_t)((uintptr_t)ptr - (uintptr_t)_buddy.getBaseAddress());
        }
    };
}

#endif
