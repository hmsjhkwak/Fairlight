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

        TransformData trData = { Vector2i(0, 0), Vector2i(0, 0) };
        EInstance none;
        HierarchyData hierData = { none, none, none, none };

        _data.push(e, trData, hierData);

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
        EInstance parent = _data.hierData[ei.index].parent;
        EInstance prevSib = _data.hierData[ei.index].prevSib;
        EInstance nextSib = _data.hierData[ei.index].nextSib;

        //Update the parent if we're the first child
        if (parent.isValid() && _data.hierData[parent.index].firstChild == ei)
        {
            _data.hierData[parent.index].firstChild = nextSib;
        }
        //Update the previous sibling to point to the next
        if (prevSib.isValid())
        {
            _data.hierData[prevSib.index].nextSib = nextSib;
        }
        //Update the next sibling to point to the previous
        if (nextSib.isValid())
        {
            _data.hierData[nextSib.index].prevSib = prevSib;
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
            EInstance parent = _data.hierData[srcIdx].parent;
            EInstance prevSib = _data.hierData[srcIdx].prevSib;
            EInstance nextSib = _data.hierData[srcIdx].nextSib;

            //Update the parent if we're the first child
            if (parent.isValid() && _data.hierData[parent.index].firstChild == src)
            {
                _data.hierData[parent.index].firstChild = dst;
            }
            //Update the previous sibling to point to the next
            if (prevSib.isValid())
            {
                _data.hierData[prevSib.index].nextSib = dst;
            }
            //Update the next sibling to point to the previous
            if (nextSib.isValid())
            {
                _data.hierData[nextSib.index].prevSib = dst;
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
        EInstance child = _data.hierData[ei.index].firstChild;
        while (child.isValid())
        {
            EInstance next = _data.hierData[child.index].nextSib;
            handleDestroyedChildren(entityManager, child);
            entityManager.destroy(getEntity(child));
            child = next;
        }
    }

    void TransformSystem::setLocalPos(EInstance ei, const Vector2i& localPos)
    {
        uint32_t idx = ei.index;
        _data.trData[idx].localPos = localPos;

        //Update world position
        EInstance parent = _data.hierData[idx].parent;
        if (parent.isValid())
        {
            updateWorldPos(ei, _data.trData[parent.index].worldPos);
        }
        else
        {
            updateWorldPos(ei, Vector2i());
        }
    }

    void TransformSystem::setWorldPos(EInstance ei, const Vector2i& worldPos)
    {
        uint32_t idx = ei.index;
        _data.trData[idx].worldPos = worldPos;

        //Update local position
        EInstance parent = _data.hierData[idx].parent;
        if (parent.isValid())
        {
            _data.trData[idx].localPos = worldPos - _data.trData[parent.index].worldPos;
        }
        else
        {
            _data.trData[idx].localPos = worldPos;
        }

        //Update child world positions
        EInstance child = _data.hierData[idx].firstChild;
        while (child.isValid())
        {
            updateWorldPos(child, _data.trData[idx].worldPos);
            child = _data.hierData[child.index].nextSib;
        }
    }

    void TransformSystem::setParent(EInstance child, EInstance parent)
    {
        if (_data.hierData[child.index].parent.isValid())
        {
            removeChild(child);
        }

        _data.hierData[child.index].parent = parent;

        //Update the parent
        EInstance oldChild = _data.hierData[parent.index].firstChild;
        _data.hierData[parent.index].firstChild = child;
        _data.hierData[child.index].nextSib = oldChild;
        if (oldChild.isValid())
        {
            _data.hierData[oldChild.index].prevSib = child;
        }
    }

    void TransformSystem::updateWorldPos(EInstance ei, const Vector2i& parPos)
    {
        _data.trData[ei.index].worldPos = parPos + _data.trData[ei.index].localPos;

        //Update child world positions
        EInstance child = _data.hierData[ei.index].firstChild;
        while (child.isValid())
        {
            updateWorldPos(child, _data.trData[ei.index].worldPos);
            child = _data.hierData[child.index].nextSib;
        }
    }
}
