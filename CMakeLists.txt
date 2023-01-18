cmake_minimum_required(VERSION 3.13)
project(bennugd_libretro)

string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-int-to-pointer-cast")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}/lib")



add_subdirectory(core)
add_subdirectory(modules)
