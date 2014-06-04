# - Find Corrade
#
# Basic usage:
#  find_package(Corrade [REQUIRED])
# This module tries to find Corrade library and then defines:
#  CORRADE_FOUND                    - True if Corrade is found
#  CORRADE_INCLUDE_DIR              - Root include dir
#  CORRADE_INTERCONNECT_LIBRARIES   - Interconnect library and dependent
#   libraries
#  CORRADE_UTILITY_LIBRARIES        - Utility library and dependent
#   libraries
#  CORRADE_PLUGINMANAGER_LIBRARIES  - PluginManager library and dependent
#   libraries
#  CORRADE_TESTSUITE_LIBRARIES      - TestSuite library and dependent
#   libraries
#  CORRADE_RC_EXECUTABLE            - Resource compiler executable
# The package is found if either debug or release version of each library is
# found. If both debug and release libraries are found, proper version is
# chosen based on actual build configuration of the project (i.e. Debug build
# is linked to debug libraries, Release build to release libraries).
#
# On multi-configuration build systems (such as Visual Studio or XCode) the
# preprocessor variable CORRADE_IS_DEBUG_BUILD is defined if given build
# configuration is Debug (not Corrade itself, but build configuration of the
# project using it). Useful e.g. for selecting proper plugin directory. On
# single-configuration build systems (such as Makefiles) this information is
# not needed and thus the variable is not defined in any case.
#
# Corrade configures the compiler to use C++11 standard. Additionally you can
# use CORRADE_CXX_FLAGS to enable additional pedantic set of warnings and
# enable hidden visibility by default.
#
# Features of found Corrade library are exposed in these variables:
#  CORRADE_GCC47_COMPATIBILITY  - Defined if compiled with compatibility
#   mode for GCC 4.7
#  CORRADE_BUILD_DEPRECATED     - Defined if compiled with deprecated APIs
#   included
#  CORRADE_BUILD_STATIC         - Defined if compiled as static libraries
#  CORRADE_TARGET_UNIX          - Defined if compiled for some Unix flavor
#   (Linux, BSD, OS X)
#  CORRADE_TARGET_APPLE         - Defined if compiled for OS X
#  CORRADE_TARGET_WINDOWS       - Defined if compiled for Windows
#  CORRADE_TARGET_NACL          - Defined if compiled for Google Chrome
#   Native Client
#  CORRADE_TARGET_NACL_NEWLIB   - Defined if compiled for Google Chrome
#   Native Client with `newlib` toolchain
#  CORRADE_TARGET_NACL_GLIBC    - Defined if compiled for Google Chrome
#   Native Client with `glibc` toolchain
#  CORRADE_TARGET_EMSCRIPTEN    - Defined if compiled for Emscripten
#  CORRADE_TARGET_ANDROID       - Defined if compiled for Android
#
# If CORRADE_BUILD_DEPRECATED is defined, the CORRADE_INCLUDE_DIR variable also
# contains path directly to Corrade directory (i.e. for includes without
# `Corrade/` prefix).
#
# Corrade provides these macros and functions:
#
#
# Add unit test using Corrade's TestSuite.
#  corrade_add_test(test_name
#                   sources...
#                   [LIBRARIES libraries...])
# Test name is also executable name. You can also specify libraries to link
# with instead of using target_link_libraries(). CORRADE_TESTSUITE_LIBRARIES
# are linked automatically to each test. Note that the enable_testing()
# function must be called explicitly.
#
#
# Compile data resources into application binary.
#  corrade_add_resource(name resources.conf)
# Depends on corrade-rc, which is part of Corrade utilities. This command
# generates resource data using given configuration file in current build
# directory. Argument name is name under which the resources can be explicitly
# loaded. Variable `name` contains compiled resource filename, which is then
# used for compiling library / executable. Example usage:
#  corrade_add_resource(app_resources resources.conf)
#  add_executable(app source1 source2 ... ${app_resources})
#
# Add dynamic plugin.
#  corrade_add_plugin(plugin_name debug_install_dir release_install_dir
#                     metadata_file sources...)
# The macro adds preprocessor directive CORRADE_DYNAMIC_PLUGIN. Additional
# libraries can be linked in via target_link_libraries(plugin_name ...). If
# debug_install_dir is set to CMAKE_CURRENT_BINARY_DIR (e.g. for testing
# purposes), the files are copied directly, without the need to perform install
# step. Note that the files are actually put into configuration-based
# subdirectory, i.e. ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}. See
# documentation of CMAKE_CFG_INTDIR variable for more information.
#
#
# Add static plugin.
#  corrade_add_static_plugin(plugin_name install_dir metadata_file
#                            sources...)
# The macro adds preprocessor directive CORRADE_STATIC_PLUGIN. Additional
# libraries can be linked in via target_link_libraries(plugin_name ...). If
# install_dir is set to CMAKE_CURRENT_BINARY_DIR (e.g. for testing purposes),
# no installation rules are added.
#
# Note that plugins built in debug configuration (e.g. with CMAKE_BUILD_TYPE
# set to Debug) have "-d" suffix to make it possible to have both debug and
# release plugins installed alongside each other.
#
#
# Additionally these variables are defined for internal usage:
#  CORRADE_INTERCONNECT_LIBRARY     - Interconnect library (w/o
#   dependencies)
#  CORRADE_UTILITY_LIBRARY          - Utility library (w/o dependencies)
#  CORRADE_PLUGINMANAGER_LIBRARY    - Plugin manager library (w/o
#   dependencies)
#  CORRADE_TESTSUITE_LIBRARY        - TestSuite library (w/o dependencies)
#  CORRADE_*_LIBRARY_DEBUG          - Debug version of given library, if found
#  CORRADE_*_LIBRARY_RELEASE        - Release version of given library, if
#                                     found
#

