cmake_minimum_required(VERSION 2.8)
project(NaoLeague)

find_package(qibuild)

# Create a executable named NaoLeague
# with the source file: main.cpp
qi_create_bin(NaoLeague "main.cpp")

# Add a simple test:
enable_testing()
qi_create_test(test_NaoLeague "test.cpp")
