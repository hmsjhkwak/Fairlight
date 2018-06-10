#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "AngelMacros.h"
#include "Scene/EntityManager.h"
#include "Scene/TagSystem.h"

using namespace scene;

namespace script
{
    bool angelTag_exists(TagSystem* tagSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, false);
        return tagSys->exists(en);
    }

    void angelTag_create(TagSystem* tagSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(!tagSys->exists(en), "Cannot create tag component; it already exists.");
        tagSys->create(en);
    }

    void angelTag_destroy(TagSystem* tagSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(tagSys->exists(en), "Cannot destroy non-existent tag component.");
        tagSys->create(en);
    }

    bool angelTag_hasTag(TagSystem* tagSys, Entity en, uint32_t tag)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, false);
        SCRIPT_ASSERT_RETVAL(tagSys->exists(en), "Entity does not have tag component.", false);
        return tagSys->hasTag(tagSys->getInstance(en), tag);
    }

    void angelTag_addTag(TagSystem* tagSys, Entity en, uint32_t tag)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(tagSys->exists(en), "Entity does not have tag component.");
        EInstance ei = tagSys->getInstance(en);
        SCRIPT_ASSERT(!tagSys->hasTag(ei, tag), "Entity already has specified tag.");
        tagSys->addTag(ei, tag);
    }

    void angelTag_removeTag(TagSystem* tagSys, Entity en, uint32_t tag)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(tagSys->exists(en), "Entity does not have tag component.");
        EInstance ei = tagSys->getInstance(en);
        SCRIPT_ASSERT(tagSys->hasTag(ei, tag), "Entity already does not have specified tag.");
        tagSys->removeTag(ei, tag);
    }

    void angelTag_RegisterTypes(asIScriptEngine* engine, scene::TagSystem** tagSys)
    {
        AS_VERIFY(engine->RegisterObjectType("CTagSystem", sizeof(TagSystem), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CTagSystem", "bool exists(Entity)", asFUNCTION(angelTag_exists), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTagSystem", "void create(Entity)", asFUNCTION(angelTag_create), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTagSystem", "void destroy(Entity)", asFUNCTION(angelTag_destroy), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTagSystem", "bool hasTag(Entity, uint)", asFUNCTION(angelTag_hasTag), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTagSystem", "void addTag(Entity, uint)", asFUNCTION(angelTag_addTag), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CTagSystem", "void removeTag(Entity, uint)", asFUNCTION(angelTag_removeTag), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterGlobalProperty("CTagSystem@ Tag", tagSys));
    }
}
