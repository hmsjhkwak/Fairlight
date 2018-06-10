#include "Core/Core.h"
#include "TagSystem.h"
#include "Asset/PackFile.h"

namespace scene
{
    //16 * 2 ^ 12 = 65k
    const size_t TAG_BUDDY_LEVELS = 12;
    const size_t TAG_LEAF_SIZE = 16;

    TagSystem::TagSystem()
    {
    }

    void TagSystem::init()
    {
        _buddy.init(TAG_LEAF_SIZE, TAG_BUDDY_LEVELS);
    }

#ifdef NW_ASSET_COOK
    uint8_t* TagSystem::convertToPrefab(Entity e, uint8_t* buffer)
    {
        EInstance ei = getInstance(e);

        //Write length
        uint32_t len = getLength(ei);
        memcpy(buffer, &len, sizeof(len)); buffer += sizeof(len);

        //Write tags
        uint32_t* tags = getTags(ei);
        memcpy(buffer, tags, sizeof(*tags) * len); buffer += sizeof(*tags) * len;

        return buffer;
    }
#endif



    EInstance TagSystem::create(Entity e, uint32_t tagCount)
    {
        EInstance ei = EInstance(_data.getSize());
        _map.insert(eastl::make_pair(e, ei));

        size_t allocSize = (tagCount + 2) * sizeof(uint32_t);
        allocSize = _buddy.getActualAllocSize(allocSize);

        //Alloc a starting block
        uint32_t* mem = (uint32_t*)_buddy.alloc(allocSize);

        _data.push(e, getOffset(mem));

        //Initialize with default tag data
        getLength(ei) = 0;
        getCapacity(ei) = (uint32_t)(allocSize / sizeof(uint32_t)) - 2;

        return ei;
    }

    void TagSystem::destroy(Entity e)
    {
        NW_ASSERT(exists(e));

        const EInstance ei = _map[e];
        const EInstance lastInst(_data.getSize() - 1);
        const Entity lastEntity = getEntity(lastInst);

        //Free the block memory
        size_t oldSize = sizeof(uint32_t) * (2 + getCapacity(ei));
        uint32_t* oldBlock = getPointer(ei);
        _buddy.free(oldBlock, oldSize);

        //Copy the last component to the removed position
        moveInstance(ei, lastInst);

        //Remove last
        _data.pop();

        //Update the keys in the map
        _map[lastEntity] = ei;
        _map.erase(e);
    }

    const uint8_t* TagSystem::instantiate(Entity e, const uint8_t* data)
    {
        uint32_t tagsLen;
        memcpy(&tagsLen, data, sizeof(tagsLen)); data += sizeof(tagsLen);

        EInstance ei = create(e, tagsLen);
        getLength(ei) = tagsLen;

        memcpy(getTags(ei), data, tagsLen * sizeof(uint32_t));
        data += tagsLen * sizeof(uint32_t);

        return data;
    }



    void TagSystem::handleDestroyed(const Entity* destroyed, size_t destroyedLen)
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



    void TagSystem::addTag(EInstance ei, uint32_t tag)
    {
        uint32_t len = getLength(ei);
        uint32_t* tags = getTags(ei);

        //Don't add it if it already exists
        for (uint32_t i = 0; i < len; i++)
        {
            if (tags[i] == tag)
            {
                return;
            }
        }

        //Resize if we need to
        if (len + 1 >= getCapacity(ei))
        {
            //Alloc a new block and copy over the data
            size_t oldSize = sizeof(uint32_t) * (2 + getCapacity(ei));
            uint32_t* oldBlock = getPointer(ei);
            uint32_t* newBlock = (uint32_t*)_buddy.alloc(2 * oldSize);
            memcpy(newBlock, oldBlock, oldSize);

            //Assign the new block; update capacity
            _data.tagsOffset[ei.index] = getOffset(newBlock);
            getCapacity(ei) = 2 * (getCapacity(ei) + 2) - 2;
            tags = getTags(ei); //Update pointer

            _buddy.free(oldBlock, oldSize);
        }

        //Add tag
        tags[len] = tag;
        getLength(ei)++;
    }

    void TagSystem::removeTag(EInstance ei, uint32_t tag)
    {
        uint32_t len = getLength(ei);
        uint32_t* tags = getTags(ei);

        //Don't add it if it already exists
        for (uint32_t i = 0; i < len; i++)
        {
            if (tags[i] == tag)
            {
                //Swap and pop the last element
                tags[i] = tags[len - 1];
                getLength(ei)--;
            }
        }
    }

    bool TagSystem::hasTag(EInstance ei, uint32_t tag)
    {
        uint32_t len = getLength(ei);
        uint32_t* tags = getTags(ei);

        for (uint32_t i = 0; i < len; i++)
        {
            if (tags[i] == tag)
            {
                return true;
            }
        }
        return false;
    }

    void TagSystem::moveInstance(EInstance dst, EInstance src)
    {
        _data.move(dst.index, src.index);
    }
}
