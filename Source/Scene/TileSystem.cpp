#include "Core/Core.h"
#include "TileSystem.h"
#include "Asset/PackFile.h"
#include "Asset/AssetManager.h"
#include "Render/RenderCommon.h"
#include "Render/Renderer2d.h"

using asset::AssetRef;

namespace scene
{
    const uint8_t FOREGROUND_DEPTH = 64;
    const uint8_t BACKGROUND_DEPTH = 192;

    TileSystem::TileSystem() :
        _fgTiles(nullptr),
        _bgTiles(nullptr),
        _collision(nullptr)
    {
    }

    TileSystem::~TileSystem()
    {
        free(_fgTiles);
        free(_bgTiles);
        free(_collision);
    }

    void TileSystem::prepare(asset::AssetManager& assetMan)
    {
        _tileMap = assetMan.getTexture(_tileMapAsset);
        for (size_t i = 0; i < 2; i++)
        {
            bgfx::VertexDecl vertexDecl;
            vertexDecl.begin();
            vertexDecl.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float);
            vertexDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            vertexDecl.end();

            _vertexBuffers[i] = bgfx::createDynamicVertexBuffer(25 * 15, vertexDecl, BGFX_BUFFER_ALLOW_RESIZE);
        }
    }

#ifdef NW_ASSET_COOK
    void TileSystem::setTileMap(AssetRef ref)
    {
        _tileMapAsset = ref;
    }

    void TileSystem::setSize(uint32_t width, uint32_t height)
    {
        _width = width;
        _height = height;
        _fgTiles = (uint16_t*)malloc(sizeof(uint16_t) * _width * _height);
        _bgTiles = (uint16_t*)malloc(sizeof(uint16_t) * _width * _height);
        _collision = (uint8_t*)malloc(sizeof(uint8_t) * _width * _height);
    }

    void TileSystem::setForegroundTile(uint32_t x, uint32_t y, uint16_t tile)
    {
        _fgTiles[y * _width + x] = tile;
    }

    void TileSystem::setBackgroundTile(uint32_t x, uint32_t y, uint16_t tile)
    {
        _bgTiles[y * _width + x] = tile;
    }

    void TileSystem::setCollision(uint32_t x, uint32_t y, uint8_t collision)
    {
        _collision[y * _width + x] = collision;
    }
