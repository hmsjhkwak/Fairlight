#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "../Scene/Scene.h"

using namespace scene;

namespace script
{
    Entity angelScene_instantiate(Scene* scene, AssetRef prefabRef)
    {
        return scene->instantiate(scene->getPrefab(prefabRef));
    }

    void angelScene_RegisterTypes(asIScriptEngine* engine, Scene** scene)
    {
        AS_VERIFY(engine->RegisterObjectType("CScene", sizeof(Entity), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CScene", "Entity instantiate(AssetRef)", asFUNCTION(angelScene_instantiate), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterObjectMethod("CScene", "float getTime()", asMETHOD(Scene, getTime), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CScene", "float getDeltaTime()", asMETHOD(Scene, getDeltaTime), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterGlobalProperty("CScene@ Scene", scene));
    }
}
