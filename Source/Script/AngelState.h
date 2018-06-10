#ifndef SCRIPT_ANGEL_STATE_H
#define SCRIPT_ANGEL_STATE_H

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <EASTL/hash_map.h>
#include "AngelType.h"

namespace asset { class AssetManager; }
namespace render { class PostProcessingManager; }
namespace scene
{
    class Scene;
    class EntityManager;
    class TagSystem;
    class TransformSystem;
    class MovementSystem;
    class SpriteSystem;
    class ScriptSystem;
    class TileSystem;
    class CameraSystem;
}
namespace path { class PathManager; }
namespace input { class Input; }

namespace script
{
    struct AngelPropertyKey
    {
        int typeId;
        uint32_t propNameHash;
        script::AngelType propType;

        inline bool operator==(AngelPropertyKey other) const
        {
            return
                typeId == other.typeId &&
                propNameHash == other.propNameHash &&
                propType == other.propType;
        }
        inline bool operator!=(AngelPropertyKey other) const
        {
            return
                typeId != other.typeId &&
                propNameHash != other.propNameHash &&
                propType != other.propType;
        }
    };
}

template <>
struct eastl::hash<script::AngelPropertyKey>
{
    size_t operator()(script::AngelPropertyKey val) const
    {
        auto* state = XXH32_createState();
        XXH32_reset(state, 0);
        XXH32_update(state, &val.typeId, sizeof(val.typeId));
        XXH32_update(state, &val.propNameHash, sizeof(val.propNameHash));
        XXH32_update(state, &val.propType.nameHash, sizeof(val.propType.nameHash));
        XXH32_update(state, &val.propType.type, sizeof(val.propType.type));
        uint32_t output = XXH32_digest(state);
        XXH32_freeState(state);
        return output;
    }
};

namespace script
{
    void registerTemplateInstance(asITypeInfo* ot);

    class AngelState
    {
    private:
        const char* MODULE_NAME = "GameModule";

        bool _isCompiling;
        CScriptBuilder _scriptBuilder;
        asIScriptEngine* _scriptEngine;
        asIScriptContext* _scriptContext;

        asset::AssetManager* _assetManager;
        render::PostProcessingManager* _postProcessingManager;
        scene::Scene* _scene;
        scene::EntityManager* _entityManager;
        scene::TagSystem* _tagSystem;
        scene::TransformSystem* _transformSystem;
        scene::MovementSystem* _movementSystem;
        scene::SpriteSystem* _spriteSystem;
        scene::ScriptSystem* _scriptSystem;
        scene::TileSystem* _tileSystem;
        scene::CameraSystem* _cameraSystem;
        path::PathManager* _pathManager;
        input::Input* _input;

        //=====================================================================
        //Caching
        //=====================================================================

        //Maps our AngelType wrapper struct to the actual angelscript type id
        //(which is generated at runtime by just bumping up an integer)
        eastl::hash_map<AngelType, int> _typeMap;

        //Constant time lookup of property indices based on name hash and type
        eastl::hash_map<AngelPropertyKey, int> _propIndexMap;

    public:
        AngelState() :
            _isCompiling(false),
            _scriptEngine(nullptr),
            _scriptContext(nullptr),
            _assetManager(nullptr),
            _postProcessingManager(nullptr),
            _scene(nullptr),
            _entityManager(nullptr),
            _tagSystem(nullptr),
            _transformSystem(nullptr),
            _movementSystem(nullptr),
            _spriteSystem(nullptr),
            _scriptSystem(nullptr),
            _tileSystem(nullptr),
            _cameraSystem(nullptr),
            _pathManager(nullptr),
            _input(nullptr)
        {
        }

        void init();
        void setAssetManager(asset::AssetManager& assetManager) { _assetManager = &assetManager; }
        void setInput(input::Input& input) { _input = &input; }
        void setPostProcessingManager(render::PostProcessingManager& postMan) { _postProcessingManager = &postMan; }
        void setScene(scene::Scene& scene);

