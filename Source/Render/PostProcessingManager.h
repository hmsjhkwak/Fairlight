#ifndef RENDER_POST_PROCESSING_MANAGER_H
#define RENDER_POST_PROCESSING_MANAGER_H

#include <bgfx/bgfx.h>

namespace render
{
    class RenderManager;

    class PostProcessingManager
    {
    private:
        uint32_t _backBufferWidth;
        uint32_t _backBufferHeight;

        bgfx::ProgramHandle _saturateProgram;
        bgfx::UniformHandle s_sceneTex;
        bgfx::UniformHandle u_saturationMultiplier;

        float _saturationMultiplier;

    public:
        void init(bgfx::ProgramHandle saturateProgram, uint32_t width, uint32_t height);
        void render(RenderManager& renderManager);

        void setSaturationMultiplier(float multiplier)
        {
            _saturationMultiplier = multiplier;
        }
    };
}

#endif
