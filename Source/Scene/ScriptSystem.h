#ifndef SCENE_SCRIPT_SYSTEM_H
#define SCENE_SCRIPT_SYSTEM_H

#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include "Core/SoaVector.h"
#include "Core/Features.h"
#include "Util/Archives.h"
#include "Script/AngelType.h"
#include "Entity.h"
#include "EInstance.h"

class asIScriptFunction;
class asIScriptObject;
class asITypeInfo;
namespace script { class AngelState; }

namespace scene
{
    class ScriptSystem
    {
    private:
        //Just keep the pointer around since this is basically global
        script::AngelState* _angelState;

        asITypeInfo* _componentBaseClass;

        eastl::hash_map<Entity, EInstance> _map;

        //Init, update, and dispose are indices into the _scriptStr array in
        //case the vector is resized (when new script functions are added).
        CLASS_SOA_VECTOR4(Storage,
            asIScriptObject*, object,
            asIScriptFunction*, updateFn,
            Entity, entities,
            script::AngelType, aType);
        Storage _data;

        //The list of entities that need their init() called
        eastl::vector<Entity> _needInit;

#ifdef NW_ASSET_COOK
        bool _isCooking;

        //When cooking a scene, script object variable overrides are stored in
        //this temporary array. Each entity has its own archive in which
        //variables can be serialized into.
        //
        //Yeah, it's a vector of vectors (ugh), but it's only for cook time.
        eastl::vector<eastl::vector<char>> _variableOverrides;
#endif

    public:
        ScriptSystem();

        void init(script::AngelState& angel);

        template <typename Archive>
        void serialize(Archive& ar)
        {
            uint32_t length = _data.getSize();
            ar.serializeU32(length);

            if (length == 0)
            {
                return;
            }

            //If we're reading, then preallocate some space
            if (ar.IsReading)
            {
                _data.resize(length + 128);
                _data.setSize(length);
            }

            //Serialize fields
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.entities, length);
            AR_SERIALIZE_ARRAY_CUSTOM(ar, _data.aType, length);

            //Add entities to map
            if (ar.IsReading)
            {
                for (uint32_t i = 0; i < length; i++)
                {
                    _map[_data.entities[i]] = i;
                    _needInit.push_back(_data.entities[i]);
                }
                memset(_data.object, 0, sizeof(*_data.object) * length);

                //Read variable overrides into separate memory
                //Kinda hacky, but I'd rather not have to include the angelscript headers
                uint32_t overrideTotalSize;
                uint32_t instanceCount;
                ar.serializeU32(overrideTotalSize);
                ar.serializeU32(instanceCount);

                eastl::vector<char> overrideData;
                overrideData.resize(overrideTotalSize);
                AR_SERIALIZE_ARRAY_CHAR(ar, overrideData.data(), overrideTotalSize);

                createLoadedScriptObjects();
                handleOverrides(instanceCount, overrideData.data(), overrideTotalSize);
            }
#ifdef NW_ASSET_COOK
            else
            {
                //Sum up totals for variable overrides
                uint32_t overrideTotalSize = 0;
                uint32_t instanceCount = 0;
                for (uint32_t i = 0; i < (uint32_t)_variableOverrides.size(); i++)
                {
                    auto& overrides = _variableOverrides[i];
                    if (overrides.size() > 0)
                    {
                        instanceCount++;
                        overrideTotalSize += sizeof(EInstance);             //EInstance
                        overrideTotalSize += sizeof(uint32_t);              //size
                        overrideTotalSize += (uint32_t)overrides.size();    //num of bytes
                    }
                }
                ar.serializeU32(overrideTotalSize);
                ar.serializeU32(instanceCount);

                //Write out variable overrides
                for (uint32_t i = 0; i < (uint32_t)_variableOverrides.size(); i++)
                {
                    auto& overrides = _variableOverrides[i];
                    if (overrides.size() > 0)
                    {
                        EInstance ei(i);
                        uint32_t size = (uint32_t)overrides.size();
                        ar.serializeCustom(ei);
                        ar.serializeU32(size);
                        AR_SERIALIZE_ARRAY_CHAR(ar, overrides.data(), size);
                    }
                }
            }
#endif
        }

        void handleOverrides(uint32_t instanceCount, char* data, uint32_t dataSize);

#ifdef NW_ASSET_COOK
        void setCooking(bool cooking) { _isCooking = cooking; }
        void addVariableOverride(EInstance ei, const void* data, uint32_t size);
        uint8_t* convertToPrefab(Entity e, uint8_t* buffer);
#endif

        void update();

        bool exists(Entity e) { return _map.find(e) != _map.end(); }
        EInstance create(Entity e, script::AngelType aType);
        void destroy(Entity e);
        const uint8_t* instantiate(Entity e, const uint8_t* data);

        void handleDestroyed(const Entity* destroyed, size_t destroyedLen);

        EInstance getInstance(Entity e)
        {
            NW_ASSERT(exists(e));
            return _map[e];
        }

        script::AngelType getAngelType(EInstance ei) { return _data.aType[ei.index]; }
        asIScriptObject* getObject(EInstance ei) { return _data.object[ei.index]; }


    private:
        void createLoadedScriptObjects();
        asIScriptObject* createObjectOfType(script::AngelType aType);
        void setObjectEntity(asIScriptObject* obj, Entity e);

        void handleInstanceOverrides(util::MemoryReadArchive& ar, EInstance ei, uint32_t size);
        void serializeVariable(util::MemoryReadArchive& ar, EInstance ei, script::AngelType parentType, void* dest);
        void serializeValue(util::MemoryReadArchive& ar, EInstance ei, script::AngelType parentType, void* dest, int propTypeId);

        void moveInstance(EInstance dst, EInstance src);

        Entity getEntity(EInstance ei) { return _data.entities[ei.index]; }

        void callMethod(Entity e, asIScriptObject* obj, asIScriptFunction* fn);
    };
}

#endif
