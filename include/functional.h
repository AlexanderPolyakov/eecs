#pragma once

namespace eecs
{

// Forward decl
struct Registry;

template<typename ComponentType>
void reg_component(Registry& reg, ComponentId<ComponentType> cid);

template<typename ComponentType>
void set_component(Registry& reg, EntityId eid, ComponentId<ComponentType> cid, ComponentType val);

template<typename ComponentType>
ComponentType get_comp_or(Registry& reg, EntityId eid, ComponentId<ComponentType> cid, const ComponentType& def);

template<typename ComponentType>
bool has_comp(Registry& reg, EntityId eid, ComponentId<ComponentType> cid);

template<typename ComponentType, typename Callable>
void query_component(Registry& reg, EntityId eid, Callable foo, ComponentId<ComponentType> cid);

template<typename Callable, typename... ComponentTypes>
void query_components(Registry& reg, EntityId eid, Callable foo, ComponentId<ComponentTypes>... cid);

EntityId create_entity(Registry& reg, const char* name = nullptr);
EntityId create_prefab(Registry& reg, const char* name = nullptr);
EntityId create_from_prefab(Registry& reg, EntityId prefabEid, const char* name = nullptr);
void make_prefab(Registry& reg, EntityId eid);
void del_entity(Registry& reg, EntityId eid);
void del_all_entities(Registry& reg);
void del_all_systems(Registry& reg);

bool is_prefab(Registry& reg, EntityId eid);

template<typename Callable>
void get_entity_name(Registry& reg, EntityId eid, Callable c);

EntityId find_entity(Registry& reg, const char* name);

template<typename ComponentType>
void del_component(Registry& reg, EntityId eid, ComponentId<ComponentType> cid);

template<typename Callable, typename... ComponentTypes>
void query_entities(Registry& registry, Callable func, ComponentId<ComponentTypes>... args);

template<typename Callable, typename... ComponentTypes>
void reg_system(Registry& reg, Callable func, ComponentId<ComponentTypes>... args);

template<typename Callable, typename... ComponentTypes>
void reg_enter(Registry& reg, Callable func, ComponentId<ComponentTypes>... args);
template<typename Callable, typename... ComponentTypes>
void reg_exit(Registry& reg, Callable func, ComponentId<ComponentTypes>... args);

void step(Registry& reg);

}

