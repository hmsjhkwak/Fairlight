#include "Core/Core.h"

#if PLATFORM_WINDOWS

void assertFailure(const char* expression)
{
    if (::IsDebuggerPresent())
    {
        OutputDebugStringA(expression);
    }
    else
    {
        printf("%s", expression);
    }

#if defined(NW_DEVELOP)
    __debugbreak();
#else
    int* crash = nullptr;
    *crash = 1;
#endif
}

#endif
