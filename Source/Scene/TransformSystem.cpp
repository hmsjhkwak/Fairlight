#include "Core/Core.h"
#include "TransformSystem.h"
#include "EntityManager.h"
#include "Asset/PackFile.h"

using namespace asset;

namespace scene
{
    TransformSystem::TransformSystem()
    {
    }

    EInstance TransformSystem::create(Entity e)
    {
        EInstance ei = EInstance(_data.getSize());
        _map.insert(eastl::make_pair(e, ei));

        EInstance none;

        _data.push(e,
            Vector2i(0, 0), Vector2i(0, 0),
            none, none, none, none
        );

        return ei;
    }

    void TransformSystem::destroy(Entity e)
    {
        NW_ASSERT(exists(e));

        //Remove children
        {
            //TODO: This shouldn't be needed. Rewrite how we're destroying these guys.
            /*
            EInstance child = _data.firstChild[_map[e].index];
            while (child.isValid())
            {
                uint32_t nextIndex = _data.nextSib[child.index].index;
                Entity next;
                if (nextIndex == UINT32_MAX)
                {
                    next = Entity();
                }
                else
                {
                    next = _data.entities[nextIndex];
                }

                destroy(getEntity(child));

                child = _map[next];
            }*/
        }

        EInstance ei = _map[e];
        const EInstance lastInst(_data.getSize() - 1);
        const Entity lastEntity = getEntity(lastInst);

        removeChild(ei);

        //Copy the last component to the removed position
        if (lastInst.index != ei.index)
        {
            moveInstance(ei, lastInst);
        }

        //Remove last
        _data.pop();

        //Update the keys in the map
        _map[lastEntity] = ei;
        _map.erase(e);
    }

    void TransformSystem::removeChild(EInstance ei)
    {
        EInstance parent = _data.parent[ei.index];
        EInstance prevSib = _data.prevSib[ei.index];
        EInstance nextSib = _data.nextSib[ei.index];

        //Update the parent if we're the first child
        if (parent.isValid() && _data.firstChild[parent.index] == ei)
        {
            _data.firstChild[parent.index] = nextSib;
        }
        //Update the previous sibling to point to the next
        if (prevSib.isValid())
        {
            _data.nextSib[prevSib.index] = nextSib;
        }
        //Update the next sibling to point to the previous
        if (nextSib.isValid())
        {
            _data.prevSib[nextSib.index] = prevSib;
        }
    }

    const uint8_t* TransformSystem::instantiate(Entity e, const uint8_t* data)
    {
        create(e);
        return data;
    }

    void TransformSystem::moveInstance(EInstance dst, EInstance src)
    {
        uint32_t srcIdx = src.index;
        uint32_t dstIdx = dst.index;

        //Copy source to destination
        _data.move(dstIdx, srcIdx);

        //Update other references to source
        {
            EInstance parent = _data.parent[srcIdx];
            EInstance prevSib = _data.prevSib[srcIdx];
            EInstance nextSib = _data.nextSib[srcIdx];

            //Update the parent if we're the first child
            if (parent.isValid() && _data.firstChild[parent.index] == src)
            {
                _data.firstChild[parent.index] = dst;
            }
            //Update the previous sibling to point to the next
            if (prevSib.isValid())
            {
                _data.nextSib[prevSib.index] = dst;
            }
            //Update the next sibling to point to the previous
            if (nextSib.isValid())
            {
                _data.prevSib[nextSib.index] = dst;
            }
        }
    }

    void TransformSystem::handleDestroyed(EntityManager& entityManager)
    {
        auto& entities = entityManager.pollDestroyed();

        //Save the length; if we destroy something with children, we dump the
        //children on the end of the list. We don't want to process them twice.
        //TODO: This totally falls apart if we delete everything manually
        size_t length = entities.size();
        for (size_t i = 0; i < length; i++)
        {
            auto result = _map.find(entities[i]);
            if (result != _map.end())
            {
                //Dump children on the destroyed list
                handleDestroyedChildren(entityManager, result->second.index);

                destroy(result->first);
            }
        }
    }

    void TransformSystem::handleDestroyedChildren(EntityManager& entityManager, EInstance ei)
    {
        EInstance child = _data.firstChild[ei.index];
        while (child.isValid())
        {
            EInstance next = _data.nextSib[child.index];
            handleDestroyedChildren(entityManager, child);
            entityManager.destroy(getEntity(child));
            child = next;
        }
    }

    void TransformSystem::setLocalPos(EInstance ei, const Vector2i& localPos)
    {
        uint32_t idx = ei.index;
        _data.localPos[idx] = localPos;

        //Update world position
        EInstance parent = _data.parent[idx];
        if (parent.isValid())
        {
            updateWorldPos(ei, _data.worldPos[parent.index]);
        }
        else
        {
            updateWorldPos(ei, Vector2i());
        }
    }

    void TransformSystem::setWorldPos(EInstance ei, const Vector2i& worldPos)
    {
        uint32_t idx = ei.index;
        _data.worldPos[idx] = worldPos;

        //Update local position
        EInstance parent = _data.parent[idx];
        if (parent.isValid())
        {
            _data.localPos[idx] = worldPos - _data.worldPos[parent.index];
        }
        else
        {
            _data.localPos[idx] = worldPos;
        }

        //Update child world positions
        EInstance child = _data.firstChild[idx];
        while (child.isValid())
        {
            updateWorldPos(child, _data.worldPos[idx]);
            child = _data.nextSib[child.index];
        }
    }

    void TransformSystem::setParent(EInstance child, EInstance parent)
    {
        if (_data.parent[child.index].isValid())
        {
            removeChild(child);
        }

        _data.parent[child.index] = parent;

        //Update the parent
        EInstance oldChild = _data.firstChild[parent.index];
        _data.firstChild[parent.index] = child;
        _data.nextSib[child.index] = oldChild;
        if (oldChild.isValid())
        {
            _data.prevSib[oldChild.index] = child;
        }
    }

    void TransformSystem::updateWorldPos(EInstance ei, const Vector2i& parPos)
    {
        _data.worldPos[ei.index] = parPos + _data.localPos[ei.index];

        //Update child world positions
        EInstance child = _data.firstChild[ei.index];
        while (child.isValid())
        {
            updateWorldPos(child, _data.worldPos[ei.index]);
            child = _data.nextSib[child.index];
        }
    }
}
