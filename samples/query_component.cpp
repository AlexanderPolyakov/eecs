#include <iostream>

#include "eecs.h"

int main(int argc, const char** argv)
{
    printf("--- QUERY COMPONENT ---\n");
    eecs::Registry reg;
    eecs::EntityId eid = create_entity(reg);

    wrap_entity(reg, eid)
        .set(COMPID(float, position), 42.f);

    eecs::query_component(reg, eid, [&](const float& position)
    {
        printf("Component value is %.2f\n", position);
    }, COMPID(float, position));
    return 0;
}

