if (NOT DEFINED PATCH_FILE )
    message(FATAL_ERROR "PATCH_FILE not supplied")
endif()

find_package(Git REQUIRED)

message("PATCH_FILE=${PATCH_FILE}")

# first try to revert the patch in case it already exists. This might fail
execute_process( COMMAND ${GIT_EXECUTABLE} apply -R "${PATCH_FILE}" 
    ERROR_VARIABLE UNDO_PATCH_RESULT
)

# apply the patch
execute_process( COMMAND ${GIT_EXECUTABLE} apply --whitespace=fix "${PATCH_FILE}"
    RESULT_VARIABLE PATCH_OUTPUT
    OUTPUT_VARIABLE PATCH_OUTPUT
    ERROR_VARIABLE PATCH_RESULT
)

if(PATCH_RESULT AND NOT PATCH_RESULT EQUAL 0)        
    message(FATAL_ERROR "patch ${PATCH_FILE} could not be applied:\n${PATCH_RESULT}")
endif()

message("Successfully applied patch ${PATCH_FILE}")