#endif

    void TileSystem::render(Renderer2d& renderer, const IntRect& view)
    {
        SCOPED_CPU_EVENT(event)(PROF_COLOR_GRAPHICS, "TileSystem::render");
        renderLayer(renderer, _vertexBuffers[0], view, _fgTiles, FOREGROUND_DEPTH);
        renderLayer(renderer, _vertexBuffers[1], view, _bgTiles, BACKGROUND_DEPTH);
    }

    void TileSystem::renderLayer(Renderer2d& renderer, bgfx::DynamicVertexBufferHandle vertexBuffer, const IntRect& view, uint16_t* layer, uint8_t depth)
    {
        SCOPED_CPU_EVENT(event)(PROF_COLOR_GRAPHICS, "TileSystem::renderLayer");

        const bgfx::TextureInfo& info = getTextureInfo(_tileMap);
        uint32_t tileMapWidth = info.width / TILE_SIZE;
        uint32_t x1 = max(view.left / TILE_SIZE, 0);
        uint32_t x2 = min((view.left + view.width) / TILE_SIZE + 1, _width);
        uint32_t y1 = max(view.top / TILE_SIZE, 0);
        uint32_t y2 = min((view.top + view.height) / TILE_SIZE + 1, _height);

#if 0
        for (uint32_t y = y1; y < y2; y++)
        {
            for (uint32_t x = x1; x < x2; x++)
            {
                uint16_t tile = layer[x + y * _width];
                if (tile > 0)
                {
                    tile--;
                    renderer.submitSprite(
                        Vector2i(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2),
                        Vector2i(TILE_SIZE, TILE_SIZE), depth, 255, _tileMap,
                        Vector2i((tile % tileMapWidth) * TILE_SIZE, (tile / tileMapWidth) * TILE_SIZE),
                        Vector2i(1, 1), 0);
                }
            }
        }
#else
        SpriteBatcher batcher;
        batcher.reserveVertices((x2 - x1 + 1) * (y2 - y1 + 1));
        batcher.setDepth(depth);
        batcher.setTexture(_tileMap);

        for (uint32_t y = y1; y < y2; y++)
        {
            for (uint32_t x = x1; x < x2; x++)
            {
                uint16_t tile = layer[x + y * _width];
                if (tile > 0)
                {
                    tile--;
                    batcher.submitSprite(
                        Vector2i(x * TILE_SIZE, y * TILE_SIZE),
                        Vector2i(TILE_SIZE, TILE_SIZE),
                        Vector2i((tile % tileMapWidth) * TILE_SIZE, (tile / tileMapWidth) * TILE_SIZE));
                }
            }
        }

        renderer.submitSpriteBatch(batcher, vertexBuffer);
#endif
    }

    Vector2i TileSystem::getSize() const
    {
        return Vector2i(_width, _height);
    }

    TileCollision TileSystem::getCollision(uint32_t x, uint32_t y) const
    {
        if (x < 0 || x >= _width || y < 0 || y >= _height) { return TILE_COLL_NONE; }
        return (TileCollision)_collision[y * _width + x];
    }

    bool TileSystem::isFree(IntRect rect) const
    {
        //Check tiles
        int x1 = rect.left / TILE_SIZE;
        int y1 = rect.top / TILE_SIZE;
        int x2 = (rect.left + rect.width - 1) / TILE_SIZE;
        int y2 = (rect.top + rect.height - 1) / TILE_SIZE;

        for (int y = y1; y <= y2; y++)
        {
            for (int x = x1; x <= x2; x++)
            {
                if (intersects(x, y, rect))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool TileSystem::intersects(int tileX, int tileY, IntRect other) const
    {
        IntRect self(tileX * TILE_SIZE, tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE);

        if (!other.intersects(self))
        {
            //Can't intersect the triangle if it doesn't intersect the bounding box
            return false;
        }

        switch (getCollision(tileX, tileY))
        {
        case TILE_COLL_NONE:
            return false;

        case TILE_COLL_SOLID:
            {
                bool collisionLeft = false;
                if (getCollision(tileX - 1, tileY) == TILE_COLL_RAMPRH)
                {
                    if (pointBelowLine(
                        Vector2i(other.left + other.width / 2, other.top + other.height),
                        Vector2i(self.left, self.top), -0.5f))
                    {
                        collisionLeft = true;
                    }
                }
                else
                {
                    collisionLeft = true;
                }

                bool collisionRight = false;
                if (getCollision(tileX + 1, tileY) == TILE_COLL_RAMPLH)
                {
                    if (pointBelowLine(
                        Vector2i(other.left + other.width / 2, other.top + other.height),
                        Vector2i(self.left + self.width, self.top), 0.5f))
                    {
                        collisionRight = true;
                    }
                }
                else
                {
                    collisionRight = true;
                }

                if (collisionLeft && collisionRight)
                {
                    return true;
                }
            }
            break;

        case TILE_COLL_RAMPLL:
            //Check if lower left hand corner passes the slope
            if (pointBelowLine(
                Vector2i(other.left + other.width / 2, other.top + other.height),
                Vector2i(self.left + self.width, self.top + self.height), 0.5f))
            {
                return true;
            }
            break;

        case TILE_COLL_RAMPLH:
            //Check if lower left hand corner passes the slope
            if (pointBelowLine(
                Vector2i(other.left + other.width / 2, other.top + other.height),
                Vector2i(self.left, self.top), 0.5f))
            {
                return true;
            }
            break;

        case TILE_COLL_RAMPRL:
            //Check if lower right hand corner passes the slope
            if (pointBelowLine(
                Vector2i(other.left + other.width / 2, other.top + other.height),
                Vector2i(self.left, self.top + self.height), -0.5f))
            {
                return true;
            }
            break;

        case TILE_COLL_RAMPRH:
            //Check if lower right hand corner passes the slope
            if (pointBelowLine(
                Vector2i(other.left + other.width / 2, other.top + other.height),
                Vector2i(self.left + self.width, self.top), -0.5f))
            {
                return true;
            }
            break;

        default:
            return false;
        }
        return false;
    }

    bool TileSystem::pointBelowLine(Vector2i point, Vector2i linePoint, float slope) const
    {
        float pointMappedY = (float)point.y - (float)point.x * slope;
        float lineMappedY = (float)linePoint.y - (float)linePoint.x * slope;
        return pointMappedY > lineMappedY;  //Below means greater (y axis increases down)
    }
}
