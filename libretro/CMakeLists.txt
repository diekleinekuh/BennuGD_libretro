
add_library(retro_BennuGD SHARED
    libco.c
    libretro.c
    
)

target_include_directories(retro_BennuGD PRIVATE 
    ./libretro-common/libco
    ./libretro-common/include
)

target_link_libraries(retro_BennuGD bgdi)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sdl-libretro/include)

set(SDL_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/sdl-libretro/libSDL_unix.a;pthread" PARENT_SCOPE)
set(SDL_MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/sdl-libretro/libSDL_mixer_unix.a;vorbis;vorbisfile;ogg;mikmod" PARENT_SCOPE)
set(SDL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdl-libretro/include" PARENT_SCOPE)
set(LIBRETRO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libretro-common/include" PARENT_SCOPE)