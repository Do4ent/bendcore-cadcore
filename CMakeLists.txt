cmake_minimum_required(VERSION 3.20)
project(BendCore-CAD-CORE VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Core library sources
set(BENDCORE_SOURCES
    src/entity_id.cpp
    src/cad_document.cpp
    src/entity_tree.cpp
)

add_library(bendcore STATIC ${BENDCORE_SOURCES})
target_include_directories(bendcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(bendcore PROPERTIES VERSION 0.1.0 SOVERSION 0)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# CAD-CORE-002: Entity ID tests
add_executable(test_entity_id tests/test_entity_id.cpp)
target_link_libraries(test_entity_id bendcore gtest_main)
add_test(NAME EntityId COMMAND test_entity_id)

# CAD-CORE-003: CAD Document tests
add_executable(test_cad_document tests/test_cad_document.cpp)
target_link_libraries(test_cad_document bendcore gtest_main)
add_test(NAME CadDocument COMMAND test_cad_document)

# CAD-CORE-004: Entity Tree tests
add_executable(test_entity_tree tests/test_entity_tree.cpp)
target_link_libraries(test_entity_tree bendcore gtest_main)
add_test(NAME EntityTree COMMAND test_entity_tree)
