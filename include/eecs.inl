template<typename ComponentType>
inline SparseSetHolder create_sparse_set_holder(ComponentId<ComponentType> cid)
{
    return SparseSetHolder(typeid(ComponentType).hash_code(), cid.sourceStr, (SparseSetBase*)new SparseSet<ComponentType>());
}

template<typename ComponentType>
void reg_component(Registry& reg, ComponentId<ComponentType> cid)
{
    size_t typeHash = typeid(ComponentType).hash_code();
    // First try to find it
    auto itf = reg.holders.find(cid.hash);
    if (itf != reg.holders.end())
    {
        // check type
        assert(itf->second.typeHash == typeHash);
        return;
    }

    reg.holders.emplace(cid.hash, create_sparse_set_holder(cid));
}

template<typename ComponentType>
void set_component(Registry& reg, EntityId eid, ComponentId<ComponentType> cid, ComponentType val)
{
    size_t typeHash = typeid(ComponentType).hash_code();
    // First try to find it
    auto itf = reg.holders.find(cid.hash);
    if (itf == reg.holders.end())
    {
        reg.holders.emplace(cid.hash, create_sparse_set_holder(cid));
        itf = reg.holders.find(cid.hash);
    }
    if (itf != reg.holders.end())
    {
        assert(itf->second.typeHash == typeHash);
        if (itf->second.typeHash != typeHash)
            return;

        std::vector<Registry::CachedQueryBase*> toExecute;
        for (Registry::CachedQueryBase* q : reg.onEnter)
            if (q->includesCompHash(cid.hash) && !q->includesEntity(reg, eid))
                toExecute.emplace_back(q);
        auto execEnter = [&]()
        {
            for (Registry::CachedQueryBase* q : toExecute)
                q->executeOn(reg, eid);
        };
        SparseSet<ComponentType>& set = *(SparseSet<ComponentType>*)itf->second.set;
        while (eid >= set.indices.size())
            set.indices.emplace_back(-1);
        int idx = set.indices[eid];
        if (idx < 0)
        {
            int i = set.entities.size();
            set.indices[eid] = i;
            set.entities.emplace_back(eid);
            set.data.emplace_back(val);

            execEnter();
            return;
        }
        assert(set.entities[idx] == eid);
        set.data[idx] = val;

        execEnter();
    }
    else
    {
        // TODO: add a error report here, we now have a name for the component!
    }
}

template<typename ComponentType>
inline ComponentType get_comp_or(Registry& reg, EntityId eid, ComponentId<ComponentType> cid, const ComponentType& def)
{
    size_t typeHash = typeid(ComponentType).hash_code();
    // First try to find it
    auto itf = reg.holders.find(cid.hash);
    if (itf == reg.holders.end())
        return def;
    assert(itf->second.typeHash == typeHash);
    if (itf->second.typeHash != typeHash)
        return def;
    SparseSet<ComponentType>& set = *(SparseSet<ComponentType>*)itf->second.set;
    if (eid >= set.indices.size())
        return def;
    int idx = set.indices[eid];
    if (idx < 0 || set.entities[idx] != eid)
        return def;
    return set.data[idx];
}

template<typename ComponentType>
inline bool has_comp(Registry& reg, EntityId eid, ComponentId<ComponentType> cid)
{
    size_t typeHash = typeid(ComponentType).hash_code();
    // First try to find it
    auto itf = reg.holders.find(cid.hash);
    if (itf == reg.holders.end())
        return false;
    assert(itf->second.typeHash == typeHash);
    if (itf->second.typeHash != typeHash)
        return false;
    SparseSet<ComponentType>& set = *(SparseSet<ComponentType>*)itf->second.set;
    if (eid >= set.indices.size())
        return false;
    int idx = set.indices[eid];
    if (idx < 0 || set.entities[idx] != eid)
        return false;
    return true;
}

template<typename ComponentType, typename Callable>
inline void query_component(Registry& reg, EntityId eid, Callable foo, ComponentId<ComponentType> cid)
{
    size_t typeHash = typeid(std::remove_const_t<ComponentType>).hash_code();
    // First try to find it
    auto itf = reg.holders.find(cid.hash);
    if (itf != reg.holders.end())
    {
        assert(itf->second.typeHash == typeHash);
        if (itf->second.typeHash != typeHash)
            return;
        SparseSet<std::remove_const_t<ComponentType>>& set = *(SparseSet<std::remove_const_t<ComponentType>>*)itf->second.set;
        if (eid >= set.indices.size())
            return;
        int idx = set.indices[eid];
        if (idx < 0 || set.entities[idx] != eid)
            return;
        foo(set.data[idx]);
    }
    else
    {
        // TODO: maybe assert? it makes sense to tell end user that you're querying some component
        // which wasn't even registered.
    }
}

