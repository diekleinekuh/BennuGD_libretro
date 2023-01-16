cmake_minimum_required(VERSION 3.16)

add_executable(bgdi "src/main.c")

target_compile_definitions(bgdi 
    PRIVATE VERSION="1.0.0"
)

target_include_directories(bgdi PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/bgdrtm/include"
)

target_link_libraries(bgdi core)