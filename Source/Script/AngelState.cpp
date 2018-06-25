#include "Core/Core.h"
#include <stdio.h>
#include <EASTL/hash_set.h>
#include <angelscript.h>
#include "AngelState.h"
#include "AngelString.h"
#include "AngelArray.h"
#include "AngelDictionary.h"
#include "AngelHandle.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetType.h"
#include "Scene/Scene.h"

using namespace asset;

void messageCallback(const asSMessageInfo *msg, void *param)
{
    NW_UNUSED(param);

    const char *type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING) { type = "WARN"; }
    else if (msg->type == asMSGTYPE_INFORMATION) { type = "INFO"; }

    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void print(const std::string& msg)
{
    printf(msg.c_str());
}

uint32_t strhash(const std::string& input)
{
    return XXH32(input.data(), input.size(), 0);
}

namespace script
{
    const char* getTypeNameById(asIScriptEngine* engine, int typeId)
    {
        switch (typeId)
        {
        case asTYPEID_BOOL: return "bool";
        case asTYPEID_INT8: return "int8";
        case asTYPEID_INT16: return "int16";
        case asTYPEID_INT32: return "int32";
        case asTYPEID_INT64: return "int64";
        case asTYPEID_UINT8: return "uint8";
        case asTYPEID_UINT16: return "uint16";
        case asTYPEID_UINT32: return "uint32";
        case asTYPEID_UINT64: return "uint64";
        case asTYPEID_FLOAT: return "float";
        case asTYPEID_DOUBLE: return "double";
        default: return engine->GetTypeInfoById(typeId)->GetName();
        }
    }

    eastl::string getTypeInfoDecl(asITypeInfo* ot)
    {
        eastl::string decl;
        decl.append(ot->GetName());
        if (ot->GetSubTypeCount() > 0)
        {
            decl.append("<");
            for (uint32_t i = 0; i < ot->GetSubTypeCount(); i++)
            {
                if (i != 0) { decl.append(","); }
                const char* name = getTypeNameById(ot->GetEngine(), ot->GetSubTypeId());
                decl.append(name);
            }
            decl.append(">");
        }
        return decl;
    }

    //Keep track of the template type instantiates for caching purposes
    //A bit of a hack, but we can't get them by iterating through the module types
    eastl::hash_set<eastl::string> g_templateInstances;
    void registerTemplateInstance(asITypeInfo* ot)
    {
        g_templateInstances.insert(getTypeInfoDecl(ot));
    }

    void AngelState::init()
    {
        AS_VERIFY(_scriptEngine = asCreateScriptEngine());
        AS_VERIFY(_scriptEngine->SetMessageCallback(asFUNCTION(messageCallback), 0, asCALL_CDECL));

        _scriptContext = _scriptEngine->CreateContext();

        RegisterStdString(_scriptEngine);
        RegisterScriptArray(_scriptEngine, false);
        RegisterScriptDictionary(_scriptEngine);
        RegisterScriptHandle(_scriptEngine);
        angelMath_RegisterTypes(_scriptEngine);
        angelAsset_RegisterTypes(_scriptEngine, &_assetManager);
        angelInput_RegisterTypes(_scriptEngine, &_input);
        angelSound_RegisterTypes(_scriptEngine);
        angelEntity_RegisterTypes(_scriptEngine, &_entityManager);
        angelScene_RegisterTypes(_scriptEngine, &_scene);
        angelTag_RegisterTypes(_scriptEngine, &_tagSystem);
        angelTransform_RegisterTypes(_scriptEngine, &_transformSystem);
        angelMovement_RegisterTypes(_scriptEngine, &_movementSystem);
        angelSprite_RegisterTypes(_scriptEngine, &_spriteSystem);
        angelScript_RegisterTypes(_scriptEngine, &_scriptSystem);
        angelTile_RegisterTypes(_scriptEngine, &_tileSystem);
        angelCamera_RegisterTypes(_scriptEngine, &_cameraSystem);
        angelPostProcess_RegisterTypes(_scriptEngine, &_postProcessingManager);

        AS_VERIFY(_scriptEngine->RegisterGlobalFunction("void print(string)", asFUNCTION(print), asCALL_CDECL));
        AS_VERIFY(_scriptEngine->RegisterGlobalFunction("uint strhash(const string &in)", asFUNCTION(strhash), asCALL_CDECL));
    }

    void AngelState::setScene(scene::Scene& scene)
    {
        _scene = &scene;
        _entityManager = &scene.getEntityManager();
        _tagSystem = &scene.getTagSystem();
        _transformSystem = &scene.getTransformSystem();
        _movementSystem = &scene.getMovementSystem();
        _spriteSystem = &scene.getSpriteSystem();
        _scriptSystem = &scene.getScriptSystem();
        _tileSystem = &scene.getTileSystem();
        _cameraSystem = &scene.getCameraSystem();
    }

    void AngelState::startCompiling()
    {
        NW_ASSERT(!_isCompiling);
        _isCompiling = true;
        AS_VERIFY(_scriptBuilder.StartNewModule(_scriptEngine, MODULE_NAME));
    }

