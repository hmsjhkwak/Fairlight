#include "Core/Core.h"

#ifdef NW_ASSET_COOK
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <EASTL/hash_set.h>
#include <SDL_mixer.h>
#include <rapidjson/document.h>
#include <bimg/bimg.h>
#include "Util/File.h"
#include "Asset/AssetManager.h"
#include "Scene/Scene.h"
#include "Cook.h"
#include "CookAsset.h"
#include "Uuid.h"
#include "Script/AngelState.h"   //For AngelState and AngelType

using namespace rapidjson;
using namespace util;
using namespace scene;

namespace cook
{
    void cookTexture2D(const char* inputFile, AssetFileWriter& writer)
    {
        writer.setAssetType(AssetType::Texture);
        writer.setCompressed(true);

        FILE* input = fopen(inputFile, "rb");

        const int bufferSize = 1024;
        uint8_t buffer[bufferSize];
        size_t numRead = 0;
        while ((numRead = fread(buffer, 1, bufferSize, input)) > 0)
        {
            AR_SERIALIZE_ARRAY_U8(writer.ar, buffer, numRead);
        }
        fclose(input);
    }

    void readLines(std::ifstream& input, std::string& buffer, const char* target)
    {
        std::string line;
        while (std::getline(input, line))
        {
            if (line == target) { return; }

            buffer += line;
            buffer += '\n';
        }
    }

    void cookShader(const char* inputFile, AssetFileWriter& writer)
    {
        writer.setAssetType(AssetType::Shader);
        writer.setCompressed(true);

        FILE* input = fopen(inputFile, "rb");

        const int bufferSize = 1024;
        uint8_t buffer[bufferSize];
        size_t numRead = 0;
        while ((numRead = fread(buffer, 1, bufferSize, input)) > 0)
        {
            AR_SERIALIZE_ARRAY_U8(writer.ar, buffer, numRead);
        }
        fclose(input);
    }

    void cookSound(const char* inputFile, AssetFileWriter& writer)
    {
        writer.setAssetType(AssetType::Sound);
        writer.setCompressed(true);

        Mix_Chunk* chunk = Mix_LoadWAV(inputFile);

        //No point in saving the length; we can get it from the file/span size
        AR_SERIALIZE_ARRAY_U8(writer.ar, chunk->abuf, chunk->alen);
        Mix_FreeChunk(chunk);
    }

    void cookMusic(const char* inputFile, AssetFileWriter& writer)
    {
        writer.setAssetType(AssetType::Music);
        writer.setCompressed(false);

        FILE* input = fopen(inputFile, "rb");

        const int bufferSize = 1024;
        uint8_t buffer[bufferSize];
        size_t numRead = 0;
        while ((numRead = fread(buffer, 1, bufferSize, input)) > 0)
        {
            AR_SERIALIZE_ARRAY_U8(writer.ar, buffer, numRead);
        }
        fclose(input);
    }



    struct SceneCompilationData
    {
        Scene* scene;

        uint32_t hashSeed;
        uint32_t tempEntityIndex;
        const std::string* assetFolder;
        eastl::hash_set<AssetRef> usedPrefabs;
        eastl::hash_set<AssetRef> usedTextures;
        eastl::hash_set<AssetRef> usedSounds;

        script::AngelState* angelState;
    };

    void cookPrefab(SceneCompilationData& compData, const char* prefabName);

    Vector2i parseVector2i(const char* str)
    {
        Vector2i vec;
        sscanf(str, "%d %d", &vec.x, &vec.y);
        return vec;
    }

    Vector2f parseVector2f(const char* str)
    {
        Vector2f vec;
        sscanf(str, "%f %f", &vec.x, &vec.y);
        return vec;
    }

    IntRect parseIntRect(const char* str)
    {
        IntRect rect;
        sscanf(str, "%d %d %d %d", &rect.left, &rect.top, &rect.width, &rect.height);
        return rect;
    }

