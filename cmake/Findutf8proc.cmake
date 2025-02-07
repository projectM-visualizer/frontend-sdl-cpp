# Find the utf8proc header directory
find_path(UTF8PROC_INCLUDE_DIR
    NAMES utf8proc.h
    PATHS
        $ENV{UTF8PROC_ROOT}/include
        /usr/local/include
        /usr/include
)

# Find the utf8proc library
find_library(UTF8PROC_LIBRARY
    NAMES utf8proc
    PATHS
        $ENV{UTF8PROC_ROOT}/lib
        /usr/local/lib
        /usr/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(utf8proc
    REQUIRED_VARS
        UTF8PROC_LIBRARY
        UTF8PROC_INCLUDE_DIR
)

if(utf8proc_FOUND AND NOT TARGET Utf8Proc::Utf8Proc)
    add_library(Utf8Proc::Utf8Proc UNKNOWN IMPORTED)
    set_target_properties(Utf8Proc::Utf8Proc PROPERTIES
        IMPORTED_LOCATION "${UTF8PROC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${UTF8PROC_INCLUDE_DIR}"
    )
endif()
