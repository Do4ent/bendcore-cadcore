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
    src/parameter_store.cpp
    src/dependency_graph.cpp
    src/rebuild_engine.cpp
    src/geometry_engine.cpp
    src/brep.cpp
    src/feature_system.cpp
    src/geometry_cache.cpp
    src/project_format.cpp
    src/serializer.cpp
    src/atomic_save.cpp
    src/recovery.cpp
    src/legacy_adapter.cpp
    src/bezier_curve.cpp
    src/boolean_operations.cpp
    src/mesh_generator.cpp
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

# CAD-CORE-009: Parameter Store tests
add_executable(test_parameter_store tests/test_parameter_store.cpp)
target_link_libraries(test_parameter_store bendcore gtest_main)
add_test(NAME ParameterStore COMMAND test_parameter_store)

# CAD-CORE-010: Dependency Graph tests
add_executable(test_dependency_graph tests/test_dependency_graph.cpp)
target_link_libraries(test_dependency_graph bendcore gtest_main)
add_test(NAME DependencyGraph COMMAND test_dependency_graph)

# CAD-CORE-011: Rebuild Engine tests
add_executable(test_rebuild_engine tests/test_rebuild_engine.cpp)
target_link_libraries(test_rebuild_engine bendcore gtest_main)
add_test(NAME RebuildEngine COMMAND test_rebuild_engine)

# CAD-CORE-012: Geometry Engine tests
add_executable(test_geometry_engine tests/test_geometry_engine.cpp)
target_link_libraries(test_geometry_engine bendcore gtest_main)
add_test(NAME GeometryEngine COMMAND test_geometry_engine)

# CAD-CORE-013: B-Rep tests
add_executable(test_brep tests/test_brep.cpp)
target_link_libraries(test_brep bendcore gtest_main)
add_test(NAME BRep COMMAND test_brep)

# CAD-CORE-015: Feature System tests
add_executable(test_feature_system tests/test_feature_system.cpp)
target_link_libraries(test_feature_system bendcore gtest_main)
add_test(NAME FeatureSystem COMMAND test_feature_system)

# CAD-CORE-016: Geometry Cache tests
add_executable(test_geometry_cache tests/test_geometry_cache.cpp)
target_link_libraries(test_geometry_cache bendcore gtest_main)
add_test(NAME GeometryCache COMMAND test_geometry_cache)

# CAD-CORE-017: Project Format tests
add_executable(test_project_format tests/test_project_format.cpp)
target_link_libraries(test_project_format bendcore gtest_main)
add_test(NAME ProjectFormat COMMAND test_project_format)

# CAD-CORE-018: Serialization tests
add_executable(test_serializer tests/test_serializer.cpp)
target_link_libraries(test_serializer bendcore gtest_main)
add_test(NAME Serializer COMMAND test_serializer)

# CAD-CORE-019: Atomic Save tests
add_executable(test_atomic_save tests/test_atomic_save.cpp)
target_link_libraries(test_atomic_save bendcore gtest_main)
add_test(NAME AtomicSave COMMAND test_atomic_save)

# CAD-CORE-020: Recovery tests
add_executable(test_recovery tests/test_recovery.cpp)
target_link_libraries(test_recovery bendcore gtest_main)
add_test(NAME Recovery COMMAND test_recovery)

# CAD-CORE-021: Legacy Adapter tests
add_executable(test_legacy_adapter tests/test_legacy_adapter.cpp)
target_link_libraries(test_legacy_adapter bendcore gtest_main)
add_test(NAME LegacyAdapter COMMAND test_legacy_adapter)

# CAD-CORE-024: E2E Tests
add_executable(test_e2e tests/test_e2e.cpp)
target_link_libraries(test_e2e bendcore gtest_main)
add_test(NAME E2E COMMAND test_e2e)

# CAD-CORE-025: Acceptance Tests
add_executable(test_acceptance tests/test_acceptance.cpp)
target_link_libraries(test_acceptance bendcore gtest_main)
add_test(NAME Acceptance COMMAND test_acceptance)

# Advanced Geometry Features (CAD-CORE-026, CAD-CORE-027)
# Tests are placeholder implementations for future development
