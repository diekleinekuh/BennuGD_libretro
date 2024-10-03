cmake_minimum_required(VERSION 3.22...3.24)
project(bennugd_libretro C)
set (CMAKE_C_STANDARD 11)

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

function(disable_all_warnings target)
    # Simply adding "/w" causes D9025 because /w overrides any /W1, /W2, /W3 or /W4 the target might have defined.
    # Some targets add those in CMAKE_C_FLAGS which makes it hard to override.
    # Adding the disabled warnings to COMPILE_OPTIONS applies them also to assembler files 
    # where they are not supported causing new warnings. 
    # So this disables warnings individually :-/
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(disabled_warnings "")
    elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")
        set(disabled_warnings "-Wno-attributes;-Wno-stringop-truncation")
    elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
        set(disabled_warnings "/wd4132;/wd4189;/wd4206;/wd4210;/wd4295;/wd4701;/wd4702;/wd4334;/wd4244")
    endif()
    get_target_property(target_source_dir ${target} SOURCE_DIR)
    cmake_path(SET target_path ${target_source_dir})
    get_target_property(target_sources ${target} SOURCES)
    foreach( src IN LISTS target_sources)
        cmake_path(APPEND target_path ${src} OUTPUT_VARIABLE src_path)
        cmake_path(GET src_path EXTENSION LAST_ONLY src_ext)
        if(src_ext MATCHES "^\.[Cc]$")
            get_source_file_property(src_compile_options ${src_path} COMPILE_OPTIONS)
            if (NOT src_compile_options)
                set(src_compile_options)
            endif()
            list(APPEND src_compile_options ${disabled_warnings})
            set_source_files_properties( "${src_path}" TARGET_DIRECTORY ${target} PROPERTIES COMPILE_OPTIONS "${src_compile_options}" )
        endif()
    endforeach()
endfunction()

if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")    
    check_linker_flag("C" "-Wl,--no-undefined" HAS_NO_UNDEFINED)
    if (HAS_NO_UNDEFINED)
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")
    endif()

    check_linker_flag("C" "-Wl,-Bsymbolic-functions" HAS_SYMBOLIC_FUNCTIONS)
    if (HAS_SYMBOLIC_FUNCTIONS)
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,-Bsymbolic-functions")
    endif()
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
    URL https://github.com/libressl/portable/releases/download/v3.9.2/libressl-3.9.2.tar.gz
)
FetchContent_MakeAvailable(libressl)

disable_all_warnings(crypto_obj)
disable_all_warnings(compat_obj)
disable_all_warnings(crypto)
disable_all_warnings(ssl_obj)
disable_all_warnings(bs_obj)
disable_all_warnings(ssl)
disable_all_warnings(tls_obj)
disable_all_warnings(tls_compat_obj)
disable_all_warnings(tls)
set(OPENSSL_CRYPTO_LIBRARY crypto)

# zlib
set(ZLIB_ENABLE_TESTS OFF CACHE BOOL "Build test binaries")
set(ZLIB_COMPAT ON CACHE BOOL "Compile with zlib compatible API" FORCE)
FetchContent_Declare(zlib-ng
    GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
    GIT_TAG        2.2.1
    GIT_SHALLOW TRUE    
)
FetchContent_MakeAvailable(zlib-ng)
disable_all_warnings(zlib)

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
