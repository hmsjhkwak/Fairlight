#include "Core/Core.h"
#include "MovementSystem.h"
#include "TransformSystem.h"
#include "TileSystem.h"
#include "Math/Math.h"
#include "Asset/PackFile.h"
#include "Asset/AssetManager.h"
#include "Render/Renderer2d.h"

namespace scene
{
    int compareCollisionPairs(const void* void1, const void* void2)
    {
        CollisionPair* pair1 = (CollisionPair*)void1;
        CollisionPair* pair2 = (CollisionPair*)void2;
        return (int)(pair1->e1.id() - pair2->e1.id());
    }



    MovementSystem::MovementSystem() : _worldCollLen(0)
    {
    }

#ifdef NW_ASSET_COOK
    uint8_t* MovementSystem::convertToPrefab(Entity e, uint8_t* buffer)
    {
        EInstance ei = getInstance(e);

        Vector2i size = getSize(ei);
        Vector2i offset = getOffset(ei);
        Vector2f velocity = getVelocity(ei);
        bool worldColl = getWorldCollision(ei);

        memcpy(buffer, &size, sizeof(size)); buffer += sizeof(size);
        memcpy(buffer, &offset, sizeof(offset)); buffer += sizeof(offset);
        memcpy(buffer, &velocity, sizeof(velocity)); buffer += sizeof(velocity);
        memcpy(buffer, &worldColl, sizeof(worldColl)); buffer += sizeof(worldColl);

        return buffer;
    }
#endif

    IntRect offsetRect(const IntRect& rect, int x, int y)
    {
        return IntRect(x + rect.left, y + rect.top, rect.width, rect.height);
    }

    void MovementSystem::update(float dt, TransformSystem& trSystem, const TileSystem& tileSystem)
    {
        SCOPED_CPU_EVENT(event)(0xFFFFFFFF, "MovementSystem::update");

        updateWorldColl(dt, trSystem, tileSystem);
        updateNonWorldColl(dt, trSystem, tileSystem);
        recordCollisions(trSystem);
    }

    void MovementSystem::updateWorldColl(float dt, TransformSystem& trSystem, const TileSystem& tileSystem)
    {
        SCOPED_CPU_EVENT(event)(0xFFFFFFFF, "MovementSystem::updateWorldColl");

        //World colliders grouped at front of array
        for (uint32_t idx = 0; idx < _worldCollLen; idx++)
        {
            Entity e = _data.entities[idx];
            EInstance trInst = trSystem.getInstance(e);
            Vector2i position = trSystem.getWorldPos(trInst);
            Vector2f partial = _data.partialPos[idx];
            IntRect rect(_data.offset[idx] - (_data.size[idx] / 2), _data.size[idx]);

            //Add full movement to partial position
            partial += _data.velocity[idx] * dt;

            //TODO: Optimize this stuff. No need to check each pixel.
            //Do horizontal movement
            int hSign = sign(partial.x);
            int hCount = (int)floor(abs(partial.x));
            for (int i = 0; i < hCount; i++)
            {
                //Step down
                if (tileSystem.isFree(offsetRect(rect, position.x + hSign, position.y + 1)) &&
                    !tileSystem.isFree(offsetRect(rect, position.x + hSign, position.y + 2)))
                {
                    position.x += hSign;
                    position.y++;

                    partial.x -= hSign;

                    _data.velocity[idx].x -= hSign * 0.5f;
                }
                //Walking
                else if (tileSystem.isFree(offsetRect(rect, position.x + hSign, position.y)))
                {
                    position.x += hSign;
                    partial.x -= hSign;
                }
                //Step up
                else if (tileSystem.isFree(offsetRect(rect, position.x + hSign, position.y - 1)))
                {
                    position.x += hSign;
                    position.y--;

                    partial.x -= hSign;

                    _data.velocity[idx].x -= hSign * 0.5f;
                }
                //Stopping
                else
                {
                    _data.velocity[idx].x = 0;
                    partial.x = 0;
                    break;
                }
            }


            //Do vertical movement
            int vSign = sign(partial.y);
            int vCount = (int)floor(abs(partial.y));
            for (int i = 0; i < vCount; i++)
            {
                //Moving
                if (tileSystem.isFree(offsetRect(rect, position.x, position.y + vSign)))
                {
                    position.y += vSign;
                    partial.y -= vSign;
                }
                //Stopping
                else
                {
                    _data.velocity[idx].y = 0;
                    partial.y = 0;
                    break;
                }
            }

            trSystem.setWorldPos(trInst, position);
            _data.partialPos[idx] = partial;
        }
    }

