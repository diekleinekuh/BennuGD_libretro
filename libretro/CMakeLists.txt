include(FetchContent)

add_subdirectory(sdl-libretro)

add_library(retro_BennuGD SHARED
    libco/libco.c
    libretro.c
)

target_include_directories(retro_BennuGD PRIVATE 
    libco
    ${libretro-common_SOURCE_DIR}/include
)

target_link_libraries(retro_BennuGD bgdi)

include_directories(${sdl-libretro_SOURCE_DIR}/include)

set(SDL_LIBRARIES "sdl-libretro" PARENT_SCOPE)
set(SDL_MIXER_LIBRARIES "sdl_mixer-libretro" PARENT_SCOPE)
set(SDL_INCLUDE_DIR "${sdl-libretro_SOURCE_DIR}/include" PARENT_SCOPE)

set(LIBRETRO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libretro-common/include" PARENT_SCOPE)
