include(CheckSymbolExists)

function(add_mod deps)
    file(GLOB LIB_SRC "*.c")

    get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    STRING(REGEX REPLACE "^lib" "" LIB_NAME ${DIR_NAME})
    add_library(${LIB_NAME} OBJECT ${LIB_SRC})
    list(APPEND MODULES ${LIB_NAME})
    target_link_libraries(${LIB_NAME} ${deps})
    if(NOT ${DIR_NAME} EQUAL ${LIB_NAME})
        target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    set(BGD_MODULE_NAME "${LIB_NAME}" PARENT_SCOPE)
   
endfunction()

file(GLOB LIBS "*")

foreach(dir ${LIBS})
    if(IS_DIRECTORY ${dir})
        if(EXISTS "${dir}/CMakeLists.txt")
            add_subdirectory(${dir})
          
            get_directory_property(BGD_MODULE_NAME DIRECTORY "${dir}" DEFINITION BGD_MODULE_NAME)
            list(APPEND BGD_MODULES ${BGD_MODULE_NAME})
        endif()
    else()
        continue()
    endif()
endforeach()

if (NOT BUILD_SHARED_LIBS)
    set(BGD_MODULES ${BGD_MODULES} PARENT_SCOPE)
endif()
