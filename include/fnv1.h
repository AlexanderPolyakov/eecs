#pragma once

using fnv1_hash_t = uint32_t;
constexpr fnv1_hash_t fnv1OffsetBasis = 2166136261U;
constexpr fnv1_hash_t fnv1Prime = 16777619;

constexpr fnv1_hash_t fnv1StrHash(const char* str)
{
    fnv1_hash_t result = fnv1OffsetBasis;
    fnv1_hash_t ch = 0;
    while ((ch = *str++) != 0)
        result = (result * fnv1Prime) ^ ch;
    return result;
}

