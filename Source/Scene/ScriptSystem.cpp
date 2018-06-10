#include "Core/Core.h"
#include "ScriptSystem.h"
#include <angelscript.h>
#include "Script/AngelState.h"
#include "Script/AngelArray.h"
#include "Asset/PackFile.h"
#include "Asset/AssetManager.h"
#include "Util/Archives.h"

namespace scene
{
    ScriptSystem::ScriptSystem()
        : _angelState(nullptr)
#ifdef NW_ASSET_COOK
        , _isCooking(false)
#endif
    {
    }

    void ScriptSystem::init(script::AngelState& angelState)
    {
        _angelState = &angelState;

        script::AngelType componentBase("ComponentBase");
        _componentBaseClass = _angelState->getTypeInfoFromAngelType(componentBase);
    }

    void ScriptSystem::createLoadedScriptObjects()
    {
        //Create all initial script objects that were loaded from file
        for (uint32_t i = 0; i < _data.getSize(); i++)
        {
            asIScriptObject* obj = createObjectOfType(_data.aType[i]);
            _data.object[i] = obj;

            setObjectEntity(obj, getEntity(EInstance(i)));
        }
    }

    void ScriptSystem::handleOverrides(uint32_t instanceCount, char* data, uint32_t dataSize)
    {
        util::MemoryReadArchive ar;
        ar.init(data, dataSize);

        for (uint32_t i = 0; i < instanceCount; i++)
        {
            EInstance ei;
            uint32_t size = 0;
            ar.serializeCustom(ei);
            ar.serializeU32(size);
            handleInstanceOverrides(ar, ei, size);
        }
    }

    void ScriptSystem::handleInstanceOverrides(util::MemoryReadArchive& ar, EInstance ei, uint32_t size)
    {
        const char* start = ar.currentPtr();
        const char* end = start + size;

        script::AngelType aType = getAngelType(ei);
        while (ar.currentPtr() < end)
        {
            serializeVariable(ar, ei, aType, nullptr);
        }
        NW_ASSERT(ar.currentPtr() == end);
    }

    void ScriptSystem::serializeVariable(util::MemoryReadArchive& ar, EInstance ei, script::AngelType parentType, void* dest)
    {
        //Read name hash and type
        uint32_t nameHash;
        script::AngelType aType;
        ar.serializeU32(nameHash);
        ar.serializeCustom(aType);

        //Find the property index
        int propIndex = _angelState->getPropertyIndex(parentType, nameHash, aType);
        NW_ASSERT(propIndex != -1);

        //Get serialized/runtime type IDs (and offset off of dest pointer)
        int serializedTypeId = _angelState->getTypeIdFromAngelType(aType);

        int runtimeTypeId, offset;
        const char* propName = nullptr;
        const asITypeInfo* typeInfo = _angelState->getTypeInfoFromAngelType(parentType);
        typeInfo->GetProperty(propIndex, &propName, &runtimeTypeId, nullptr, nullptr, &offset);

        //Make sure that the type is correct
        if (serializedTypeId == runtimeTypeId)
        {
            //If dest is null, that means that this is the script component object
            if (dest == nullptr)
            {
                asIScriptObject* obj = getObject(ei);
                NW_ASSERT(obj != nullptr);
                dest = obj->GetAddressOfProperty(propIndex);
            }
            else
            {
                dest = (char*)dest + offset;
            }

            serializeValue(ar, ei, parentType, dest, runtimeTypeId);
        }
        else
        {
            printf("Warning: Overriding variable %s. Types do not match.\n", propName);

            //Changing this to assert for now. Eventually, we want to hotswap
            //scripts and so we should be able to ignore certain fields that
            //no longer match up, but we're not currently storing the original
            //size of the data. So how would we know how far to skip ahead?
            NW_REQUIRE(false);
        }
    }