    void MovementSystem::updateNonWorldColl(float dt, TransformSystem& trSystem, const TileSystem& tileSystem)
    {
        NW_UNUSED(tileSystem);

        SCOPED_CPU_EVENT(event)(0xFFFFFFFF, "MovementSystem::updateNonWorldColl");

        //Non world colliders grouped at end of array
        for (uint32_t idx = _worldCollLen; idx < _data.getSize(); idx++)
        {
            _data.partialPos[idx] += _data.velocity[idx] * dt;

            //Calculate how many whole pixels the character has moved
            Vector2i temp = Vector2i(
                roundToZero(_data.partialPos[idx].x),
                roundToZero(_data.partialPos[idx].y));

            //Add value to real position and remove it from partial
            Entity e = _data.entities[idx];
            EInstance trInst = trSystem.getInstance(e);
            trSystem.setWorldPos(trInst, trSystem.getWorldPos(trInst) + temp);

            _data.partialPos[idx].x -= temp.x;
            _data.partialPos[idx].y -= temp.y;
        }
    }

    void MovementSystem::recordCollisions(TransformSystem& trSystem)
    {
        SCOPED_CPU_EVENT(event)(0xFFFFFFFF, "MovementSystem::recordCollisions");

        _collisionPairs.clear();

        //TODO: Something better than n^2 checks
        for (uint32_t idx1 = 0; idx1 < _data.getSize(); idx1++)
        {
            IntRect rect1 = getCollRect(trSystem, EInstance(idx1));

            for (uint32_t idx2 = idx1 + 1; idx2 < _data.getSize(); idx2++)
            {
                IntRect rect2 = getCollRect(trSystem, EInstance(idx2));

                if (rect1.intersects(rect2))
                {
                    //Store the pair both ways
                    CollisionPair pair;
                    pair.e1 = _data.entities[idx1];
                    pair.e2 = _data.entities[idx2];
                    _collisionPairs.push_back(pair);

                    pair.e1 = _data.entities[idx2];
                    pair.e2 = _data.entities[idx1];
                    _collisionPairs.push_back(pair);
                }
            }
        }

        //Sort the pairs for later
        if (_collisionPairs.size() > 0)
        {
            SCOPED_CPU_EVENT(sortEvent)(0xFFFFFFFF, "Sorting");

            qsort(&_collisionPairs[0], _collisionPairs.size(), sizeof(CollisionPair), compareCollisionPairs);
        }
    }



    bool MovementSystem::exists(Entity e)
    {
        return _map.find(e) != _map.end();
    }

    EInstance MovementSystem::create(Entity e)
    {
        EInstance ei = EInstance(_data.getSize());
        _map.insert(eastl::make_pair(e, ei));

        _data.push(e,
            Vector2i(0, 0),
            Vector2i(0, 0),
            Vector2f(0, 0),
            Vector2f(0, 0));

        return ei;
    }

    EInstance MovementSystem::createOrGetInstance(Entity e)
    {
        if (exists(e)) { return getInstance(e); }
        else { return create(e); }
    }