    void AngelState::addScriptSection(const char* name, const char* section)
    {
        NW_ASSERT(_isCompiling);
        AS_VERIFY(_scriptBuilder.AddSectionFromMemory(name, section));
    }

    void AngelState::endCompiling()
    {
        NW_ASSERT(_isCompiling);

        int r = (_scriptBuilder.BuildModule());
        NW_REQUIRE(r >= 0);

        //Manually cache primitive types
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::Bool), asTYPEID_BOOL));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::Int8), asTYPEID_INT8));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::Int16), asTYPEID_INT16));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::Int32), asTYPEID_INT32));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::Int64), asTYPEID_INT64));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::UInt8), asTYPEID_UINT8));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::UInt16), asTYPEID_UINT16));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::UInt32), asTYPEID_UINT32));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::UInt64), asTYPEID_UINT64));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::Float32), asTYPEID_FLOAT));
        _typeMap.insert(eastl::make_pair(AngelType(AngelType::Type::Float64), asTYPEID_DOUBLE));

        //Cache registered types
        for (uint32_t i = 0; i < _scriptEngine->GetObjectTypeCount(); i++)
        {
            asITypeInfo* typeInfo = _scriptEngine->GetObjectTypeByIndex(i);
            cacheType(typeInfo);
        }

        //Cache template instantiations
        for (const auto& decl : g_templateInstances)
        {
            asITypeInfo* typeInfo = _scriptEngine->GetTypeInfoByDecl(decl.c_str());
            cacheType(typeInfo);
        }

        //Cache module types
        asIScriptModule* module = _scriptEngine->GetModule(MODULE_NAME);
        for (uint32_t i = 0; i < module->GetObjectTypeCount(); i++)
        {
            asITypeInfo* typeInfo = module->GetObjectTypeByIndex(i);
            cacheType(typeInfo);
        }

        _isCompiling = false;
    }

    void AngelState::cacheType(asITypeInfo* typeInfo)
    {
        eastl::string typeDecl = getTypeInfoDecl(typeInfo);
        AngelType aType(typeDecl.c_str());

        printf("%s %08x %08x\n", typeDecl.c_str(), aType.nameHash, typeInfo->GetTypeId());

        //Check for hash collisions
        if (_typeMap.find(aType) != _typeMap.end())
        {
            NW_ASSERT(false);
        }

        //Cache types
        _typeMap.insert(eastl::make_pair(aType, typeInfo->GetTypeId()));

        //Cache property indices
        for (uint32_t propIndex = 0; propIndex < typeInfo->GetPropertyCount(); propIndex++)
        {
            const char* name;
            int typeId;
            typeInfo->GetProperty(propIndex, &name, &typeId);
            uint32_t propNameHash = XXH32(name, strlen(name), 0);

            AngelPropertyKey key;
            key.typeId = typeInfo->GetTypeId();
            key.propNameHash = propNameHash;
            key.propType = getAngelTypeFromTypeId(typeId);

            printf("    %20s id:%08x name:%08x type:%2d | %08x\n", name, typeId, key.propNameHash, key.propType.type, key.propType.nameHash);

            //Check for hash collisions
            auto findResult = _propIndexMap.find(key);
            if (findResult != _propIndexMap.end())
            {
                printf("Collision!\n");
                NW_ASSERT(false);
            }

            _propIndexMap.insert(eastl::make_pair(key, propIndex));
        }
    }

    thread_local AngelState* g_angelState;
    AngelState* AngelState::getCurrent() { return g_angelState; }
    void AngelState::startExecution() { g_angelState = this; }
    void AngelState::endExecution() { g_angelState = nullptr; }

    AngelType AngelState::getAngelTypeFromTypeId(int typeId)
    {
        switch (typeId)
        {
        case asTYPEID_BOOL: return AngelType(AngelType::Type::Bool);
        case asTYPEID_INT8: return AngelType(AngelType::Type::Int8);
        case asTYPEID_INT16: return AngelType(AngelType::Type::Int16);
        case asTYPEID_INT32: return AngelType(AngelType::Type::Int32);
        case asTYPEID_INT64: return AngelType(AngelType::Type::Int64);
        case asTYPEID_UINT8: return AngelType(AngelType::Type::UInt8);
        case asTYPEID_UINT16: return AngelType(AngelType::Type::UInt16);
        case asTYPEID_UINT32: return AngelType(AngelType::Type::UInt32);
        case asTYPEID_UINT64: return AngelType(AngelType::Type::UInt64);
        case asTYPEID_FLOAT: return AngelType(AngelType::Type::Float32);
        case asTYPEID_DOUBLE: return AngelType(AngelType::Type::Float64);
        default:
            {
                asITypeInfo* type = _scriptEngine->GetTypeInfoById(typeId);
                //Small optimization: avoid string allocation for non template types
                if (type->GetFlags() & asOBJ_TEMPLATE)
                {
                    eastl::string decl = getTypeInfoDecl(type);
                    return AngelType(decl.c_str());
                }
                else
                {
                    return AngelType(type->GetName());
                }
            }
        }
    }
}
