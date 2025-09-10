#include <iostream>

#include "eecs.h"

struct vec2f
{
    float x, y;
};

int main(int argc, const char** argv)
{
    printf("--- PREFABS ---\n");
    eecs::Registry reg;
    eecs::EntityId eid = create_entity(reg);

    // Make a prefab with one component
    printf("Creating a base prefab...\n");
    eecs::EntityWrap basePrefab = create_prefab_wrap(reg)
        .set(COMPID(vec2f, position), {0.f, 1.f})
        .set(COMPID(float, hitpoints), 100.f)
        .set(COMPID(float, productionTimer), 0.f);

    printf("Querying entities for position...\n");
    eecs::query_entities(reg, [&](eecs::EntityId, const vec2f& position)
    {
        printf("We expect it not to trigger, as prefabs are filtered from queries and systems!\n");
    }, COMPID(const vec2f, position));

    eecs::EntityWrap namedBasePrefab = create_wrap_from_prefab(reg, basePrefab) // create from existing prefab
        .toprefab() // Convert to prefab too
        .set(COMPID(std::string, name), {"Noname"});

    create_wrap_from_prefab(reg, namedBasePrefab);

    create_wrap_from_prefab(reg, namedBasePrefab)
        .set(COMPID(vec2f, position), {0.f, 0.f})
        .set(COMPID(std::string, name), {"Station A"});

    create_wrap_from_prefab(reg, namedBasePrefab)
        .set(COMPID(vec2f, position), {10.f, -3.f})
        .set(COMPID(std::string, name), {"Station B"});

    printf("Querying bases...\n");
    eecs::query_entities(reg, [&](eecs::EntityId, const vec2f& position, const std::string& name, float hitpoints, float productionTimer)
    {
        printf("Found a base '%s' at (%.2f, %.2f) at %.2fhp and with %.2f production timer!\n",
                name.c_str(), position.x, position.y, hitpoints, productionTimer);
    }, COMPID(const vec2f, position), COMPID(const std::string, name), COMPID(const float, hitpoints), COMPID(const float, productionTimer));

    return 0;
}

