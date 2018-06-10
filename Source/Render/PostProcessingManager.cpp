#include "Core/Core.h"
#include "PostProcessingManager.h"
#include "RenderCommon.h"
#include "RenderManager.h"

namespace render
{
    void PostProcessingManager::init(bgfx::ProgramHandle saturateProgram, uint32_t width, uint32_t height)
    {
        _saturateProgram = saturateProgram;
        _backBufferWidth = width;
        _backBufferHeight = height;

        s_sceneTex = bgfx::createUniform("s_sceneTex", bgfx::UniformType::Int1, 1);
        u_saturationMultiplier = bgfx::createUniform("u_saturationMultiplier", bgfx::UniformType::Vec4, 1);
    }

    void PostProcessingManager::render(RenderManager& renderManager)
    {
        SCOPED_CPU_EVENT(event)(PROF_COLOR_GRAPHICS, "PostProcessingManager::render");
        renderManager.getRenderer2d().bindSpriteGeometry();

        bgfx::setViewFrameBuffer(VIEW_ID_POST, BGFX_INVALID_HANDLE);
        bgfx::setViewRect(VIEW_ID_POST, 0, 0, (uint16_t)_backBufferWidth, (uint16_t)_backBufferHeight);
        bgfx::setViewScissor(VIEW_ID_POST, 0, 0, (uint16_t)_backBufferWidth, (uint16_t)_backBufferHeight);

        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
        bgfx::setTexture(0, s_sceneTex, renderManager.globalRes.sceneColorTexture);
        bgfx::setUniform(u_saturationMultiplier, &_saturationMultiplier);

        bgfx::submit(VIEW_ID_POST, _saturateProgram);
    }
}