#
#   This file is part of Corrade.
#
#   Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014
#             Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

# Libraries
foreach(_component Interconnect Utility PluginManager TestSuite)
    string(TOUPPER ${_component} _COMPONENT)

    # Try to find both debug and release version
    find_library(CORRADE_${_COMPONENT}_LIBRARY_DEBUG Corrade${_component}-d)
    find_library(CORRADE_${_COMPONENT}_LIBRARY_RELEASE Corrade${_component})

    # Set the _LIBRARY variable based on what was found
    if(CORRADE_${_COMPONENT}_LIBRARY_DEBUG AND CORRADE_${_COMPONENT}_LIBRARY_RELEASE)
        set(CORRADE_${_COMPONENT}_LIBRARY
            debug ${CORRADE_${_COMPONENT}_LIBRARY_DEBUG}
            optimized ${CORRADE_${_COMPONENT}_LIBRARY_RELEASE})
    elseif(CORRADE_${_COMPONENT}_LIBRARY_DEBUG)
        set(CORRADE_${_COMPONENT}_LIBRARY ${CORRADE_${_COMPONENT}_LIBRARY_DEBUG})
    elseif(CORRADE_${_COMPONENT}_LIBRARY_RELEASE)
        set(CORRADE_${_COMPONENT}_LIBRARY ${CORRADE_${_COMPONENT}_LIBRARY_RELEASE})
    endif()

    mark_as_advanced(CORRADE_${_COMPONENT}_LIBRARY_DEBUG
        CORRADE_${_COMPONENT}_LIBRARY_RELEASE
        CORRADE_${_COMPONENT}_LIBRARY)
endforeach()

# RC executable
find_program(CORRADE_RC_EXECUTABLE corrade-rc)

# Include dir
find_path(_CORRADE_INCLUDE_DIR
    NAMES Corrade/PluginManager Corrade/Utility)

# CMake module dir
find_path(_CORRADE_MODULE_DIR
    NAMES UseCorrade.cmake CorradeLibSuffix.cmake
    PATH_SUFFIXES share/cmake/Corrade)

# If not found, try system module dir
find_path(_CORRADE_MODULE_DIR
    NAMES UseCorrade.cmake CorradeLibSuffix.cmake
    PATHS ${CMAKE_ROOT}/Modules
    NO_CMAKE_FIND_ROOT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Corrade DEFAULT_MSG
    CORRADE_UTILITY_LIBRARY
    CORRADE_INTERCONNECT_LIBRARY
    CORRADE_PLUGINMANAGER_LIBRARY
    CORRADE_TESTSUITE_LIBRARY
    _CORRADE_INCLUDE_DIR
    CORRADE_RC_EXECUTABLE
    _CORRADE_MODULE_DIR)

if(NOT CORRADE_FOUND)
    return()
endif()

# Configuration
file(READ ${_CORRADE_INCLUDE_DIR}/Corrade/configure.h _corradeConfigure)

