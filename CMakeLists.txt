cmake_minimum_required(VERSION 3.13)
project(eecs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(eecs INTERFACE)
target_include_directories(eecs INTERFACE include)
if(MSVC)
    target_compile_options(eecs INTERFACE /Zc:preprocessor)
endif()

add_executable(sample_create_entity samples/create_entity.cpp)
add_executable(sample_query_component samples/query_component.cpp)
add_executable(sample_query_multiple_component samples/query_multiple_components.cpp)
add_executable(sample_query_entities samples/query_entities.cpp)
add_executable(sample_basic_system samples/basic_system.cpp)
add_executable(sample_prefabs samples/prefabs.cpp)
add_executable(sample_perf samples/perf.cpp)

foreach(sample IN ITEMS
    sample_create_entity
    sample_query_component
    sample_query_multiple_component
    sample_query_entities
    sample_basic_system
    sample_prefabs
    sample_perf)
    target_link_libraries(${sample} PRIVATE eecs)
endforeach()
