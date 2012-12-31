# - Find HarfBuzz
#
# This module tries to find HarfBuzz library and then defines:
#  HARFBUZZ_FOUND          - True if HarfBuzz library is found
#  HARFBUZZ_INCLUDE_DIRS   - Include dirs
#  HARFBUZZ_LIBRARIES      - HarfBuzz libraries
#
# Additionally these variables are defined for internal usage:
#  HARFBUZZ_INCLUDE_DIR    - Include dir (w/o dependencies)
#  HARFBUZZ_LIBRARY        - HarfBuzz library (w/o dependencies)
#

# Library
find_library(HARFBUZZ_LIBRARY NAMES harfbuzz)

# Include dir
find_path(HARFBUZZ_INCLUDE_DIR
    NAMES hb.h
    PATH_SUFFIXES harfbuzz
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("HarfBuzz" DEFAULT_MSG
    HARFBUZZ_LIBRARY
    HARFBUZZ_INCLUDE_DIR
)

set(HARFBUZZ_INCLUDE_DIRS ${HARFBUZZ_INCLUDE_DIR})
set(HARFBUZZ_LIBRARIES ${HARFBUZZ_LIBRARY})

mark_as_advanced(FORCE
    HARFBUZZ_LIBRARY
    HARFBUZZ_INCLUDE_DIR)
