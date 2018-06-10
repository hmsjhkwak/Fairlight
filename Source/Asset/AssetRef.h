#ifndef ASSET_ASSET_REF_H
#define ASSET_ASSET_REF_H

#include <stdint.h>
#include "../Util/ScopeTimer.h"

#ifdef NW_DEVELOP
    #define USE_ASSET_REF_NAMES 1
#else
    #define USE_ASSET_REF_NAMES 0
#endif

namespace asset
{
#if USE_ASSET_REF_NAMES
    namespace AssetNameTable
    {
        void loadAssetNames(const char* filename);
        uint32_t getIndex(uint32_t hash);
        const char* getName(uint32_t index);
    }
#endif

    struct AssetRef
    {
        uint32_t hash;
#if USE_ASSET_REF_NAMES
        uint32_t nameIndex;
#endif

        AssetRef() { }
        AssetRef(uint32_t hash) : hash(hash)
        {
#if USE_ASSET_REF_NAMES
            nameIndex = AssetNameTable::getIndex(hash);
#endif
        }

        template <typename Archive> void serialize(Archive& ar)
        {
            ar.serializeU32(hash);
#if USE_ASSET_REF_NAMES
            if (ar.IsReading)
            {
                nameIndex = AssetNameTable::getIndex(hash);
            }
#endif
        }
        bool operator==(const AssetRef& other) const { return hash == other.hash; }
        bool operator!=(const AssetRef& other) const { return hash != other.hash; }
    };
}

#include <EASTL/functional.h>
template <>
struct eastl::hash<asset::AssetRef>
{
    size_t operator()(asset::AssetRef val) const
    {
        return static_cast<size_t>(val.hash);
    }
};

#endif
