# CMake generated Testfile for 
# Source directory: /home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests
# Build directory: /home/nadezhda/mipt/mipt_cpp_course/build/asan/task1_1
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[task1.1.phishing_macro]=] "/usr/bin/cmake" "-DEXE=/home/nadezhda/mipt/mipt_cpp_course/build/asan/nano-edr" "-DLOG=/home/nadezhda/mipt/mipt_cpp_course/scenarios/phishing_macro.log" "-DEXPECTED=/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/expected/phishing_macro.out" "-P" "/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/run_case.cmake")
set_tests_properties([=[task1.1.phishing_macro]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/CMakeLists.txt;34;add_test;/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/CMakeLists.txt;0;")
add_test([=[task1.1.ransomware]=] "/usr/bin/cmake" "-DEXE=/home/nadezhda/mipt/mipt_cpp_course/build/asan/nano-edr" "-DLOG=/home/nadezhda/mipt/mipt_cpp_course/scenarios/ransomware.log" "-DEXPECTED=/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/expected/ransomware.out" "-P" "/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/run_case.cmake")
set_tests_properties([=[task1.1.ransomware]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/CMakeLists.txt;34;add_test;/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/CMakeLists.txt;0;")
add_test([=[task1.1.clean_office]=] "/usr/bin/cmake" "-DEXE=/home/nadezhda/mipt/mipt_cpp_course/build/asan/nano-edr" "-DLOG=/home/nadezhda/mipt/mipt_cpp_course/scenarios/clean_office.log" "-DEXPECTED=/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/expected/clean_office.out" "-P" "/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/run_case.cmake")
set_tests_properties([=[task1.1.clean_office]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/CMakeLists.txt;34;add_test;/home/nadezhda/mipt/mipt_cpp_course/tasks/1.1/tests/CMakeLists.txt;0;")
