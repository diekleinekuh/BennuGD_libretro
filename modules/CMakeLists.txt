set(SDL_BUILDING_LIBRARY TRUE)
find_package(SDL 1.2 REQUIRED)

include_directories(${SDL_INCLUDE_DIR})


function(add_mod deps)
    file(GLOB LIB_SRC "*.c")
    get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    STRING(REGEX REPLACE "^lib" "" LIB_NAME ${DIR_NAME})
    add_library(${LIB_NAME} SHARED ${LIB_SRC})
    target_link_libraries(${LIB_NAME} ${deps})
    if(NOT ${DIR_NAME} EQUAL ${LIB_NAME})
        target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    target_precompile_headers(${LIB_NAME} REUSE_FROM core)
endfunction()

file(GLOB LIBS "*")

foreach(dir ${LIBS})
    if(IS_DIRECTORY ${dir})
        if(EXISTS "${dir}/CMakeLists.txt")
            add_subdirectory(${dir})
        endif()
    else()
        continue()
    endif()
endforeach()
