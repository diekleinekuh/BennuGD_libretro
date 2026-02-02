cmake_minimum_required(VERSION 3.14...3.31)
include(FetchContent)
include(CheckSymbolExists)
include(ExternalProject)
include(bundle_static_library)

GET_PROPERTY(TARGET_SUPPORTS_SHARED_LIBS_PROPERTY GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)

if (${TARGET_SUPPORTS_SHARED_LIBS_PROPERTY})
    set( CORE_LIBRARY_TYPE SHARED)
else()
    set( CORE_LIBRARY_TYPE STATIC)
endif()
 

add_library(bennugd_libretro ${CORE_LIBRARY_TYPE}
    ${libretro-common_SOURCE_DIR}/libco/libco.c
    ${libretro-common_SOURCE_DIR}/vfs/vfs_implementation.c
    ${libretro-common_SOURCE_DIR}/file/file_path.c
    ${libretro-common_SOURCE_DIR}/file/file_path_io.c
    ${libretro-common_SOURCE_DIR}/file/retro_dirent.c
    ${libretro-common_SOURCE_DIR}/string/stdstring.c
    ${libretro-common_SOURCE_DIR}/encodings/encoding_utf.c
    ${libretro-common_SOURCE_DIR}/time/rtime.c
    ${libretro-common_SOURCE_DIR}/compat/compat_strl.c
    ${libretro-common_SOURCE_DIR}/compat/compat_strldup.c
    ${libretro-common_SOURCE_DIR}/streams/file_stream.c
    ${libretro-common_SOURCE_DIR}/streams/trans_stream_zlib.c
    ${libretro-common_SOURCE_DIR}/streams/file_stream_transforms.c
    ${libretro-common_SOURCE_DIR}/compat/compat_posix_string.c
    ${libretro-common_SOURCE_DIR}/compat/fopen_utf8.c
    ${libretro-common_SOURCE_DIR}/rthreads/rthreads.c
    libretro.c
    filesystem.c
    filestream_gzip.c
)

# libco expects HAVE_POSIX_MEMALIGN to be defined
check_symbol_exists(posix_memalign stdlib.h BENNUGD_LIBRETRO_HAVE_POSIX_MEMALIGN)
if (BENNUGD_LIBRETRO_HAVE_POSIX_MEMALIGN)
    target_compile_definitions(bennugd_libretro PRIVATE HAVE_POSIX_MEMALIGN=1 )
else()
    target_compile_definitions(bennugd_libretro PRIVATE HAVE_POSIX_MEMALIGN=0 )
endif()

if (CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch")
    target_compile_definitions(bennugd_libretro PRIVATE SWITCH=1 )
endif()

if (VITA)
    target_compile_definitions(bennugd_libretro PRIVATE VITA=1 )
endif()

target_include_directories(bennugd_libretro PRIVATE "${libretro-common_SOURCE_DIR}/include")

if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set_source_files_properties(${libretro-common_SOURCE_DIR}/libco/libco.c PROPERTIES COMPILE_OPTIONS "/wd4005")
endif()

target_include_directories(bennugd_libretro PRIVATE 
    ${libretro-common_SOURCE_DIR}/include
)

set_target_properties(bennugd_libretro PROPERTIES PREFIX "")
set_target_properties(bennugd_libretro PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(bennugd_libretro PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(bennugd_libretro PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(bennugd_libretro PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

target_link_libraries(bennugd_libretro bgdi)

if (CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch")

    bundle_static_library(bennugd_libretro bennugd_libretro_merged)

    ExternalProject_Add( retroarch
        GIT_REPOSITORY https://github.com/diekleinekuh/RetroArch.git
        GIT_TAG switch_audio_drivers_initially_blocking
        CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE TRUE
        BUILD_ALWAYS TRUE
        BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Starting retroarch build"
        COMMAND cp "$<TARGET_FILE:bennugd_libretro_merged>" libretro_libnx.a
        COMMAND touch runloop.c # Force a relink 
        COMMAND make NO_ICON=1 -j -f Makefile.libnx
        COMMAND cp retroarch_switch.nro ${CMAKE_BINARY_DIR}/bennugd_libretro_libnx.nro
        INSTALL_COMMAND ""
        DEPENDS bennugd_libretro_merged
    )
endif()

if (VITA)
    # https://docs.libretro.com/development/retroarch/compilation/psvita/
    bundle_static_library(bennugd_libretro bennugd_libretro_merged)
    ExternalProject_Add( retroarch
        GIT_REPOSITORY https://github.com/libretro/RetroArch.git
        GIT_TAG master
        CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE TRUE
        BUILD_ALWAYS TRUE
        BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Starting retroarch build for PS Vita"
        COMMAND cp "$<TARGET_FILE:bennugd_libretro_merged>" libretro_vita.a
        #COMMAND make -f Makefile.vita.salamander
        #COMMAND make -f Makefile.griffin platform=vita
        COMMAND make -f Makefile.vita
        COMMAND vita-make-fself retroarch_vita.elf ${CMAKE_BINARY_DIR}/bennugd_libretro_vita.self
        INSTALL_COMMAND ""
        DEPENDS bennugd_libretro_merged
    )
endif()