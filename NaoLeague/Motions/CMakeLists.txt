cmake_minimum_required(VERSION 2.8)
project(Motions)

find_package(qibuild)

# Create a executable named Motions
# with the source file: main.cpp
qi_create_bin(Motions "Motions.cpp")

qi_use_lib(Motions ALCOMMON)
