#include "Core/Core.h"
#include <angelscript.h>
#include "AngelState.h"
#include "Asset/AssetRef.h"
#include "Asset/AssetManager.h"

using namespace asset;

namespace script
{
    void angelAsset_AssetRef_Construct(AssetRef* ref, uint32_t id)
    {
        new (ref) AssetRef(id);
    }

    void angelAsset_RegisterTypes(asIScriptEngine* engine, AssetManager** assetManager)
    {
        AS_VERIFY(engine->RegisterObjectType("AssetRef", sizeof(AssetRef), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AssetRef>()));
        //HACK: This shouldn't be needed eventually
        AS_VERIFY(engine->RegisterObjectBehaviour("AssetRef", asBEHAVE_CONSTRUCT, "void f(uint id)", asFUNCTION(angelAsset_AssetRef_Construct), asCALL_CDECL_OBJFIRST));

        AS_VERIFY(engine->RegisterObjectType("CAssetManager", sizeof(AssetManager), asOBJ_REF | asOBJ_NOCOUNT));
        AS_VERIFY(engine->RegisterGlobalProperty("CAssetManager@ AssetManager", assetManager));
    }
}
