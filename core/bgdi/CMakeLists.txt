cmake_minimum_required(VERSION 3.14)

if(libretro_core)
    add_library(bgdi "src/main.c")
else()
    add_executable(bgdi "src/main.c")
endif()

target_include_directories(bgdi PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(bgdi core ${BGD_MODULES} )