# - Find OpenGL ES 2
#
# This module defines:
#
#  OPENGLES2_FOUND          - True if OpenGL ES 2 library is found
#  OPENGLES2_LIBRARY        - OpenGL ES 2 library
#  OPENGLES2_INCLUDE_DIR    - Include dir
#

# Library
find_library(OPENGLES2_LIBRARY GLESv2)

# Include dir
find_path(OPENGLES2_INCLUDE_DIR
    NAMES gl2.h
    PATH_SUFFIXES GLES2
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("OpenGLES2" DEFAULT_MSG
    OPENGLES2_LIBRARY
    OPENGLES2_INCLUDE_DIR
)
