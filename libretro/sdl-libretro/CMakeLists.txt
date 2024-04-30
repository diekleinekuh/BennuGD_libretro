cmake_minimum_required(VERSION 3.13)
project(sdl-libretro)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "compile as PIC by default")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set (SDL-src
    src/SDL.c
    src/SDL_error.c
    src/SDL_fatal.c

    src/audio/SDL_audio.c
    src/audio/SDL_audiocvt.c
    src/audio/SDL_mixer.c
    src/audio/SDL_mixer_MMX.c
    src/audio/SDL_mixer_MMX_VC.c
    src/audio/SDL_mixer_m68k.c
    src/audio/SDL_wave.c

    src/cdrom/SDL_cdrom.c

    src/cpuinfo/SDL_cpuinfo.c

    src/events/SDL_active.c
    src/events/SDL_events.c
    src/events/SDL_expose.c
    src/events/SDL_keyboard.c
    src/events/SDL_mouse.c
    src/events/SDL_quit.c
    src/events/SDL_resize.c

    src/file/SDL_rwops.c

    src/stdlib/SDL_getenv.c
    src/stdlib/SDL_iconv.c
    src/stdlib/SDL_malloc.c
    src/stdlib/SDL_qsort.c
    src/stdlib/SDL_stdlib.c
    src/stdlib/SDL_string.c

    src/thread/SDL_thread.c

    src/timer/SDL_timer.c
    src/timer/libretro/SDL_systimer.c

    src/video/SDL_RLEaccel.c
    src/video/SDL_blit.c
    src/video/SDL_blit_0.c
    src/video/SDL_blit_1.c
    src/video/SDL_blit_A.c
    src/video/SDL_blit_N.c
    src/video/SDL_bmp.c
    src/video/SDL_cursor.c
    src/video/SDL_gamma.c
    src/video/SDL_pixels.c
    src/video/SDL_stretch.c
    src/video/SDL_surface.c
    src/video/SDL_video.c
    src/video/SDL_yuv.c
    src/video/SDL_yuv_mmx.c
    src/video/SDL_yuv_sw.c
    src/video/libretro/SDL_LIBRETROevents.c
    src/video/libretro/SDL_LIBRETROkeyboard.c
    src/video/libretro/SDL_LIBRETROmouse.c
    src/video/libretro/SDL_LIBRETROvideo.c

    src/joystick/SDL_joystick.c
    src/joystick/libretro/SDL_sysjoystick.c

    src/loadso/dummy/SDL_sysloadso.c
)

add_compile_definitions(__LIBRETRO__ LSB_FIRST )

if (WIN32)
    set( SDL-src ${SDL-src}
        src/cdrom/win32/SDL_syscdrom.c
        src/thread/win32/SDL_sysmutex.c
        src/thread/win32/SDL_syssem.c
        src/thread/win32/SDL_systhread.c
        src/thread/generic/SDL_syscond.c
    )    
else()
    set( SDL-src ${SDL-src}
        src/cdrom/linux/SDL_syscdrom.c
        src/thread/pthread/SDL_syscond.c
        src/thread/pthread/SDL_sysmutex.c
        src/thread/pthread/SDL_syssem.c
        src/thread/pthread/SDL_systhread.c
    )
    add_compile_definitions(__LINUX__ )
endif()

add_library( sdl-libretro STATIC ${SDL-src})
target_link_libraries(sdl-libretro Threads::Threads)
include_directories(include)


include(FetchContent)

set(BUILD_TESTING_backup ${BUILD_TESTING})
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  ogg
  GIT_REPOSITORY https://gitlab.xiph.org/xiph/ogg.git
  GIT_TAG        v1.3.5
  GIT_SHALLOW TRUE
  PATCH_COMMAND "${CMAKE_COMMAND}" "-D" "PATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/patches/patch_ogg_min_required_version.patch" "-P" "${CMAKE_CURRENT_SOURCE_DIR}/patches/ApplyPatch.cmake"
)

FetchContent_Declare(
  vorbis
  GIT_REPOSITORY https://gitlab.xiph.org/xiph/vorbis.git
  GIT_TAG        84c023699cdf023a32fa4ded32019f194afcdad0
  GIT_SHALLOW TRUE
  PATCH_COMMAND "${CMAKE_COMMAND}" "-D" "PATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/patches/patch_vorbis_min_required_version.patch" "-P" "${CMAKE_CURRENT_SOURCE_DIR}/patches/ApplyPatch.cmake"
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
    GIT_TAG acdebd3ee2e28810288acfbb1b22cb37e7bc8c66
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR libmikmod
)

FetchContent_MakeAvailable( ogg vorbis mikmod )
set(BUILD_TESTING ${BUILD_TESTING_backup})

add_compile_definitions(WAV_MUSIC OGG_MUSIC MOD_MUSIC)
set (SDL_mixer-src
    sdl_mixer/dynamic_flac.c
    sdl_mixer/dynamic_fluidsynth.c
    sdl_mixer/dynamic_mod.c
    sdl_mixer/dynamic_mp3.c
    sdl_mixer/dynamic_ogg.c
    sdl_mixer/effect_position.c
    sdl_mixer/effect_stereoreverse.c
    sdl_mixer/effects_internal.c
    sdl_mixer/fluidsynth.c
    sdl_mixer/load_aiff.c
    sdl_mixer/load_flac.c
    sdl_mixer/load_ogg.c
    sdl_mixer/load_voc.c
    sdl_mixer/mixer.c
    sdl_mixer/music.c
    sdl_mixer/music_cmd.c
    sdl_mixer/music_flac.c
    sdl_mixer/music_mad.c
    sdl_mixer/music_mod.c
    sdl_mixer/music_modplug.c
    sdl_mixer/music_ogg.c
    sdl_mixer/wavestream.c
)

add_library( sdl_mixer-libretro STATIC ${SDL_mixer-src})

target_link_libraries(sdl_mixer-libretro Vorbis::vorbis;Vorbis::vorbisfile;Ogg::ogg;mikmod-static)