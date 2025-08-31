#include <iostream>

#include "eecs.h"

int main(int argc, const char** argv)
{
    printf("--- BASIC SYSTEM ---\n");

    eecs::Registry reg;
    create_entity_wrap(reg)
        .set(COMPID(float, position), 10.f)
        .set(COMPID(float, velocity), 10.f);

    create_entity_wrap(reg)
        .set(COMPID(float, position), 0.f)
        .set(COMPID(float, velocity), 0.f)
        .set(COMPID(float, acceleration), 2.f);

    const float dt = 1.f / 60.f;
    eecs::reg_system(reg, [&](eecs::EntityId eid, float& position, float velocity)
            {
                position += velocity * dt;
            }, COMPID(float, position), COMPID(float, velocity));

    eecs::reg_system(reg, [&](eecs::EntityId eid, float& velocity, float acceleration)
            {
                velocity += acceleration * dt;
            }, COMPID(float, velocity), COMPID(float, acceleration));

    int frameNo = 0;
    eecs::reg_system(reg, [&](eecs::EntityId eid, float position)
            {
                if (frameNo % 100 == 0)
                    printf("Ent(%d) pos (%.2f)\n", eid, position);
            }, COMPID(float, position));

    for (size_t i = 0; i < 1000; ++i)
    {
        eecs::step(reg);
        frameNo++;
    }
    return 0;
}
