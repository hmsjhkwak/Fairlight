#ifndef RENDER_RENDER_HANDLER_H
#define RENDER_RENDER_HANDLER_H

#include <bgfx/bgfx.h>
#include "Renderer2d.h"
#include "PostProcessingManager.h"

namespace render
{
    struct GlobalRenderResources
    {
        bool initialized;
        bgfx::TextureHandle sceneColorTexture;
        bgfx::TextureHandle sceneDepthTexture;
        bgfx::FrameBufferHandle sceneFrameBuffer;

        bgfx::TextureHandle postProcessingTexture;
        bgfx::FrameBufferHandle postFrameBuffer;

        GlobalRenderResources() : initialized(false) { }

        void create(uint32_t sceneWidth, uint32_t sceneHeight);
        void destroy();
    };

    class RenderManager
    {
    public:
        GlobalRenderResources globalRes;

    private:
        Renderer2d _renderer2d;
        PostProcessingManager _postProcessingManager;

    public:
        void init(uint32_t sceneWidth, uint32_t sceneHeight);
        void renderPostProcessing();

        Renderer2d& getRenderer2d() { return _renderer2d; }
        PostProcessingManager& getPostProcessingManager() { return _postProcessingManager; }
    };
}

#endif
