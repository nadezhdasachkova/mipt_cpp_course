# CMake generated Testfile for 
# Source directory: /home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests
# Build directory: /home/nadezhda/projects/mipt_cpp_course/project/build/strict/task1_1
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[task1.1.phishing_macro]=] "/usr/bin/cmake" "-DEXE=/home/nadezhda/projects/mipt_cpp_course/project/build/strict/nano-edr" "-DLOG=/home/nadezhda/projects/mipt_cpp_course/project/scenarios/phishing_macro.log" "-DEXPECTED=/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/expected/phishing_macro.out" "-P" "/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/run_case.cmake")
set_tests_properties([=[task1.1.phishing_macro]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/CMakeLists.txt;34;add_test;/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/CMakeLists.txt;0;")
add_test([=[task1.1.ransomware]=] "/usr/bin/cmake" "-DEXE=/home/nadezhda/projects/mipt_cpp_course/project/build/strict/nano-edr" "-DLOG=/home/nadezhda/projects/mipt_cpp_course/project/scenarios/ransomware.log" "-DEXPECTED=/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/expected/ransomware.out" "-P" "/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/run_case.cmake")
set_tests_properties([=[task1.1.ransomware]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/CMakeLists.txt;34;add_test;/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/CMakeLists.txt;0;")
add_test([=[task1.1.clean_office]=] "/usr/bin/cmake" "-DEXE=/home/nadezhda/projects/mipt_cpp_course/project/build/strict/nano-edr" "-DLOG=/home/nadezhda/projects/mipt_cpp_course/project/scenarios/clean_office.log" "-DEXPECTED=/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/expected/clean_office.out" "-P" "/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/run_case.cmake")
set_tests_properties([=[task1.1.clean_office]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/CMakeLists.txt;34;add_test;/home/nadezhda/projects/mipt_cpp_course/project/tasks/1.1/tests/CMakeLists.txt;0;")
