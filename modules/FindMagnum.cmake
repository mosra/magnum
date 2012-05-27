# - Find Magnum
#
# Basic usage:
#
#  find_package(Magnum [REQUIRED])
#
# This command tries to find Magnum library and then defines:
#
#  MAGNUM_FOUND                 - Whether the library was found
#  MAGNUM_LIBRARY               - Magnum library
#  MAGNUM_INCLUDE_DIR           - Root include dir
#  MAGNUM_PLUGINS_IMPORTER_DIR  - Directory with importer plugins
#
# This command will try to find only the base library, not the optional
# components. The base library depends on Corrade, OpenGL and GLEW libraries.
# Additional dependencies are specified by the components. The optional
# components are:
#
#  GlutContext   - GLUT context (depends on GLUT package)
#  MeshTools     - MeshTools library
#  Physics       - Physics library
#  Primitives    - Library with stock geometric primitives (static)
#  Shaders       - Library with stock shaders
#
# Example usage with specifying additional components is:
#
#  find_package(Magnum [REQUIRED|COMPONENTS] MeshTools Primitives GlutContext)
#
# For each component is then defined:
#
#  MAGNUM_*_FOUND   - Whether the component was found
#  MAGNUM_*_LIBRARY - Component library
#
# Additionally these variables are defined for internal usage:
#
#  MAGNUM_LIBRARY_INSTALL_DIR           - Library installation directory
#  MAGNUM_PLUGINS_INSTALL_DIR           - Plugin installation directory
#  MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR  - Importer plugin installation directory
#  MAGNUM_CMAKE_MODULE_INSTALL_DIR      - Installation dir for CMake modules
#  MAGNUM_INCLUDE_INSTALL_DIR           - Header installation directory
#  MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR   - Plugin header installation directory
#

# Dependencies
find_package(Corrade REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Magnum library
find_library(MAGNUM_LIBRARY Magnum)

# Root include dir
find_path(MAGNUM_INCLUDE_DIR
    NAMES Magnum.h
    PATH_SUFFIXES Magnum
)

# Additional components
foreach(component ${Magnum_FIND_COMPONENTS})
    string(TOUPPER ${component} _COMPONENT)

    # Find the library
    find_library(MAGNUM_${_COMPONENT}_LIBRARY Magnum${component})

    set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX ${component})

    # Contexts
    if(${component} MATCHES .+Context)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Contexts)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES ${component}.h)

        # GLUT context dependencies
        if(${component} STREQUAL GlutContext)
            find_package(GLUT)
            if(NOT GLUT_FOUND)
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()

        # SDL2 context dependencies
        if(${component} STREQUAL Sdl2Context)
            find_package(SDL2)
            if(NOT SDL2_FOUND)
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()
    endif()

    # Mesh tools library
    if(${component} STREQUAL MeshTools)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES CompressIndices.h)
    endif()

    # Physics library
    if(${component} STREQUAL Physics)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES AbstractShape.h)
    endif()

    # Primitives library
    if(${component} STREQUAL Primitives)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Cube.h)
    endif()

    # Shaders library
    if(${component} STREQUAL Shaders)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES PhongShader.h)
    endif()

    # Try to find the includes
    if(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES)
        find_path(_MAGNUM_${_COMPONENT}_INCLUDE_DIR
            NAMES ${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES}
            PATHS ${MAGNUM_INCLUDE_DIR}/${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX}
        )
    endif()

    # Decide if the library was found
    if(MAGNUM_${_COMPONENT}_LIBRARY AND _MAGNUM_${_COMPONENT}_INCLUDE_DIR)
        set(Magnum_${component}_FOUND TRUE)
    else()
        set(Magnum_${component}_FOUND FALSE)
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Magnum
    REQUIRED_VARS MAGNUM_INCLUDE_DIR MAGNUM_LIBRARY
    HANDLE_COMPONENTS
)

# Installation dirs
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_PLUGINS_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum)
set(MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/importers)
set(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)
set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum/Plugins)
mark_as_advanced(FORCE
    MAGNUM_LIBRARY_INSTALL_DIR
    MAGNUM_PLUGINS_INSTALL_DIR
    MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR
    MAGNUM_CMAKE_MODULE_INSTALL_DIR
    MAGNUM_INCLUDE_INSTALL_DIR
    MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR
)

# Importer plugins dir
if(NOT WIN32)
    set(MAGNUM_PLUGINS_IMPORTER_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/importers)
else()
    set(MAGNUM_PLUGINS_IMPORTER_DIR importers)
endif()
