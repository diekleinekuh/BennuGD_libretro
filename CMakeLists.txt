cmake_minimum_required(VERSION 3.13)
project(bennugd_libretro)

option(libretro_core "build libretro_core instead of standalone version" true)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}/lib")

if (NOT BUILD_SHARED_LIBS)
    add_compile_definitions(__STATIC__=1)
endif()

string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined -Wl,-Bsymbolic-functions")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-deprecated-declarations")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(libretro_core)
    add_compile_definitions(LIBRETRO_CORE=1)
    add_subdirectory(libretro)
else()
    set(SDL_BUILDING_LIBRARY TRUE)
    find_package(SDL 1.2 REQUIRED)
    find_package(SDL_mixer REQUIRED)
endif()

add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(core/bgdi)
