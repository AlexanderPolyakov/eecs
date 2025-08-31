## Brief
EathECS. An ECS with a focus on ease of use.

Core of the system is sparse set ecs implementation with some C++ fold expressions to make queries and systems work naturally.
Components are identified by their "name hash" instead of type, so essentially eecs allows you to have any number of components of the same type.

## Build
To use `eecs` you just need to `#include "eath_ecs.h"` in your files and that should be it. All functions are inlined and are accessible by your code.

If you want to build samples, you can go the cmake route:
```
cmake -B build
cmake --build build
```
And then run executables built in `build/` directory.

## TODOs
- [ ] Shrink SparseSets
- [ ] More examples:
    - [ ] Deletion of entities
    - [ ] Deletion of components
    - [ ] Query from inside another query or system
- [ ] Somehow (tm) figure out how to fix component and lambda arguments duplication?
- [ ] Better description
- [ ] Serialization
- [ ] Script integration (?)
- [ ] Tests!

## Examples
Look in the `samples` folder, they're concise, but generally you have to
* Create your registry somewhere: `eecs::Registry reg;`
* Create an entity: `eecs::EntityId eid = create_entity(reg);`
* Set value for components: `set_component(reg, eid, COMPID(float, position), 0.f);`
* Or maybe you want to utilize entity wrap (so you can use `.set()` function to set components):

```c++
create_entity_wrap(reg)
    .set(COMPID(float, position), 1.f)
    .set(COMPID(float, velocity), 0.f);
```

* Query single component:

```c++
eecs::query_component(reg, eid, [&](const float& position)
{
    printf("Component value is %.2f\n", position);
}, COMPID(float, position));
```

* Query multiple components:

```c++
eecs::query_components(reg, eid, [&](const float& position, float velocity)
{
    printf("Position %.2f, Velocity %.2f\n", position, velocity);
}, COMPID(float, position), COMPID(float, velocity));
```

* Query all entities with components:

```c++
eecs::query_entities(reg,
[&](eecs::EntityId eid, float position, float velocity)
{
    printf("Entity(%d): pos (%.2f), vel (%.2f)\n", eid, position, velocity);
}, COMPID(float, position), COMPID(float, velocity));
```

* Register a system:

```c++
eecs::reg_system(reg, [&](eecs::EntityId eid, float& position, float velocity)
{
    position += velocity * dt;
}, COMPID(float, position), COMPID(float, velocity));
```

* And step all systems in registry: `eecs::step(reg);`
    * Systems are executed in the order of their registration, there's no way to reorder systems (yet?)

## Projects using eecs
### [Scampi](https://apoly.itch.io/scampi)
![Screenshot of a game scampi, showing a submarine hanging on a barge with an updates screen on the right side and game world on the left side](https://img.itch.zone/aW1hZ2UvMzQ1NzM2MC8yMDYyMzI3Mi5wbmc=/original/h0jhEL.png)
Scampi was rewritten to `eecs` (from `flecs`) as a proof of concept that this library works in practice at
least on a small scale project. Functionaly it uses all current features of `eecs` and essentially was a
testing ground for the library.
