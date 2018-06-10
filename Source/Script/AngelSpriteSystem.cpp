#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "AngelMacros.h"
#include "Scene/EntityManager.h"
#include "Scene/SpriteSystem.h"

using namespace scene;

namespace script
{
    bool angelSprite_exists(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, false);
        return spriteSys->exists(en);
    }

    void angelSprite_create(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(!spriteSys->exists(en), "Cannot create sprite component; it already exists.");
        spriteSys->create(en);
    }

    void angelSprite_destroy(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Cannot destroy non-existent sprite component.");
        spriteSys->create(en);
    }

    Vector2i angelSprite_getSize(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Vector2i());
        SCRIPT_ASSERT_RETVAL(spriteSys->exists(en), "Entity doesn't have sprite component.", Vector2i());
        return spriteSys->getSize(spriteSys->getInstance(en));
    }

    Vector2i angelSprite_getOffset(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Vector2i());
        SCRIPT_ASSERT_RETVAL(spriteSys->exists(en), "Entity doesn't have sprite component.", Vector2i());
        return spriteSys->getOffset(spriteSys->getInstance(en));
    }

    uint8_t angelSprite_getDepth(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, 0);
        SCRIPT_ASSERT_RETVAL(spriteSys->exists(en), "Entity doesn't have sprite component.", 0);
        return spriteSys->getDepth(spriteSys->getInstance(en));
    }

    uint8_t angelSprite_getAlpha(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, 0);
        SCRIPT_ASSERT_RETVAL(spriteSys->exists(en), "Entity doesn't have sprite component.", 0);
        return spriteSys->getAlpha(spriteSys->getInstance(en));
    }

    Vector2i angelSprite_getTexOffset(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, Vector2i());
        SCRIPT_ASSERT_RETVAL(spriteSys->exists(en), "Entity doesn't have sprite component.", Vector2i());
        return spriteSys->getTexOffset(spriteSys->getInstance(en));
    }

    asset::AssetRef angelSprite_getTexture(SpriteSystem* spriteSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, asset::AssetRef());
        SCRIPT_ASSERT_RETVAL(spriteSys->exists(en), "Entity doesn't have sprite component.", asset::AssetRef());
        return spriteSys->getTextureRef(spriteSys->getInstance(en));
    }

    void angelSprite_setSize(SpriteSystem* spriteSys, Entity en, Vector2i size)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setSize(spriteSys->getInstance(en), size);
    }

    void angelSprite_setOffset(SpriteSystem* spriteSys, Entity en, Vector2i offset)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setOffset(spriteSys->getInstance(en), offset);
    }

    void angelSprite_setDepth(SpriteSystem* spriteSys, Entity en, uint8_t depth)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setDepth(spriteSys->getInstance(en), depth);
    }

    void angelSprite_setAlpha(SpriteSystem* spriteSys, Entity en, uint8_t alpha)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setAlpha(spriteSys->getInstance(en), alpha);
    }

    void angelSprite_setTexOffset(SpriteSystem* spriteSys, Entity en, Vector2i texOffset)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setTexOffset(spriteSys->getInstance(en), texOffset);
    }

    void angelSprite_setTexture(SpriteSystem* spriteSys, Entity en, asset::AssetRef ref)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setTextureRef(spriteSys->getInstance(en), ref);
    }

    void angelSprite_setHorFlip(SpriteSystem* spriteSys, Entity en, bool horFlip)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setHorFlip(spriteSys->getInstance(en), horFlip);
    }

    void angelSprite_setVerFlip(SpriteSystem* spriteSys, Entity en, bool verFlip)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setVerFlip(spriteSys->getInstance(en), verFlip);
    }

    void angelSprite_setRotation(SpriteSystem* spriteSys, Entity en, int32_t rotation)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(spriteSys->exists(en), "Entity doesn't have sprite component.");
        spriteSys->setRotation(spriteSys->getInstance(en), rotation);
    }

    void angelSprite_RegisterTypes(asIScriptEngine* engine, scene::SpriteSystem** spriteSys)
    {
        AS_VERIFY(engine->RegisterObjectType("CSpriteSystem", sizeof(SpriteSystem), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "bool exists(Entity)", asFUNCTION(angelSprite_exists), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void create(Entity)", asFUNCTION(angelSprite_create), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void destroy(Entity)", asFUNCTION(angelSprite_destroy), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "Vector2i getSize(Entity)", asFUNCTION(angelSprite_getSize), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "Vector2i getOffset(Entity)", asFUNCTION(angelSprite_getOffset), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "uint8 getDepth(Entity)", asFUNCTION(angelSprite_getDepth), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "uint8 getAlpha(Entity)", asFUNCTION(angelSprite_getAlpha), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "Vector2i getTexOffset(Entity)", asFUNCTION(angelSprite_getTexOffset), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "AssetRef getTexture(Entity)", asFUNCTION(angelSprite_getTexture), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setSize(Entity, Vector2i)", asFUNCTION(angelSprite_setSize), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setOffset(Entity, Vector2i)", asFUNCTION(angelSprite_setOffset), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setDepth(Entity, uint8)", asFUNCTION(angelSprite_setDepth), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setAlpha(Entity, uint8)", asFUNCTION(angelSprite_setAlpha), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setTexOffset(Entity, Vector2i)", asFUNCTION(angelSprite_setTexOffset), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setTexture(Entity, AssetRef)", asFUNCTION(angelSprite_setTexture), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setHorFlip(Entity, bool)", asFUNCTION(angelSprite_setHorFlip), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setVerFlip(Entity, bool)", asFUNCTION(angelSprite_setVerFlip), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CSpriteSystem", "void setRotation(Entity, int)", asFUNCTION(angelSprite_setRotation), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterGlobalProperty("CSpriteSystem@ Sprite", spriteSys));
    }
}
