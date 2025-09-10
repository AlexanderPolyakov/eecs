#pragma once

namespace eecs
{

struct Registry
{
    struct CachedQueryBase
    {
        virtual void execute(Registry& reg) = 0;
        virtual ~CachedQueryBase() {};
    };

    template<typename Callable, typename... ComponentTypes>
    struct CachedQuery : public CachedQueryBase
    {
        CachedQuery(Callable _func, ComponentId<ComponentTypes>... args)
        : componentIds(std::make_tuple(args...)), func(_func)
        {
        }

        std::tuple<ComponentId<ComponentTypes>...> componentIds;
        Callable func;

        void execute(Registry& reg) final;
        ~CachedQuery() final {};
    };

    std::unordered_map<fnv1_hash_t, SparseSetHolder> holders;
    std::vector<CachedQueryBase*> systems;

    std::unordered_map<EntityId, std::string> entityToName;
    std::unordered_map<std::string, EntityId> entityNames; // TODO: I don't like that it's a separate thing, can we make it a component and have an unordered map there for quick nav?

    std::vector<EntityId> freeEidsList;
    EntityId lastValidEid = 0;
};

}

