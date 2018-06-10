#include "Core/Core.h"
#include "EInstance.h"

namespace scene
{
    EInstance::EInstance()
    {
        //Default to none
        index = UINT32_MAX;
    }

    EInstance::EInstance(uint32_t index)
    {
        this->index = index;
    }

    bool EInstance::isValid()
    {
        return index != UINT32_MAX;
    }

    bool EInstance::operator==(const EInstance& other) const
    {
        return index == other.index;
    }
}