    std::vector<std::string> tokenize(std::string& input, char delim)
    {
        std::vector<std::string> output;
        std::stringstream stream(input);
        std::string item;
        while (std::getline(stream, item, delim))
        {
            output.push_back(item);
        }
        return output;
    }

    void addTags(SceneCompilationData& compData, Entity e, Value& jTags)
    {
        TagSystem& tagSystem = compData.scene->getTagSystem();
        EInstance ei = tagSystem.createOrGetInstance(e);

        for (auto jTag = jTags.Begin(); jTag < jTags.End(); jTag++)
        {
            const char* tag = jTag->GetString();
            auto tagLen = jTag->GetStringLength();
            uint32_t hash = XXH32(tag, tagLen, 0);
            tagSystem.addTag(ei, hash);
        }
    }

    void addTransformComponent(SceneCompilationData& compData, Entity e, const Value& jTransform,
        eastl::hash_map<Uuid, Entity>& uuidMap)
    {
        TransformSystem& trSystem = compData.scene->getTransformSystem();
        EInstance ei = trSystem.createOrGetInstance(e);

        //Set parent before we set local position
        if (jTransform.HasMember("parent"))
        {
            Uuid parentUuid(jTransform["parent"].GetString());
            trSystem.setParent(ei, trSystem.createOrGetInstance(uuidMap[parentUuid]));
        }
        if (jTransform.HasMember("pos"))
        {
            trSystem.setLocalPos(ei, parseVector2i(jTransform["pos"].GetString()));
        }
    }

    void addSpriteComponent(SceneCompilationData& compData, Entity e, const Value& jSprite)
    {
        SpriteSystem& spriteSystem = compData.scene->getSpriteSystem();
        EInstance ei = spriteSystem.createOrGetInstance(e);

        if (jSprite.HasMember("size"))
        {
            spriteSystem.setSize(ei, parseVector2i(jSprite["size"].GetString()));
        }
        if (jSprite.HasMember("offset"))
        {
            spriteSystem.setOffset(ei, parseVector2i(jSprite["offset"].GetString()));
        }
        if (jSprite.HasMember("depth"))
        {
            spriteSystem.setDepth(ei, (uint8_t)jSprite["depth"].GetInt());
        }
        if (jSprite.HasMember("texture"))
        {
            const char* textureName = jSprite["texture"].GetString();
            AssetRef textureRef = { XXH32(textureName, strlen(textureName), compData.hashSeed) };
            spriteSystem.setTextureRef(ei, textureRef);
            compData.usedTextures.emplace(textureRef);
        }
        if (jSprite.HasMember("texOffset"))
        {
            spriteSystem.setTexOffset(ei, parseVector2i(jSprite["texOffset"].GetString()));
        }
    }

    void addMovementComponent(SceneCompilationData& compData, Entity e, const Value& jMovement)
    {
        MovementSystem& moveSystem = compData.scene->getMovementSystem();
        EInstance ei = moveSystem.createOrGetInstance(e);

        if (jMovement.HasMember("size"))
        {
            moveSystem.setSize(ei, parseVector2i(jMovement["size"].GetString()));
        }
        if (jMovement.HasMember("offset"))
        {
            moveSystem.setOffset(ei, parseVector2i(jMovement["offset"].GetString()));
        }
        if (jMovement.HasMember("velocity"))
        {
            moveSystem.setVelocity(ei, parseVector2f(jMovement["velocity"].GetString()));
        }
        if (jMovement.HasMember("worldColl"))
        {
            moveSystem.setWorldCollision(ei, jMovement["worldColl"].GetBool());
        }
    }

    bool checkValueTypeDecl(const Value& input, const char* type, const char** output)
    {
        if (input.IsString())
        {
            const char* inputStr = input.GetString();
            size_t typeLen = strlen(type);
            if (strncmp(inputStr, type, typeLen) == 0)
            {
                *output = inputStr + typeLen;
                return true;
            }
        }
        return false;
    }