        void startCompiling();
        void addScriptSection(const char* name, const char* section);
        void endCompiling();

        asIScriptEngine* getScriptEngine() { return _scriptEngine; }
        asIScriptContext* getScriptContext() { return _scriptContext; }

        asset::AssetManager* getAssetManager() { return _assetManager; }
        render::PostProcessingManager* getPostProcessingManager() { return _postProcessingManager; }
        scene::Scene* getScene() { return _scene; }
        scene::EntityManager* getEntityManager() { return _entityManager; }
        scene::TagSystem* getTagSystem() { return _tagSystem; }
        scene::TransformSystem* getTransformSystem() { return _transformSystem; }
        scene::MovementSystem* getMovementSystem() { return _movementSystem; }
        scene::SpriteSystem* getSpriteSystem() { return _spriteSystem; }
        scene::ScriptSystem* getScriptSystem() { return _scriptSystem; }
        scene::TileSystem* getTileSystem() { return _tileSystem; }
        scene::CameraSystem* getCameraSystem() { return _cameraSystem; }
        path::PathManager* getPathManager() { return _pathManager; }
        input::Input* getInput() { return _input; }

        //Returns the currently executing angel state in the thread
        static AngelState* getCurrent();
        void startExecution();
        void endExecution();

        script::AngelType getAngelTypeFromTypeId(int typeId);

        //=====================================================================
        // Caching
        //=====================================================================
        asITypeInfo* getTypeInfoFromAngelType(AngelType aType)
        {
            if (aType.type != AngelType::Type::CustomType) { return nullptr; }

            auto result = _typeMap.find(aType);
            if (result != _typeMap.end())
            {
                return _scriptEngine->GetTypeInfoById(result->second);
            }
            else
            {
                return nullptr;
            }
        }

        int getTypeIdFromAngelType(AngelType aType)
        {
            auto result = _typeMap.find(aType);
            if (result != _typeMap.end())
            {
                return result->second;
            }
            else
            {
                return -1;
            }
        }

        int getPropertyIndex(AngelType objType, uint32_t propNameHash, AngelType propType)
        {
            AngelPropertyKey key;
            key.typeId = getTypeIdFromAngelType(objType);
            key.propNameHash = propNameHash;
            key.propType = propType;
            auto result = _propIndexMap.find(key);
            if (result != _propIndexMap.end())
            {
                return result->second;
            }
            else
            {
                return -1;
            }
        }

    private:
        void cacheType(asITypeInfo* typeInfo);
    };

    void angelMath_RegisterTypes(asIScriptEngine* engine);
    void angelAsset_RegisterTypes(asIScriptEngine* engine, asset::AssetManager** assetManager);
    void angelInput_RegisterTypes(asIScriptEngine* engine, input::Input** input);
    void angelSound_RegisterTypes(asIScriptEngine* engine);
    void angelEntity_RegisterTypes(asIScriptEngine* engine, scene::EntityManager** entityManager);
    void angelScene_RegisterTypes(asIScriptEngine* engine, scene::Scene** scene);
    void angelTag_RegisterTypes(asIScriptEngine* engine, scene::TagSystem** tagSys);
    void angelTransform_RegisterTypes(asIScriptEngine* engine, scene::TransformSystem** trSys);
    void angelMovement_RegisterTypes(asIScriptEngine* engine, scene::MovementSystem** moveSys);
    void angelSprite_RegisterTypes(asIScriptEngine* engine, scene::SpriteSystem** spriteSys);
    void angelScript_RegisterTypes(asIScriptEngine* engine, scene::ScriptSystem** scriptSys);
    void angelTile_RegisterTypes(asIScriptEngine* engine, scene::TileSystem** tileSys);
    void angelCamera_RegisterTypes(asIScriptEngine* engine, scene::CameraSystem** camSys);
    void angelPostProcess_RegisterTypes(asIScriptEngine* engine, render::PostProcessingManager** postMan);
}

#define AS_VERIFY(s) NW_VERIFY((s) >= 0);

#endif
