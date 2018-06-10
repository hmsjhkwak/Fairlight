#include "Core/Core.h"

#ifdef NW_ASSET_COOK
#include <filesystem>
#include <EASTL/vector.h>
#include "Pack.h"
#include "CookImpl.h"
#include "Asset/PackFile.h"

using namespace asset;

namespace cook
{
    void packAssets(const char* cacheFolder)
    {
        fs::path cachePath(cacheFolder);

        FILE* file = fopen("Assets.cpk", "wb");

        //Get the hash seed from the file
        uint32_t seed = readSeedFile(getSeedFilePath(cachePath));
        uint32_t fileCount = 0;

        //Storage for file headers
        eastl::vector<uint32_t> fileHashes;
        eastl::vector<FileSpan> fileSpans;
        const int HEADER_SIZE = sizeof(FileSpan::compressedSize) + sizeof(FileSpan::assetType);

        //We only iterate through the directory once.
        //Later, we open the files based on their hashed names.
        fs::directory_iterator endIter;
        for (fs::directory_iterator iter(cachePath); iter != endIter; iter++)
        {
            if (fs::is_regular_file(iter->status()))
            {
                auto path = iter->path().string();
                auto hexString = iter->path().filename().string();

                FileSpan span;
                span.offset = 0;

                FILE* spanFile = fopen(path.c_str(), "rb");
                fread(&span.assetType, sizeof(span.assetType), 1, spanFile);
                fread(&span.size, sizeof(span.size), 1, spanFile);
                fclose(spanFile);

                if (span.size == 0)
                {
                    span.compressedSize = 0;
                    span.size = (uint32_t)fs::file_size(path) - HEADER_SIZE;
                }
                else
                {
                    span.compressedSize = (uint32_t)fs::file_size(path) - HEADER_SIZE;
                }

                //Store file header info
                fileHashes.push_back(std::stoul(hexString, nullptr, 16));
                fileSpans.push_back(span);
                fileCount++;
            }
        }

        //Write pack header
        fwrite(&PackFile::MAGIC_NUMBER, sizeof(PackFile::MAGIC_NUMBER), 1, file);
        fwrite(&seed, sizeof(seed), 1, file);
        fwrite(&fileCount, sizeof(fileCount), 1, file);

        //The files start after all the file headers
        //We need to move the offset to after all the headers
        uint32_t offset = sizeof(PackFile::MAGIC_NUMBER) + sizeof(seed) + sizeof(fileCount);
        offset += fileCount * (sizeof(uint32_t) + sizeof(FileSpan));

        //Write file headers
        for (size_t i = 0; i < fileHashes.size(); i++)
        {
            fileSpans[i].offset = offset;

            //Write hash and file span
            fwrite(&fileHashes[i], sizeof(fileHashes[i]), 1, file);
            fwrite(&fileSpans[i], sizeof(fileSpans[i]), 1, file);

            //The next file starts immediately after the one that this header corresponds to
            uint32_t packSize = (fileSpans[i].compressedSize == 0)
                ? fileSpans[i].size
                : fileSpans[i].compressedSize;
            offset += packSize;
        }

        const uint32_t BUFFER_SIZE = 4096;
        uint8_t buffer[BUFFER_SIZE];
        //Write each asset file to the pack file
        for (size_t i = 0; i < fileHashes.size(); i++)
        {
#if 0
            fflush(file);
            uint32_t curPos = ftell(file);
            NW_ASSERT(curPos == fileSpans[i].offset);
#endif

            //We use the hashed name to open the file
            //instead of reiterating through the directory
            auto path = cachePath / hashToPath(fileHashes[i]);
            FILE* assetFile = fopen(path.string().c_str(), "rb");

            //Skip the header
            fseek(assetFile, HEADER_SIZE, SEEK_SET);

            //Copy BUFFER_SIZE bytes at a time
            size_t readCount;
            while ((readCount = fread(buffer, 1, BUFFER_SIZE, assetFile)) > 0)
            {
                fwrite(buffer, 1, readCount, file);
            }

            fclose(assetFile);
        }

        fclose(file);
    }
}
#endif
