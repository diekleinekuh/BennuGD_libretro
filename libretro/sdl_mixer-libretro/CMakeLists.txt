cmake_minimum_required(VERSION 3.14)
project(sdl-mixer C)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "compile as PIC by default")
set(THREADS_PREFER_PTHREAD_FLAG ON)

include(FetchContent)

if (NOT NO_SYSTEM_DEPENDENCIES)
  find_package(Ogg)
  find_package(Vorbis)
  find_package(Mikmod)
endif()

set(BUILD_TESTING_backup ${BUILD_TESTING})
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

if (NO_SYSTEM_DEPENDENCIES OR NOT Ogg_FOUND)
  FetchContent_Declare(
    ogg
    GIT_REPOSITORY https://github.com/xiph/ogg.git
    GIT_TAG        db5c7a49ce7ebda47b15b78471e78fb7f2483e22 #v1.3.5
    PATCH_COMMAND "${CMAKE_COMMAND}" "-D" "PATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/patches/patch_ogg_min_required_version.patch" "-P" "${CMAKE_CURRENT_SOURCE_DIR}/patches/ApplyPatch.cmake"
    EXCLUDE_FROM_ALL
  )
  FetchContent_MakeAvailable( ogg )
endif()

if (NO_SYSTEM_DEPENDENCIES OR NOT Vorbis_FOUND)
  FetchContent_Declare(
    vorbis
    GIT_REPOSITORY https://github.com/xiph/vorbis.git
    GIT_TAG        84c023699cdf023a32fa4ded32019f194afcdad0
    PATCH_COMMAND "${CMAKE_COMMAND}" "-D" "PATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/patches/patch_vorbis_min_required_version.patch" "-P" "${CMAKE_CURRENT_SOURCE_DIR}/patches/ApplyPatch.cmake"
    EXCLUDE_FROM_ALL
  )
  FetchContent_MakeAvailable( vorbis )
endif()

set(BUILD_TESTING ${BUILD_TESTING_backup})

if (NO_SYSTEM_DEPENDENCIES OR NOT Mikmod_FOUND)
  set(ENABLE_ALSA OFF CACHE BOOL "Include the ALSA driver" FORCE)
  set(ENABLE_OSS OFF CACHE BOOL "Include the OSS driver" FORCE)
  set(ENABLE_PULSE OFF CACHE BOOL "Include the PulseAudio driver" FORCE)
  set(ENABLE_COREAUDIO OFF CACHE BOOL "Include the CoreAudio driver" FORCE)
  set(ENABLE_DSOUND OFF CACHE BOOL "Include the DirectSound driver" FORCE)
  set(ENABLE_MMSOUND OFF CACHE BOOL "Include the Windows MCI driver" FORCE)
  set(ENABLE_STDOUT OFF CACHE BOOL "Support for output to stdout" FORCE)
  set(ENABLE_PIPE OFF CACHE BOOL "Support for output via a pipe to another command" FORCE)
  set(ENABLE_SNDIO OFF CACHE BOOL "Include the OpenBSD sndio driver" FORCE)
  set(ENABLE_AIX OFF CACHE BOOL "Include the AIX audio driver" FORCE)
  set(ENABLE_HPUX OFF CACHE BOOL "Include the HP-UX audio driver" FORCE)
  set(ENABLE_SUNAUDIO OFF CACHE BOOL "Include the SunAudio (or compatibles) driver" FORCE)
  set(ENABLE_AIFF OFF CACHE BOOL "Support for output to an .aiff file" FORCE)
  set(ENABLE_WAV OFF CACHE BOOL "Support for output to a .wav file" FORCE)
  set(ENABLE_RAW OFF CACHE BOOL "Support for output raw pcm data to a file" FORCE)
  set(ENABLE_DOC OFF CACHE BOOL "" FORCE)
  set(ENABLE_SHARED OFF CACHE BOOL "Whether to build the shared library" FORCE)

  # cmake 3.18 introduced the SOURCE_SUBDIR but in 3.14 it's not avaiable, this is a workaround
  FetchContent_Declare(
    mikmod-dummy
    GIT_REPOSITORY https://github.com/sezero/mikmod.git
    GIT_TAG 72424e1bebab470fbf8a2914dcd7903b742205e6 #libmikmod-3.3.12
    CONFIGURE_COMMAND " "
    BUILD_COMMAND " "
    INSTALL_COMMAND " "
  )
  FetchContent_MakeAvailable( mikmod-dummy )

  FetchContent_Declare(
    mikmod
    SOURCE_DIR "${mikmod-dummy_SOURCE_DIR}/libmikmod"
  )

  FetchContent_MakeAvailable( mikmod )
  disable_all_warnings(mikmod-static)

  add_library (mymikmod INTERFACE)
  target_link_libraries(mymikmod INTERFACE mikmod-static)
  target_include_directories(mymikmod INTERFACE $<BUILD_INTERFACE:${mikmod-dummy_SOURCE_DIR}/libmikmod/include> )
  target_compile_definitions(mymikmod INTERFACE MIKMOD_STATIC=1)
  add_library(Mikmod::Mikmod ALIAS mymikmod)
endif()

set (SDL_mixer-src
    dynamic_flac.c
    dynamic_fluidsynth.c
    dynamic_mod.c
    dynamic_mp3.c
    dynamic_ogg.c
    effect_position.c
    effect_stereoreverse.c
    effects_internal.c
    fluidsynth.c
    load_aiff.c
    load_flac.c
    load_ogg.c
    load_voc.c
    mixer.c
    music.c
    music_cmd.c
    music_flac.c
    music_mad.c
    music_mod.c
    music_modplug.c
    music_ogg.c
    wavestream.c
)

add_library( sdl_mixer-libretro STATIC ${SDL_mixer-src})

target_link_libraries(sdl_mixer-libretro Vorbis::vorbis;Vorbis::vorbisfile;Ogg::ogg;Mikmod::Mikmod;sdl-libretro )
target_include_directories(sdl_mixer-libretro PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(sdl_mixer-libretro PRIVATE "WAV_MUSIC;OGG_MUSIC;MOD_MUSIC")