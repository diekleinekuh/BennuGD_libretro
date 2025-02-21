
if (NOT libretro_core)
    if (NOT WIN32)
        find_package("X11")
        set(LIBMOD_PLATFORM_LIBS X11::X11)        
    endif()
endif()

add_mod("video;SDL::SDL;${LIBMOD_PLATFORM_LIBS}")