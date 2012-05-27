# - Find SDL2
#
# This module defines:
#
#  SDL2_FOUND               - True if SDL2 library is found
#  SDL2_LIBRARY             - SDL2 dynamic library
#  SDL2_INCLUDE_DIR         - Include dir
#

# Library
find_library(SDL2_LIBRARY SDL2)

# Include dir
find_path(SDL2_INCLUDE_DIR
    NAMES SDL.h SDL_scancode.h
    PATH_SUFFIXES SDL2
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("SDL2" DEFAULT_MSG
    SDL2_LIBRARY
    SDL2_INCLUDE_DIR
)
