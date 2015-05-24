cmake_minimum_required(VERSION 3.1)
project(c11NHF)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libgcc")

link_directories("C:\\Link\\programozas\\C++\\Clion\\c11NHF\\SDL2\\i686-w64-mingw32\\lib")
include_directories("C:\\Link\\programozas\\C++\\Clion\\c11NHF\\SDL2\\i686-w64-mingw32\\include")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:\\Link\\programozas\\C++\\Clion\\c11NHF\\out")
set(SOURCE_FILES main.cpp)
add_executable(c11NHF ${SOURCE_FILES} Expressions.h Expressions.cpp )

target_link_libraries(c11NHF stdc++ mingw32 SDL2main SDL2 stdc++)