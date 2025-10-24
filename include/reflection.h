#pragma once

#include <functional>

namespace eecs
{

struct TypeHandler
{
    virtual ~TypeHandler() {}

    virtual void handleType(const std::string_view& view, void* val) const = 0;
};

template<typename T>
struct LambdaHandler : public TypeHandler
{
    std::function<void(const std::string_view&, T)> handler;

    template<typename Callable>
    LambdaHandler(Callable c) : handler(c) {};
    virtual ~LambdaHandler() {};

    void handleType(const std::string_view& view, void* val) const final
    {
        handler(view, *(T*)val);
    }
};

struct ComponentHandlers
{
    std::unordered_map<size_t, TypeHandler*> typeHandlers;

    ~ComponentHandlers()
    {
        for (auto& [typeHash, handler] : typeHandlers)
            delete handler;
    }

    template<typename T, typename Callable>
    void addTypeHandler(Callable c)
    {
        LambdaHandler<T>* handler = new LambdaHandler<T>(c);
        typeHandlers.emplace(typeid(T).hash_code(), handler);
    }
};

inline void handle_entity_components(const eecs::Registry& reg, eecs::EntityId eid, const ComponentHandlers& handlers)
{
    for (auto& [hash, holder] : reg.holders)
    {
        if (!holder.set)
            continue;
        if (eid >= holder.set->indices.size())
            continue;
        const int idx = holder.set->indices[eid];
        if (idx < 0 || holder.set->entities[idx] != eid)
            continue;
        auto typeItf = handlers.typeHandlers.find(holder.typeHash);
        if (typeItf == handlers.typeHandlers.end())
        {
            printf("Component name: '%s'\n", holder.name.c_str());
            assert(typeItf != handlers.typeHandlers.end() && "Should provide handler for component");
            continue;
        }
        typeItf->second->handleType(holder.name, holder.set->getCompPtr(eid));
    }
}

}

