#pragma once

namespace eecs
{

struct InternalTag {};

constexpr eecs::ComponentId<eecs::InternalTag> kPrefabTag = COMPID(eecs::InternalTag, eecs_internal_prefab);

};

