# Find Corrade - Corrade handling module for CMake
#
# This module defines:
#
# CORRADECORE_FOUND                 - True if Corrade library is found
#
# CORRADE_INCLUDE_DIR               - Include dir for Corrade
#
# CORRADE_LIBRARIES                 - All corrade libraries
# CORRADE_UTILITY_LIBRARY           - Corrade Utility library
# CORRADE_PLUGINMANAGER_LIBRARY     - Corrade Plugin manager library
# CORRADE_RC_EXECUTABLE             - Corrade resource compiler executable
#
# CORRADE_BINARY_INSTALL_DIR        - Binary installation directory
# CORRADE_LIBRARY_INSTALL_DIR       - Library installation directory
# CORRADE_CMAKE_MODULE_INSTALL_DIR  - Installation dir for CMake modules
# CORRADE_INCLUDE_INSTALL_DIR       - Include installation directory for Corrade headers
#

if (CORRADE_UTILITY_INCLUDE_DIR AND CORRADE_PLUGINMANAGER_INCLUDE_DIR AND CORRADE_UTILITY_LIBRARY AND CORRADE_PLUGINMANAGER_LIBRARY AND CORRADE_RC_EXECUTABLE)

    # Already in cache
    set(CORRADECORE_FOUND TRUE)

else()
    # Libraries
    find_library(CORRADE_UTILITY_LIBRARY CorradeUtility)
    find_library(CORRADE_PLUGINMANAGER_LIBRARY CorradePluginManager)

    # RC executable
    find_program(CORRADE_RC_EXECUTABLE corrade-rc)

    # Paths
    find_path(CORRADE_INCLUDE_DIR
        NAMES PluginManager Utility
        PATH_SUFFIXES Corrade
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args("Corrade" DEFAULT_MSG
        CORRADE_INCLUDE_DIR
        CORRADE_UTILITY_LIBRARY
        CORRADE_PLUGINMANAGER_LIBRARY
        CORRADE_RC_EXECUTABLE
    )

endif()

if(CORRADE_FOUND)
    include(CorradeMacros)

    # Installation dirs
    if(WIN32)
        set_parent_scope(CORRADE_BINARY_INSTALL_DIR .)
        set_parent_scope(CORRADE_LIBRARY_INSTALL_DIR .)
        set_parent_scope(CORRADE_CMAKE_MODULE_INSTALL_DIR Modules)
        set_parent_scope(CORRADE_INCLUDE_INSTALL_DIR include/Corrade)
    else()
        include(CorradeLibSuffix)
        set_parent_scope(CORRADE_BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
        set_parent_scope(CORRADE_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
        set_parent_scope(CORRADE_CMAKE_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
        set_parent_scope(CORRADE_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Corrade)
    endif()

    set_parent_scope(CORRADE_LIBRARIES ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
endif()
