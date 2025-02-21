include (FindPackageHandleStandardArgs)

find_path(MIKMOD_INCLUDE_DIR mikmod.h)
find_library(MIKMOD_LIBRARY mikmod)

find_package_handle_standard_args( Mikmod DEFAULT_MSG
    MIKMOD_LIBRARY MIKMOD_INCLUDE_DIR )

add_library (Mikmod::Mikmod UNKNOWN IMPORTED)
set_target_properties (Mikmod::Mikmod PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${MIKMOD_INCLUDE_DIR}"
    IMPORTED_LOCATION "${MIKMOD_LIBRARY}"
)
