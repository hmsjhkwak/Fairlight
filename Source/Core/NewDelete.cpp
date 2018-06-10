#include "Core/Core.h"
#include <stddef.h>

void* operator new[](size_t size, const char* /*name*/, int /*flags*/,
                     unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
#if defined(_MSC_VER)
    return _aligned_offset_malloc(size, 1, 0);
#else
#error Not implemented
#endif
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* /*name*/,
                     int flags, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    NW_UNUSED(flags);
#if defined(_MSC_VER)
    return _aligned_offset_malloc(size, alignment, alignmentOffset);
#else
#error Not implemented
#endif
}

void* operator new(size_t size)
{
#if defined(_MSC_VER)
    return _aligned_offset_malloc(size, 1, 0);
#else
#error Not implemented
#endif
}

void* operator new[](size_t size)
{
#if defined(_MSC_VER)
    return _aligned_offset_malloc(size, 1, 0);
#else
#error Not implemented
#endif
}

void operator delete(void* p)
{
    if (p)
    {
#if defined(_MSC_VER)
        return _aligned_free(p);
#else
#error Not implemented
#endif
    }
}

void operator delete[](void* p)
{
    if (p)
    {
#if defined(_MSC_VER)
        return _aligned_free(p);
#else
#error Not implemented
#endif
    }
}



