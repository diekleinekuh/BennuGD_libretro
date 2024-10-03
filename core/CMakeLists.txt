cmake_minimum_required(VERSION 3.16)
include(CMakePrintHelpers)

set(COMMON_SRC
    "common/allocator.c"
    "common/b_crypt.c"
    "common/debug.c"
    "common/files.c"
    "common/xctype.c"
)

set(BGDRTM_SRC
    "bgdrtm/src/config.c"
    "bgdrtm/src/copy.c"
    "bgdrtm/src/dcbr.c"
    "bgdrtm/src/dirs.c"
    "bgdrtm/src/fmath.c"
    "bgdrtm/src/instance.c"
    "bgdrtm/src/interpreter.c"
    "bgdrtm/src/misc.c"
    "bgdrtm/src/strings.c"
    "bgdrtm/src/sysprocs.c"
    "bgdrtm/src/varspace_file.c"
)

FetchContent_Declare(
    rpmalloc
    GIT_REPOSITORY "https://github.com/mjansson/rpmalloc.git"
    GIT_SHALLOW TRUE
    GIT_TAG "1.4.5"
)
FetchContent_MakeAvailable(rpmalloc)

add_library(core OBJECT ${COMMON_SRC} ${BGDRTM_SRC})
target_include_directories(core PRIVATE "${rpmalloc_SOURCE_DIR}/rpmalloc/")

if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set_source_files_properties("common/allocator.c" PROPERTIES COMPILE_OPTIONS "/experimental:c11atomics")
endif()

if (NOT WIN32)
    target_link_libraries(core m dl)
endif()

if (NOT libretro_core)
    if (WIN32)
        target_compile_definitions(core PUBLIC _WIN32)
    else()
        target_compile_definitions(core PUBLIC TARGET_LINUX)
    endif()
endif()

target_compile_definitions(core 
    PRIVATE __BGDRTM__
)

target_include_directories(core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/bgdrtm/include"
    "${buddy-alloc_SOURCE_DIR}"
)

target_include_directories(core PRIVATE ${LIBRETRO_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})

target_link_libraries(core
    ${OPENSSL_CRYPTO_LIBRARY}
    zlib
    ${SDL_LIBRARIES})

