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
    src/command_manager.cpp
    src/transaction.cpp
    src/history_stack.cpp
    src/document_event_bus.cpp
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

# CAD-CORE-005: Command Manager tests
add_executable(test_command_manager tests/test_command_manager.cpp)
target_link_libraries(test_command_manager bendcore gtest_main)
add_test(NAME CommandManager COMMAND test_command_manager)

# CAD-CORE-006: Transaction tests
add_executable(test_transaction tests/test_transaction.cpp)
target_link_libraries(test_transaction bendcore gtest_main)
add_test(NAME Transaction COMMAND test_transaction)

# CAD-CORE-007: History Stack tests
add_executable(test_history_stack tests/test_history_stack.cpp)
target_link_libraries(test_history_stack bendcore gtest_main)
add_test(NAME HistoryStack COMMAND test_history_stack)

# CAD-CORE-008: Document Event Bus tests
add_executable(test_document_event_bus tests/test_document_event_bus.cpp)
target_link_libraries(test_document_event_bus bendcore gtest_main)
add_test(NAME DocumentEventBus COMMAND test_document_event_bus)
