#include "Core/Core.h"
#include "MusicStream.h"

namespace asset
{
    int64_t musicStreamSize(SDL_RWops* rw)
    {
        MusicStream* stream = (MusicStream*)rw->hidden.unknown.data1;
        return stream->span.size;
    }

    int64_t musicStreamSeek(SDL_RWops* rw, int64_t offset, int whence)
    {
        MusicStream* stream = (MusicStream*)rw->hidden.unknown.data1;

        switch (whence)
        {
        case RW_SEEK_SET:
            stream->position = (uint32_t)offset;
            break;
        case RW_SEEK_CUR:
            stream->position += (uint32_t)offset;
            break;
        case RW_SEEK_END:
            stream->position = stream->span.size + (uint32_t)offset;
            break;
        default:
            NW_ASSERT(0);
        }

        return stream->position;
    }

    size_t musicStreamRead(SDL_RWops* rw, void* buf, size_t size, size_t maxNum)
    {
        MusicStream* stream = (MusicStream*)rw->hidden.unknown.data1;
        stream->pack->lock();

        uint32_t start = stream->span.offset;
        uint32_t remainingNum = (stream->span.size - stream->position) / (uint32_t)size;
        if (maxNum > remainingNum) { maxNum = remainingNum; }

        //Handle the seeking here to prevent issues with the pack file being
        //used in other places (not that it should be...)
        uint32_t totalSize = (uint32_t)(size * maxNum);
        stream->pack->read(start + stream->position, totalSize, buf);
        stream->position += totalSize;

        stream->pack->unlock();

        return maxNum;
    }

    size_t musicStreamWrite(SDL_RWops* rw, const void* buf, size_t size, size_t maxNum)
    {
        NW_UNUSED(rw);
        NW_UNUSED(buf);
        NW_UNUSED(size);
        NW_UNUSED(maxNum);

        //Cannot write
        NW_REQUIRE(0);
        return (size_t)-1;
    }

    int musicStreamClose(SDL_RWops* rw)
    {
        NW_UNUSED(rw);

        //Pretty sure we don't do a damn thing
        return 0;
    }



    MusicStream::MusicStream()
        : pack(nullptr), music(nullptr)
    {
    }

    void MusicStream::init(PackFile& packFile, AssetRef asset)
    {
        if (music != nullptr)
        {
            Mix_FreeMusic(music);
        }

        pack = &packFile;
        span = pack->getFileSpan(asset);
        position = 0;

        rw.hidden.unknown.data1 = this;
        rw.type = SDL_RWOPS_UNKNOWN;
        rw.size = musicStreamSize;
        rw.seek = musicStreamSeek;
        rw.read = musicStreamRead;
        rw.write = musicStreamWrite;
        rw.close = musicStreamClose;

        music = Mix_LoadMUS_RW(&rw, SDL_FALSE);
    }
}
