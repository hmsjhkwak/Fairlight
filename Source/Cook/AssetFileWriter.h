#ifndef COOK_ASSET_FILE_WRITER_H
#define COOK_ASSET_FILE_WRITER_H

#include <stdint.h>
#include <EASTL/vector.h>
#include "../Asset/AssetType.h"
#include "../Util/Archives.h"

namespace cook
{
    class AssetFileWriter
    {
    private:
        static const int INIT_SIZE = 1024;

        bool _compressOnSave;
        uint32_t _assetType;

    public:
        util::EndianVectorWriteArchive ar;

        AssetFileWriter();
        void saveToFile(const char* outputName);

        void setCompressed(bool compressed);
        void setAssetType(asset::AssetType type);
    };
}

#endif
