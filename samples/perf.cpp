#include <iostream>
#include <chrono>

#include "eecs.h"

struct vec2f
{
    float x, y;
};

constexpr size_t numIterations = 10000000;

int main(int argc, const char** argv)
{
    printf("--- PERF ---\n");
    eecs::Registry reg;
    eecs::EntityId eid = create_entity(reg);

    // Make a prefab with one component
    printf("Creating a particle prefab...\n");
    eecs::EntityWrap particlePrefab = create_prefab_wrap(reg)
        .set(COMPID(vec2f, position), {0.f, 1.f})
        .set(COMPID(float, wind), 100.f)
        .set(COMPID(float, grav), 0.f);

    eecs::EntityWrap namedParticlePrefab = create_wrap_from_prefab(reg, particlePrefab) // create from existing prefab
        .toprefab() // Convert to prefab too
        .set(COMPID(std::string, name), {"Noname"});

    create_wrap_from_prefab(reg, namedParticlePrefab)
        .set(COMPID(vec2f, position), {0.f, 0.f})
        .set(COMPID(std::string, name), {"Particle A"});

    create_wrap_from_prefab(reg, namedParticlePrefab)
        .set(COMPID(vec2f, position), {10.f, -3.f})
        .set(COMPID(std::string, name), {"Particle B"});

    printf("Querying particles...\n");
    const auto start{std::chrono::high_resolution_clock::now()};
    for (size_t i = 0; i < numIterations; ++i)
    {
        eecs::query_entities(reg, [&](eecs::EntityId, vec2f& position, const std::string& name, float wind, float grav)
        {
            // upd position so no nop
            position.x += wind;
            position.y += grav;
        }, COMPID(vec2f, position), COMPID(const std::string, name), COMPID(const float, wind), COMPID(const float, grav));
    }
    const auto end{std::chrono::high_resolution_clock::now()};

    const std::chrono::duration<double> elapsedSeconds{end - start};
    printf("%fs per %ldk iterations (%dk iter/second)\n", elapsedSeconds.count(), numIterations/1000, int(float(numIterations) / elapsedSeconds.count())/1000);

    return 0;
}

