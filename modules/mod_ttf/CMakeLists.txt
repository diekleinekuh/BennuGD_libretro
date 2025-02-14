include(FetchContent)

if (NOT NO_SYSTEM_DEPENDENCIES)
    find_package(Freetype)
endif()

if (NO_SYSTEM_DEPENDENCIES OR NOT Freetype_FOUND)
    set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

    FetchContent_Declare(freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG e2bcca40ca0d82ae64ba25e5c5e252dc0d083162
    )
    FetchContent_MakeAvailable(freetype)
else()
    add_library(freetype INTERFACE)
    target_link_libraries(freetype INTERFACE ${FREETYPE_LIBRARIES})
    target_include_directories(freetype INTERFACE "${FREETYPE_INCLUDE_DIR_freetype2};${FREETYPE_INCLUDE_DIR_ft2build}")
endif()

add_mod("grbase;font;freetype")