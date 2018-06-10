#ifndef RENDER_RENDERER_2D_H
#define RENDER_RENDERER_2D_H

#include <bgfx/bgfx.h>
#include "../Math/Vector2i.h"
#include "../Math/Vector2f.h"
#include "../Math/IntRect.h"

using namespace math;

namespace render
{
    class SpriteBatcher;

    struct SpriteVertex
    {
        Vector2f position;
        Vector2f texCoord;
    };



    class Renderer2d
    {
    private:
        uint64_t _spriteRenderState;
        bgfx::VertexBufferHandle _spriteVertices;
        bgfx::IndexBufferHandle _spriteIndices;
        bgfx::VertexDecl _spriteVertexDecl;

        bgfx::ProgramHandle _spriteProgram;
        bgfx::UniformHandle u_viewSize;
        bgfx::UniformHandle u_transformPosScale;
        bgfx::UniformHandle u_transformDepthRot;
        bgfx::UniformHandle u_texScaleOffset;
        bgfx::UniformHandle u_alpha;
        bgfx::UniformHandle s_spriteTex;

        IntRect _view;

    public:
        Renderer2d();
        void init(bgfx::ProgramHandle program);

        IntRect getView() const { return _view; }
        void setView(IntRect view);

        void bindSpriteGeometry();

        void submitSprite(Vector2i pos, Vector2i size, uint8_t depth, uint8_t alpha,
            bgfx::TextureHandle tex, Vector2i texOffset, Vector2i texFlip, float rotation);
        void submitSpriteBatch(SpriteBatcher& batcher, bgfx::DynamicVertexBufferHandle vertexBuffer);
    };



    class SpriteBatcher
    {
    private:
        uint8_t _depth;
        bgfx::TextureHandle _texture;

        eastl::vector<SpriteVertex> _vertices;

    public:
        void submitSprite(Vector2i pos, Vector2i size, Vector2i texOffset);

        void reserveVertices(size_t size) { _vertices.reserve(size); }
        void reset() { _vertices.clear(); }

        void setDepth(uint8_t depth) { _depth = depth; }
        void setTexture(bgfx::TextureHandle texture) { _texture = texture; }

        uint8_t getDepth() { return _depth; }
        bgfx::TextureHandle getTexture() { return _texture; }
        size_t getVertexCount() { return _vertices.size(); }
        SpriteVertex* getVertices() { return _vertices.data(); }
    };
}

#endif