    bool checkObjectIsValueType(script::AngelState* angelState, int typeId)
    {
        asITypeInfo* info = angelState->getScriptEngine()->GetTypeInfoById(typeId);
        if (info != nullptr)
        {
            int32_t flags = info->GetFlags();
            return ((flags & asOBJ_VALUE) != 0);
        }
        return false;
    }

    //Forward declaration
    bool serializeVariable(
        SceneCompilationData& compData,
        util::EndianVectorWriteArchive& ar,
        int typeId,
        Value::ConstMemberIterator iter);

    bool serializeValue(
        SceneCompilationData& compData,
        util::EndianVectorWriteArchive& ar,
        const char* varName,
        int typeId,
        const Value& jValue)
    {
        bool typeMatches = false;   //Captured in if statements
        const char* valueStr = nullptr;

        //Other types we need to check against
        //Should cache these instead of hashing every time, but whatever...
        const int assetRefTypeId = compData.angelState->getTypeIdFromAngelType(script::AngelType("AssetRef"));

        if (typeId == asTYPEID_BOOL && (typeMatches = jValue.IsBool()) == true)
        {
            uint8_t temp = (uint8_t)jValue.GetBool();
            ar.serializeU8(temp);
        }
        else if (typeId == asTYPEID_INT8 && (typeMatches = jValue.IsInt()) == true)
        {
            int8_t temp = (int8_t)jValue.GetInt();
            ar.serializeI8(temp);
        }
        else if (typeId == asTYPEID_INT16 && (typeMatches = jValue.IsInt()) == true)
        {
            int16_t temp = (int16_t)jValue.GetInt();
            ar.serializeI16(temp);
        }
        else if (typeId == asTYPEID_INT32 && (typeMatches = jValue.IsInt()) == true)
        {
            int32_t temp = jValue.GetInt();
            ar.serializeI32(temp);
        }
        else if (typeId == asTYPEID_INT64 && (typeMatches = jValue.IsInt64()) == true)
        {
            int64_t temp = jValue.GetInt64();
            ar.serializeI64(temp);
        }
        else if (typeId == asTYPEID_UINT8 && (typeMatches = jValue.IsUint()) == true)
        {
            uint8_t temp = (uint8_t)jValue.GetUint();
            ar.serializeU8(temp);
        }
        else if (typeId == asTYPEID_UINT16 && (typeMatches = jValue.IsUint()) == true)
        {
            uint16_t temp = (uint16_t)jValue.GetUint();
            ar.serializeU16(temp);
        }
        else if (typeId == asTYPEID_UINT32 && (typeMatches = jValue.IsUint()) == true)
        {
            uint32_t temp = jValue.GetUint();
            ar.serializeU32(temp);
        }
        else if (typeId == asTYPEID_UINT64 && (typeMatches = jValue.IsUint64()) == true)
        {
            uint64_t temp = jValue.GetUint64();
            ar.serializeU64(temp);
        }
        else if (typeId == asTYPEID_FLOAT && (typeMatches = jValue.IsNumber()) == true)
        {
            float temp = (float)jValue.GetDouble();
            ar.serializeF32(temp);
        }
        else if (typeId == asTYPEID_DOUBLE && (typeMatches = jValue.IsNumber()) == true)
        {
            double temp = jValue.GetDouble();
            ar.serializeF64(temp);
        }
        else if (typeId == assetRefTypeId && (typeMatches = checkValueTypeDecl(jValue, "prefab:", &valueStr)) == true)
        {
            AssetRef prefabRef = { XXH32(valueStr, strlen(valueStr), compData.hashSeed) };
            ar.serializeCustom(prefabRef);

            //Cook if needed
            if (compData.usedPrefabs.find(prefabRef) == compData.usedPrefabs.end())
            {
                compData.usedPrefabs.insert(prefabRef);
                cookPrefab(compData, valueStr);
            }
        }
        else if (typeId == assetRefTypeId && (typeMatches = checkValueTypeDecl(jValue, "texture:", &valueStr)) == true)
        {
            AssetRef texRef = { XXH32(valueStr, strlen(valueStr), compData.hashSeed) };
            ar.serializeCustom(texRef);

            //Add to list of used textures
            compData.usedTextures.emplace(texRef);
        }
        else if (typeId == assetRefTypeId && (typeMatches = checkValueTypeDecl(jValue, "sound:", &valueStr)) == true)
        {
            AssetRef soundRef = { XXH32(valueStr, strlen(valueStr), compData.hashSeed) };
            ar.serializeCustom(soundRef);

            //Add to list of used sounds
            compData.usedSounds.emplace(soundRef);
        }
        else if (typeId == assetRefTypeId && (typeMatches = checkValueTypeDecl(jValue, "music:", &valueStr)) == true)
        {
            AssetRef soundRef = { XXH32(valueStr, strlen(valueStr), compData.hashSeed) };
            ar.serializeCustom(soundRef);
        }
        else if (jValue.IsArray())
        {
            asITypeInfo* arrayTypeInfo = compData.angelState->getScriptEngine()->GetTypeInfoById(typeId);
            typeMatches = (strcmp(arrayTypeInfo->GetName(), "array") == 0);

            if (typeMatches)
            {
                size_t arraySizeOffset = ar.getOffset();
                uint32_t arraySize = jValue.Size();
                ar.serializeU32(arraySize);

                //Get type id of type stored in array (T for array<T>)
                NW_ASSERT(arrayTypeInfo->GetSubTypeCount() == 1);
                int subTypeId = arrayTypeInfo->GetSubTypeId(0);

                arraySize = 0;  //Reset to 0 and count up on success
                for (uint32_t i = 0; i < jValue.Size(); i++)
                {
                    const Value& jElementValue = jValue[i];
                    bool success = serializeValue(compData, ar, varName, subTypeId, jElementValue);
                    if (success)
                    {
                        arraySize++;
                    }
                }

                //Correct the array length in case we failed to serialize a value
                ar.offsetSerializeU32(arraySizeOffset, arraySize);
            }
        }
        else if (jValue.IsObject() && (typeMatches = checkObjectIsValueType(compData.angelState, typeId)) == true)
        {
            size_t propCountOffset = ar.getOffset();
            uint32_t propCount = jValue.MemberCount();
            ar.serializeU32(propCount);

            propCount = 0;  //Reset to 0 and count up on success
            for (auto it = jValue.MemberBegin(); it != jValue.MemberEnd(); ++it)
            {
                const asITypeInfo* typeInfo = compData.angelState->getScriptEngine()->GetTypeInfoById(typeId);

                const char* propName = nullptr;
                int propTypeId = 0;

                //Find the matching property
                for (uint32_t i = 0; i < typeInfo->GetPropertyCount(); i++)
                {
                    typeInfo->GetProperty(i, &propName, &propTypeId);

                    if (strcmp(propName, it->name.GetString()) == 0)
                    {
                        bool success = serializeVariable(compData, ar, propTypeId, it);
                        if (success)
                        {
                            propCount++;
                        }

                        break;
                    }
                }
            }

            //Correct the array length in case we failed to serialize a value
            ar.offsetSerializeU32(propCountOffset, propCount);
        }

        if (!typeMatches)
        {
            printf("Warning: For variable \"%s\", JSON type does not match AngelScript type.\n", varName);
            return false;
        }

        return true;
    }

