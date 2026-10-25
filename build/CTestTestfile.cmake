# CMake generated Testfile for 
# Source directory: /tmp/bendcore-cadcore
# Build directory: /tmp/bendcore-cadcore/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[EntityId]=] "/tmp/bendcore-cadcore/build/test_entity_id")
set_tests_properties([=[EntityId]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;40;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[CadDocument]=] "/tmp/bendcore-cadcore/build/test_cad_document")
set_tests_properties([=[CadDocument]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;45;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[EntityTree]=] "/tmp/bendcore-cadcore/build/test_entity_tree")
set_tests_properties([=[EntityTree]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;50;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[CommandManager]=] "/tmp/bendcore-cadcore/build/test_command_manager")
set_tests_properties([=[CommandManager]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;55;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[Transaction]=] "/tmp/bendcore-cadcore/build/test_transaction")
set_tests_properties([=[Transaction]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;60;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[HistoryStack]=] "/tmp/bendcore-cadcore/build/test_history_stack")
set_tests_properties([=[HistoryStack]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;65;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[DocumentEventBus]=] "/tmp/bendcore-cadcore/build/test_document_event_bus")
set_tests_properties([=[DocumentEventBus]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;70;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
