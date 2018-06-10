#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "AngelMacros.h"
#include "Render/PostProcessingManager.h"

using namespace render;

namespace script
{
    void angelPostProcess_setSaturation(PostProcessingManager* postMan, float saturation)
    {
        postMan->setSaturationMultiplier(saturation);
    }

    void angelPostProcess_RegisterTypes(asIScriptEngine* engine, PostProcessingManager** postMan)
    {
        AS_VERIFY(engine->RegisterObjectType("CPostProcess", sizeof(PostProcessingManager), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CPostProcess", "void setSaturation(float)", asFUNCTION(angelPostProcess_setSaturation), asCALL_CDECL_OBJFIRST));
        AS_VERIFY(engine->RegisterGlobalProperty("CPostProcess@ PostProcess", postMan));
    }
}