    bool serializeVariable(
        SceneCompilationData& compData,
        util::EndianVectorWriteArchive& ar,
        int typeId,
        Value::ConstMemberIterator iter)
    {
        const Value& value = iter->value;
        uint32_t propNameHash = XXH32(iter->name.GetString(), iter->name.GetStringLength(), 0);
        script::AngelType propAngelType = compData.angelState->getAngelTypeFromTypeId(typeId);

        //Serialize common values
        ar.serializeU32(propNameHash);
        ar.serializeCustom(propAngelType);

        return serializeValue(compData, ar, iter->name.GetString(), typeId, value);
    }

    void addScriptComponent(SceneCompilationData& compData, Entity e, const Value& jScript)
    {
        ScriptSystem& scriptSystem = compData.scene->getScriptSystem();

        const char* typeName = nullptr;
        if (jScript.HasMember("type") && jScript["type"].IsString())
        {
            typeName = jScript["type"].GetString();
        }
        else
        {
            NW_ASSERT(false);
            printf("Failed to add script component. No type provided.\n");
            return;
        }

        script::AngelType aType(typeName);
        asITypeInfo* componentType = compData.angelState->getTypeInfoFromAngelType(aType);

        EInstance ei;
        if (scriptSystem.exists(e))
        {
            ei = scriptSystem.getInstance(e);
            NW_ASSERT(scriptSystem.getAngelType(ei) == aType);
        }
        else
        {
            ei = scriptSystem.create(e, aType);
        }

        if (!ei.isValid())
        {
            NW_ASSERT(false);
            printf("Failed to create script object of type %s. Verify that it has a default factory function.", typeName);
            return;
        }

        //Handle variable overrides
        if (jScript.HasMember("variables"))
        {
            EndianVectorWriteArchive ar;
            const Value& jVariables = jScript["variables"];
            for (auto it = jVariables.MemberBegin(); it != jVariables.MemberEnd(); ++it)
            {
                const char* propName = it->name.GetString();

                int propTypeId = 0;
                bool varFound = false;
                uint32_t varIdx = 0;
                for (; varIdx < componentType->GetPropertyCount(); varIdx++)
                {
                    const char* tempName;
                    componentType->GetProperty(varIdx, &tempName, &propTypeId);

                    if (strcmp(propName, tempName) == 0)
                    {
                        varFound = true;
                        break;
                    }
                }

                if (!varFound)
                {
                    printf("Warning: Variable %s not found in type %s.\n", propName, typeName);
                    continue;
                }

                serializeVariable(compData, ar, propTypeId, it);
            }

            //Add overrides to object
            scriptSystem.addVariableOverride(ei, ar.data(), (uint32_t)ar.size());
        }
    }