template<typename ComponentType>
inline SparseSet<std::remove_const_t<ComponentType>>* registry_get(const Registry& reg, ComponentId<ComponentType> cid)
{
    auto it = reg.holders.find(cid.hash);
    if (it != reg.holders.end())
        return (SparseSet<std::remove_const_t<ComponentType>>*)(it->second.set);
    return nullptr;
}

template<typename Callable, typename... ComponentTypes, std::size_t... Is>
void query_components_impl(Registry& registry, EntityId eid, Callable func, const std::tuple<ComponentId<ComponentTypes>...>& args_tuple,
        std::index_sequence<Is...>)
{
    // If no components were requested, there's nothing to do.
    if constexpr (sizeof...(ComponentTypes) == 0)
        return;

    std::tuple<SparseSet<std::remove_const_t<ComponentTypes>>*...> componentSets = { registry_get<ComponentTypes>(registry, std::get<Is>(args_tuple))... };

    if ((... || (std::get<Is>(componentSets) == nullptr)))
        return;

    const bool inAllSets = (std::get<Is>(componentSets)->has(eid) && ...);

    if (inAllSets)
        func(std::get<Is>(componentSets)->get(eid)...);
}

template<typename Callable, typename... ComponentTypes>
void query_components(Registry& reg, EntityId eid, Callable func, ComponentId<ComponentTypes>... cid)
{
    query_components_impl(reg, eid, func, std::make_tuple(cid...), std::index_sequence_for<ComponentTypes...>{});
}

inline EntityId create_or_find_entity(Registry& reg, const char* name)
{
    eecs::EntityId eid = find_entity(reg, name);
    if (eid == invalid_eid)
        return create_entity(reg, name);
    return eid;
}

inline EntityId create_entity(Registry& reg, const char* name)
{
    auto retEid = [&](EntityId eid)
    {
        if (name)
        {
            reg.entityNames[std::string(name)] = eid;
            reg.entityToName[eid] = std::string(name);
        }
        return eid;
    };
    if (reg.freeEidsList.empty())
        return retEid(reg.lastValidEid++);
    EntityId lastEid = reg.freeEidsList.back();
    reg.freeEidsList.pop_back();
    return retEid(lastEid);
}

inline EntityId create_prefab(Registry& reg, const char* name)
{
    EntityId eid = create_entity(reg, name);
    set_component(reg, eid, eecs::kPrefabTag, {});
    return eid;
}

inline EntityId create_from_prefab(Registry& reg, EntityId prefabEid, const char* name)
{
    EntityId eid = create_entity(reg, name);
    for (auto& [hash, holder] : reg.holders)
    {
        if (hash == kPrefabTag.hash)
            continue;
        if (!holder.set || !holder.set->has(prefabEid))
            continue;
        holder.set->cloneEntity(prefabEid, eid);
    }
    return eid;
}

inline void copy_from_prefab(Registry& reg, EntityId prefabEid, EntityId eid)
{   // if this is correct we can replace the duplicate code in create_from_prefab
    for (auto& [hash, holder] : reg.holders)
    {
        if (hash == kPrefabTag.hash)
            continue;
        if (!holder.set || !holder.set->has(prefabEid))
            continue;
        holder.set->cloneEntity(prefabEid, eid);
    }
}

inline void make_prefab(Registry& reg, EntityId eid)
{
    set_component(reg, eid, eecs::kPrefabTag, {});
}

inline bool is_prefab(Registry& reg, EntityId eid)
{
    auto itf = reg.holders.find(kPrefabTag.hash);
    if (itf == reg.holders.end())
        return false;

    const SparseSetBase& set = *itf->second.set;
    return set.has(eid);
}

template<typename Callable>
void entity_name(const Registry& reg, EntityId eid, Callable c)
{
    auto itf = reg.entityToName.find(eid);
    if (itf != reg.entityToName.end())
        c(itf->second);
}

inline EntityId find_entity(Registry& reg, const char* name)
{
    auto itf = reg.entityNames.find(name);
    if (itf == reg.entityNames.end())
    {
        //printf("cannot find entity with the name '%s'\n", name);
        return invalid_eid;
    }
    return itf->second;
}

