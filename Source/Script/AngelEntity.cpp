#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "Scene/Entity.h"
#include "Scene/EntityManager.h"

using namespace scene;

namespace script
{
    void angelEntity_Entity_DefaultConstruct(Entity* en)
    {
        new (en) Entity();
    }

    void angelEntity_Entity_Construct(Entity* en, uint32_t id)
    {
        new (en) Entity(id);
    }

    void angelEntity_setDebugName(EntityManager* entityMan, Entity en, const std::string& name)
    {
#ifdef USE_ENTITY_DEBUG_NAMES
        entityMan->setDebugName(en, name.c_str());
#else
        NW_UNUSED(entityMan);
        NW_UNUSED(en);
        NW_UNUSED(name);
#endif
    }

    void angelEntity_RegisterTypes(asIScriptEngine* engine, EntityManager** entityManager)
    {
        AS_VERIFY(engine->RegisterObjectType("Entity", sizeof(Entity), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Entity>()));
        AS_VERIFY(engine->RegisterObjectBehaviour("Entity", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(angelEntity_Entity_DefaultConstruct), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("Entity", "bool isValid()", asMETHOD(Entity, isValid), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("Entity", "bool opEquals(Entity other)", asMETHOD(Entity, operator==), asCALL_THISCALL));

        AS_VERIFY(engine->RegisterObjectType("CEntityManager", sizeof(EntityManager), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CEntityManager", "bool alive(Entity)", asMETHOD(EntityManager, alive), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CEntityManager", "Entity create()", asMETHOD(EntityManager, create), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CEntityManager", "void destroy(Entity)", asMETHOD(EntityManager, destroy), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CEntityManager", "void setDebugName(Entity, const string &in)", asFUNCTION(angelEntity_setDebugName), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterGlobalProperty("CEntityManager@ EntityManager", entityManager));
    }
}