    eastl::vector<uint8_t> readTileMapCollision(const std::string& assetFolder, const char* textureName)
    {
        std::string path = assetFolder + "/" + textureName + ".coll";
        std::string json = file::readAllText(path.c_str());
        Document jRoot;
        jRoot.Parse(json.c_str());
        Value& jColls = jRoot["collision"];

        eastl::vector<uint8_t> coll;
        coll.push_back(0);   //Makes it 1-indexed (and makes no tile have no collision)
        for (auto jColl = jColls.Begin(); jColl < jColls.End(); jColl++)
        {
            coll.push_back((uint8_t)jColl->GetInt());
        }
        return coll;
    }

    void cookTileSystem(SceneCompilationData& compData, const std::string& assetFolder, const Value& jTileSystem)
    {
        TileSystem& tileSystem = compData.scene->getTileSystem();

        const char* textureName = jTileSystem["tileMap"].GetString();
        AssetRef textureRef = { XXH32(textureName, strlen(textureName), compData.hashSeed) };
        tileSystem.setTileMap(textureRef);
        compData.usedTextures.emplace(textureRef);

        //Load the collision data for the tilemap
        eastl::vector<uint8_t> coll = readTileMapCollision(assetFolder, textureName);

        uint32_t width = jTileSystem["width"].GetInt();
        uint32_t height = jTileSystem["height"].GetInt();
        tileSystem.setSize(width, height);

        uint32_t x = 0;
        const Value& jFgTiles = jTileSystem["foreground"];
        for (auto jTile = jFgTiles.Begin(); x < width * height && jTile < jFgTiles.End(); jTile++)
        {
            uint16_t tile = jTile->GetInt();
            tileSystem.setForegroundTile(x, 0, tile);
            tileSystem.setCollision(x, 0, coll[tile]);
            x++;
        }

        x = 0;
        const Value& jBgTiles = jTileSystem["background"];
        for (auto jTile = jBgTiles.Begin(); x < width * height && jTile < jBgTiles.End(); jTile++)
        {
            uint16_t tile = jTile->GetInt();
            tileSystem.setBackgroundTile(x, 0, tile);
            x++;
        }
    }

