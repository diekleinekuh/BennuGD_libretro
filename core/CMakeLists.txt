cmake_minimum_required(VERSION 3.16)
include(CMakePrintHelpers)

find_package(OpenSSL REQUIRED)
find_package(ZLIB)

file(GLOB COMMON_SRC
    "common/*.c"
)

#cmake_print_variables(COMMON_SRC)

file(GLOB BGDRTM_SRC
    "bgdrtm/src/*.c"
)

#cmake_print_variables(BGDRTM_SRC)

add_library(core SHARED
    ${COMMON_SRC}
    ${BGDRTM_SRC}
)

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
target_link_libraries(core ${OPENSSL_CRYPTO_LIBRARY} ${ZLIB_LIBRARIES})

add_subdirectory("bgdi")
