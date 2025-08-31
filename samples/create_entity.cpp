#include <iostream>

#include "eecs.h"

int main(int argc, const char** argv)
{
    printf("--- CREATE ENTITY ---\n");
    eecs::Registry reg;
    eecs::EntityId eid = create_entity(reg);
    printf("Created entity with entity id = %d\n", eid);
    return 0;
}

