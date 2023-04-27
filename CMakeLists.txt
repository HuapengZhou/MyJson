cmake_minimum_required(VERSION 3.19)
project(building-and-querying-models)
set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp CommandProcessor.cpp JSONParser.cpp Model.cpp)