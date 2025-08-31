#include <iostream>

#include "eecs.h"

int main(int argc, const char** argv)
{
    printf("--- QUERY MULTIPLE COMPONENTS ---\n");
    eecs::Registry reg;
    eecs::EntityId eid = create_entity(reg);

    wrap_entity(reg, eid)
        .set(COMPID(float, position), 42.f)
        .set(COMPID(float, velocity), 1.23f);

    eecs::query_components(reg, eid, [&](const float& position, float velocity)
    {
        printf("Position %.2f, Velocity %.2f\n", position, velocity);
    }, COMPID(float, position), COMPID(float, velocity));
    return 0;
}

