#ifndef SCENE_SCENE_H
#define SCENE_SCENE_H

#include <EASTL/vector.h>
#include "EntityManager.h"
#include "TransformSystem.h"
#include "SpriteSystem.h"
#include "MovementSystem.h"
#include "ScriptSystem.h"
#include "TagSystem.h"
#include "TileSystem.h"
#include "CameraSystem.h"

namespace asset { class AssetManager; struct FileSpan; }
namespace render { class RenderManager; }
using namespace asset;
using namespace render;

namespace scene
{
    class Scene
    {
    public:
        //Stores a reference to the beginning of a prefab's data (stored inside
        //the specific scene which it was originally queried from).
        struct PrefabData
        {
            //Offset into the scene's prefab data array
            uint32_t offset;

            template <typename Archive> void serialize(Archive& ar) { ar.serializeU32(offset); }
        };

    private:
        uint32_t _deltaTime;
        uint32_t _sceneTime;

        EntityManager _entityManager;
        TransformSystem _trSystem;
        SpriteSystem _spriteSystem;
        MovementSystem _moveSystem;
        ScriptSystem _scriptSystem;
        TagSystem _tagSystem;
        TileSystem _tileSystem;
        CameraSystem _camSystem;

        eastl::vector<uint8_t> _prefabData;
        //Maps prefab hash to offset into _prefabData
        eastl::hash_map<AssetRef, PrefabData> _prefabMap;

    public:
        Scene();

        template <typename Archive> void serialize(Archive& ar);
        void load(AssetManager& assetMan, PackFile& pack, const FileSpan& span, script::AngelState& angelState);
#ifdef NW_ASSET_COOK
        void save(EndianVectorWriteArchive& ar, uint32_t entityCount);
        void addPrefab(AssetRef ref, const uint8_t* buffer, uint32_t length);
#endif

        void handleInstantiated(AssetManager& assetMan);
        void update(AssetManager& assetMan, uint32_t deltaTime);
        void render(RenderManager& renderManager);

        PrefabData getPrefab(AssetRef ref);
        Entity instantiate(PrefabData prefab);
        void instantiate(Entity e, PrefabData prefab);

        inline float getTime() { return (float)_sceneTime / 1000.0f; }
        inline float getDeltaTime() { return (float)_deltaTime / 1000.0f; }

        inline EntityManager& getEntityManager() { return _entityManager; }
        inline TransformSystem& getTransformSystem() { return _trSystem; }
        inline SpriteSystem& getSpriteSystem() { return _spriteSystem; }
        inline MovementSystem& getMovementSystem() { return _moveSystem; }
        inline ScriptSystem& getScriptSystem() { return _scriptSystem; }
        inline TagSystem& getTagSystem() { return _tagSystem; }
        inline TileSystem& getTileSystem() { return _tileSystem; }
        inline CameraSystem& getCameraSystem() { return _camSystem; }
    };
}

#endif
