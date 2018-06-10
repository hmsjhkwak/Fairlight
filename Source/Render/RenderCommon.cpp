#include "Core/Core.h"
#include "RenderCommon.h"
#include <bx/allocator.h>
#include <bgfx/platform.h>
#include <bimg/decode.h>

namespace render
{
    bx::DefaultAllocator g_ImgAllocator;

    eastl::vector<bgfx::TextureInfo> g_TextureInfoTable;

    void initRendering(const bgfx::PlatformData& platformData)
    {
        bgfx::setPlatformData(platformData);
        bgfx::init(bgfx::RendererType::Direct3D11);

        auto* caps = bgfx::getCaps();
        g_TextureInfoTable.resize(caps->limits.maxTextures);
    }


    static void imageReleaseCb(void* _ptr, void* _userData)
    {
        NW_UNUSED(_ptr);
        bimg::ImageContainer* imageContainer = (bimg::ImageContainer*)_userData;
        bimg::imageFree(imageContainer);
    }

    bgfx::TextureHandle createTexture(const void* data, uint32_t size, uint32_t flags)
    {
        bimg::ImageContainer* imageContainer = bimg::imageParse(&g_ImgAllocator, data, size);

        const bgfx::Memory* mem = bgfx::makeRef(
            imageContainer->m_data,
            imageContainer->m_size,
            imageReleaseCb,
            imageContainer
        );

        bgfx::TextureInfo info;
        bgfx::TextureHandle handle = bgfx::createTexture2D(
            uint16_t(imageContainer->m_width),
            uint16_t(imageContainer->m_height),
            1 < imageContainer->m_numMips,
            imageContainer->m_numLayers,
            bgfx::TextureFormat::Enum(imageContainer->m_format),
            flags,
            mem
        );

        info.width = (uint16_t)imageContainer->m_width;
        info.height = (uint16_t)imageContainer->m_height;
        info.numMips = imageContainer->m_numMips;
        info.numLayers = imageContainer->m_numLayers;
        g_TextureInfoTable[handle.idx] = info;

        return handle;
    }

    bgfx::TextureInfo& getTextureInfo(bgfx::TextureHandle handle)
    {
        return g_TextureInfoTable[handle.idx];
    }

    void destroyTexture(bgfx::TextureHandle handle)
    {
        bgfx::destroy(handle);
        memset(&g_TextureInfoTable[handle.idx], 0, sizeof(g_TextureInfoTable[handle.idx]));
    }
}
