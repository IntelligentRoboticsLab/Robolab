cmake_minimum_required(VERSION 2.8)
project(GameController)

find_package(qibuild)

# Create a executable named NaoLeague
# with the source file: main.cpp
qi_create_bin(GameController "main.cpp")