template<typename T>
inline void del_comp_impl(SparseSet<T>& set, EntityId eid)
{
    if (eid >= set.indices.size())
        return; // can do nothing

    int idx = set.indices[eid];
    if (idx < 0)
        return; // can do nothing

    set.indices[eid] = -1; // TODO: work on shrink to fit here
    size_t lastIdx = set.data.size() - 1;
    if (idx == lastIdx) // last entity
    {
        set.entities.erase(set.entities.begin() + idx);
        set.data.erase(set.data.begin() + idx);
    }
    else
    {
        EntityId lastEid = set.entities[lastIdx];

        set.indices[lastEid] = idx;
        set.entities[idx] = set.entities[lastIdx];
        set.data[idx] = set.data[lastIdx];

        set.entities.erase(set.entities.begin() + lastIdx);
        set.data.erase(set.data.begin() + lastIdx);
    }
}

inline void delete_component(Registry& reg, SparseSetHolder& holder, fnv1_hash_t nameHash, EntityId eid)
{
    if (!holder.set)
        return;
    for (Registry::CachedQueryBase* q : reg.onExit)
        if (q->includesCompHash(nameHash))
            q->executeOn(reg, eid);
    holder.set->delComponent(eid);
}

inline void del_entity(Registry& reg, EntityId eid)
{
    if (eid + 1 == reg.lastValidEid)
        reg.lastValidEid = eid;
    else
        reg.freeEidsList.push_back(eid);
    // Go through all holders and delete components
    for (auto& [nameHash, holder] : reg.holders)
        delete_component(reg, holder, nameHash, eid);
    // Search for name and clear it
    auto itf = reg.entityToName.find(eid);
    if (itf != reg.entityToName.end())
    {
        reg.entityNames.erase(itf->second);
        reg.entityToName.erase(itf);
    }
}

inline void del_all_entities(Registry& reg)
{
  reg.lastValidEid = 0;
  reg.freeEidsList.clear();
  reg.entityNames.clear();
  reg.entityToName.clear();
  reg.holders.clear();
}

inline void del_all_systems(Registry& reg)
{
  for (Registry::CachedQueryBase* q : reg.systems)
    delete q;
  for (Registry::CachedQueryBase* q : reg.onEnter)
    delete q;
  for (Registry::CachedQueryBase* q : reg.onExit)
    delete q;
  for (auto& [hash, qs] : reg.eventHandlers)
  {
      for (Registry::EventHandlerBase* q : qs)
          delete q;
      qs.clear();
  }
  reg.systems.clear();
  reg.onEnter.clear();
  reg.onExit.clear();
  reg.eventHandlers.clear();
}

template<typename ComponentType>
inline void del_component(Registry& reg, EntityId eid, ComponentId<ComponentType> cid)
{
    size_t typeHash = typeid(ComponentType).hash_code();
    // First try to find it
    auto itf = reg.holders.find(cid.hash);
    if (itf != reg.holders.end())
    {
        assert(itf->second.typeHash == typeHash);
        if (itf->second.typeHash != typeHash)
            return;
        SparseSet<ComponentType>& set = *(SparseSet<ComponentType>*)itf->second.set;
        delete_component(reg, itf->second, cid.hash, eid);
    }
}

template<typename Callable, typename... ComponentTypes, std::size_t... Is>
inline void execute_impl(Registry& registry, EntityId entity, Callable func, const std::tuple<ComponentId<ComponentTypes>...>& args_tuple,
                         std::index_sequence<Is...>)
{
    // If no components were requested, there's nothing to do.
    if constexpr (sizeof...(ComponentTypes) == 0)
        return;

    std::tuple<SparseSet<std::remove_const_t<ComponentTypes>>*...> componentSets = { registry_get<ComponentTypes>(registry, std::get<Is>(args_tuple))... };
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
    if ((... || (std::get<Is>(componentSets) == nullptr)))
        return;
#pragma clang diagnostic pop
    const bool inAllSets = (std::get<Is>(componentSets)->has(entity) && ...);
    //const bool isPrefab = is_prefab(registry, entity);

    if (inAllSets)
        func(entity, std::get<Is>(componentSets)->get(entity)...);
}

