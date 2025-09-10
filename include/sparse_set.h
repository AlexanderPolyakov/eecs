#pragma once

namespace eecs
{

struct SparseSetBase
{
    std::vector<int> indices;
    std::vector<EntityId> entities;

    inline bool has(EntityId eid) const { return eid < indices.size() && indices[eid] != invalid_eid; }
    virtual ~SparseSetBase() {}

    virtual void delComponent(EntityId eid) = 0;
    virtual void cloneEntity(EntityId from, EntityId to) = 0;
};

// Forward declaration for inline virtual destructor
template<typename ComponentType> struct SparseSet;
template<typename T> void del_comp_impl(SparseSet<T>& set, EntityId eid);

namespace details
{
    template<typename T>
    inline void clone_entity(SparseSet<T>& set, EntityId from, EntityId to)
    {
        while (to >= set.indices.size())
            set.indices.emplace_back(-1);
        int idx = set.indices[to];
        if (idx < 0)
        {
            int i = set.entities.size();
            set.indices[to] = i;
            set.entities.emplace_back(to);
            set.data.emplace_back(set.data[set.indices[from]]);
            return;
        }
        set.data[idx] = set.data[set.indices[from]];
    }
}

template<typename ComponentType>
struct SparseSet : public SparseSetBase
{
    std::vector<ComponentType> data;

    ComponentType& get(EntityId eid) { return data[indices[eid]]; }
    virtual ~SparseSet() {}
    void delComponent(EntityId eid) final { del_comp_impl(*this, eid); };
    void cloneEntity(EntityId from, EntityId to) final { details::clone_entity(*this, from, to); }
};

// Due to std::vector<bool> being not really a vector of bools, but implementation specific thing.
// It's less memory optimal this way, but for simplicity we prefer to have a better interface vs
// memory optimizations
template<>
struct SparseSet<bool> : public SparseSetBase
{
    std::vector<char> data;

    bool& get(EntityId eid) { return (bool&)data[indices[eid]]; }
    virtual ~SparseSet() {}
    void delComponent(EntityId eid) final { del_comp_impl(*this, eid); };
    void cloneEntity(EntityId from, EntityId to) final { details::clone_entity(*this, from, to); }
};

struct SparseSetHolder
{
    size_t typeHash; // for validation
    SparseSetBase* set = nullptr;

    SparseSetHolder() = delete;
    SparseSetHolder(size_t type_hash, SparseSetBase* in_set) : typeHash(type_hash), set(in_set) {}
    SparseSetHolder(const SparseSetHolder& hold) = delete;
    SparseSetHolder(SparseSetHolder&& hold) : typeHash(hold.typeHash)
    {
        std::swap(set, hold.set);
    }

    ~SparseSetHolder()
    {
        delete set;
    }

    SparseSetHolder& operator=(const SparseSetHolder& hold) = delete;
    SparseSetHolder& operator=(SparseSetHolder&& hold)
    {
        typeHash = hold.typeHash;
        std::swap(set, hold.set);
        return *this;
    }
};

}

