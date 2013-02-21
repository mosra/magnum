# Find GLEW - OpenGL Extension Wrangler handling module for CMake
#
# This module defines:
#
# GLEW_FOUND                - True if GLEW library is found
# GLEW_LIBRARIES            - GLEW libraries
# GLEW_INCLUDE_DIR          - Include dir
#

# Include dir
find_path(GLEW_INCLUDE_DIR
    NAMES GL/glew.h)

# Library
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    set(GLEW_LIBRARIES_ GLEW_LIBRARY)
    mark_as_advanced(GLEW_LIBRARY)
else()
    find_library(GLEW_LIBRARY_DLL glew32)
    find_library(GLEW_LIBRARY_LIB glew32)
    set(GLEW_LIBRARIES_ GLEW_LIBRARY_DLL GLEW_LIBRARY_LIB)
    mark_as_advanced(GLEW_LIBRARY_DLL GLEW_LIBRARY_LIB)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("GLEW" DEFAULT_MSG
    ${GLEW_LIBRARIES_}
    GLEW_INCLUDE_DIR)

if(NOT GLEW_FOUND)
    return()
endif()

unset(GLEW_LIBRARIES_)

if(NOT WIN32)
    set(GLEW_LIBRARIES ${GLEW_LIBRARY})
    mark_as_advanced(GLEW_LIBRARY)
else()
    set(GLEW_LIBRARIES ${GLEW_LIBRARY_DLL} ${GLEW_LIBRARY_LIB})
    mark_as_advanced(GLEW_LIBRARY_DLL GLEW_LIBRARY_LIB)
endif()
