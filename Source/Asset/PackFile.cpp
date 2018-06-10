#include "Core/Core.h"
#include "PackFile.h"
#include <lz4\lz4.h>

namespace asset
{
    FileSpan::FileSpan() : offset(0), size(0), compressedSize(0), assetType(AssetType::Unknown) { }

    PackFile::PackFile() : _file(nullptr)
    {
    }

    PackFile::~PackFile()
    {
        if (_file != nullptr)
        {
            fclose(_file);
        }
    }

    bool PackFile::load(const char* fileName)
    {
        _file = fopen(fileName, "rb");
        if (_file == nullptr) { return false; }

        //Check the magic number
        uint32_t magicNumber = 0;
        fread(&magicNumber, sizeof(uint32_t), 1, _file);
        if (magicNumber != PackFile::MAGIC_NUMBER)
        {
            return false;
        }

        //Read hashSeed and fileCount
        uint32_t fileCount;
        fread(&_hashSeed, sizeof(uint32_t), 1, _file);
        fread(&fileCount, sizeof(uint32_t), 1, _file);

        //Read the file headers
        eastl::pair<AssetRef, FileSpan> temp;
        for (uint32_t i = 0; i < fileCount; i++)
        {
            uint32_t hash;
            fread(&hash, sizeof(hash), 1, _file);
            fread(&temp.second, sizeof(FileSpan), 1, _file);

            temp.first = AssetRef(hash);

            _fileSpans.insert(temp);
        }

        return true;
    }

    uint32_t PackFile::getHashSeed()
    {
        return _hashSeed;
    }

    FileSpan PackFile::getFileSpan(AssetRef ref)
    {
        auto search = _fileSpans.find(ref);
        if (search == _fileSpans.end())
        {
            return FileSpan();
        }
        else
        {
            return search->second;
        }
    }

    void PackFile::lock()
    {
        _mutex.lock();
    }

    void PackFile::unlock()
    {
        _mutex.unlock();
    }

    void PackFile::seek(uint32_t offset)
    {
        NW_ASSERT(!_mutex.try_lock());
        fseek(_file, offset, SEEK_SET);
    }

    void PackFile::read(uint32_t size, void* buffer)
    {
        NW_ASSERT(!_mutex.try_lock());
        fread(buffer, 1, size, _file);
    }

    void PackFile::read(uint32_t offset, uint32_t size, void* buffer)
    {
        NW_ASSERT(!_mutex.try_lock());
        fseek(_file, offset, SEEK_SET);
        fread(buffer, 1, size, _file);
    }

    void PackFile::decompress(const FileSpan& span, void* buffer)
    {
        NW_ASSERT(!_mutex.try_lock());
        LZ4_streamDecode_t lz4StreamDecode_body;
        LZ4_streamDecode_t* lz4StreamDecode = &lz4StreamDecode_body;

        //Seek to the proper place in the file
        fseek(_file, span.offset, SEEK_SET);

        char decBuf[2][BLOCK_BYTES];
        int decBufIndex = 0;
        size_t totalBytesRead = 0;
        size_t totalUncompressed = 0;

        LZ4_setStreamDecode(lz4StreamDecode, NULL, 0);
        while (1)
        {
            char cmpBuf[LZ4_COMPRESSBOUND(BLOCK_BYTES)];
            int cmpBytes = 0;
            {
                const size_t readCount0 = fread(&cmpBytes, sizeof(cmpBytes), 1, _file);
                totalBytesRead += sizeof(cmpBytes);
                if (readCount0 != 1 || cmpBytes <= 0)
                {
                    break;
                }

                const size_t readCount1 = fread(cmpBuf, 1, (size_t)cmpBytes, _file);
                totalBytesRead += cmpBytes;
                if (readCount1 != (size_t)cmpBytes)
                {
                    break;
                }
            }

            char* const decPtr = decBuf[decBufIndex];
            const int decBytes = LZ4_decompress_safe_continue(lz4StreamDecode, cmpBuf, decPtr, cmpBytes, BLOCK_BYTES);
            totalUncompressed += decBytes;
            if (decBytes <= 0)
            {
                break;
            }
            memcpy(buffer, decPtr, (size_t)decBytes);
            buffer = (char*)buffer + decBytes;
            decBufIndex = (decBufIndex + 1) % 2;
        }

        NW_ASSERT(totalBytesRead == span.compressedSize);
        NW_ASSERT(totalUncompressed == span.size);
    }
}
