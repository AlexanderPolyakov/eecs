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

    EntityWrap& toprefab()
    {
        make_prefab(reg, eid);
        return *this;
    }

    template<typename Component>
    bool has(ComponentId<Component> cid)
    {
        return has_comp(reg, eid, cid);
    }

    template<typename Component>
    Component get_or(ComponentId<Component> cid, Component def)
    {
        return get_comp_or(reg, eid, cid, def);
    }

    template<typename Component, typename Callable>
    void query_comp(Callable c, ComponentId<Component> cid)
    {
        query_component(reg, eid, c, cid);
    }
    template<typename... Component, typename Callable>
    void query_comps(Callable c, ComponentId<Component>... cid)
    {
        query_components(reg, eid, c, cid...);
    }
};

inline EntityWrap wrap_entity(Registry& reg, EntityId eid) { return EntityWrap(reg, eid); }
inline EntityWrap create_entity_wrap(Registry& reg, const char* name = nullptr) { return EntityWrap(reg, create_entity(reg, name)); }
inline EntityWrap create_or_find_entity_wrap(Registry& reg, const char* name) { return EntityWrap(reg, create_or_find_entity(reg, name)); }
inline EntityWrap create_prefab_wrap(Registry& reg, const char* name = nullptr) { return EntityWrap(reg, create_prefab(reg, name)); }
inline EntityWrap create_wrap_from_prefab(Registry& reg, EntityId prefabEid, const char* name = nullptr) { return EntityWrap(reg, create_from_prefab(reg, prefabEid, name)); }
inline EntityWrap create_wrap_from_prefab(Registry& reg, EntityWrap prefabWrap, const char* name = nullptr) { return EntityWrap(reg, create_from_prefab(reg, prefabWrap.eid, name)); }
inline EntityWrap find_entity_wrap(Registry& reg, const char* name) { return EntityWrap(reg, eecs::find_entity(reg, name)); }

}

