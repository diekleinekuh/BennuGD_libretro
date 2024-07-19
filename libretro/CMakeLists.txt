include(FetchContent)

FetchContent_Declare(
  libretro-common
  GIT_REPOSITORY https://github.com/diekleinekuh/libretro-common.git
  GIT_TAG        3d8d8c3e081d209395190c39688b24ea6b233cba
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

FetchContent_MakeAvailable(  libretro-common )

add_subdirectory(sdl-libretro)

add_library(retro_BennuGD SHARED
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
    #${libretro-common_SOURCE_DIR}/streams/trans_stream.c
    ${libretro-common_SOURCE_DIR}/streams/trans_stream_zlib.c
    ${libretro-common_SOURCE_DIR}/streams/file_stream_transforms.c
    #${libretro-common_SOURCE_DIR}/streams/trans_stream_pipe.c
    ${libretro-common_SOURCE_DIR}/compat/compat_posix_string.c
    ${libretro-common_SOURCE_DIR}/compat/fopen_utf8.c
    libretro.c
    filesystem.c
    filestream_gzip.c
)

target_include_directories(retro_BennuGD PRIVATE 
    ${libretro-common_SOURCE_DIR}/include
)

target_link_libraries(retro_BennuGD bgdi)

include_directories(${sdl-libretro_SOURCE_DIR}/include)

set(SDL_LIBRARIES "sdl-libretro" PARENT_SCOPE)
set(SDL_MIXER_LIBRARIES "sdl_mixer-libretro" PARENT_SCOPE)
set(SDL_INCLUDE_DIR "${sdl-libretro_SOURCE_DIR}/include" PARENT_SCOPE)

set(LIBRETRO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libretro-common/include" PARENT_SCOPE)
