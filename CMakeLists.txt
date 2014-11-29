cmake_minimum_required(VERSION 2.8.4)
project(Goobbue)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Uncomment the following two lines to generate gprof information:
# add_definitions(-pg)
# set(CMAKE_EXE_LINKER_FLAGS "-pg")

set(SOURCE_FILES main.cpp jsonLoader.cpp actionApplication.cpp craftingClass.cpp action.cpp expectimax.cpp crafter.cpp recipe.cpp ThirdParty/jsoncpp.cpp)
add_executable(Goobbue ${SOURCE_FILES})