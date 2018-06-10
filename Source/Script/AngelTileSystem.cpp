#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "AngelMacros.h"
#include "Scene/EntityManager.h"
#include "Scene/TileSystem.h"

using namespace scene;

namespace script
{
    void angelTile_RegisterTypes(asIScriptEngine* engine, TileSystem** camSys)
    {
        AS_VERIFY(engine->RegisterObjectType("CTileSystem", sizeof(TileSystem), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterObjectMethod("CTileSystem", "bool isFree(IntRect)", asMETHOD(TileSystem, isFree), asCALL_THISCALL));
        AS_VERIFY(engine->RegisterGlobalProperty("CTileSystem@ Tile", camSys));
    }
}
