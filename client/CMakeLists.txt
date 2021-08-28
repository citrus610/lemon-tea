# CMakeList.txt : CMake project for client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# set the project name
project ("lt-client")

# link sfml statically
set(SFML_STATIC_LIBRARIES TRUE)

## set sfml direcory
# example:
# set(SFML_DIR "F:/sfml-64/SFML-2.5.1/lib/cmake/SFML")
set(SFML_DIR "<insert-your-sfml-dir-here>/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics window audio network system)

# create a sources variable with a link to all cpp files to compile
set(SOURCES
    src/piece.cpp
    src/board.cpp
    src/key.cpp
    src/input.cpp
    src/game.cpp
    src/gamebot.cpp
    src/main.cpp
)

# add source to this project's executable.
add_executable(${PROJECT_NAME} ${SOURCES})

# add static lib
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-audio sfml-network sfml-system )
target_link_libraries(${PROJECT_NAME} lib::bot)

# add include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/inc)

# set target property
# set c++ version to 20 to make use of LZCNT and POPCNT
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
