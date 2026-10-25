cmake_minimum_required(VERSION 3.20)
project(BendCore-CAD-CORE VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

# Disable Taskflow tests to avoid compilation errors
set(TF_BUILD_TESTS OFF CACHE BOOL "Disable Taskflow tests")
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "Disable Taskflow examples")

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
    src/pipe_profile.cpp
    src/pipe_fittings.cpp
    src/constraint_system.cpp
    src/bend_calculator.cpp
    src/format_exporters.cpp
    src/manufacturing.cpp
    src/database_manager.cpp
    # Extended Features (Phase M6)
    src/loft.cpp
    src/sweep.cpp
    src/revolve.cpp
    # Phase M6.1: Surface Modeling Enhancements
    src/geometry/surface_features.cpp
    # Phase M6.5: OCCT Integration
    src/geometry/occt_wrapper.cpp
    # Phase M7: Assembly Solver
    src/assembly/mate_solver.cpp
    # Phase M8: Assembly UI/UX
    src/assembly/assembly_ui.cpp
    # Phase M9: Direct Editing & Parametrics
    src/geometry/direct_editing.cpp
    # Phase M10: Manufacturing & Export
    src/manufacturing/nc_generator.cpp
    # Phase M11: Sheet Metal Operations
    src/geometry/sheet_metal_ops.cpp
    # Phase M12: Advanced Sketcher Tools
    src/sketch/advanced_sketcher.cpp
    # Phase M13: Inspection & Quality Analysis
    src/analysis/inspection_tools.cpp
    # Integration wrappers for external libraries
    src/math/matrix_ops.cpp
    src/math/geometry_ops.cpp
    src/io/model_importer.cpp
    src/parallel/parallel_executor.cpp
    src/serialization/protobuf_serializer.cpp
)

add_library(bendcore STATIC ${BENDCORE_SOURCES})
target_include_directories(bendcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Include Taskflow (header-only, always available from FetchContent)
target_include_directories(bendcore PUBLIC ${taskflow_SOURCE_DIR})

# Optionally link Eigen
if(Eigen3_FOUND)
    target_link_libraries(bendcore PUBLIC Eigen3::Eigen)
    message(STATUS "Linking with Eigen3 (system)")
else()
    message(STATUS "Eigen3 not available - using internal math operations")
endif()

# Optionally link Assimp
if(assimp_FOUND)
    target_link_libraries(bendcore PUBLIC assimp)
    message(STATUS "Linking with Assimp (system)")
else()
    message(STATUS "Assimp not available - model import will be limited")
endif()

# Optionally link Protobuf
if(Protobuf_FOUND)
    target_link_libraries(bendcore PUBLIC protobuf::libprotobuf_lite)
    message(STATUS "Linking with Protobuf (system)")
else()
    message(STATUS "Protobuf not available - using basic serialization")
endif()

# Optionally link CGAL
if(CGAL_FOUND)
    target_link_libraries(bendcore PUBLIC CGAL)
    message(STATUS "Linking with CGAL (system)")
else()
    message(STATUS "CGAL not available - geometry operations will be limited")
endif()

set_target_properties(bendcore PROPERTIES VERSION 0.1.0 SOVERSION 0)

# Dependencies with FetchContent
include(FetchContent)

# GoogleTest (required for tests)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    TIMEOUT 60
)

# Taskflow - Header-only (required for parallel execution)
FetchContent_Declare(
    taskflow
    GIT_REPOSITORY https://github.com/taskflow/taskflow.git
    GIT_TAG v3.6.0
    TIMEOUT 60
    FIND_PACKAGE_ARGS
)

# Optional: Try to use system packages
find_package(Eigen3 3.3 QUIET)
find_package(assimp QUIET)
find_package(Protobuf QUIET)
find_package(CGAL QUIET)

# Always fetch GoogleTest and Taskflow
FetchContent_MakeAvailable(googletest taskflow)

# Fallback: Create interface library for missing dependencies
if(NOT Eigen3_FOUND)
    message(STATUS "Eigen3 not found - will use internal stubs")
    # We'll use internal implementation
    set(Eigen3_FOUND FALSE)
endif()

if(NOT assimp_FOUND)
    message(STATUS "Assimp not found - will use internal stubs")
    # Create dummy target
    add_library(assimp INTERFACE)
    set(assimp_FOUND FALSE)
