#ifndef SCENE_ENTITY_H
#define SCENE_ENTITY_H

#include <stdint.h>
#include <EASTL/functional.h>

namespace scene
{
    const uint32_t ENTITY_INDEX_BITS = 23;
    const uint32_t ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;

    //If this is increased beyond 8 bits, you need to update the EntityManager as well
    const uint32_t ENTITY_GEN_BITS = 8;
    const uint32_t ENTITY_GEN_MASK = (1 << ENTITY_GEN_BITS) - 1;

    struct Entity
    {
    private:
        union
        {
            struct
            {
                uint32_t _index : ENTITY_INDEX_BITS;
                uint32_t _gen : ENTITY_GEN_BITS;
                uint32_t _invalidBit : 1;
            };
            uint32_t _id;
        };

    public:
        Entity() : _id(UINT32_MAX) { }
        Entity(uint32_t id) : _id(id) { }
        Entity(uint32_t index, uint8_t gen)
        {
            NW_ASSERT(index <= ENTITY_INDEX_MASK);
            NW_ASSERT(gen <= ENTITY_GEN_MASK);
            _index = index;
            _gen = gen;
            _invalidBit = 0;
        }

        uint32_t id() const { return _id; }
        uint32_t index() const { return _index; }
        uint8_t gen() const { return _gen; }
        static_assert(ENTITY_GEN_BITS <= 8, "gen() is returning a uint8_t");

        bool isValid()
        {
            NW_ASSERT((bool)_invalidBit == (_id == UINT32_MAX));
            return !_invalidBit;
        }

        bool operator==(Entity other) const { return _id == other._id; }
        bool operator!=(Entity other) const { return _id != other._id; }

        template <typename Archive> void serialize(Archive& ar) { ar.serializeU32(_id); }
    };
}

template <>
struct eastl::hash<scene::Entity>
{
    size_t operator()(scene::Entity val) const
    {
        return static_cast<size_t>(val.id());
    }
};

//I miss derive(Hash)
#include <functional>
namespace std
{
    template <>
    struct hash<scene::Entity>
    {
        std::size_t operator()(const scene::Entity& k) const
        {
            return std::hash<uint32_t>()(k.id());
        }
    };
}

#endif
