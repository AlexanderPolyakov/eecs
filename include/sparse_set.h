
namespace eecs
{

struct SparseSetBase
{
    std::vector<int> indices;
    std::vector<EntityId> entities;

    inline bool has(EntityId eid) { return eid < indices.size() && indices[eid] != invalid_eid; }
    virtual ~SparseSetBase() {}

    virtual void delComponent(EntityId eid) = 0;
};

// Forward declaration for inline virtual destructor
template<typename ComponentType> struct SparseSet;
template<typename T> void del_comp_impl(SparseSet<T>& set, EntityId eid);

template<typename ComponentType>
struct SparseSet : public SparseSetBase
{
    std::vector<ComponentType> data;

    ComponentType& get(EntityId eid) { return data[indices[eid]]; }
    virtual ~SparseSet() {}
    void delComponent(EntityId eid) final { del_comp_impl(*this, eid); };
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

