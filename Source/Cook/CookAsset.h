#ifdef NW_ASSET_COOK

#ifndef COOK_COOK_ASSET_H
#define COOK_COOK_ASSET_H

#include "AssetFileWriter.h"

using namespace asset;
using namespace util;
namespace script { class AngelState; }

namespace cook
{
    struct AssetCookData;

    void cookTexture2D(const char* inputFile, AssetFileWriter& writer);
    void cookShader(const char* inputFile, AssetFileWriter& writer);
    void cookSound(const char* inputFile, AssetFileWriter& writer);
    void cookMusic(const char* inputFile, AssetFileWriter& writer);
    void cookScene(const AssetCookData& cdat, script::AngelState& angelState);
    void cookAngelScript(const AssetCookData& cdat, script::AngelState& angelState);
}

#endif

#endif
