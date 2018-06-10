#ifndef ASSET_ASSET_TYPE_H
#define ASSET_ASSET_TYPE_H

namespace asset
{
    enum class AssetType
    {
        Unknown,

        //Graphics
        Texture,
        Shader,
        Font,

        //Sound
        Sound,
        Music,

        //Game
        AngelScript,
        Scene,
    };
}

#endif