endif()

if(NOT Protobuf_FOUND)
    message(STATUS "Protobuf not found - will use internal implementation")
    # Create dummy target
    add_library(protobuf INTERFACE)
    set(protobuf_FOUND FALSE)
endif()

if(NOT CGAL_FOUND)
    message(STATUS "CGAL not found - geometry operations will be limited")
    # Create dummy target
    add_library(CGAL INTERFACE)
endif()

# Make Eigen available even if not system package
if(NOT Eigen3_FOUND)
    set(Eigen Eigen)
else()
    set(Eigen Eigen3::Eigen)
endif()

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

# Pipe Components Tests (PIPE-001)
add_executable(test_pipe_components tests/test_pipe_components.cpp)
target_link_libraries(test_pipe_components bendcore gtest_main)
add_test(NAME PipeComponents COMMAND test_pipe_components)

# Bend Calculator Tests (BEND-001)
add_executable(test_bend_calculator tests/test_bend_calculator.cpp)
target_link_libraries(test_bend_calculator bendcore gtest_main)
add_test(NAME BendCalculator COMMAND test_bend_calculator)

# Format Exporters Tests (IO-001)
add_executable(test_format_exporters tests/test_format_exporters.cpp)
target_link_libraries(test_format_exporters bendcore gtest_main)
add_test(NAME FormatExporters COMMAND test_format_exporters)

# Manufacturing Tests (MFG-001)
add_executable(test_manufacturing tests/test_manufacturing.cpp)
target_link_libraries(test_manufacturing bendcore gtest_main)
add_test(NAME Manufacturing COMMAND test_manufacturing)

# Database Manager Tests (DB-001)
add_executable(test_database_manager tests/test_database_manager.cpp)
target_link_libraries(test_database_manager bendcore gtest_main)
add_test(NAME DatabaseManager COMMAND test_database_manager)

# ===== Integration Library Tests =====

# Matrix Operations Tests (EIGEN)
add_executable(test_matrix_ops tests/test_matrix_ops.cpp)
target_link_libraries(test_matrix_ops bendcore gtest_main)
if(Eigen3_FOUND)
    target_link_libraries(test_matrix_ops Eigen3::Eigen)
endif()
add_test(NAME MatrixOps COMMAND test_matrix_ops)

# Geometry Operations Tests (CGAL)
add_executable(test_geometry_ops tests/test_geometry_ops.cpp)
target_link_libraries(test_geometry_ops bendcore gtest_main)
add_test(NAME GeometryOps COMMAND test_geometry_ops)

# Model Importer Tests (ASSIMP)
add_executable(test_model_importer tests/test_model_importer.cpp)
target_link_libraries(test_model_importer bendcore gtest_main)
if(assimp_FOUND)
    target_link_libraries(test_model_importer assimp)
endif()
add_test(NAME ModelImporter COMMAND test_model_importer)

# Parallel Executor Tests (TASKFLOW)
add_executable(test_parallel_executor tests/test_parallel_executor.cpp)
target_link_libraries(test_parallel_executor bendcore gtest_main)
add_test(NAME ParallelExecutor COMMAND test_parallel_executor)

# Protobuf Serializer Tests (PROTOBUF)
add_executable(test_protobuf_serializer tests/test_protobuf_serializer.cpp)
target_link_libraries(test_protobuf_serializer bendcore gtest_main)
if(Protobuf_FOUND)
    target_link_libraries(test_protobuf_serializer protobuf::libprotobuf_lite)
endif()
add_test(NAME ProtobufSerializer COMMAND test_protobuf_serializer)

# ═════════════════════════════════════════════════════════════════
# Phase M6: Extended Features (Loft, Sweep, Revolve)
# ═════════════════════════════════════════════════════════════════

# Loft Feature Tests (M6-001)
add_executable(test_loft_feature tests/test_loft_feature.cpp)
target_link_libraries(test_loft_feature bendcore gtest_main)
add_test(NAME LoftFeature COMMAND test_loft_feature)

# Sweep Feature Tests (M6-002)
add_executable(test_sweep_feature tests/test_sweep_feature.cpp)
target_link_libraries(test_sweep_feature bendcore gtest_main)
add_test(NAME SweepFeature COMMAND test_sweep_feature)

