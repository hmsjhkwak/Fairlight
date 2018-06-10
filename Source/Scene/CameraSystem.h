#ifndef SCENE_CAMERA_SYSTEM_H
#define SCENE_CAMERA_SYSTEM_H

#include <cstdio>
#include "../Core/Features.h"
#include "../Math/Vector2i.h"
#include "../Math/IntRect.h"

using namespace math;

namespace scene
{
    class TileSystem;

    class CameraSystem
    {
    private:
        uint32_t _mapWidth;
        uint32_t _mapHeight;

        IntRect _view;

    public:
        CameraSystem();

        void prepare(TileSystem& tileSystem);

        IntRect getView();
        Vector2i getLeftTop() const;
        Vector2i getCenter() const;

        void setLeftTop(Vector2i pos);
        void setCenter(Vector2i pos);
    };
}

#endif
