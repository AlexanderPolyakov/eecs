project(eecs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_executable(sample_create_entity samples/create_entity.cpp)
add_executable(sample_query_component samples/query_component.cpp)
add_executable(sample_query_multiple_component samples/query_multiple_components.cpp)
add_executable(sample_query_entities samples/query_entities.cpp)
add_executable(sample_basic_system samples/basic_system.cpp)

add_library(eecs INTERFACE)
target_include_directories(eecs INTERFACE include)
