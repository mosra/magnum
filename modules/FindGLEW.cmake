# Find GLEW - OpenGL Extension Wrangler handling module for CMake
#
# This module defines:
#
# GLEW_FOUND                - True if GLEW library is found
# GLEW_LIBRARY              - GLEW dynamic library
# GLEW_INCLUDE_DIR          - Include dir
#

if(GLEW_LIBRARY AND GLEW_INCLUDE_DIR)

    set(GLEW_FOUND TRUE)

else()

    # Static library
    find_library(GLEW_LIBRARY GLEW)

    # Include dir
    find_path(GLEW_INCLUDE_DIR
        NAMES glew.h
        PATH_SUFFIXES GL
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args("GLEW" DEFAULT_MSG
        GLEW_LIBRARY
        GLEW_INCLUDE_DIR
    )

endif()
