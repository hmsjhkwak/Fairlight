#include "Core/Core.h"
#include "Script/AngelState.h"
#include "AngelApplication.h"
#include "Application.h"

void angelApplication_RegisterTypes(asIScriptEngine* engine, Application** app)
{
    AS_VERIFY(engine->RegisterObjectType("CApplication", sizeof(Application), asOBJ_REF | asOBJ_NOCOUNT));
    AS_VERIFY(engine->RegisterObjectMethod("CApplication", "void loadScene(AssetRef)", asMETHOD(Application, loadScene), asCALL_THISCALL));
    AS_VERIFY(engine->RegisterObjectMethod("CApplication", "void restartScene()", asMETHOD(Application, restartScene), asCALL_THISCALL));
    AS_VERIFY(engine->RegisterObjectMethod("CApplication", "void exit()", asMETHOD(Application, exit), asCALL_THISCALL));
    AS_VERIFY(engine->RegisterGlobalProperty("CApplication@ Application", app));
}