// TODO: think about deduplication
template<typename Callable, typename... ComponentTypes, std::size_t... Is>
inline void event_impl(Registry& registry, EntityId entity, EntityId sourceEid, Callable func, const std::tuple<ComponentId<ComponentTypes>...>& args_tuple,
                         std::index_sequence<Is...>)
{
    // If no components were requested, there's nothing to do.
    if constexpr (sizeof...(ComponentTypes) == 0)
        return;

    std::tuple<SparseSet<std::remove_const_t<ComponentTypes>>*...> componentSets = { registry_get<ComponentTypes>(registry, std::get<Is>(args_tuple))... };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
    if ((... || (std::get<Is>(componentSets) == nullptr)))
        return;
#pragma clang diagnostic pop

    if (entity == eecs::invalid_eid) // means we're broadcasting!
    {
        // TODO: remove copy paste from query_entities_impl and unify
        size_t minSize = std::numeric_limits<size_t>::max();
        const SparseSetBase* smallestSetPtr = nullptr;

        // This fold expression finds the size of each set by index and tracks the smallest.
        ((void)(
            (std::get<Is>(componentSets)->entities.size() < minSize) &&
            (minSize = std::get<Is>(componentSets)->entities.size(), smallestSetPtr = static_cast<const SparseSetBase*>(std::get<Is>(componentSets)))
        ), ...);

        if (minSize == 0)
            return; // No entities have all components if one set is empty.

        // TODO: this is actually a copy which can be expensive, but we might get a better solution if we omit copying (though insert/erase will be more painful this way)
        std::vector<EntityId> entitiesToCheck = smallestSetPtr->entities;

        for (EntityId ent : entitiesToCheck)
        {
            // For each entity, check if it exists in ALL the other sets.
            const bool inAllSets = (std::get<Is>(componentSets)->has(ent) && ...);

            if (inAllSets)
                func(ent, sourceEid, std::get<Is>(componentSets)->get(ent)...);
        }
    }
    else
    {
        const bool inAllSets = (std::get<Is>(componentSets)->has(entity) && ...);
        //const bool isPrefab = is_prefab(registry, entity);

        if (inAllSets)
            func(entity, sourceEid, std::get<Is>(componentSets)->get(entity)...);
    }
}

template<typename... ComponentTypes, std::size_t... Is>
inline bool includes_entity_impl(Registry& registry, EntityId entity, const std::tuple<ComponentId<ComponentTypes>...>& args_tuple,
                                 std::index_sequence<Is...>)
{
    // If no components were requested, there's nothing to do.
    if constexpr (sizeof...(ComponentTypes) == 0)
        return false;

    std::tuple<SparseSet<std::remove_const_t<ComponentTypes>>*...> componentSets = { registry_get<ComponentTypes>(registry, std::get<Is>(args_tuple))... };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
    if ((... || (std::get<Is>(componentSets) == nullptr)))
        return false;
#pragma clang diagnostic pop

    const bool inAllSets = (std::get<Is>(componentSets)->has(entity) && ...);

    return inAllSets;
}

template<typename Callable, typename... ComponentTypes, std::size_t... Is>
inline void query_entities_impl(Registry& registry, Callable func, const std::tuple<ComponentId<ComponentTypes>...>& args_tuple,
                                std::index_sequence<Is...>)
{
    // If no components were requested, there's nothing to do.
    if constexpr (sizeof...(ComponentTypes) == 0)
        return;

    std::tuple<SparseSet<std::remove_const_t<ComponentTypes>>*...> componentSets = { registry_get<ComponentTypes>(registry, std::get<Is>(args_tuple))... };
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
    if ((... || (std::get<Is>(componentSets) == nullptr)))
        return;
#pragma clang diagnostic pop

    size_t minSize = std::numeric_limits<size_t>::max();
    const SparseSetBase* smallestSetPtr = nullptr;

    // This fold expression finds the size of each set by index and tracks the smallest.
    ((void)(
        (std::get<Is>(componentSets)->entities.size() < minSize) &&
        (minSize = std::get<Is>(componentSets)->entities.size(), smallestSetPtr = static_cast<const SparseSetBase*>(std::get<Is>(componentSets)))
    ), ...);

    if (minSize == 0)
        return; // No entities have all components if one set is empty.

    // TODO: this is actually a copy which can be expensive, but we might get a better solution if we omit copying (though insert/erase will be more painful this way)
    std::vector<EntityId> entitiesToCheck = smallestSetPtr->entities;

    for (EntityId entity : entitiesToCheck)
    {
        // For each entity, check if it exists in ALL the other sets.
        const bool inAllSets = (std::get<Is>(componentSets)->has(entity) && ...);
        const bool isPrefab = is_prefab(registry, entity);

        if (inAllSets && !isPrefab)
            func(entity, std::get<Is>(componentSets)->get(entity)...);
    }
}

