#ifndef SCENE_ENTITY_MANAGER_H
#define SCENE_ENTITY_MANAGER_H

#include <EASTL/vector.h>
#include <EASTL/bonus/ring_buffer.h>
#include "Entity.h"

#ifdef NW_DEVELOP
#define USE_ENTITY_DEBUG_NAMES
#endif

namespace scene
{
    class EntityManager
    {
    private:
        //Keeps track of the generation alive at each index.
        //An entity is alive if gens[index] is the same as the entity's gen.
        //When an entity is destroyed, we simply increment the gen counter.
        eastl::vector<uint8_t> _gens;

        //To keep the generation from wrapping around too frequently, a list
        //of free indices is used to keep them from being reused too often.
        eastl::ring_buffer<uint32_t> _freeIndices;

        //Keeps track of which entities have destroyed since clearDestroyed() was called
        eastl::vector<Entity> _destroyed;

#ifdef USE_ENTITY_DEBUG_NAMES
        static EntityManager* sInstance;    //Static instance used for natvis
        eastl::vector<eastl::string> _debugNames;
#endif

    public:
        EntityManager();
        bool alive(Entity e);
        Entity create();
        void destroy(Entity e);

        eastl::vector<Entity>& pollDestroyed();
        void clearDestroyed();

#ifdef USE_ENTITY_DEBUG_NAMES
        void setDebugName(Entity en, const char* name);
        const char* getDebugName(Entity en) { return _debugNames[en.index()].c_str(); }
#endif
    };
}

#endif
