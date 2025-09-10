#pragma once
#include "fnv1.h"

namespace eecs
{

template<typename T>
struct ComponentId
{
    constexpr ComponentId(const char* str) : sourceStr(str), hash(fnv1StrHash(str)) {}

    const char* sourceStr;
    fnv1_hash_t hash;
};

template<typename ComponentType>
constexpr ComponentId<ComponentType> comp_id(const char* str) { return ComponentId<ComponentType>(str); };

#define COMPID(type, name) eecs::comp_id<type>(#name)

}

