#include "Core/Core.h"
#include "AssetFileWriter.h"
#include "Math/Math.h"
#include <lz4/lz4.h>
#include <lz4/lz4hc.h>
#include <stdio.h>

#define IMPLEMENT_WRITE_FN(classname, fnname, type)\
    void classname::fnname(type data)\
    {\
        union { type value; char bytes[sizeof(data)]; } temp;\
        temp.value = data;\
        if (!_flipBytes) { for (int i = 0; i < sizeof(data); i++) { _data.push_back(temp.bytes[i]); } }\
        else             { for (int i = sizeof(data) - 1; i >= 0; i++) { _data.push_back(temp.bytes[i]); } }\
    }

namespace cook
{
    AssetFileWriter::AssetFileWriter() :
        _compressOnSave(false),
        _assetType((uint32_t)asset::AssetType::Scene)
    {
    }

    void AssetFileWriter::saveToFile(const char* outputName)
    {
        size_t totalCompressedSize = 0;
        //Don't write anything if there is nothing to write.
        if (ar.size() == 0)
        {
            return;
        }

        FILE* file = fopen(outputName, "wb");

        //Write header
        fwrite(&_assetType, sizeof(_assetType), 1, file);
        uint32_t uncompressedSize = (_compressOnSave)
            ? (uint32_t)ar.size()
            : 0;    //0 is used to identify uncompressed files
        fwrite(&uncompressedSize, sizeof(uncompressedSize), 1, file);

        if (_compressOnSave)
        {
            LZ4_stream_t lz4StreamBody;
            LZ4_stream_t* lz4Stream = &lz4StreamBody;

            const int BLOCK_BYTES = 1024 * 8;
            char inBuffer[2][BLOCK_BYTES];
            int inputBufferIndex = 0; //Current index
            LZ4_resetStream(lz4Stream);
            NW_ASSERT(lz4Stream); //If this is null don't continue


            int dataOffset = 0;

            //"Read" the data as if it were from file, breaking it up into the two pages of inBuffer.
            while (1)
            {
                char* const inputPtr = inBuffer[inputBufferIndex];

                int32_t dataSize = math::min((int32_t)ar.size() - dataOffset, BLOCK_BYTES);
                memcpy(inputPtr, &ar.data()[dataOffset], dataSize);

                char compressedBuffer[LZ4_COMPRESSBOUND(BLOCK_BYTES)];
                const int32_t cmpBytes = LZ4_compress_fast_continue(lz4Stream,
                    inputPtr, compressedBuffer, dataSize, sizeof(compressedBuffer), 1);

                inputBufferIndex = (inputBufferIndex + 1) % 2;

                fwrite(&cmpBytes, sizeof(cmpBytes), 1, file);
                fwrite(&compressedBuffer, 1, (size_t)cmpBytes, file);
                totalCompressedSize += cmpBytes;
                if ((size_t)(dataOffset + BLOCK_BYTES) > ar.size())
                {
                    break; //No more data to compress
                }

                dataOffset += BLOCK_BYTES;
            }

            //Write a 0 to mark that there are no more bytes left
            int32_t marker = 0;
            fwrite(&marker, sizeof(marker), 1, file);
        }
        else
        {
            fwrite(ar.data(), 1, ar.size(), file);
        }

        fclose(file);
    }

    void AssetFileWriter::setCompressed(bool compressed)
    {
        _compressOnSave = compressed;
    }

    void AssetFileWriter::setAssetType(asset::AssetType type)
    {
        _assetType = (uint32_t)type;
    }
}
