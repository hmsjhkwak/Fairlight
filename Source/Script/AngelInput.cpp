#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "Input/Input.h"

using namespace input;

namespace script
{
    void angelInput_RegisterTypes(asIScriptEngine* engine, Input** input)
    {
        AS_VERIFY(engine->RegisterObjectType("CInputManager", sizeof(Input), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CInputManager", "bool moveLeft()", asMETHOD(Input, moveLeft), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CInputManager", "bool moveRight()", asMETHOD(Input, moveRight), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CInputManager", "bool moveUp()", asMETHOD(Input, moveUp), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CInputManager", "bool moveDown()", asMETHOD(Input, moveDown), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CInputManager", "bool jump()", asMETHOD(Input, jump), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CInputManager", "bool jumpHeld()", asMETHOD(Input, jumpHeld), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterObjectMethod("CInputManager", "bool attack()", asMETHOD(Input, attack), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterGlobalProperty("CInputManager@ Input", input));
    }
}