    void MovementSystem::destroy(Entity e)
    {
        NW_ASSERT(exists(e));

        const EInstance ei = _map[e];

        if (getWorldCollision(ei))
        {
            //Move last WC instance to the remove position
            const EInstance lastWcInst((uint32_t)_worldCollLen - 1);
            const Entity lastWcEntity = getEntity(lastWcInst);
            moveInstance(ei, lastWcInst);

            //Move last Non-WC instance to the old last WC instance
            const EInstance lastNwcInst(_data.getSize() - 1);
            const Entity lastNwcEntity = getEntity(lastNwcInst);
            moveInstance(lastWcInst, lastNwcInst);

            //Update the keys in the map
            _map[lastWcEntity] = ei;
            _map[lastNwcEntity] = lastWcInst;
            _map.erase(e);

            _worldCollLen -= 1;
        }
        else
        {
            //Copy the last component to the removed position
            const EInstance lastInst(_data.getSize() - 1);
            const Entity lastEntity = getEntity(lastInst);
            moveInstance(ei, lastInst);

            //Update the keys in the map
            _map[lastEntity] = ei;
            _map.erase(e);
        }

        //Remove last
        _data.pop();
    }

    const uint8_t* MovementSystem::instantiate(Entity e, const uint8_t* data)
    {
        EInstance ei = create(e);
        Vector2i size, offset;
        Vector2f velocity;
        bool worldColl;

        memcpy(&size, data, sizeof(size)); data += sizeof(size);
        memcpy(&offset, data, sizeof(offset)); data += sizeof(offset);
        memcpy(&velocity, data, sizeof(velocity)); data += sizeof(velocity);
        memcpy(&worldColl, data, sizeof(worldColl)); data += sizeof(worldColl);

        setSize(ei, size);
        setOffset(ei, offset);
        setVelocity(ei, velocity);
        setWorldCollision(ei, worldColl);

        return data;
    }

    void MovementSystem::moveInstance(EInstance dst, EInstance src)
    {
        _data.move(dst.index, src.index);
    }

    void MovementSystem::swapInstances(EInstance inst1, EInstance inst2)
    {
        Entity e1 = getEntity(inst1);
        Entity e2 = getEntity(inst2);

        _data.swap(inst1.index, inst2.index);

        //Update map with new entity positions
        _map[e1] = inst2;
        _map[e2] = inst1;
    }

    void MovementSystem::handleDestroyed(const Entity* destroyed, size_t destroyedLen)
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



    void MovementSystem::setWorldCollision(EInstance ei, bool worldColl)
    {
        //Do nothing if we don't need to change the state
        bool currentWc = getWorldCollision(ei);
        if (worldColl && !currentWc)
        {
            swapInstances(ei, EInstance(_worldCollLen));
            _worldCollLen++;
        }
        else if (!worldColl && currentWc)
        {
            swapInstances(ei, EInstance(_worldCollLen - 1));
            _worldCollLen--;
        }
    }



    bool MovementSystem::intersectsWorld(TransformSystem& tr, TileSystem& tile, EInstance ei)
    {
        bool v = !tile.isFree(getCollRect(tr, ei));
        return v;
    }

    uint32_t MovementSystem::getFirstCollisionIndex(Entity e)
    {
        if (_collisionPairs.size() == 0) { return 0; }

        //Prepare fake collision pair for binary search
        CollisionPair fake;
        fake.e1 = e;

        auto* result = (CollisionPair*)bsearch(&fake, &_collisionPairs[0],
            _collisionPairs.size(), sizeof(CollisionPair), compareCollisionPairs);
        if (result == nullptr) { return 0; }

        //Calculate index
        uint32_t index = (uint32_t)(result - &_collisionPairs[0]);

        //Walk backward until we know we got the first one
        while (_collisionPairs[index].e1 == e && index > 0) { index--; }

        //We might have walked past it, so return the next one
        if (_collisionPairs[index].e1 != e) { index++; }

        return index;
    }

    CollisionPair MovementSystem::getCollision(uint32_t index)
    {
        if (index < _collisionPairs.size())
        {
            return _collisionPairs[index];
        }
        else
        {
            CollisionPair fake; //Constructed with invalid entities
            return fake;
        }
    }

    IntRect MovementSystem::getCollRect(TransformSystem& tr, EInstance ei)
    {
        Entity e = _data.entities[ei.index];
        Vector2i pos = tr.getWorldPos(tr.getInstance(e));
        return IntRect(pos + _data.offset[ei.index] - (_data.size[ei.index] / 2), _data.size[ei.index]);
    }
}
