# CMake generated Testfile for 
# Source directory: /tmp/bendcore-cadcore
# Build directory: /tmp/bendcore-cadcore/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[EntityId]=] "/tmp/bendcore-cadcore/build/test_entity_id")
set_tests_properties([=[EntityId]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;43;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[CadDocument]=] "/tmp/bendcore-cadcore/build/test_cad_document")
set_tests_properties([=[CadDocument]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;48;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[EntityTree]=] "/tmp/bendcore-cadcore/build/test_entity_tree")
set_tests_properties([=[EntityTree]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;53;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[CommandManager]=] "/tmp/bendcore-cadcore/build/test_command_manager")
set_tests_properties([=[CommandManager]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;58;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[Transaction]=] "/tmp/bendcore-cadcore/build/test_transaction")
set_tests_properties([=[Transaction]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;63;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[HistoryStack]=] "/tmp/bendcore-cadcore/build/test_history_stack")
set_tests_properties([=[HistoryStack]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;68;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[DocumentEventBus]=] "/tmp/bendcore-cadcore/build/test_document_event_bus")
set_tests_properties([=[DocumentEventBus]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;73;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[ParameterStore]=] "/tmp/bendcore-cadcore/build/test_parameter_store")
set_tests_properties([=[ParameterStore]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;78;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[DependencyGraph]=] "/tmp/bendcore-cadcore/build/test_dependency_graph")
set_tests_properties([=[DependencyGraph]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;83;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[RebuildEngine]=] "/tmp/bendcore-cadcore/build/test_rebuild_engine")
set_tests_properties([=[RebuildEngine]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;88;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
