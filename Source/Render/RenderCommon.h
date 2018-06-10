#ifndef RENDER__RENDER_COMMON_H
#define RENDER__RENDER_COMMON_H

#include <bgfx/bgfx.h>

namespace bgfx
{
    struct PlatformData;
}

const uint16_t VIEW_ID_SCENE = 0;
const uint16_t VIEW_ID_POST = 1;

namespace render
{
    void initRendering(const bgfx::PlatformData& platformData);

    bgfx::TextureHandle createTexture(const void* data, uint32_t size, uint32_t flags = 0);
    bgfx::TextureInfo& getTextureInfo(bgfx::TextureHandle handle);
    void destroyTexture(bgfx::TextureHandle handle);

    bgfx::ProgramHandle createProgram(bgfx::ShaderHandle vertexShader, bgfx::ShaderHandle pixelShader);
    void destroyProgram(bgfx::ProgramHandle program);
}

#endif
