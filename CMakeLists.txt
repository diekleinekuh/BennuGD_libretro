cmake_minimum_required(VERSION 3.24)
project(bennugd_libretro)

option(libretro_core "build libretro_core instead of standalone version" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}/lib")

if (NOT BUILD_SHARED_LIBS)
    add_compile_definitions(__STATIC__=1)
endif()

include(CheckLinkerFlag)
include(CheckCCompilerFlag)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    check_linker_flag("C" "-Wl,--no-undefined" HAS_NO_UNDEFINED)
    if (HAS_NO_UNDEFINED)
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")
    endif()

    check_linker_flag("C" "-Wl,-Bsymbolic-functions" HAS_SYMBOLIC_FUNCTIONS)
    if (HAS_SYMBOLIC_FUNCTIONS)
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,-Bsymbolic-functions")
    endif()   
endif()

check_c_compiler_flag("-Wno-int-to-pointer-cast" HAS_NO_INT_TO_POINTER_CAST)
if (HAS_NO_INT_TO_POINTER_CAST)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-int-to-pointer-cast")
endif()

check_c_compiler_flag("-Wno-int-to-pointer-cast" HAS_NO_POINTER_TO_INT_CAST)
if (HAS_NO_POINTER_TO_INT_CAST)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-to-int-cast")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "compile as PIC by default")


include(FetchContent)

# turn off building shared libraries for dependencies
set(BUILD_SHARED_LIBS_backup "$CACHE{BUILD_SHARED_LIBS}")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)

# libressl
set (LIBRESSL_APPS OFF CACHE BOOL "Build apps" FORCE)
set (LIBRESSL_TESTS OFF CACHE BOOL "Build tests" FORCE)
FetchContent_Declare( libressl 
    URL https://github.com/libressl/portable/releases/download/v3.9.1/libressl-3.9.1.tar.gz
)
FetchContent_MakeAvailable(libressl)
set(OPENSSL_CRYPTO_LIBRARY crypto)

# zlib
set(ZLIB_ENABLE_TESTS OFF CACHE BOOL "Build test binaries")
set(ZLIB_COMPAT ON CACHE BOOL "Compile with zlib compatible API" FORCE)
FetchContent_Declare(zlib-ng
    GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
    GIT_TAG        2.1.6
    GIT_SHALLOW TRUE    
)
FetchContent_MakeAvailable(zlib-ng)

# libpng
set(PNG_SHARED OFF CACHE BOOL "Build libpng as a shared library" FORCE)
set(PNG_TESTS OFF CACHE BOOL "Build the libpng tests" FORCE)
set(PNG_TOOLS OFF CACHE BOOL "" FORCE)
set(ZLIB_LIBRARY ZLIB::ZLIB CACHE STRING "Path to zlib library" FORCE)
set(ZLIB_INCLUDE_DIR ${zlib-ng_BINARY_DIR} CACHE STRING "Include directory for zlib" FORCE)
add_library(ZLIB::ZLIB ALIAS zlib)

FetchContent_Declare( libpng 
    GIT_REPOSITORY https://github.com/pnggroup/libpng.git
    GIT_TAG v1.6.43
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libpng)
set (PNG_LIBRARIES png_static)
add_library(mypng INTERFACE)
target_link_libraries(mypng INTERFACE png_static)
target_include_directories(mypng INTERFACE "${libpng_BINARY_DIR};${libpng_SOURCE_DIR}")
add_library(PNG::PNG ALIAS mypng)

# restore state of BUILD_SHARED_LIBS
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_backup} CACHE INTERNAL "" FORCE)

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
