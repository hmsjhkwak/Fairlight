#ifndef CORE__CORE_WINDOWS_H
#define CORE__CORE_WINDOWS_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <assert.h>

#define NW_FORCEINLINE __forceinline


void assertFailure(const char*);

//Assertion macros
// NW_ASSERT  : Removed on non-debug builds.
#if defined(NW_DEBUG)
    #define NW_ASSERT(expression) \
        EA_DISABLE_VC_WARNING(4127) \
        do { \
            if (!(expression)) { assertFailure(#expression); } \
        } while (0) \
        EA_RESTORE_VC_WARNING()
#else
    #define NW_ASSERT(cond) do { } while(0)
#endif

// NW_VERIFY  : Assertion is removed on non-develop builds, but code inside
//              macro exists in all configs.
#if defined(NW_DEVELOP)
    #define NW_VERIFY(expression) \
        EA_DISABLE_VC_WARNING(4127) \
        do { \
            if (!(expression)) { assertFailure(#expression); } \
        } while (0) \
        EA_RESTORE_VC_WARNING()
#else
    #define NW_VERIFY(cond) (cond)
#endif

// NW_REQUIRE : Use for unrecoverable errors. Asserts on development builds.
//              Crashes on release builds.
#define NW_REQUIRE(expression) \
    EA_DISABLE_VC_WARNING(4127) \
    do { \
        if (!(expression)) { assertFailure(#expression); } \
    } while (0) \
    EA_RESTORE_VC_WARNING()



namespace nw
{
    class Mutex
    {
    private:
        CRITICAL_SECTION handle;

        Mutex(const Mutex&);
        Mutex& operator=(const Mutex&);

    public:
        NW_FORCEINLINE Mutex()
        {
            InitializeCriticalSection(&handle);
        }

        NW_FORCEINLINE ~Mutex()
        {
            DeleteCriticalSection(&handle);
        }

        NW_FORCEINLINE void lock()
        {
            EnterCriticalSection(&handle);
        }

        NW_FORCEINLINE bool tryLock()
        {
            return (TryEnterCriticalSection(&handle) != 0);
        }

        NW_FORCEINLINE void unlock()
        {
            LeaveCriticalSection(&handle);
        }
    };
}

#endif

