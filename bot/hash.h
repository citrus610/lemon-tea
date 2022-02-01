#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <bit>

#if defined(_MSC_VER)
#   define FORCE_INLINE __forceinline
#else
#   define FORCE_INLINE inline __attribute__((always_inline))
#endif

FORCE_INLINE static uint32_t getblock32(const uint32_t* p, int i)
{
    return p[i];
};

FORCE_INLINE static uint32_t fmix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
};

static uint32_t murmur_hash_3_x86_32(const uint8_t* data, int len, uint32_t seed)
{
    const int nblocks = len / 4;
    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);
    for (int i = -nblocks; i; ++i) {
        uint32_t k1 = getblock32(blocks, i);
        k1 *= c1;
        k1 = std::rotl(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = std::rotl(h1, 13); 
        h1 = h1 * 5 + 0xe6546b64;
    }
    const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);
    uint32_t k1 = 0;
    switch (len & 3)
    {
    case 3:
        k1 ^= tail[2] << 16;
    case 2:
        k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];
        k1 *= c1;
        k1 = std::rotl(k1, 15);
        k1 *= c2;
        h1 ^= k1;
    }
    h1 ^= len;
    h1 = fmix32(h1);
    return h1;
};