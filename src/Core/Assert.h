#pragma once
#include "Core/Log.h"
#include <cstdlib>

#ifdef _DEBUG
    #define CHAOS_ASSERT(x, msg) \
        if (!(x)) { \
            Log::Fatal(msg); \
            __debugbreak(); \
        }
#else
    #define CHAOS_ASSERT(x, msg)
#endif
