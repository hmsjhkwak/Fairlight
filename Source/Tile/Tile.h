#ifndef TILE_TILE_H
#define TILE_TILE_H

namespace tile
{
    const int TILE_SIZE = 32;
    enum TileCollision
    {
        TILE_COLL_NONE,
        TILE_COLL_SOLID,
        TILE_COLL_RAMPLL,   //Going left low
        TILE_COLL_RAMPLH,   //Going left high
        TILE_COLL_RAMPRL,   //Going right low
        TILE_COLL_RAMPRH,   //Going right high
    };
}

#endif
