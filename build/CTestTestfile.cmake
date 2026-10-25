# CMake generated Testfile for 
# Source directory: /tmp/bendcore-cadcore
# Build directory: /tmp/bendcore-cadcore/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[EntityId]=] "/tmp/bendcore-cadcore/build/test_entity_id")
set_tests_properties([=[EntityId]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;37;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[CadDocument]=] "/tmp/bendcore-cadcore/build/test_cad_document")
set_tests_properties([=[CadDocument]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;42;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[EntityTree]=] "/tmp/bendcore-cadcore/build/test_entity_tree")
set_tests_properties([=[EntityTree]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;47;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
add_test([=[CommandManager]=] "/tmp/bendcore-cadcore/build/test_command_manager")
set_tests_properties([=[CommandManager]=] PROPERTIES  _BACKTRACE_TRIPLES "/tmp/bendcore-cadcore/CMakeLists.txt;52;add_test;/tmp/bendcore-cadcore/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
