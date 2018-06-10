#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "AngelMacros.h"
#include "../Scene/EntityManager.h"
#include "../Scene/CameraSystem.h"

using namespace scene;

namespace script
{
    void angelCamera_RegisterTypes(asIScriptEngine* engine, CameraSystem** camSys)
    {
        AS_VERIFY(engine->RegisterObjectType("CCameraSystem", sizeof(CameraSystem), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CCameraSystem", "Vector2i getLeftTop()", asMETHOD(CameraSystem, getLeftTop), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CCameraSystem", "Vector2i getCenter()", asMETHOD(CameraSystem, getCenter), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CCameraSystem", "void setLeftTop(Vector2i)", asMETHOD(CameraSystem, setLeftTop), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CCameraSystem", "void setCenter(Vector2i)", asMETHOD(CameraSystem, setCenter), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterGlobalProperty("CCameraSystem@ Camera", camSys));
    }
}
