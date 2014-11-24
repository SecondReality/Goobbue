cmake_minimum_required(VERSION 2.8.4)
project(Goobbue)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp ThirdParty/jsoncpp.cpp)
add_executable(Goobbue ${SOURCE_FILES})