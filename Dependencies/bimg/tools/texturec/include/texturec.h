/*
 * Copyright 2011-2018 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bimg#license-bsd-2-clause
 */

#include <bx/uint32_t.h>

#include <bimg/encode.h>

#if BX_PLATFORM_WINDOWS
#   define TEXC_SYMBOL_EXPORT __declspec(dllexport)
#   define TEXC_SYMBOL_IMPORT __declspec(dllimport)
#else
#   define BGFX_SYMBOL_EXPORT __attribute__((visibility("default")))
#   define BGFX_SYMBOL_IMPORT
#endif // BX_PLATFORM_WINDOWS

namespace bx
{
    struct AllocatorI;
    class Error;
}

namespace texturec
{
    struct Options
    {
        Options()
            : maxSize(UINT32_MAX)
            , edge(0.0f)
            , format(bimg::TextureFormat::Count)
            , quality(bimg::Quality::Default)
            , mips(false)
            , normalMap(false)
            , equirect(false)
            , iqa(false)
            , pma(false)
            , sdf(false)
            , alphaTest(false)
        {
        }

        void dump();

        uint32_t maxSize;
        float edge;
        bimg::TextureFormat::Enum format;
        bimg::Quality::Enum quality;
        bool mips;
        bool normalMap;
        bool equirect;
        bool iqa;
        bool pma;
        bool sdf;
        bool alphaTest;
    };

    TEXC_SYMBOL_EXPORT bimg::ImageContainer* convert(bx::AllocatorI* _allocator, const void* _inputData, uint32_t _inputSize, const Options& _options, bx::Error* _err);
}
