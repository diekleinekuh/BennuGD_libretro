# SPDX-License-Identifier: MIT
#
# MIT License
#
# Copyright (c) 2019 Cristian Adam
# Copyright (c) 2025 Byborg Ip SARL. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

function(bundle_static_library tgt_name bundled_tgt_name)
  list(APPEND static_libs ${tgt_name})

  function(_recursively_collect_dependencies input_target)
    # Inherit static_libs from parent scope
    set(static_libs ${static_libs})

    set(_input_link_libraries LINK_LIBRARIES)
    get_target_property(_input_type ${input_target} TYPE)
    if (${_input_type} STREQUAL "INTERFACE_LIBRARY")
      set(_input_link_libraries INTERFACE_LINK_LIBRARIES)
    endif()
    get_target_property(public_dependencies ${input_target} ${_input_link_libraries})
    foreach(dependency IN LISTS public_dependencies)
      if(TARGET ${dependency})
        get_target_property(alias ${dependency} ALIASED_TARGET)
        if (TARGET ${alias})
          set(dependency ${alias})
        endif()
        get_target_property(_type ${dependency} TYPE)
        if (${_type} STREQUAL "STATIC_LIBRARY")
          list(APPEND static_libs ${dependency})
        endif()

        get_property(library_already_added
          GLOBAL PROPERTY _${tgt_name}_static_bundle_${dependency})
        if (NOT library_already_added)
          set_property(GLOBAL PROPERTY _${tgt_name}_static_bundle_${dependency} ON)
          _recursively_collect_dependencies(${dependency})
        endif()
      endif()
    endforeach()
    set(static_libs ${static_libs} PARENT_SCOPE)
  endfunction()

  _recursively_collect_dependencies(${tgt_name})

  list(REMOVE_DUPLICATES static_libs)

  set(bundled_tgt_full_name
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${bundled_tgt_name}${CMAKE_STATIC_LIBRARY_SUFFIX})

  if (APPLE)
    foreach(tgt IN LISTS static_libs)
      list(APPEND static_libs_full_names $<TARGET_FILE:${tgt}>)
    endforeach()

    add_custom_command(
      COMMAND libtool -static -o ${bundled_tgt_full_name} ${static_libs_full_names}
      OUTPUT ${bundled_tgt_full_name}
      DEPENDS ${static_libs}
      COMMENT "Bundling ${bundled_tgt_name}"
      VERBATIM)
  elseif ((CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$") OR (CMAKE_C_COMPILER_ID MATCHES "^(Clang|GNU)$"))
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${bundled_tgt_name}.ar.in
      "CREATE ${bundled_tgt_full_name}\n" )

    foreach(tgt IN LISTS static_libs)
      file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${bundled_tgt_name}.ar.in
        "ADDLIB $<TARGET_FILE:${tgt}>\n")
    endforeach()

    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${bundled_tgt_name}.ar.in "SAVE\n")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${bundled_tgt_name}.ar.in "END\n")

    file(GENERATE
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${bundled_tgt_name}.ar
      INPUT ${CMAKE_CURRENT_BINARY_DIR}/${bundled_tgt_name}.ar.in)

    set(ar_tool ${CMAKE_AR})
    if (CMAKE_INTERPROCEDURAL_OPTIMIZATION)
      if (CMAKE_CXX_COMPILER_AR)
        set(ar_tool ${CMAKE_CXX_COMPILER_AR})
      elseif (CMAKE_C_COMPILER_AR)
        set(ar_tool ${CMAKE_C_COMPILER_AR})
      endif()
    endif()

    add_custom_command(
      COMMAND ${ar_tool} -M < ${CMAKE_CURRENT_BINARY_DIR}/${bundled_tgt_name}.ar
      OUTPUT ${bundled_tgt_full_name}
      DEPENDS ${static_libs}
      COMMENT "Bundling ${bundled_tgt_name}"
      VERBATIM)
  elseif(MSVC)
    find_program(lib_tool lib)

    foreach(tgt IN LISTS static_libs)
      list(APPEND static_libs_full_names $<TARGET_FILE:${tgt}>)
    endforeach()

    add_custom_command(
      COMMAND ${lib_tool} /NOLOGO /OUT:${bundled_tgt_full_name} ${static_libs_full_names}
      OUTPUT ${bundled_tgt_full_name}
      DEPENDS ${static_libs}
      COMMENT "Bundling ${bundled_tgt_name}"
      VERBATIM)
  else()
    message(FATAL_ERROR "Unknown bundle scenario!")
  endif()

  add_custom_target(bundling_target_${bundled_tgt_name} ALL DEPENDS ${bundled_tgt_full_name})
  add_dependencies(bundling_target_${bundled_tgt_name} ${static_libs})

  add_library(${bundled_tgt_name} STATIC IMPORTED)
  set_target_properties(${bundled_tgt_name}
    PROPERTIES
      IMPORTED_LOCATION ${bundled_tgt_full_name}
      INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${tgt_name},INTERFACE_INCLUDE_DIRECTORIES>
      BUNDLE_LIBRARY_FILE ${bundled_tgt_full_name})
  add_dependencies(${bundled_tgt_name} bundling_target_${bundled_tgt_name})

  # Export the bundled library file path to parent scope for installation
  set(${bundled_tgt_name}_LIBRARY_FILE ${bundled_tgt_full_name} PARENT_SCOPE)

endfunction()
