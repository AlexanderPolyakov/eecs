#pragma once

namespace eecs
{

struct Registry
{
    struct CachedQueryBase
    {
        virtual void execute(Registry& reg) const = 0;
        virtual void executeOn(Registry& reg, EntityId eid) const = 0;
        virtual bool includesEntity(Registry& reg, EntityId eid) const = 0;
        virtual ~CachedQueryBase() {};

        virtual bool includesCompHash(fnv1_hash_t hash) const = 0;
    };

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Weverything"
    template<typename Callable, typename... ComponentTypes>
    struct CachedQuery final : public CachedQueryBase
    {
//#pragma diagnostic pop
        CachedQuery(Callable _func, ComponentId<ComponentTypes>... args)
        : componentIds(std::make_tuple(args...)), func(_func)
        {
        }

        std::tuple<ComponentId<ComponentTypes>...> componentIds;
        Callable func;

        void execute(Registry& reg) const final;
        void executeOn(Registry& reg, EntityId eid) const final;
        bool includesEntity(Registry& reg, EntityId eid) const final;
        ~CachedQuery() final {};
        bool includesCompHash(fnv1_hash_t hash) const final;
    };

    struct EventHandlerBase
    {
        virtual void onEvent(Registry& reg, EntityId eid, EntityId sourceEid) const = 0;
        virtual ~EventHandlerBase() {};
    };

    template<typename Callable, typename... ComponentTypes>
    struct EventHandler : public EventHandlerBase
    {
        EventHandler(Callable _func, ComponentId<ComponentTypes>... args)
        : componentIds(std::make_tuple(args...)), func(_func)
        {
        }

        std::tuple<ComponentId<ComponentTypes>...> componentIds;
        Callable func;

        void onEvent(Registry& reg, EntityId eid, EntityId sourceEid) const final;
        ~EventHandler() final {};
    };

    std::unordered_map<fnv1_hash_t, SparseSetHolder> holders;
    std::vector<CachedQueryBase*> systems;

    std::unordered_map<fnv1_hash_t, std::vector<EventHandlerBase*>> eventHandlers;
    std::vector<CachedQueryBase*> onEnter;
    std::vector<CachedQueryBase*> onExit;

    std::unordered_map<EntityId, std::string> entityToName;
    std::unordered_map<std::string, EntityId> entityNames; // TODO: I don't like that it's a separate thing, can we make it a component and have an unordered map there for quick nav?

    std::vector<EntityId> freeEidsList;
    EntityId lastValidEid = 0;
};

}

