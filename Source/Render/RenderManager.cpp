#include "Core/Core.h"
#include "RenderManager.h"
#include <cstring>

namespace render
{
    void GlobalRenderResources::create(uint32_t sceneWidth, uint32_t sceneHeight)
    {
        NW_ASSERT(!initialized);

        sceneColorTexture = bgfx::createTexture2D(
            (uint16_t)sceneWidth, (uint16_t)sceneHeight,
            false,
            1,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT);
        sceneDepthTexture = bgfx::createTexture2D(
            (uint16_t)sceneWidth, (uint16_t)sceneHeight,
            false,
            1,
            bgfx::TextureFormat::D16,
            BGFX_TEXTURE_RT);

        bgfx::Attachment sceneAttachments[2];
        {
            sceneAttachments[0].handle = sceneColorTexture;
            sceneAttachments[0].layer = 0;
            sceneAttachments[0].mip = 0;

            sceneAttachments[1].handle = sceneDepthTexture;
            sceneAttachments[1].layer = 0;
            sceneAttachments[1].mip = 0;
        }
        sceneFrameBuffer = bgfx::createFrameBuffer(2, sceneAttachments);

        postProcessingTexture = bgfx::createTexture2D(
            (uint16_t)sceneWidth, (uint16_t)sceneHeight,
            false,
            1,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT);

        bgfx::Attachment postAttachments[1];
        {
            postAttachments[0].handle = postProcessingTexture;
            postAttachments[0].layer = 0;
            postAttachments[0].mip = 0;
        }
        postFrameBuffer = bgfx::createFrameBuffer(1, postAttachments);

        initialized = true;
    }

    void GlobalRenderResources::destroy()
    {
        NW_ASSERT(initialized);

        bgfx::destroy(sceneFrameBuffer);
        bgfx::destroy(sceneColorTexture);
        bgfx::destroy(sceneDepthTexture);

        bgfx::destroy(postFrameBuffer);
        bgfx::destroy(postProcessingTexture);

        initialized = false;
    }

    void RenderManager::init(uint32_t sceneWidth, uint32_t sceneHeight)
    {
        globalRes.create(sceneWidth, sceneHeight);
    }

    void RenderManager::renderPostProcessing()
    {
        _postProcessingManager.render(*this);
    }
}
