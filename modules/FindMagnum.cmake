# - Find Magnum
#
# Basic usage:
#  find_package(Magnum [REQUIRED])
# This command tries to find Magnum library and then defines:
#  MAGNUM_FOUND                 - Whether the library was found
#  MAGNUM_TARGET_GLES           - Defined if Magnum was built for OpenGL
#   ES, slightly reducing feature count. The same variable is also
#   #defined in Magnum headers.
#  MAGNUM_LIBRARIES             - Magnum library and dependent libraries
#  MAGNUM_INCLUDE_DIRS          - Root include dir and include dirs of
#   dependencies
#  MAGNUM_PLUGINS_IMPORTER_DIR  - Directory with importer plugins
# This command will try to find only the base library, not the optional
# components. The base library depends on Corrade, OpenGL and GLEW
# libraries. Additional dependencies are specified by the components. The
# optional components are:
#  DebugTools    - DebugTools library (depends on MeshTools, Physics,
#                  Primitives, SceneGraph and Shaders components)
#  MeshTools     - MeshTools library
#  Physics       - Physics library
#  Primitives    - Library with stock geometric primitives (static)
#  SceneGraph    - Scene graph library
#  Shaders       - Library with stock shaders
#  Text          - Text rendering library
#  TextureTools  - TextureTools library
#  GlxApplication - GLX application (depends on X11 libraries)
#  XEglApplication - X/EGL application (depends on EGL and X11 libraries)
#  WindowlessGlxApplication - Windowless GLX application (depends on X11
#   libraries)
#  GlutApplication - GLUT application (depends on GLUT library)
#  Sdl2Application - SDL2 application (depends on SDL2 library)
#  NaClApplication - NaCl application (only if targetting Google Chrome
#   Native Client)
# Example usage with specifying additional components is:
#  find_package(Magnum [REQUIRED|COMPONENTS]
#               MeshTools Primitives GlutApplication)
# For each component is then defined:
#  MAGNUM_*_FOUND   - Whether the component was found
#  MAGNUM_*_LIBRARIES - Component library and dependent libraries
#  MAGNUM_*_INCLUDE_DIRS - Include dirs of module dependencies
#
# Additionally these variables are defined for internal usage:
#  MAGNUM_INCLUDE_DIR                   - Root include dir (w/o
#   dependencies)
#  MAGNUM_LIBRARY                       - Magnum library (w/o
#   dependencies)
#  MAGNUM_*_LIBRARY                     - Component libraries (w/o
#   dependencies)
#  MAGNUM_LIBRARY_INSTALL_DIR           - Library installation directory
#  MAGNUM_PLUGINS_INSTALL_DIR           - Plugin installation directory
#  MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR  - Importer plugin installation
#   directory
#  MAGNUM_CMAKE_MODULE_INSTALL_DIR      - Installation dir for CMake
#   modules
#  MAGNUM_INCLUDE_INSTALL_DIR           - Header installation directory
#  MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR   - Plugin header installation
#   directory
#

# Dependencies
find_package(Corrade REQUIRED)

# Magnum library
find_library(MAGNUM_LIBRARY Magnum)

# Root include dir
find_path(MAGNUM_INCLUDE_DIR
    NAMES Magnum.h
    PATH_SUFFIXES Magnum)

# Configuration
file(READ ${MAGNUM_INCLUDE_DIR}/magnumConfigure.h _magnumConfigure)

# Built for specific target?
string(FIND "${_magnumConfigure}" "#define MAGNUM_TARGET_GLES" _TARGET_GLES)
if(NOT _TARGET_GLES EQUAL -1)
    set(MAGNUM_TARGET_GLES 1)
endif()
string(FIND "${_magnumConfigure}" "#define MAGNUM_TARGET_GLES2" _TARGET_GLES2)
if(NOT _TARGET_GLES2 EQUAL -1)
    set(MAGNUM_TARGET_GLES2 1)
endif()
string(FIND "${_magnumConfigure}" "#define MAGNUM_TARGET_NACL" _TARGET_NACL)
if(NOT _TARGET_NACL EQUAL -1)
    set(MAGNUM_TARGET_NACL 1)
endif()

if(NOT MAGNUM_TARGET_GLES)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
else()
    find_package(OpenGLES2 REQUIRED)
endif()

# On Windows, *Application libraries need to have ${MAGNUM_LIBRARY} listed
# in dependencies also after *Application.lib static library name to avoid
# linker errors
if(WIN32)
    set(_WINDOWCONTEXT_MAGNUM_LIBRARY_DEPENDENCY ${MAGNUM_LIBRARY})
endif()

