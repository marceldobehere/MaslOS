#pragma once
#include <stdint.h>

namespace RND
{
    extern __uint128_t g_lehmer64_state;

    uint64_t lehmer64();

    double RandomDouble();

    uint64_t RandomInt();
};