    void cookPrefab(SceneCompilationData& compData, const char* prefabName)
    {
        Scene& scene = *compData.scene;

        //Read the prefab from file
        AssetRef prefabRef = { XXH32(prefabName, strlen(prefabName), compData.hashSeed) };
        std::string prefabFile = *compData.assetFolder + "/" + prefabName;
        std::string prefabJson = file::readAllText(prefabFile.c_str());
        Document jPrefab;
        jPrefab.Parse(prefabJson.c_str());

        if (!jPrefab.IsObject())
        {
            printf("Failed to read prefab: %s\n", prefabName);
            NW_ASSERT(false);
        }

        //Using a fixed size buffer for the prefabs; increase if needed
        uint8_t buffer[4096];
        uint8_t components = 0 |
            (jPrefab.HasMember("tags") << 0) |
            (jPrefab.HasMember("transform") << 1) |
            (jPrefab.HasMember("sprite") << 2) |
            (jPrefab.HasMember("movement") << 3) |
            (jPrefab.HasMember("script") << 4) |
            (jPrefab.HasMember("text") << 5);
        buffer[0] = components;

        uint8_t* bufferPtr = buffer + 1;

        //We use temporary entities to create temporary components
        //This lets us keep the prefab logic inside the system
        Entity tempEn(compData.tempEntityIndex, 0);
        compData.tempEntityIndex--;

        if (jPrefab.HasMember("tags"))
        {
            addTags(compData, tempEn, jPrefab["tags"]);
            bufferPtr = scene.getTagSystem().convertToPrefab(tempEn, bufferPtr);
            scene.getTagSystem().destroy(tempEn);
        }

        /*if (jPrefab.HasMember("transform"))
        {
            auto& jTransform = jPrefab["transform"];
        }*/

        if (jPrefab.HasMember("sprite"))
        {
            addSpriteComponent(compData, tempEn, jPrefab["sprite"]);
            bufferPtr = scene.getSpriteSystem().convertToPrefab(tempEn, bufferPtr);
            scene.getSpriteSystem().destroy(tempEn);
        }

        if (jPrefab.HasMember("movement"))
        {
            addMovementComponent(compData, tempEn, jPrefab["movement"]);
            bufferPtr = scene.getMovementSystem().convertToPrefab(tempEn, bufferPtr);
            scene.getMovementSystem().destroy(tempEn);
        }

        if (jPrefab.HasMember("script"))
        {
            addScriptComponent(compData, tempEn, jPrefab["script"]);
            bufferPtr = scene.getScriptSystem().convertToPrefab(tempEn, bufferPtr);
            scene.getScriptSystem().destroy(tempEn);
        }

        compData.tempEntityIndex++;
        scene.addPrefab(prefabRef, buffer, bufferPtr - buffer);
    }

