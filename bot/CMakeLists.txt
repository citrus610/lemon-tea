# CMakeList.txt : CMake project for bot, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# set the project name
project ("bot")

# create a sources variable with a link to all cpp files to compile
set(SOURCES
    src/bitboard.cpp
    src/eval.cpp
    src/genmove.cpp
    src/pathfinder.cpp
    src/tree.cpp
    src/bot.cpp
)

# add a library with the above sources
add_library(${PROJECT_NAME} ${SOURCES})
add_library(lib::bot ALIAS ${PROJECT_NAME})

# add include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/inc)

# set target property
# set c++ version to 20 to make use of LZCNT and POPCNT
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)