    void ScriptSystem::serializeValue(util::MemoryReadArchive& ar, EInstance ei, script::AngelType parentType, void* dest, int propTypeId)
    {
        asITypeInfo* propTypeInfo = _angelState->getScriptEngine()->GetTypeInfoById(propTypeId);

        switch (propTypeId)
        {
        case asTYPEID_BOOL:
        {
            //Bool might not be 1 byte
            uint8_t temp;
            ar.serializeU8(temp);
            *(bool*)dest = (temp != 0);
        }
        break;
        case asTYPEID_INT8: ar.serializeI8(*(int8_t*)dest); break;
        case asTYPEID_INT16: ar.serializeI16(*(int16_t*)dest); break;
        case asTYPEID_INT32: ar.serializeI32(*(int32_t*)dest); break;
        case asTYPEID_INT64: ar.serializeI64(*(int64_t*)dest); break;
        case asTYPEID_UINT8: ar.serializeU8(*(uint8_t*)dest); break;
        case asTYPEID_UINT16: ar.serializeU16(*(uint16_t*)dest); break;
        case asTYPEID_UINT32: ar.serializeU32(*(uint32_t*)dest); break;
        case asTYPEID_UINT64: ar.serializeU64(*(uint64_t*)dest); break;
        case asTYPEID_FLOAT: ar.serializeF32(*(float*)dest); break;
        case asTYPEID_DOUBLE: ar.serializeF64(*(double*)dest); break;
        default:
            //This is a little dirty, but I'm not sure we can get around it.
            //For custom types, we can recurse down until we reach a primitive
            //and serialize that. But for types like AssetRef that are opaque
            //to the scripting language, we don't want to expose those fields.
            if (propTypeId == _angelState->getTypeIdFromAngelType(script::AngelType("AssetRef")))
            {
                ar.serializeCustom(*(asset::AssetRef*)dest);
            }
            else if (propTypeInfo != nullptr && strcmp(propTypeInfo->GetName(), "array") == 0)
            {
                uint32_t arraySize = 0;
                ar.serializeU32(arraySize);

                CScriptArray* output = (CScriptArray*)dest;
                NW_ASSERT(propTypeInfo->GetSubTypeCount() == 1);
                int subTypeId = propTypeInfo->GetSubTypeId(0);

                //Resize array to correct size
                output->Resize(arraySize);

                for (uint32_t i = 0; i < arraySize; i++)
                {
                    serializeValue(ar, ei, parentType, output->At(i), subTypeId);
                }
            }
            else
            {
                uint32_t propCount = 0;
                ar.serializeU32(propCount);

                for (uint32_t i = 0; i < propCount; i++)
                {
                    serializeVariable(ar, ei, _angelState->getAngelTypeFromTypeId(propTypeId), dest);
                }
            }
            break;
        }
    }


#ifdef NW_ASSET_COOK
    void ScriptSystem::addVariableOverride(EInstance ei, const void* data, uint32_t size)
    {
        auto& overrides = _variableOverrides[ei.index];
        size_t oldSize = overrides.size();
        overrides.resize(oldSize + size);
        memcpy(overrides.data() + oldSize, data, size);
    }

    uint8_t* ScriptSystem::convertToPrefab(Entity e, uint8_t* buffer)
    {
        EInstance ei = getInstance(e);

        script::AngelType aType = _data.aType[ei.index];
        memcpy(buffer, &aType, sizeof(aType)); buffer += sizeof(aType);

        //Copy size and variable override data
        auto& overrides = _variableOverrides[ei.index];
        uint32_t varOverrideSize = (uint32_t)overrides.size();
        memcpy(buffer, &varOverrideSize, sizeof(varOverrideSize)); buffer += sizeof(varOverrideSize);
        memcpy(buffer, overrides.data(), varOverrideSize); buffer += varOverrideSize;

        return buffer;
    }
#endif

    void ScriptSystem::update()
    {
        SCOPED_CPU_EVENT(event)(0xFFFFFFFF, "ScriptSystem::update");

        _angelState->startExecution();

        //Call the init functions that we need to
        for (Entity e : _needInit)
        {
            EInstance ei = getInstance(e);
            asIScriptObject* obj = _data.object[ei.index];

            //TODO: Cache this function lookup
            asITypeInfo* type = obj->GetObjectType();
            asIScriptFunction* fn = type->GetMethodByName("init");
            if (fn != nullptr)
            {
                callMethod(e, obj, fn);
            }
        }

        _needInit.clear();

        //Save the value (it changes when entities are added)
        uint32_t length = _data.getSize();
        for (size_t i = 0; i < length; i++)
        {
            //TODO: Separate array of entities that need update called
            //Then we can cut out the branch

            asIScriptObject* obj = _data.object[i];
            //TODO: Cache this function lookup
            asITypeInfo* type = obj->GetObjectType();
            asIScriptFunction* fn = type->GetMethodByName("update");
            if (fn != nullptr)
            {
                callMethod(_data.entities[i], obj, fn);
            }
        }

        _angelState->endExecution();
    }

    void ScriptSystem::setObjectEntity(asIScriptObject* obj, Entity e)
    {
        //Set the entity variable
        //TODO: Cache the offset?
        for (uint32_t i = 0; i < obj->GetPropertyCount(); i++)
        {
            const char* name = obj->GetPropertyName(i);
            if (strcmp(name, "_entity") == 0)
            {
                Entity* enPtr = (Entity*)obj->GetAddressOfProperty(i);
                *enPtr = e;
                break;
            }
        }
    }

