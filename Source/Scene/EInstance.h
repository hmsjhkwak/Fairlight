#ifndef SCENE_EINSTANCE_H
#define SCENE_EINSTANCE_H

#include <stdint.h>

namespace scene
{
    struct EInstance
    {
        uint32_t index;

        EInstance();
        EInstance(uint32_t index);

        bool isValid();
        bool operator==(const EInstance& other) const;

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar.serializeU32(index);
        }
    };
}

#endif
