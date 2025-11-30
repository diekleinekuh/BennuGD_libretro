cmake_minimum_required(VERSION 3.14...3.24)
project(bennugd_libretro C)
set (CMAKE_C_STANDARD 11)

option(NO_SYSTEM_DEPENDENCIES "Don't search dependencies but build them locally" ON)
option(libretro_core "build libretro_core instead of standalone version" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}/lib")

if (NOT BUILD_SHARED_LIBS)
    add_compile_definitions(__STATIC__=1)
endif()

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.18.0")
    include(CheckLinkerFlag)
endif()

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19.0")
    include(CheckCompilerFlag)
endif()

function(disable_all_warnings target)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.20.0")
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
    endif()
endfunction()

if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    if (UNIX AND NOT APPLE)
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,-Bsymbolic")
    endif()

    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.18.0")
        check_linker_flag("C" "-Wl,--no-undefined" HAS_NO_UNDEFINED)
        if (HAS_NO_UNDEFINED)
            string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--no-undefined")
        endif()
    endif()
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "compile as PIC by default")


if (CMAKE_TOOLCHAIN_FILE MATCHES "vita.toolchain.cmake")
    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
    set(VITA TRUE)
endif()

include(FetchContent)

# turn off building shared libraries for dependencies
set(BUILD_SHARED_LIBS_backup "$CACHE{BUILD_SHARED_LIBS}")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)

# libressl
if (NOT NO_SYSTEM_DEPENDENCIES)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    find_package(OpenSSL)
    find_package(ZLIB)
    find_package(PNG)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake-bundle_static_library")

if (NO_SYSTEM_DEPENDENCIES OR NOT OpenSSL_FOUND)

    if (USE_WOLFSSL)
        set (WOLFSSL_EXAMPLES "no")
        set (WOLFSSL_CRYPT_TESTS "no")
        set (WOLFSSL_DES3 "yes")
        set (WOLFSSL_OPENSSLEXTRA "yes")
        set(WOLFSSL_CURL "yes")
        set(WOLFSSL_FILESYSTEM "yes")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_SCECLIB=1")

        FetchContent_Declare( wolfssl
            URL https://github.com/wolfSSL/wolfssl/archive/refs/tags/v5.8.2-stable.tar.gz
            URL_HASH SHA256=3ef126e3466e2f8f6ebb62b916a7f8fb26c6709dbdf2b63a167759f2fdb53068
        )    
        FetchContent_MakeAvailable(wolfssl)    
        set(OPENSSL_CRYPTO_LIBRARY wolfssl::wolfssl)
        target_compile_definitions(wolfssl PUBLIC USE_WOLFSSL)
        target_compile_definitions(wolfssl PRIVATE NO_WRITEV)
        check_compiler_flag(C -Wno-unused-parameter HAVE_NO_WUNUSED_PARAMETER)
        if (HAVE_NO_WUNUSED_PARAMETER) 
            target_compile_options(wolfssl PRIVATE "-Wno-unused-parameter")
        endif()
        message("using internal wolfssl")
    else()
        set (LIBRESSL_APPS OFF CACHE BOOL "Build apps" FORCE)
        set (LIBRESSL_TESTS OFF CACHE BOOL "Build tests" FORCE)
        set (ENABLE_ASM OFF CACHE BOOL "Enable assembly" FORCE)
        FetchContent_Declare( libressl 
            URL https://github.com/libressl/portable/releases/download/v3.9.2/libressl-3.9.2.tar.gz
            URL_HASH SHA256=7b031dac64a59eb6ee3304f7ffb75dad33ab8c9d279c847f92c89fb846068f97
            EXCLUDE_FROM_ALL
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
        message("using internal libcrypto")
    endif()
else()
    set(OPENSSL_CRYPTO_LIBRARY OpenSSL::Crypto)
endif()

# zlib
if (NO_SYSTEM_DEPENDENCIES OR NOT ZLIB_FOUND)
    set(ZLIB_ENABLE_TESTS OFF CACHE BOOL "Build test binaries" FORCE)
    set(ZLIB_COMPAT ON CACHE BOOL "Compile with zlib compatible API" FORCE)
    set(SKIP_INSTALL_ALL TRUE)
    FetchContent_Declare(zlib
        GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
        GIT_TAG        cbb6ec1d74e8061efdf7251f8c2dae778bed14fd # 2.2.3
        EXCLUDE_FROM_ALL
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(zlib)

    add_library(zlib_interface INTERFACE)
    target_link_libraries(zlib_interface INTERFACE zlib)
    target_include_directories(zlib_interface INTERFACE ${zlib_BINARY_DIR})
    add_library(ZLIB::ZLIB ALIAS zlib_interface)

    set(ZLIB_LIBRARY ZLIB::ZLIB)
    set(ZLIB_LIBRARIES ZLIB::ZLIB)
    set(ZLIB_INCLUDE_DIRS ${zlib_BINARY_DIR})
    message("using internal zlib")
endif()

# libpng
if (NO_SYSTEM_DEPENDENCIES OR NOT PNG_FOUND)
    set(PNG_SHARED OFF CACHE BOOL "Build libpng as a shared library" FORCE)
    set(PNG_TESTS OFF CACHE BOOL "Build the libpng tests" FORCE)
    set(PNG_TOOLS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare( libpng 
        GIT_REPOSITORY https://github.com/pnggroup/libpng.git
        GIT_TAG 51f5bd68b9b806d2c92b4318164d28b49357da31 # 1.6.45
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(libpng)
    set (PNG_LIBRARIES png_static)
    add_library(mypng INTERFACE)
    target_link_libraries(mypng INTERFACE png_static)
    target_include_directories(mypng INTERFACE "${libpng_BINARY_DIR};${libpng_SOURCE_DIR}")
    add_library(PNG::PNG ALIAS mypng)
    message("using internal libpng")
else()
    set (PNG_LIBRARIES png)
endif()

# restore state of BUILD_SHARED_LIBS
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_backup} CACHE INTERNAL "" FORCE)

if(libretro_core)
    add_compile_definitions(LIBRETRO_CORE=1)

    FetchContent_Declare(
    libretro-common
    GIT_REPOSITORY https://github.com/libretro/libretro-common.git
    GIT_TAG        0abedaac6a795c093f2e1a22f3028fca9efdf3c9
    CONFIGURE_COMMAND " "
    BUILD_COMMAND " "
    INSTALL_COMMAND " "
    )

    FetchContent_MakeAvailable(  libretro-common )

    include_directories(${libretro-common_SOURCE_DIR}/include)
    add_subdirectory(libretro/sdl-libretro)
    add_subdirectory(libretro/sdl_mixer-libretro)
    set(SDL_MIXER_LIBRARIES "sdl_mixer-libretro")
    add_library(SDL::SDL ALIAS sdl-libretro)

else()
    set(SDL_BUILDING_LIBRARY TRUE)
    find_package(SDL 1.2 REQUIRED)
    find_package(SDL_mixer REQUIRED)
    if (NOT TARGET SDL::SDL) # The modules was changed in cmake 3.19
        add_library(sdl_interface INTERFACE)
        target_link_libraries(sdl_interface INTERFACE "${SDL_LIBRARY}")
        target_include_directories(sdl_interface INTERFACE "${SDL_INCLUDE_DIR}")
        add_library(SDL::SDL ALIAS sdl_interface)
    endif()
    set (SDL_LIBRARIES SDL::SDL)
endif()

add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(core/bgdi)

if(libretro_core)
    add_subdirectory(libretro)
endif()