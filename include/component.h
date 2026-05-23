#pragma once
#include "fnv1.h"

namespace eecs
{

struct HashedName
{
    constexpr HashedName(const char* str) : sourceStr(str), hash(fnv1StrHash(str)) {}

    const char* sourceStr;
    fnv1_hash_t hash;
};

template<typename T>
struct ComponentId
{
    constexpr ComponentId(const char* str)     : sourceStr(str),         hash(fnv1StrHash(str)) {}
    constexpr ComponentId(const HashedName& h) : sourceStr(h.sourceStr), hash(h.hash) {}

    const char* sourceStr;
    fnv1_hash_t hash;
};

template<typename ComponentType>
constexpr ComponentId<ComponentType> comp_id(const char* str) { return ComponentId<ComponentType>(str); };

#define COMPID(type, name) eecs::comp_id<type>(#name)

// Variadic stringify: COMPS(a, b, c) -> eecs::HashedName{"a"}, eecs::HashedName{"b"}, eecs::HashedName{"c"}
// Supports up to 8 names; extend by adding more EECS_FE_N rungs and slots in EECS_PICK.
#define EECS_HN(x)         eecs::HashedName{#x}
#define EECS_FE_1(x)       EECS_HN(x)
#define EECS_FE_2(x, ...)  EECS_HN(x), EECS_FE_1(__VA_ARGS__)
#define EECS_FE_3(x, ...)  EECS_HN(x), EECS_FE_2(__VA_ARGS__)
#define EECS_FE_4(x, ...)  EECS_HN(x), EECS_FE_3(__VA_ARGS__)
#define EECS_FE_5(x, ...)  EECS_HN(x), EECS_FE_4(__VA_ARGS__)
#define EECS_FE_6(x, ...)  EECS_HN(x), EECS_FE_5(__VA_ARGS__)
#define EECS_FE_7(x, ...)  EECS_HN(x), EECS_FE_6(__VA_ARGS__)
#define EECS_FE_8(x, ...)  EECS_HN(x), EECS_FE_7(__VA_ARGS__)
#define EECS_PICK(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define COMPS(...) \
    EECS_PICK(__VA_ARGS__, EECS_FE_8, EECS_FE_7, EECS_FE_6, EECS_FE_5, \
                           EECS_FE_4, EECS_FE_3, EECS_FE_2, EECS_FE_1)(__VA_ARGS__)

}

