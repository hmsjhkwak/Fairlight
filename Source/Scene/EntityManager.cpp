#include "Core/Core.h"
#include "EntityManager.h"

namespace scene
{
    const uint32_t MINIMUM_FREE_INDICES = 1024;

#ifdef USE_ENTITY_DEBUG_NAMES
    EntityManager* EntityManager::sInstance = nullptr;
#endif

    EntityManager::EntityManager()
    {
#ifdef USE_ENTITY_DEBUG_NAMES
        sInstance = this;
#endif
    }

    bool EntityManager::alive(Entity e)
    {
        return e.isValid() && _gens[e.index()] == e.gen();
    }

    Entity EntityManager::create()
    {
        uint32_t index;

        if (_freeIndices.size() > MINIMUM_FREE_INDICES)
        {
            index = _freeIndices.front();
            _freeIndices.pop_front();
        }
        else
        {
            _gens.push_back(0);
            index = (uint32_t)_gens.size() - 1;

            NW_ASSERT(index <= ENTITY_INDEX_MASK);
        }

#ifdef USE_ENTITY_DEBUG_NAMES
        //printf("Entity created (%d, %d)\n", index, _gens[index]);
        _debugNames.reserve(index + 1);
        if (_debugNames.size() < _debugNames.capacity())
        {
            _debugNames.resize(_debugNames.capacity());
        }
#endif

        return Entity(index, _gens[index]);
    }

    void EntityManager::destroy(Entity e)
    {
        NW_ASSERT(alive(e));

        auto index = e.index();
        _gens[index]++;  //Increment gen to destroy
        _freeIndices.push_back(index);

        //Keep track of the destroyed entities
        _destroyed.push_back(e);

#ifdef USE_ENTITY_DEBUG_NAMES
        //printf("Entity destroyed (%d, %d)\n", e.index(), e.gen());
        _debugNames[e.index()].clear();
#endif
    }

    eastl::vector<Entity>& EntityManager::pollDestroyed()
    {
        return _destroyed;
    }

    void EntityManager::clearDestroyed()
    {
        _destroyed.clear();
    }

#ifdef USE_ENTITY_DEBUG_NAMES
    void EntityManager::setDebugName(Entity en, const char* name)
    {
        NW_ASSERT(en.index() < _debugNames.size());
        _debugNames[en.index()] = name;
    }
#endif
}
