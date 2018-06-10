#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "AngelMacros.h"
#include "AngelHandle.h"
#include "Scene/EntityManager.h"
#include "Scene/ScriptSystem.h"

using namespace scene;

namespace script
{
    bool angelScript_exists(ScriptSystem* scriptSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE_RETVAL(en, false);
        return scriptSys->exists(en);
    }

    void angelScript_create(ScriptSystem* scriptSys, Entity en, const std::string& typeName)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(!scriptSys->exists(en), "Cannot create script component; it already exists.");
        AngelType type(typeName.c_str());
        SCRIPT_ASSERT(AngelState::getCurrent()->getTypeIdFromAngelType(type) != -1, "Cannot create script; type doesn't exist.");
        scriptSys->create(en, type);
    }

    void angelScript_destroy(ScriptSystem* scriptSys, Entity en)
    {
        SCRIPT_ASSERT_ALIVE(en);
        SCRIPT_ASSERT(scriptSys->exists(en), "Cannot destroy non-existent script component.");
        scriptSys->destroy(en);
    }

    CScriptHandle angelScript_getComponent(ScriptSystem* scriptSys, Entity en)
    {
        EInstance ei = scriptSys->getInstance(en);
        asIScriptObject* obj = scriptSys->getObject(ei);
        CScriptHandle handle;
        handle.Set(obj, obj->GetObjectType());
        return handle;
    }

    void angelScript_RegisterTypes(asIScriptEngine* engine, scene::ScriptSystem** scriptSys)
    {
        AS_VERIFY(engine->RegisterObjectType("CScriptSystem", sizeof(ScriptSystem), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CScriptSystem", "bool exists(Entity)", asFUNCTION(angelScript_exists), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CScriptSystem", "void create(Entity, const string &in)", asFUNCTION(angelScript_create), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CScriptSystem", "void destroy(Entity)", asFUNCTION(angelScript_destroy), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CScriptSystem", "ref getComponent(Entity)", asFUNCTION(angelScript_getComponent), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterGlobalProperty("CScriptSystem@ Script", scriptSys));
    }
}
