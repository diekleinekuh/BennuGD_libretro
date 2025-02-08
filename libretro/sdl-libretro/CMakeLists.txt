cmake_minimum_required(VERSION 3.14)
project(sdl-libretro C)
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
    src/cdrom/dummy/SDL_syscdrom.c

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
    src/thread/generic/SDL_syssem.c
    src/thread/libretro/SDL_syscond.c
    src/thread/libretro/SDL_sysmutex.c
    src/thread/libretro/SDL_systhread.c

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

add_library( sdl-libretro STATIC ${SDL-src})
target_link_libraries(sdl-libretro Threads::Threads)
target_include_directories(sdl-libretro PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