# Additional components
foreach(component ${Magnum_FIND_COMPONENTS})
    string(TOUPPER ${component} _COMPONENT)

    # Find the library
    find_library(MAGNUM_${_COMPONENT}_LIBRARY Magnum${component})

    set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX ${component})

    # Applications
    if(${component} MATCHES .+Application)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX Platform)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES ${component}.h)

        # GLUT application dependencies
        if(${component} STREQUAL GlutApplication)
            find_package(GLUT)
            if(GLUT_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${GLUT_LIBRARIES} ${_WINDOWCONTEXT_MAGNUM_LIBRARY_DEPENDENCY})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()

        # SDL2 application dependencies
        if(${component} STREQUAL Sdl2Application)
            find_package(SDL2)
            if(SDL2_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${SDL2_LIBRARY} ${_WINDOWCONTEXT_MAGNUM_LIBRARY_DEPENDENCY})
                set(_MAGNUM_${_COMPONENT}_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()

        # NaCl application has no additional dependencies

        # GLX application dependencies
        if(${component} STREQUAL GlxApplication)
            find_package(X11)
            if(X11_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${X11_LIBRARIES} ${_WINDOWCONTEXT_MAGNUM_LIBRARY_DEPENDENCY})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()

        # X/EGL application dependencies
        if(${component} STREQUAL XEglApplication)
            find_package(EGL)
            find_package(X11)
            if(EGL_FOUND AND X11_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${EGL_LIBRARY} ${X11_LIBRARIES} ${_WINDOWCONTEXT_MAGNUM_LIBRARY_DEPENDENCY})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()

        # Windowless GLX application dependencies
        if(${component} STREQUAL WindowlessGlxApplication)
            find_package(X11)
            if(X11_FOUND)
                set(_MAGNUM_${_COMPONENT}_LIBRARIES ${X11_LIBRARIES} ${_WINDOWCONTEXT_MAGNUM_LIBRARY_DEPENDENCY})
            else()
                unset(MAGNUM_${_COMPONENT}_LIBRARY)
            endif()
        endif()
    endif()

    # DebugTools library
    if(${component} STREQUAL DebugTools)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Profiler.h)
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

    # Scene graph library
    if(${component} STREQUAL SceneGraph)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Scene.h)
    endif()

    # Shaders library
    if(${component} STREQUAL Shaders)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES PhongShader.h)
    endif()

    # Text library
    if(${component} STREQUAL Text)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Font.h)
    endif()

    # TextureTools library
    if(${component} STREQUAL TextureTools)
        set(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES Atlas.h)
    endif()

    # Try to find the includes
    if(_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES)
        find_path(_MAGNUM_${_COMPONENT}_INCLUDE_DIR
            NAMES ${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_NAMES}
            PATHS ${MAGNUM_INCLUDE_DIR}/${_MAGNUM_${_COMPONENT}_INCLUDE_PATH_SUFFIX})
    endif()

    # Decide if the library was found
    if(MAGNUM_${_COMPONENT}_LIBRARY AND _MAGNUM_${_COMPONENT}_INCLUDE_DIR)
        set(MAGNUM_${_COMPONENT}_LIBRARIES ${MAGNUM_${_COMPONENT}_LIBRARY} ${_MAGNUM_${_COMPONENT}_LIBRARIES})
        set(MAGNUM_${_COMPONENT}_INCLUDE_DIRS ${_MAGNUM_${_COMPONENT}_INCLUDE_DIRS})

        set(Magnum_${component}_FOUND TRUE)

        # Don't expose variables w/o dependencies to end users
        mark_as_advanced(FORCE MAGNUM_${_COMPONENT}_LIBRARY _MAGNUM_${_COMPONENT}_INCLUDE_DIR)
    else()
        set(Magnum_${component}_FOUND FALSE)
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Magnum
    REQUIRED_VARS MAGNUM_LIBRARY MAGNUM_INCLUDE_DIR
    HANDLE_COMPONENTS)

# Dependent libraries and includes
set(MAGNUM_INCLUDE_DIRS ${MAGNUM_INCLUDE_DIR}
    ${MAGNUM_INCLUDE_DIR}/external
    ${CORRADE_INCLUDE_DIR})
set(MAGNUM_LIBRARIES ${MAGNUM_LIBRARY}
    ${CORRADE_UTILITY_LIBRARY}
    ${CORRADE_PLUGINMANAGER_LIBRARY})
if(NOT MAGNUM_TARGET_GLES)
    set(MAGNUM_LIBRARIES ${MAGNUM_LIBRARIES}
        ${OPENGL_gl_LIBRARY}
        ${GLEW_LIBRARY})
else()
    set(MAGNUM_LIBRARIES ${MAGNUM_LIBRARIES}
        ${OPENGLES2_LIBRARY})
endif()

# Installation dirs
set(MAGNUM_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(MAGNUM_PLUGINS_INSTALL_DIR ${MAGNUM_LIBRARY_INSTALL_DIR}/magnum)
set(MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/importers)
set(MAGNUM_CMAKE_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
set(MAGNUM_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum)
set(MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Magnum/Plugins)
mark_as_advanced(FORCE
    MAGNUM_LIBRARY
    MAGNUM_INCLUDE_DIR
    MAGNUM_LIBRARY_INSTALL_DIR
    MAGNUM_PLUGINS_INSTALL_DIR
    MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR
    MAGNUM_CMAKE_MODULE_INSTALL_DIR
    MAGNUM_INCLUDE_INSTALL_DIR
    MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR)

# Importer plugins dir
if(NOT WIN32)
    set(MAGNUM_PLUGINS_IMPORTER_DIR ${MAGNUM_PLUGINS_INSTALL_DIR}/importers)
else()
    set(MAGNUM_PLUGINS_IMPORTER_DIR importers)
endif()
