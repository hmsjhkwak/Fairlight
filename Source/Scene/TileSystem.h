#ifndef SCENE_TILE_SYSTEM_H
#define SCENE_TILE_SYSTEM_H

#include <cstdio>
#include <bgfx/bgfx.h>
#include "../Core/Features.h"
#include "../Asset/AssetManager.h"
#include "../Tile/Tile.h"
#include "../Math/Vector2i.h"
#include "../Math/IntRect.h"

namespace render { class Renderer2d; }
using namespace math;
using namespace tile;
using namespace render;

namespace scene
{
    class TileSystem
    {
    private:
        uint16_t* _fgTiles; //Foreground (used for collision data)
        uint16_t* _bgTiles; //Background
        uint8_t* _collision;
        uint32_t _width;
        uint32_t _height;

        asset::AssetRef _tileMapAsset;
        bgfx::TextureHandle _tileMap;
        bgfx::DynamicVertexBufferHandle _vertexBuffers[2];

    public:
        TileSystem();
        ~TileSystem();

        template <typename Archive>
        void serialize(Archive& ar)
        {
            //Read texture
            ar.serializeCustom(_tileMapAsset);

            //Read size
            ar.serializeU32(_width);
            ar.serializeU32(_height);

            //Allocate space if needed and read tiles
            uint32_t size = _width * _height;
            if (ar.IsReading)
            {
                _fgTiles = (uint16_t*)malloc(sizeof(uint16_t) * size);
                _bgTiles = (uint16_t*)malloc(sizeof(uint16_t) * size);
                _collision = (uint8_t*)malloc(sizeof(uint8_t) * size);
            }
            AR_SERIALIZE_ARRAY_U16(ar, _fgTiles, size);
            AR_SERIALIZE_ARRAY_U16(ar, _bgTiles, size);
            AR_SERIALIZE_ARRAY_U8(ar, _collision, size);
        }

        void prepare(asset::AssetManager& assetMan);
#ifdef NW_ASSET_COOK
        void setTileMap(asset::AssetRef ref);
        void setSize(uint32_t width, uint32_t height);
        void setForegroundTile(uint32_t x, uint32_t y, uint16_t tile);
        void setBackgroundTile(uint32_t x, uint32_t y, uint16_t tile);
        void setCollision(uint32_t x, uint32_t y, uint8_t collision);
#endif

        void render(Renderer2d& renderer, const IntRect& view);

        Vector2i getSize() const;
        TileCollision getCollision(uint32_t x, uint32_t y) const;
        bool isFree(IntRect rect) const;
    private:
        void renderLayer(Renderer2d& renderer, bgfx::DynamicVertexBufferHandle vertexBuffer, const IntRect& view, uint16_t* layer, uint8_t depth);
        bool intersects(int tileX, int tileY, IntRect other) const;
        bool pointBelowLine(Vector2i point, Vector2i linePoint, float slope) const;
    };
}

#endif
