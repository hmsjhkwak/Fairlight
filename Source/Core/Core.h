#ifndef CORE__CORE_H
#define CORE__CORE_H

//The compiler specifies NW_CONFIG_[DEBUG|DEVELOP|RELEASE]
#if defined(NW_CONFIG_DEBUG)
    #define NW_DEBUG
    #define NW_DEVELOP
#elif defined(NW_CONFIG_DEVELOP)
    #define NW_DEVELOP
#elif defined(NW_CONFIG_RELEASE)
    #define NW_RELEASE
#endif



//Platform detection
#ifdef _WIN32
    #define PLATFORM_WINDOWS 1
#elif __linux__
    #define PLATFORM_LINUX 1
#else
    #error Unknown platform
#endif


//Standard library includes
#include <stdint.h>
#include <string.h>
#include <new>
#include <string>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>



// Core defines
#define NW_UNUSED(x) (void)(x)



//Platform specific includes
#if PLATFORM_WINDOWS
    #include "CoreWindows.h"
#elif PLATFORM_LINUX
    #include "CoreLinux.h"
#else
    #error Unknown platform
#endif


//Core library includes
#include "Features.h"
#include "CpuTiming.h"
#include "Hash.h"

#endif
