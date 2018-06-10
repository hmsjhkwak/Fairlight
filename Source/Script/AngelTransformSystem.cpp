#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "AngelMacros.h"
#include "Scene/EntityManager.h"
#include "Scene/TransformSystem.h"

using namespace scene;

namespace script
{
    bool angelTransform_exists(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, false);
        return trSys->exists(en);
    }

    void angelTransform_create(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(!trSys->exists(en), "Cannot create transform component; it already exists.");
        trSys->create(en);
    }

    void angelTransform_destroy(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(trSys->exists(en), "Cannot destroy non-existent transform component.");
        trSys->create(en);
    }

    Vector2i angelTransform_getLocalPos(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Vector2i());
        SCRIPT_ASSERT_RETVAL(trSys->exists(en), "Entity doesn't have transform component.", Vector2i());
        return trSys->getLocalPos(trSys->getInstance(en));
    }

    Vector2i angelTransform_getWorldPos(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Vector2i());
        SCRIPT_ASSERT_RETVAL(trSys->exists(en), "Entity doesn't have transform component.", Vector2i());
        return trSys->getWorldPos(trSys->getInstance(en));
    }

    Entity angelTransform_getParent(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Entity());
        SCRIPT_ASSERT_RETVAL(trSys->exists(en), "Entity doesn't have transform component.", Entity());
        return trSys->getEntity(trSys->getParent(trSys->getInstance(en)));
    }

    Entity angelTransform_getFirstChild(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Entity());
        SCRIPT_ASSERT_RETVAL(trSys->exists(en), "Entity doesn't have transform component.", Entity());
        return trSys->getEntity(trSys->getFirstChild(trSys->getInstance(en)));
    }

    Entity angelTransform_getNextSib(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Entity());
        SCRIPT_ASSERT_RETVAL(trSys->exists(en), "Entity doesn't have transform component.", Entity());
        return trSys->getEntity(trSys->getNextSib(trSys->getInstance(en)));
    }

    Entity angelTransform_getPrevSib(TransformSystem* trSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Entity());
        SCRIPT_ASSERT_RETVAL(trSys->exists(en), "Entity doesn't have transform component.", Entity());
        return trSys->getEntity(trSys->getPrevSib(trSys->getInstance(en)));
    }

    void angelTransform_setLocalPos(TransformSystem* trSys, Entity en, Vector2i pos)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(trSys->exists(en), "Entity doesn't have transform component.");
        trSys->setLocalPos(trSys->getInstance(en), pos);
    }

    void angelTransform_setWorldPos(TransformSystem* trSys, Entity en, Vector2i pos)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(trSys->exists(en), "Entity doesn't have transform component.");
        trSys->setWorldPos(trSys->getInstance(en), pos);
    }

    void angelTransform_setParent(TransformSystem* trSys, Entity en, Entity parent)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(trSys->exists(en), "Entity doesn't have transform component.");
        trSys->setParent(trSys->getInstance(en), trSys->getInstance(parent));
    }

    void angelTransform_RegisterTypes(asIScriptEngine* engine, scene::TransformSystem** trSys)
    {
        AS_VERIFY(engine->RegisterObjectType("CTransformSystem", sizeof(TransformSystem), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "bool exists(Entity)", asFUNCTION(angelTransform_exists), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "void create(Entity)", asFUNCTION(angelTransform_create), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "void destroy(Entity)", asFUNCTION(angelTransform_destroy), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "Vector2i getLocalPos(Entity)", asFUNCTION(angelTransform_getLocalPos), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "Vector2i getWorldPos(Entity)", asFUNCTION(angelTransform_getWorldPos), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "Entity getParent(Entity)", asFUNCTION(angelTransform_getParent), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "Entity getFirstChild(Entity)", asFUNCTION(angelTransform_getFirstChild), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "Entity getNextSib(Entity)", asFUNCTION(angelTransform_getNextSib), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "Entity getPrevSib(Entity)", asFUNCTION(angelTransform_getPrevSib), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "void setLocalPos(Entity, Vector2i)", asFUNCTION(angelTransform_setLocalPos), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "void setWorldPos(Entity, Vector2i)", asFUNCTION(angelTransform_setWorldPos), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTransformSystem", "void setParent(Entity, Entity parent)", asFUNCTION(angelTransform_setParent), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterGlobalProperty("CTransformSystem@ Transform", trSys));
    }
}