template<typename Callable, typename... ComponentTypes>
inline void query_entities(Registry& registry, Callable func, ComponentId<ComponentTypes>... args)
{
    // Create an index sequence for the component types
    // and call the implementation function.
    query_entities_impl(registry, func, std::make_tuple(args...), std::index_sequence_for<ComponentTypes...>{});
}

template<typename Callable, typename... ComponentTypes>
void Registry::CachedQuery<Callable, ComponentTypes...>::execute(Registry& reg) const
{
    query_entities_impl(reg, func, componentIds, std::index_sequence_for<ComponentTypes...>{});
}

template<typename Callable, typename... ComponentTypes>
void Registry::CachedQuery<Callable, ComponentTypes...>::executeOn(Registry& reg, EntityId eid) const
{
    execute_impl(reg, eid, func, componentIds, std::index_sequence_for<ComponentTypes...>{});
}

template<typename Callable, typename... ComponentTypes>
void Registry::EventHandler<Callable, ComponentTypes...>::onEvent(Registry& reg, EntityId eid, EntityId sourceEid) const
{
    event_impl(reg, eid, sourceEid, func, componentIds, std::index_sequence_for<ComponentTypes...>{});
}

template<typename Callable, typename... ComponentTypes>
bool Registry::CachedQuery<Callable, ComponentTypes...>::includesEntity(Registry& reg, EntityId eid) const
{
    return includes_entity_impl(reg, eid, componentIds, std::index_sequence_for<ComponentTypes...>{});
}

template<typename... ComponentTypes, std::size_t... Is>
inline bool has_hash(fnv1_hash_t hash, const std::tuple<ComponentId<ComponentTypes>...>& args_tuple, std::index_sequence<Is...>)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
    return (... || (std::get<Is>(args_tuple).hash == hash));
#pragma clang diagnostic pop
}

template<typename Callable, typename... ComponentTypes>
bool Registry::CachedQuery<Callable, ComponentTypes...>::includesCompHash(fnv1_hash_t hash) const
{
    return has_hash(hash, componentIds, std::index_sequence_for<ComponentTypes...>{});
}

template<typename Callable, typename... ComponentTypes>
inline void reg_system(Registry& reg, Callable func, ComponentId<ComponentTypes>... args)
{
    Registry::CachedQuery<Callable, ComponentTypes...>* q = new Registry::CachedQuery<Callable, ComponentTypes...>(func, args...);
    reg.systems.emplace_back(q);
}

template<typename Callable, typename... ComponentTypes>
inline void reg_enter(Registry& reg, Callable func, ComponentId<ComponentTypes>... args)
{
    Registry::CachedQuery<Callable, ComponentTypes...>* q = new Registry::CachedQuery<Callable, ComponentTypes...>(func, args...);
    reg.onEnter.emplace_back(q);
}

template<typename Callable, typename... ComponentTypes>
inline void reg_exit(Registry& reg, Callable func, ComponentId<ComponentTypes>... args)
{
    Registry::CachedQuery<Callable, ComponentTypes...>* q = new Registry::CachedQuery<Callable, ComponentTypes...>(func, args...);
    reg.onExit.emplace_back(q);
}

template<typename Callable, typename... ComponentTypes>
inline void on_event(Registry& reg, fnv1_hash_t evtName, Callable func, ComponentId<ComponentTypes>... args)
{
    Registry::EventHandler<Callable, ComponentTypes...>* q = new Registry::EventHandler<Callable, ComponentTypes...>(func, args...);
    auto itf = reg.eventHandlers.find(evtName);
    if (itf == reg.eventHandlers.end())
        reg.eventHandlers.emplace(evtName, std::vector<Registry::EventHandlerBase*>({q}));
    else
        itf->second.emplace_back(q);
}

inline void emit_event(Registry& reg, fnv1_hash_t evtName, EntityId eid, EntityId sourceEid)
{
    auto itf = reg.eventHandlers.find(evtName);
    if (itf == reg.eventHandlers.end())
    {
        //assert(false && "Trying to emit event without anyone subscribed to it!");
        return;
    }
    for (Registry::EventHandlerBase* q : itf->second)
        q->onEvent(reg, eid, sourceEid);
}

inline void step(Registry& reg)
{
    for (Registry::CachedQueryBase* q : reg.systems)
        q->execute(reg);
}

