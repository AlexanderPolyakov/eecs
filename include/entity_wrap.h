#pragma once

namespace eecs
{

// Just a simple wrapper for entity id, so we can write a simpler setters
struct EntityWrap
{
    Registry& reg;
    EntityId eid;

    EntityWrap(Registry& _reg, EntityId _eid) : reg(_reg), eid(_eid) {}

    template<typename Component>
    EntityWrap& set(ComponentId<Component> cid, Component value)
    {
        set_component(reg, eid, cid, value);
        return *this;
    }

    template<typename Component>
    EntityWrap& tag(ComponentId<Component> cid)
    {
        set_component(reg, eid, cid, Component{});
        return *this;
    }
};

inline EntityWrap wrap_entity(Registry& reg, EntityId eid) { return EntityWrap(reg, eid); }
inline EntityWrap create_entity_wrap(Registry& reg, const char* name = nullptr) { return EntityWrap(reg, create_entity(reg, name)); }

}

