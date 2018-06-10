#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "AngelMacros.h"
#include "Scene/EntityManager.h"
#include "Scene/MovementSystem.h"

using namespace scene;

namespace script
{
    bool angelMovement_exists(MovementSystem* moveSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, false);
        return moveSys->exists(en);
    }

    void angelMovement_create(MovementSystem* moveSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(!moveSys->exists(en), "Cannot create movement component; it already exists.");
        moveSys->create(en);
    }

    void angelMovement_destroy(MovementSystem* moveSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(moveSys->exists(en), "Cannot destroy non-existent movement component.");
        moveSys->create(en);
    }

    Vector2i angelMovement_getSize(MovementSystem* moveSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Vector2i());
        SCRIPT_ASSERT_RETVAL(moveSys->exists(en), "Entity doesn't have movement component.", Vector2i());
        return moveSys->getSize(moveSys->getInstance(en));
    }

    Vector2i angelMovement_getOffset(MovementSystem* moveSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Vector2i());
        SCRIPT_ASSERT_RETVAL(moveSys->exists(en), "Entity doesn't have movement component.", Vector2i());
        return moveSys->getOffset(moveSys->getInstance(en));
    }

    Vector2f angelMovement_getVelocity(MovementSystem* moveSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Vector2f());
        SCRIPT_ASSERT_RETVAL(moveSys->exists(en), "Entity doesn't have movement component.", Vector2f());
        return moveSys->getVelocity(moveSys->getInstance(en));
    }

    bool angelMovement_getWorldCollision(MovementSystem* moveSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, false);
        SCRIPT_ASSERT_RETVAL(moveSys->exists(en), "Entity doesn't have movement component.", false);
        return moveSys->getWorldCollision(moveSys->getInstance(en));
    }

    void angelMovement_setSize(MovementSystem* moveSys, Entity en, Vector2i size)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(moveSys->exists(en), "Entity doesn't have movement component.");
        moveSys->setSize(moveSys->getInstance(en), size);
    }

    void angelMovement_setOffset(MovementSystem* moveSys, Entity en, Vector2i offset)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(moveSys->exists(en), "Entity doesn't have movement component.");
        moveSys->setOffset(moveSys->getInstance(en), offset);
    }

    void angelMovement_setVelocity(MovementSystem* moveSys, Entity en, Vector2f velocity)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(moveSys->exists(en), "Entity doesn't have movement component.");
        moveSys->setVelocity(moveSys->getInstance(en), velocity);
    }

    void angelMovement_setWorldCollision(MovementSystem* moveSys, Entity en, bool worldColl)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(moveSys->exists(en), "Entity doesn't have movement component.");
        moveSys->setWorldCollision(moveSys->getInstance(en), worldColl);
    }

    bool angelMovement_intersectsWorld(MovementSystem* moveSys, Entity en)
    {
        TransformSystem* trSys = AngelState::getCurrent()->getTransformSystem();
        TileSystem* tileSys = AngelState::getCurrent()->getTileSystem();
        return moveSys->intersectsWorld(*trSys, *tileSys, moveSys->getInstance(en));
    }

    uint32_t angelMovement_getFirstCollisionIndex(MovementSystem* moveSys, Entity en)
    {
        return moveSys->getFirstCollisionIndex(en);
    }

    void angelMovement_getCollision(MovementSystem* moveSys, uint32_t index, Entity* e1, Entity* e2)
    {
        CollisionPair pair = moveSys->getCollision(index);
        *e1 = pair.e1;
        *e2 = pair.e2;
    }

    void angelMovement_RegisterTypes(asIScriptEngine* engine, scene::MovementSystem** moveSys)
    {
        AS_VERIFY(engine->RegisterObjectType("CMovementSystem", sizeof(MovementSystem), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "bool exists(Entity)", asFUNCTION(angelMovement_exists), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "void create(Entity)", asFUNCTION(angelMovement_create), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "void destroy(Entity)", asFUNCTION(angelMovement_destroy), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "Vector2i getSize(Entity)", asFUNCTION(angelMovement_getSize), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "Vector2i getOffset(Entity)", asFUNCTION(angelMovement_getOffset), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "Vector2f getVelocity(Entity)", asFUNCTION(angelMovement_getVelocity), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "bool getWorldCollision(Entity)", asFUNCTION(angelMovement_getWorldCollision), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "void setSize(Entity, Vector2i)", asFUNCTION(angelMovement_setSize), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "void setOffset(Entity, Vector2i)", asFUNCTION(angelMovement_setOffset), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "void setVelocity(Entity, Vector2f)", asFUNCTION(angelMovement_setVelocity), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "void setWorldCollision(Entity, bool)", asFUNCTION(angelMovement_setWorldCollision), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "bool intersectsWorld(Entity)", asFUNCTION(angelMovement_intersectsWorld), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "uint getFirstCollisionIndex(Entity)", asFUNCTION(angelMovement_getFirstCollisionIndex), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CMovementSystem", "void getCollision(uint index, Entity &out e1, Entity &out e2)", asFUNCTION(angelMovement_getCollision), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterGlobalProperty("CMovementSystem@ Movement", moveSys));
    }
}
