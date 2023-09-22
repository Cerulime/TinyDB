cmake_minimum_required(VERSION 3.27.0)
project(TinyDB VERSION 0.1.0 LANGUAGES CXX)
set(LIBRARY_OUTPUT_PATH lib)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(CTest)
enable_testing()

include_directories(include)
add_subdirectory(src)

add_executable(TinyDB TinyDB.cpp)

target_link_libraries(TinyDB HashTable REPL)

#include(CPack)
