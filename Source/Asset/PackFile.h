#ifndef ASSET_PACK_FILE_H
#define ASSET_PACK_FILE_H

#include <stdint.h>
#include <mutex>
#include <EASTL/hash_map.h>
#include "AssetType.h"
#include "AssetRef.h"

namespace asset
{
    class PackFileReader;

    struct FileSpan
    {
        uint32_t offset;
        uint32_t size;
        uint32_t compressedSize;
        AssetType assetType;

        FileSpan();
    };

    class PackFile
    {
    public:
        static const uint32_t MAGIC_NUMBER = 0x6b70632e;

    private:
        FILE* _file;
        uint32_t _hashSeed;
        eastl::hash_map<AssetRef, FileSpan> _fileSpans;
        std::mutex _mutex;
        static const int BLOCK_BYTES = 1024 * 8;

    public:
        PackFile();
        ~PackFile();
        bool load(const char* fileName);
        uint32_t getHashSeed();

        FileSpan getFileSpan(AssetRef ref);

        void lock();
        void unlock();
        void seek(uint32_t offset);
        void read(uint32_t size, void* buffer);
        void read(uint32_t offset, uint32_t size, void* buffer);
        void decompress(const FileSpan& span, void* buffer);

        typedef eastl::hashtable_iterator<eastl::pair<const AssetRef, FileSpan>, true, false> FileSpanIterator;
        FileSpanIterator fileSpanBegin() { return _fileSpans.cbegin(); }
        FileSpanIterator fileSpanEnd() { return _fileSpans.cend(); }
    };
}

#endif