# Compatibility?
string(FIND "${_corradeConfigure}" "#define CORRADE_GCC47_COMPATIBILITY" _GCC47_COMPATIBILITY)
if(NOT _GCC47_COMPATIBILITY EQUAL -1)
    set(CORRADE_GCC47_COMPATIBILITY 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_BUILD_DEPRECATED" _BUILD_DEPRECATED)
if(NOT _BUILD_DEPRECATED EQUAL -1)
    set(CORRADE_BUILD_DEPRECATED 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_BUILD_STATIC" _BUILD_STATIC)
if(NOT _BUILD_STATIC EQUAL -1)
    set(CORRADE_BUILD_STATIC 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_TARGET_UNIX" _TARGET_UNIX)
if(NOT _TARGET_UNIX EQUAL -1)
    set(CORRADE_TARGET_UNIX 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_TARGET_APPLE" _TARGET_APPLE)
if(NOT _TARGET_APPLE EQUAL -1)
    set(CORRADE_TARGET_APPLE 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_TARGET_WINDOWS" _TARGET_WINDOWS)
if(NOT _TARGET_WINDOWS EQUAL -1)
    set(CORRADE_TARGET_WINDOWS 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_TARGET_NACL" _TARGET_NACL)
if(NOT _TARGET_NACL EQUAL -1)
    set(CORRADE_TARGET_NACL 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_TARGET_NACL_NEWLIB" _TARGET_NACL_NEWLIB)
if(NOT _TARGET_NACL_NEWLIB EQUAL -1)
    set(CORRADE_TARGET_NACL_NEWLIB 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_TARGET_NACL_GLIBC" _TARGET_NACL_GLIBC)
if(NOT _TARGET_NACL_GLIBC EQUAL -1)
    set(CORRADE_TARGET_NACL_GLIBC 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_TARGET_EMSCRIPTEN" _TARGET_EMSCRIPTEN)
if(NOT _TARGET_EMSCRIPTEN EQUAL -1)
    set(CORRADE_TARGET_EMSCRIPTEN 1)
endif()
string(FIND "${_corradeConfigure}" "#define CORRADE_TARGET_ANDROID" _TARGET_ANDROID)
if(NOT _TARGET_ANDROID EQUAL -1)
    set(CORRADE_TARGET_ANDROID 1)
endif()

set(CORRADE_UTILITY_LIBRARIES ${CORRADE_UTILITY_LIBRARY})
set(CORRADE_INTERCONNECT_LIBRARIES ${CORRADE_INTERCONNECT_LIBRARY} ${CORRADE_UTILITY_LIBRARIES})
set(CORRADE_PLUGINMANAGER_LIBRARIES ${CORRADE_PLUGINMANAGER_LIBRARY} ${CORRADE_UTILITY_LIBRARIES})
set(CORRADE_TESTSUITE_LIBRARIES ${CORRADE_TESTSUITE_LIBRARY} ${CORRADE_UTILITY_LIBRARIES})

# At least static build needs this
if(CORRADE_TARGET_UNIX OR CORRADE_TARGET_NACL_GLIBC)
    set(CORRADE_PLUGINMANAGER_LIBRARIES ${CORRADE_PLUGINMANAGER_LIBRARIES} ${CMAKE_DL_LIBS})
endif()

# AndroidLogStreamBuffer class needs to be linked to log library
if(CORRADE_TARGET_ANDROID)
    set(CORRADE_UTILITY_LIBRARIES ${CORRADE_UTILITY_LIBRARIES} log)
endif()

mark_as_advanced(_CORRADE_INCLUDE_DIR
    _CORRADE_MODULE_DIR)

# Add Corrade dir to include path if this is deprecated build
if(CORRADE_BUILD_DEPRECATED)
    set(CORRADE_INCLUDE_DIR ${_CORRADE_INCLUDE_DIR} ${CORRADE_INCLUDE_DIR}/Corrade)
else()
    set(CORRADE_INCLUDE_DIR ${_CORRADE_INCLUDE_DIR})
endif()

# Include our module dir, if we have any
if(NOT "${_CORRADE_MODULE_DIR}" STREQUAL "${CMAKE_ROOT}/Modules")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${_CORRADE_MODULE_DIR}")
endif()

# Finalize the finding process
include(UseCorrade)
