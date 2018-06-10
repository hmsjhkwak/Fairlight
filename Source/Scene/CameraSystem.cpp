#include "Core/Core.h"
#include "CameraSystem.h"
#include "TileSystem.h"

namespace scene
{
    CameraSystem::CameraSystem() :
        _mapWidth(20), _mapHeight(12),
        _view(0, 0, 640, 360)
    {
    }

    void CameraSystem::prepare(TileSystem& tileSystem)
    {
        Vector2i size = tileSystem.getSize();
        _mapWidth = size.x;
        _mapHeight = size.y;
    }

    IntRect CameraSystem::getView() { return _view; }

    Vector2i CameraSystem::getLeftTop() const
    {
        return Vector2i(_view.left, _view.top);
    }

    Vector2i CameraSystem::getCenter() const
    {
        return Vector2i(
            _view.left + _view.width / 2,
            _view.top + _view.height / 2);
    }

    void CameraSystem::setLeftTop(Vector2i pos)
    {
        _view.left = max(min(pos.x, _mapWidth * TILE_SIZE - _view.width), 0);
        _view.top = max(min(pos.y, _mapHeight * TILE_SIZE - _view.height), 0);
    }

    void CameraSystem::setCenter(Vector2i pos)
    {
        setLeftTop(pos - Vector2i(_view.width / 2, _view.height / 2));
    }
}
