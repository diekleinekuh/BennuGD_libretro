cmake_minimum_required(VERSION 3.14)
project(sdl-mixer C)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "compile as PIC by default")
set(THREADS_PREFER_PTHREAD_FLAG ON)

include(FetchContent)

set(BUILD_TESTING_backup ${BUILD_TESTING})
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  ogg
  GIT_REPOSITORY https://gitlab.xiph.org/xiph/ogg.git
  GIT_TAG        7cf42ea17aef7bc1b7b21af70724840a96c2e7d0
  PATCH_COMMAND "${CMAKE_COMMAND}" "-D" "PATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/patches/patch_ogg_min_required_version.patch" "-P" "${CMAKE_CURRENT_SOURCE_DIR}/patches/ApplyPatch.cmake"
  EXCLUDE_FROM_ALL
)

FetchContent_Declare(
  vorbis
  GIT_REPOSITORY https://gitlab.xiph.org/xiph/vorbis.git
  GIT_TAG        84c023699cdf023a32fa4ded32019f194afcdad0
  PATCH_COMMAND "${CMAKE_COMMAND}" "-D" "PATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/patches/patch_vorbis_min_required_version.patch" "-P" "${CMAKE_CURRENT_SOURCE_DIR}/patches/ApplyPatch.cmake"
  EXCLUDE_FROM_ALL
)

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

FetchContent_Declare(
    mikmod
    GIT_REPOSITORY https://github.com/sezero/mikmod.git
    GIT_TAG 56af6ef8d596e5aa885475cdb40b76ff063707cc            
    SOURCE_SUBDIR libmikmod
)

FetchContent_MakeAvailable( ogg vorbis mikmod )
disable_all_warnings(mikmod-static)

set(BUILD_TESTING ${BUILD_TESTING_backup})

add_compile_definitions(WAV_MUSIC OGG_MUSIC MOD_MUSIC)
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

target_link_libraries(sdl_mixer-libretro Vorbis::vorbis;Vorbis::vorbisfile;Ogg::ogg;mikmod-static;sdl-libretro )
target_include_directories(sdl_mixer-libretro PRIVATE "${mikmod_SOURCE_DIR}/libmikmod/include;${SDL_INCLUDE_DIR}")
target_include_directories(sdl_mixer-libretro PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(sdl_mixer-libretro PRIVATE "MIKMOD_STATIC=1")