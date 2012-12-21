cmake_minimum_required(VERSION 2.8)
project(MotionSkeleton)

find_package(qibuild)

# Create a executable named MotionSkeleton
# with the source file: main.cpp
qi_create_bin(MotionSkeleton "main.cpp")

# link with the ALCOMMON library
qi_use_lib(MotionSkeleton ALCOMMON)