    void cookScene(const AssetCookData& cdat, script::AngelState& angelState)
    {
        AssetFileWriter& writer = *cdat.writer;
        writer.setAssetType(AssetType::Scene);
        writer.setCompressed(true);

        Scene scene;
        scene.getTagSystem().init();
        scene.getScriptSystem().init(angelState);
        scene.getScriptSystem().setCooking(true);
        uint32_t entityCount;
        auto& entityManager = scene.getEntityManager();

        //Stores commonly used data together
        SceneCompilationData compData;
        compData.scene = &scene;
        compData.hashSeed = cdat.hashSeed;
        compData.tempEntityIndex = ENTITY_INDEX_MASK;
        compData.assetFolder = &cdat.assetFolder;
        compData.angelState = &angelState;

        //Parse json
        std::string json = file::readAllText(cdat.inFile.c_str());
        Document jRoot;
        jRoot.Parse(json.c_str());

        //Cook entities
        {
            Value& jEntities = jRoot["entities"];
            entityCount = jEntities.Size();

            //Before we cook any entities, we need to create a hashmap that maps UUIDs to Entities
            eastl::hash_map<Uuid, Entity> uuidMap;
            for (auto jEntity = jEntities.Begin(); jEntity < jEntities.End(); jEntity++)
            {
                Entity e = entityManager.create();
                const char* uuid = (*jEntity)["id"].GetString();
                uuidMap.insert(eastl::make_pair(Uuid(uuid), e));
            }

            //Second pass: Add components
            for (auto jEntity = jEntities.Begin(); jEntity < jEntities.End(); jEntity++)
            {
                const char* uuid = (*jEntity)["id"].GetString();
                Entity e = uuidMap[Uuid(uuid)];
                if (jEntity->HasMember("prefab"))
                {
                    const char* prefabName = (*jEntity)["prefab"].GetString();
                    AssetRef prefabRef = { XXH32(prefabName, strlen(prefabName), cdat.hashSeed) };

                    //Cook if needed
                    if (compData.usedPrefabs.find(prefabRef) == compData.usedPrefabs.end())
                    {
                        compData.usedPrefabs.insert(prefabRef);
                        cookPrefab(compData, prefabName);
                    }

                    scene.instantiate(e, scene.getPrefab(prefabRef));
                }

                if (jEntity->HasMember("tags")) { addTags(compData, e, (*jEntity)["tags"]); }
                if (jEntity->HasMember("transform")) { addTransformComponent(compData, e, (*jEntity)["transform"], uuidMap); }
                if (jEntity->HasMember("sprite")) { addSpriteComponent(compData, e, (*jEntity)["sprite"]); }
                if (jEntity->HasMember("movement")) { addMovementComponent(compData, e, (*jEntity)["movement"]); }
                if (jEntity->HasMember("script")) { addScriptComponent(compData, e, (*jEntity)["script"]); }
            }
        }

        //Cook additional systems
        cookTileSystem(compData, cdat.assetFolder, jRoot["tileSystem"]);



        //Write to file
        auto& ar = writer.ar;
        scene.save(ar, entityCount);

        //Write list of resources
        uint32_t texturesLen = (uint32_t)compData.usedTextures.size();
        uint32_t soundsLen = (uint32_t)compData.usedSounds.size();
        ar.serializeU32(texturesLen);
        ar.serializeU32(soundsLen);
        for (AssetRef texture : compData.usedTextures) { ar.serializeCustom(texture); }
        for (AssetRef sound : compData.usedSounds) { ar.serializeCustom(sound); }
    }



    void cookAngelScript(const AssetCookData& cdat, script::AngelState& angelState)
    {
        AssetFileWriter& writer = *cdat.writer;
        writer.setAssetType(AssetType::AngelScript);
        writer.setCompressed(true);

        const char* inputFile = cdat.inFile.c_str();
        std::string text = file::readAllText(inputFile);

        //Add script for compilation
        angelState.addScriptSection(cdat.inAssetPath.c_str(), text.c_str());

        //Write string sizes
        auto& ar = writer.ar;
        uint32_t nameLen = (uint32_t)strlen(inputFile);
        uint32_t textLen = (uint32_t)text.size();
        ar.serializeU32(nameLen);
        ar.serializeU32(textLen);

        //Write name and contents of script file
        AR_SERIALIZE_ARRAY_CHAR(ar, inputFile, nameLen);
        AR_SERIALIZE_ARRAY_CHAR(ar, text.data(), text.size());
    }
}
#endif
