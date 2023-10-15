cmake_minimum_required(VERSION 3.27.0)
project(TinyDB VERSION 1.0.0 LANGUAGES CXX)
set(LIBRARY_OUTPUT_PATH lib)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -march=native -mtune=native")
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  ITTAPI
  URL https://github.com/intel/ittapi/archive/refs/tags/v3.24.2.zip
)
FetchContent_MakeAvailable(ITTAPI)

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)

add_executable(TinyDB TinyDB.cpp)

target_link_libraries(TinyDB REPL ittnotify)

install(TARGETS ${PROJECT_NAME}
        DESTINATION ./bin
        COMPONENT ${PROJECT_NAME}
        CONFIGURATIONS Release)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/TinyDB.cpp
        DESTINATION .
        COMPONENT ${PROJECT_NAME}
        CONFIGURATIONS Release)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        DESTINATION .
        COMPONENT ${PROJECT_NAME}
        CONFIGURATIONS Release)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION .
        COMPONENT ${PROJECT_NAME}
        CONFIGURATIONS Release)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        DESTINATION .
        COMPONENT ${PROJECT_NAME}
        CONFIGURATIONS Release)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
        DESTINATION .
        COMPONENT ${PROJECT_NAME}
        CONFIGURATIONS Release)

set(CPACK_LIB_INSTALL_DIR "lib/")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_GENERATOR "ZIP")
include(CPack)