# Revolve Feature Tests (M6-003)
add_executable(test_revolve_feature tests/test_revolve_feature.cpp)
target_link_libraries(test_revolve_feature bendcore gtest_main)
add_test(NAME RevolveFeature COMMAND test_revolve_feature)

# ═════════════════════════════════════════════════════════════════
# Phase M6.1: Surface Modeling Enhancements
# ═════════════════════════════════════════════════════════════════

# Surface Features Tests (CAD-DEV-026 to CAD-DEV-030)
add_executable(test_surface_features tests/test_surface_features.cpp)
target_link_libraries(test_surface_features bendcore gtest_main)
add_test(NAME SurfaceFeatures COMMAND test_surface_features)

# ═════════════════════════════════════════════════════════════════
# Phase M6.5: OCCT Integration
# ═════════════════════════════════════════════════════════════════

# OCCT Wrapper Tests (CAD-DEV-051 to CAD-DEV-030)
add_executable(test_occt_wrapper tests/test_occt_wrapper.cpp)
target_link_libraries(test_occt_wrapper bendcore gtest_main)
add_test(NAME OccWrapper COMMAND test_occt_wrapper)

# ═════════════════════════════════════════════════════════════════
# Phase M7: Assembly Solver
# ═════════════════════════════════════════════════════════════════

# Mate Solver Tests (CAD-DEV-031 to CAD-DEV-040)
add_executable(test_mate_solver tests/test_mate_solver.cpp)
target_link_libraries(test_mate_solver bendcore gtest_main)
add_test(NAME MateSolver COMMAND test_mate_solver)

# ═════════════════════════════════════════════════════════════════
# Phase M8: Assembly UI/UX
# ═════════════════════════════════════════════════════════════════

# Assembly UI Tests (CAD-DEV-043 to CAD-DEV-050)
add_executable(test_assembly_ui tests/test_assembly_ui.cpp)
target_link_libraries(test_assembly_ui bendcore gtest_main)
add_test(NAME AssemblyUI COMMAND test_assembly_ui)

# ═════════════════════════════════════════════════════════════════
# Phase M9: Direct Editing & Parametrics
# ═════════════════════════════════════════════════════════════════

# Direct Editing Tests (CAD-DEV-051 to CAD-DEV-075)
add_executable(test_direct_editing tests/test_direct_editing.cpp)
target_link_libraries(test_direct_editing bendcore gtest_main)
add_test(NAME DirectEditing COMMAND test_direct_editing)

# ═════════════════════════════════════════════════════════════════
# Phase M10: Manufacturing & Export
# ═════════════════════════════════════════════════════════════════

# Manufacturing Tests (CAD-DEV-076 to CAD-DEV-100)
add_executable(test_nc_generator tests/test_nc_generator.cpp)
target_link_libraries(test_nc_generator bendcore gtest_main)
add_test(NAME NCGenerator COMMAND test_nc_generator)

# ═════════════════════════════════════════════════════════════════
# Phase M11: Sheet Metal Operations
# ═════════════════════════════════════════════════════════════════

# Sheet Metal Tests (CAD-DEV-101 to CAD-DEV-125)
add_executable(test_sheet_metal_ops tests/test_sheet_metal_ops.cpp)
target_link_libraries(test_sheet_metal_ops bendcore gtest_main)
add_test(NAME SheetMetalOps COMMAND test_sheet_metal_ops)

# ═════════════════════════════════════════════════════════════════
# Phase M12: Advanced Sketcher Tools
# ═════════════════════════════════════════════════════════════════

# Advanced Sketcher Tests (CAD-DEV-126 to CAD-DEV-150)
add_executable(test_advanced_sketcher tests/test_advanced_sketcher.cpp)
target_link_libraries(test_advanced_sketcher bendcore gtest_main)
add_test(NAME AdvancedSketcher COMMAND test_advanced_sketcher)

# ═════════════════════════════════════════════════════════════════
# Phase M13: Inspection & Quality Analysis
# ═════════════════════════════════════════════════════════════════

# Inspection Tests (CAD-DEV-151 to CAD-DEV-175)
add_executable(test_inspection_tools tests/test_inspection_tools.cpp)
target_link_libraries(test_inspection_tools bendcore gtest_main)
add_test(NAME InspectionTools COMMAND test_inspection_tools)
