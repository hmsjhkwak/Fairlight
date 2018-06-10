#include "Core/Core.h"
#include <cstring>
#include "RenderCommon.h"
#include "Renderer2d.h"
#include "Math/Vector2f.h"

namespace render
{
    SpriteVertex quadVerts[] = {
        { Vector2f(-1, -1), Vector2f(0, 0) },
        { Vector2f(-1, 1), Vector2f(0, 1) },
        { Vector2f(1, 1), Vector2f(1, 1) },
        { Vector2f(1, -1), Vector2f(1, 0) }
    };
    uint16_t quadIndices[] = { 0, 1, 2, 0, 2, 3 };


    struct CBSprite
    {
        Vector2f viewSize;
        Vector2f transformPos;
        float transformDepth;
        float transformRot;
        Vector2f transformScale;
        Vector2f texOffset;
        Vector2f texScale;
        float alpha;
    };


    Renderer2d::Renderer2d()
    {
    }

    void Renderer2d::init(bgfx::ProgramHandle program)
    {
        _spriteRenderState =
            BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
            BGFX_STATE_DEPTH_TEST_LEQUAL | BGFX_STATE_WRITE_Z;

        _spriteVertexDecl
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();

        _spriteVertices = bgfx::createVertexBuffer(
            bgfx::makeRef(quadVerts, sizeof(quadVerts[0]) * 4), _spriteVertexDecl);
        _spriteIndices = bgfx::createIndexBuffer(
            bgfx::makeRef(quadIndices, sizeof(quadIndices)));
        _spriteProgram = program;

        u_viewSize = bgfx::createUniform("u_viewSize", bgfx::UniformType::Vec4);
        u_transformPosScale = bgfx::createUniform("u_transformPosScale", bgfx::UniformType::Vec4);
        u_transformDepthRot = bgfx::createUniform("u_transformDepthRot", bgfx::UniformType::Vec4);
        u_texScaleOffset = bgfx::createUniform("u_texScaleOffset", bgfx::UniformType::Vec4);
        u_alpha = bgfx::createUniform("u_alpha", bgfx::UniformType::Vec4);

        s_spriteTex = bgfx::createUniform("s_spriteTex", bgfx::UniformType::Int1);
    }

    void Renderer2d::setView(IntRect view)
    {
        _view = view;
    }

    void Renderer2d::bindSpriteGeometry()
    {
        bgfx::setVertexBuffer(0, _spriteVertices);
        bgfx::setIndexBuffer(_spriteIndices);
    }

    void Renderer2d::submitSprite(Vector2i pos, Vector2i size, uint8_t depth, uint8_t alpha,
        bgfx::TextureHandle tex, Vector2i texOffset, Vector2i texFlip, float rotation)
    {
        SCOPED_CPU_EVENT(event)(PROF_COLOR_GRAPHICS, "Renderer2d::submitSprite");

        Vector2i halfSize = size / 2;
        const bgfx::TextureInfo& texInfo = getTextureInfo(tex);

        bgfx::setState(_spriteRenderState);

        bindSpriteGeometry();

        float viewSize[4] = { (float)_view.width, (float)_view.height, 0, 0 };
        float transformPosScale[4] = {
            (float)pos.x - (float)_view.left,
            (float)pos.y - (float)_view.top,
            (float)halfSize.x * texFlip.x,
            (float)halfSize.y * texFlip.y };
        float transformDepthRot[4] = { (float)depth / 255.0f, rotation * DEG2RAD, 0, 0};
        float texScaleOffset[4] = {
            (float)size.x / (float)texInfo.width,
            (float)size.y / (float)texInfo.height,
            (float)texOffset.x / (float)texInfo.width,
            (float)texOffset.y / (float)texInfo.height };
        float alphaArr[4] = { (float)alpha / 255.0f, 0, 0, 0 };

        bgfx::setUniform(u_viewSize, viewSize);
        bgfx::setUniform(u_transformPosScale, transformPosScale);
        bgfx::setUniform(u_transformDepthRot, transformDepthRot);
        bgfx::setUniform(u_texScaleOffset, texScaleOffset);
        bgfx::setUniform(u_alpha, alphaArr);

        bgfx::setTexture(0, s_spriteTex, tex);

        bgfx::submit(VIEW_ID_SCENE, _spriteProgram);
    }

    void Renderer2d::submitSpriteBatch(SpriteBatcher& batcher, bgfx::DynamicVertexBufferHandle vertexBuffer)
    {
        SCOPED_CPU_EVENT(event)(PROF_COLOR_GRAPHICS, "Renderer2d::submitSpriteBatch");

        //Update geometry
        bgfx::updateDynamicVertexBuffer(vertexBuffer, 0,
            bgfx::copy(batcher.getVertices(), (uint32_t)batcher.getVertexCount() * sizeof(SpriteVertex)));

        bgfx::setState(_spriteRenderState);
        bgfx::setVertexBuffer(0, vertexBuffer);

        float viewSize[4] = { (float)_view.width, (float)_view.height, 0, 0 };
        float transformPosScale[4] = { (float)-_view.left, (float)-_view.top, 1.0f, 1.0f };
        float transformDepthRot[4] = { (float)batcher.getDepth() / 255.0f, 0.0f, 0, 0};
        float texScaleOffset[4] = { 1.0f, 1.0f, 0.0f, 0.0f };
        float alphaArr[4] = { 1.0f, 0, 0, 0 };

        bgfx::setUniform(u_viewSize, viewSize);
        bgfx::setUniform(u_transformPosScale, transformPosScale);
        bgfx::setUniform(u_transformDepthRot, transformDepthRot);
        bgfx::setUniform(u_texScaleOffset, texScaleOffset);
        bgfx::setUniform(u_alpha, alphaArr);

        bgfx::TextureHandle tex = batcher.getTexture();
        bgfx::setTexture(0, s_spriteTex, tex);

        bgfx::submit(VIEW_ID_SCENE, _spriteProgram);
    }

    void SpriteBatcher::submitSprite(Vector2i pos, Vector2i isize, Vector2i texOffset)
    {
        Vector2f size = Vector2f((float)isize.x, (float)isize.y);
        const bgfx::TextureInfo& info = getTextureInfo(_texture);
        Vector2f texSize = Vector2f(size.x / (float)info.width, size.y / (float)info.height);

        SpriteVertex vertex;
        vertex.position = Vector2f((float)pos.x, (float)pos.y);
        vertex.texCoord = Vector2f((float)texOffset.x / info.width, (float)texOffset.y / info.height);
        _vertices.push_back(vertex);

        vertex.position.x += size.x;
        vertex.texCoord.x += texSize.x;
        _vertices.push_back(vertex);

        vertex.position.y += size.y;
        vertex.texCoord.y += texSize.y;
        _vertices.push_back(vertex);

        _vertices.push_back(vertex);

        vertex.position.x -= size.x;
        vertex.texCoord.x -= texSize.x;
        _vertices.push_back(vertex);

        vertex.position.y -= size.y;
        vertex.texCoord.y -= texSize.y;
        _vertices.push_back(vertex);
    }
}
