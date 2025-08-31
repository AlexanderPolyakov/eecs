#include <iostream>

#include "eecs.h"

int main(int argc, const char** argv)
{
    printf("--- QUERY ENTITIES ---\n");
    eecs::Registry reg;

    create_entity_wrap(reg)
        .set(COMPID(float, position), 1.f)
        .set(COMPID(float, velocity), 0.f);
    create_entity_wrap(reg)
        .set(COMPID(float, position), 2.f)
        .set(COMPID(float, velocity), 1.f);

    eecs::query_entities(reg,
    [&](eecs::EntityId eid, float position, float velocity)
    {
        printf("Entity(%d): pos (%.2f), vel (%.2f)\n", eid, position, velocity);
    }, COMPID(float, position), COMPID(float, velocity));
    return 0;
}
