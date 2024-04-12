cmake_minimum_required(VERSION 3.16)
include(CMakePrintHelpers)

find_package(OpenSSL REQUIRED)
find_package(ZLIB)

file(GLOB COMMON_SRC
    "common/*.c"
)

file(GLOB BGDRTM_SRC
    "bgdrtm/src/*.c"
)


add_library(core OBJECT ${COMMON_SRC} ${BGDRTM_SRC})


if (WIN32)
    target_compile_definitions(core PUBLIC _WIN32)
else()
    target_compile_definitions(core PUBLIC TARGET_LINUX)
    target_link_libraries(core m dl)
endif()

target_compile_definitions(core 
    PRIVATE __BGDRTM__
)

target_include_directories(core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/bgdrtm/include"
)

target_include_directories(core PRIVATE ${LIBRETRO_INCLUDE_DIR} ${SDL_INCLUDE_DIR})

target_link_libraries(core ${OPENSSL_CRYPTO_LIBRARY} ${ZLIB_LIBRARIES} ${SDL_LIBRARY})

