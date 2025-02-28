cmake_minimum_required(VERSION 3.14...3.31)
include(FetchContent)
include(CheckSymbolExists)

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
add_subdirectory(sdl-libretro)

add_subdirectory(sdl_mixer-libretro)
set(SDL_MIXER_LIBRARIES "sdl_mixer-libretro" PARENT_SCOPE)

add_library(bennugd_libretro SHARED
    ${libretro-common_SOURCE_DIR}/libco/libco.c
    ${libretro-common_SOURCE_DIR}/vfs/vfs_implementation.c
    ${libretro-common_SOURCE_DIR}/file/file_path.c
    ${libretro-common_SOURCE_DIR}/file/file_path_io.c
    ${libretro-common_SOURCE_DIR}/file/retro_dirent.c
    ${libretro-common_SOURCE_DIR}/string/stdstring.c
    ${libretro-common_SOURCE_DIR}/encodings/encoding_utf.c
    ${libretro-common_SOURCE_DIR}/time/rtime.c
    ${libretro-common_SOURCE_DIR}/compat/compat_strl.c
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


target_include_directories(bennugd_libretro PRIVATE "${libretro-common_SOURCE_DIR}/include")

if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set_source_files_properties(${libretro-common_SOURCE_DIR}/libco/libco.c PROPERTIES COMPILE_OPTIONS "/wd4005")
endif()

target_include_directories(bennugd_libretro PRIVATE 
    ${libretro-common_SOURCE_DIR}/include
)

set_target_properties(bennugd_libretro PROPERTIES PREFIX "")
set_target_properties(bennugd_libretro PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(bennugd_libretro PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(bennugd_libretro PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

target_link_libraries(bennugd_libretro bgdi)

add_library(SDL::SDL ALIAS sdl-libretro)
