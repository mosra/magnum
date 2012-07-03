# - Find EGL
#
# This module defines:
#
#  EGL_FOUND            - True if EGL library is found
#  EGL_LIBRARY          - EGL library
#  EGL_INCLUDE_DIR      - Include dir
#

# Library
find_library(EGL_LIBRARY EGL)

# Include dir
find_path(EGL_INCLUDE_DIR
    NAMES egl.h
    PATH_SUFFIXES EGL
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("EGL" DEFAULT_MSG
    EGL_LIBRARY
    EGL_INCLUDE_DIR
)
