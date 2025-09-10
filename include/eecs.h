#pragma once

#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <assert.h> // TODO: replace with in-house assert?

#include "entity.h"
#include "sparse_set.h"
#include "component.h"
#include "prefab.h"
#include "functional.h"
#include "registry.h"
#include "entity_wrap.h" // This one is optional btw, we can probably let user decide if they want it (though it's very useful)

namespace eecs
{

#include "eecs.inl"

#define REG_COMP(reg, type, name) eecs::reg_component<type>(reg, eecs::comp_id<type>(#name))

};

