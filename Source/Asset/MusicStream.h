#ifndef ASSET_MUSIC_STREAM_H
#define ASSET_MUSIC_STREAM_H

#include <SDL_mixer.h>
#include <stdint.h>
#include "PackFile.h"

namespace asset
{
    //Note to self:
    //
    //I really don't like the way this is implemented. I'm a little scared of
    //letting the MusicStream hold a reference to the PackFile. As of right
    //now, there are no other users of the PackFile in the middle of the game,
    //but it still seems a little hacky. The mutex should prevent any position
    //thrashing, but it might lock up for too long during a level load.
    struct MusicStream
    {
        PackFile* pack;
        Mix_Music* music;
        FileSpan span;
        SDL_RWops rw;
        uint32_t position;   //Relative to start

        MusicStream();
        void init(PackFile& packFile, AssetRef asset);
    };
}

#endif
