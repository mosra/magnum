# Find Magnum - Magnum handling module for CMake
#
# This module depends on Corrade and additionally defines:
#
# MAGNUM_FOUND          - True if Magnum library is found
# MAGNUM_INCLUDE_DIR    - Include dir for Magnum
# MAGNUM_LIBRARY        - Magnum library
#
# MAGNUM_LIBRARY_INSTALL_DIR        - Library installation directory
# MAGNUM_CMAKE_MODULE_INSTALL_DIR   - Installation dir for CMake modules
# MAGNUM_INCLUDE_INSTALL_DIR        - Include installation directory for headers
#

find_package(Corrade REQUIRED)

if (MAGNUM_INCLUDE_DIR AND MAGNUM_LIBRARY)

    # Already in cache
    set(MAGNUM_FOUND TRUE)

else()
    # Libraries
    find_library(MAGNUM_LIBRARY Magnum)

    # Paths
    find_path(MAGNUM_INCLUDE_DIR
        NAMES Magnum.h
        PATH_SUFFIXES Magnum
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args("Magnum" DEFAULT_MSG
        MAGNUM_INCLUDE_DIR
        MAGNUM_LIBRARY
    )

endif()

# Installation dirs
set_parent_scope(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set_parent_scope(MAGNUM_PLUGINS_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum)
set_parent_scope(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
set_parent_scope(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)
set_parent_scope(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum/Plugins)