    asIScriptObject* ScriptSystem::createObjectOfType(script::AngelType aType)
    {
        asITypeInfo* type = _angelState->getTypeInfoFromAngelType(aType);

        //Make sure that the type inherits from the correct base class
        //Debug/development builds only?
        if (!type->DerivesFrom(_componentBaseClass))
        {
            printf("Type %s doesn't inherit from %s.", type->GetName(), _componentBaseClass->GetName());
            return nullptr;
        }

        //Create the object
        asIScriptEngine* engine = _angelState->getScriptEngine();
        asIScriptObject* obj = (asIScriptObject*)engine->CreateScriptObject(type);

        return obj;
    }

    EInstance ScriptSystem::create(Entity e, script::AngelType aType)
    {
        asIScriptObject* obj = createObjectOfType(aType);
        if (obj == nullptr)
        {
            return EInstance();
        }
        obj->AddRef();  //We're keeping this reference around

        setObjectEntity(obj, e);

        EInstance ei = EInstance(_data.getSize());
        _map.insert(eastl::make_pair(e, ei));

        _data.push(
            obj,
            nullptr,
            e,
            aType);

#ifdef NW_ASSET_COOK
        if (_isCooking)
        {
            _variableOverrides.push_back();
        }
#endif

        _needInit.push_back(e);

        return ei;
    }

    void ScriptSystem::destroy(Entity e)
    {
        NW_ASSERT(exists(e));

        const EInstance ei = _map[e];
        const EInstance lastInst(_data.getSize() - 1);
        const Entity lastEntity = getEntity(lastInst);

        //Decrement the ref count
        _data.object[ei.index]->Release();

        //Copy the last component to the removed position
        moveInstance(ei, lastInst);

        //Remove last
        _data.pop();

#ifdef NW_ASSET_COOK
        if (_isCooking)
        {
            if (lastInst.index != ei.index)
            {
                _variableOverrides[ei.index] = _variableOverrides[lastInst.index];
            }
            _variableOverrides.pop_back();
        }
#endif

        //Update the keys in the map
        _map[lastEntity] = ei;
        _map.erase(e);
    }

    const uint8_t* ScriptSystem::instantiate(Entity e, const uint8_t* data)
    {
        script::AngelType aType;
        uint32_t varOverrideSize;
        memcpy(&aType, data, sizeof(aType)); data += sizeof(aType);
        memcpy(&varOverrideSize, data, sizeof(varOverrideSize)); data += sizeof(varOverrideSize);

        EInstance ei = create(e, aType);

#ifdef NW_ASSET_COOK
        if (_isCooking)
        {
            //When cooking, we don't actually need to properly deserialize the
            //variables onto the angelscript object. Instead, we just add the
            //data to the _overrideVariables array and append to it later.
            //
            //This means that a variable might be overwritten twice (once from
            //the prefab changing it, once from the object changing it), but
            //for now it's not a big deal.
            addVariableOverride(ei, data, varOverrideSize);
            data += varOverrideSize;
        }
        else
#endif
        {
            //Serialize the overriden variables
            util::MemoryReadArchive ar;
            ar.init(data, varOverrideSize);
            handleInstanceOverrides(ar, ei, varOverrideSize);
            data += varOverrideSize;
        }

        return data;
    }

    void ScriptSystem::moveInstance(EInstance dst, EInstance src)
    {
        _data.move(dst.index, src.index);
    }

    void ScriptSystem::handleDestroyed(const Entity* destroyed, size_t destroyedLen)
    {
        _angelState->startExecution();

        for (size_t i = 0; i < destroyedLen; i++)
        {
            auto result = _map.find(destroyed[i]);
            if (result != _map.end())
            {
                //Call disposal function
                //TODO
                //uint32_t offset = _data.dispose[result->second.index];
                //if (offset != UINT32_MAX) { callFn(destroyed[i], offset); }

                //Destroy the component
                destroy(result->first);
            }
        }

        _angelState->endExecution();
    }



    void ScriptSystem::callMethod(Entity e, asIScriptObject* obj, asIScriptFunction* fn)
    {
        NW_UNUSED(e);

#ifdef NW_DEVELOP
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s::%s", obj->GetObjectType()->GetName(), fn->GetName());
        SCOPED_CPU_EVENT(event)(0xFFFFFFFF, buffer);
#endif

        auto* ctx = _angelState->getScriptContext();

        ctx->Prepare(fn);
        ctx->SetObject(obj);
        int r = ctx->Execute();
        if (r > 0)
        {
            printf("Error executing %s : %s\n", fn->GetName(), ctx->GetExceptionString());
            for (uint32_t i = 0; i < ctx->GetCallstackSize(); i++)
            {
                asIScriptFunction *func;
                const char *scriptSection;
                int line, column;
                func = ctx->GetFunction(i);
                line = ctx->GetLineNumber(i, &column, &scriptSection);
                printf("%s:%s:%d,%d\n", scriptSection, func->GetDeclaration(), line, column);
            }
        }
    }
}
