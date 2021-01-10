#.rst:
# Find Corrade
# ------------
#
# Finds the Corrade library. Basic usage::
#
#  find_package(Corrade REQUIRED)
#
# This module tries to find the base Corrade library and then defines the
# following:
#
#  Corrade_FOUND                  - Whether the base library was found
#  CORRADE_LIB_SUFFIX_MODULE      - Path to CorradeLibSuffix.cmake module
#  CORRADE_INCLUDE_INSTALL_PREFIX - Prefix where to put platform-independent
#   include and other files, defaults to ``.``. If a relative path is used,
#   it's relative to :variable:`CMAKE_INSTALL_PREFIX`.
#
# This command will try to find only the base library, not the optional
# components, which are:
#
#  Containers                   - Containers library
#  PluginManager                - PluginManager library
#  TestSuite                    - TestSuite library
#  Utility                      - Utility library
#  rc                           - corrade-rc executable
#
# Example usage with specifying additional components is::
#
#  find_package(Corrade REQUIRED Utility TestSuite)
#
# For each component is then defined:
#
#  Corrade_*_FOUND              - Whether the component was found
#  Corrade::*                   - Component imported target
#
# The package is found if either debug or release version of each library is
# found. If both debug and release libraries are found, proper version is
# chosen based on actual build configuration of the project (i.e. Debug build
# is linked to debug libraries, Release build to release libraries).
#
# Corrade conditionally defines ``CORRADE_IS_DEBUG_BUILD`` preprocessor
# variable in case build configuration is ``Debug`` (not Corrade itself, but
# build configuration of the project using it). Useful e.g. for selecting
# proper plugin directory.
#
# Corrade defines the following custom target properties:
#
#  CORRADE_CXX_STANDARD         - C++ standard to require when compiling given
#   target. Does nothing if :variable:`CMAKE_CXX_FLAGS` already contains
#   particular standard setting flag or if given target contains
#   :prop_tgt:`CMAKE_CXX_STANDARD` property. Allowed value is 11, 14 or 17.
#  INTERFACE_CORRADE_CXX_STANDARD - C++ standard to require when using given
#   target. Does nothing if :variable:`CMAKE_CXX_FLAGS` already contains
#   particular standard setting flag or if given target contains
#   :prop_tgt:`CMAKE_CXX_STANDARD` property. Allowed value is 11, 14 or 17.
#  CORRADE_USE_PEDANTIC_FLAGS   - Enable additional compiler/linker flags.
#   Boolean.
#
# These properties are inherited from directory properties, meaning that if you
# set them on directories, they get implicitly set on all targets in given
# directory (with a possibility to do target-specific overrides). All Corrade
# libraries have the :prop_tgt:`INTERFACE_CORRADE_CXX_STANDARD` property set to
# 11, meaning that you will always have at least C++11 enabled once you link to
# any Corrade library.
#
# Features of found Corrade library are exposed in these variables:
#
#  CORRADE_MSVC2019_COMPATIBILITY - Defined if compiled with compatibility
#   mode for MSVC 2019
#  CORRADE_MSVC2017_COMPATIBILITY - Defined if compiled with compatibility
#   mode for MSVC 2017
#  CORRADE_MSVC2015_COMPATIBILITY - Defined if compiled with compatibility
#   mode for MSVC 2015
#  CORRADE_BUILD_DEPRECATED     - Defined if compiled with deprecated APIs
#   included
#  CORRADE_BUILD_STATIC         - Defined if compiled as static libraries.
#   Default are shared libraries.
#  CORRADE_BUILD_STATIC_UNIQUE_GLOBALS - Defined if static libraries keep their
#   globals unique even across different shared libraries. Enabled by default
#   for static builds.
#  CORRADE_BUILD_MULTITHREADED  - Defined if compiled in a way that makes it
#   possible to safely use certain Corrade features simultaneously in multiple
#   threads
#  CORRADE_TARGET_UNIX          - Defined if compiled for some Unix flavor
#   (Linux, BSD, macOS)
#  CORRADE_TARGET_APPLE         - Defined if compiled for Apple platforms
#  CORRADE_TARGET_IOS           - Defined if compiled for iOS (device or
#   simulator)
#  CORRADE_TARGET_IOS_SIMULATOR - Defined if compiled for iOS Simulator
#  CORRADE_TARGET_WINDOWS       - Defined if compiled for Windows
#  CORRADE_TARGET_WINDOWS_RT    - Defined if compiled for Windows RT
#  CORRADE_TARGET_EMSCRIPTEN    - Defined if compiled for Emscripten
#  CORRADE_TARGET_ANDROID       - Defined if compiled for Android
#  CORRADE_TARGET_GCC           - Defined if compiling with GCC or GCC-
#   compatible Clang
#  CORRADE_TARGET_CLANG         - Defined if compiling with Clang or any of its
#   variants
#  CORRADE_TARGET_APPLE_CLANG   - Defined if compiling with Apple's Clang
#  CORRADE_TARGET_CLANG_CL      - Defined if compiling with Clang-CL (Clang
#   with a MSVC frontend)
#  CORRADE_TARGET_MSVC          - Defined if compiling with MSVC or Clang with
#   a MSVC frontend
#  CORRADE_TARGET_MINGW         - Defined if compiling under MinGW
#  CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT - Defined if PluginManager
#   doesn't support dynamic plugin loading due to platform limitations
#  CORRADE_TESTSUITE_TARGET_XCTEST - Defined if TestSuite is targetting Xcode
#   XCTest
#  CORRADE_UTILITY_USE_ANSI_COLORS - Defined if ANSI escape sequences are used
#   for colored output with Utility::Debug on Windows
#
# Additionally these variables are defined for internal usage:
#
#  CORRADE_INCLUDE_DIR          - Root include dir
#  CORRADE_*_LIBRARY_DEBUG      - Debug version of given library, if found
#  CORRADE_*_LIBRARY_RELEASE    - Release version of given library, if found
#  CORRADE_*_EXECUTABLE         - Location of given executable, if found
#  CORRADE_USE_MODULE           - Path to UseCorrade.cmake module (included
#   automatically)
#  CORRADE_TESTSUITE_XCTEST_RUNNER - Path to XCTestRunner.mm.in file
#  CORRADE_TESTSUITE_ADB_RUNNER - Path to AdbRunner.sh file
#  CORRADE_PEDANTIC_COMPILER_OPTIONS - List of pedantic compiler options used
#   for targets with :prop_tgt:`CORRADE_USE_PEDANTIC_FLAGS` enabled
#  CORRADE_PEDANTIC_COMPILER_DEFINITIONS - List of pedantic compiler
#   definitions used for targets with :prop_tgt:`CORRADE_USE_PEDANTIC_FLAGS`
#   enabled
#  CORRADE_CXX{11,14,17,20}_STANDARD_FLAG - Compiler flag to use for targeting
#   C++11, 14, 17 or 20 in cases where it's not possible to use
#   :prop_tgt:`CORRADE_CXX_STANDARD`. Not defined if a standard switch is
#   already present in :variable:`CMAKE_CXX_FLAGS`.
#
# Corrade provides these macros and functions:
#
# .. command:: corrade_add_test
#
# Add unit test using Corrade's TestSuite::
#
#  corrade_add_test(<test name>
#                   <sources>...
#                   [LIBRARIES <libraries>...]
#                   [FILES <files>...]
#                   [ARGUMENTS <arguments>...])
#
# Test name is also executable name. You can use ``LIBRARIES`` to specify
# libraries to link with instead of using :command:`target_link_libraries()`.
# The ``Corrade::TestSuite`` target is linked automatically to each test. Note
# that the :command:`enable_testing()` function must be called explicitly.
# Arguments passed after ``ARGUMENTS`` will be appended to the test
# command line. ``ARGUMENTS`` are supported everywhere except when
# ``CORRADE_TESTSUITE_TARGET_XCTEST`` is enabled.
#
# You can list files needed by the test in the ``FILES`` section. If given
# filename is relative, it is treated relatively to `CMAKE_CURRENT_SOURCE_DIR`.
# The files are added to the :prop_test:`REQUIRED_FILES` target property. On
# Emscripten they are bundled to the executable and available in the virtual
# filesystem root. On Android they are copied along the executable to the
# target. In case of Emscripten and Android, if the file is absolute or
# contains ``..``, only the leaf name is used. Alternatively you can have a
# filename formatted as ``<input>@<output>``, in which case the ``<input>`` is
# treated as local filesystem location and ``<output>`` as remote/virtual
# filesystem location. The remote location can't be absolute or contain ``..``
# / ``@`` characters.
#
# Unless :variable:`CORRADE_TESTSUITE_TARGET_XCTEST` is set, test cases on iOS
# targets are created as bundles with bundle identifier set to CMake project
# name by default. Use the cache variable :variable:`CORRADE_TESTSUITE_BUNDLE_IDENTIFIER_PREFIX`
# to change it to something else.
#
# .. command:: corrade_add_resource
#
# Compile data resources into application binary::
#
#  corrade_add_resource(<name> <resources.conf>)
#
# Depends on ``Corrade::rc``, which is part of Corrade utilities. This command
# generates resource data using given configuration file in current build
# directory. Argument name is name under which the resources can be explicitly
# loaded. Variable ``<name>`` contains compiled resource filename, which is
# then used for compiling library / executable. On CMake >= 3.1 the
# `resources.conf` file can contain UTF-8-encoded filenames. Example usage::
#
#  corrade_add_resource(app_resources resources.conf)
#  add_executable(app source1 source2 ... ${app_resources})
#
# .. command:: corrade_add_plugin
#
# Add dynamic plugin::
#
#  corrade_add_plugin(<plugin name>
#                     "<debug binary install dir>;<debug library install dir>"
#                     "<release binary install dir>;<release library install dir>"
#                     <metadata file>
#                     <sources>...)
#
# The macro adds a preprocessor directive ``CORRADE_DYNAMIC_PLUGIN`` when
# compiling ``<sources>``. Additional libraries can be linked in via
# :command:`target_link_libraries(plugin_name ...) <target_link_libraries>`.
# On DLL platforms, the plugin DLLs and metadata files are put into
# ``<debug binary install dir>`` / ``<release binary install dir>`` and the
# ``*.lib`` files into ``<debug library install dir>`` /
# ``<release library install dir>``. On non-DLL platforms everything is put
# into ``<debug library install dir>`` / ``<release library install dir>``.
#
# If the plugin interface disables plugin metadata files, the
# ``<metadata file>`` can be set to ``""``, in which case no metadata file is
# copied anywhere. Otherwise the metadata file is copied and renamed to
# ``<plugin name>``, retaining its original extension.
#
#  corrade_add_plugin(<plugin name>
#                     <debug install dir>
#                     <release install dir>
#                     <metadata file>
#                     <sources>...)
#
# Unline the above version this puts everything into ``<debug install dir>`` on
# both DLL and non-DLL platforms. If ``<debug install dir>`` is set to
# :variable:`CMAKE_CURRENT_BINARY_DIR` (e.g. for testing purposes), the files
# are copied directly, without the need to perform install step. Note that the
# files are actually put into configuration-based subdirectory, i.e.
# ``${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}``. See documentation of
# :variable:`CMAKE_CFG_INTDIR` variable for more information.
#
# .. command:: corrade_add_static_plugin
#
# Add static plugin::
#
#  corrade_add_static_plugin(<plugin name>
#                            "<binary install dir>;<library install dir>"
#                            <metadata file>
#                            <sources>...)
#
# The macro adds a preprocessor directive ``CORRADE_STATIC_PLUGIN`` when
# compiling ``<sources>``. Additional libraries can be linked in via
# :command:`target_link_libraries(plugin_name ...) <target_link_libraries>`.
# The ``<binary install dir>`` is ignored and included just for compatibility
# with the :command:`corrade_add_plugin` command, everything is installed into
# ``<library install dir>``. Note that plugins built in debug configuration
# (e.g. with :variable:`CMAKE_BUILD_TYPE` set to ``Debug``) have ``"-d"``
# suffix to make it possible to have both debug and release plugins installed
# alongside each other.
#
# If the plugin interface disables plugin metadata files, the
# ``<metadata file>`` can be set to ``""``, in which case no metadata file is
# used. Otherwise the metadata file is bundled and renamed to
# ``<plugin name>``, retaining its original extension.
#
#  corrade_add_static_plugin(<plugin name>
#                            <install dir>
#                            <metadata file>
#                            <sources>...)
#
# Equivalent to the above with ``<library install dir>`` set to ``<install dir>``.
# If ``<install dir>`` is set to :variable:`CMAKE_CURRENT_BINARY_DIR` (e.g. for
# testing purposes), no installation rules are added.
#
# .. command:: corrade_find_dlls_for_libs
#
# Find corresponding DLLs for library files::
#
#  corrade_find_dlls_for_libs(<output variable> <libs>...)
#
# Available only on Windows, for all ``*.lib`` files tries to find
# corresponding DLL file. Useful for bundling dependencies for e.g. WinRT
# packages.
#

#
#   This file is part of Corrade.
#
#   Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
#               2017, 2018, 2019, 2020, 2021
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

# Root include dir
find_path(CORRADE_INCLUDE_DIR
    NAMES Corrade/Corrade.h)
mark_as_advanced(CORRADE_INCLUDE_DIR)

# Configuration file
find_file(_CORRADE_CONFIGURE_FILE configure.h
    HINTS ${CORRADE_INCLUDE_DIR}/Corrade/)
mark_as_advanced(_CORRADE_CONFIGURE_FILE)

# We need to open configure.h file from CORRADE_INCLUDE_DIR before we check for
# the components. Bail out with proper error message if it wasn't found. The
# complete check with all components is further below.
if(NOT CORRADE_INCLUDE_DIR)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Corrade
        REQUIRED_VARS CORRADE_INCLUDE_DIR _CORRADE_CONFIGURE_FILE)
endif()

# Read flags from configuration
file(READ ${_CORRADE_CONFIGURE_FILE} _corradeConfigure)
string(REGEX REPLACE ";" "\\\\;" _corradeConfigure "${_corradeConfigure}")
string(REGEX REPLACE "\n" ";" _corradeConfigure "${_corradeConfigure}")
set(_corradeFlags
    MSVC2015_COMPATIBILITY
    MSVC2017_COMPATIBILITY
    MSVC2019_COMPATIBILITY
    BUILD_DEPRECATED
    BUILD_STATIC
    BUILD_STATIC_UNIQUE_GLOBALS
    BUILD_MULTITHREADED
    TARGET_UNIX
    TARGET_APPLE
    TARGET_IOS
    TARGET_IOS_SIMULATOR
    TARGET_WINDOWS
    TARGET_WINDOWS_RT
    TARGET_EMSCRIPTEN
    TARGET_ANDROID
    # TARGET_X86 etc and TARGET_LIBCXX are not exposed to CMake as the meaning
    # is unclear on platforms with multi-arch binaries or when mixing different
    # STL implementations. TARGET_GCC etc are figured out via UseCorrade.cmake,
    # as the compiler can be different when compiling the lib & when using it.
    PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    TESTSUITE_TARGET_XCTEST
    UTILITY_USE_ANSI_COLORS)
foreach(_corradeFlag ${_corradeFlags})
    list(FIND _corradeConfigure "#define CORRADE_${_corradeFlag}" _corrade_${_corradeFlag})
    if(NOT _corrade_${_corradeFlag} EQUAL -1)
        set(CORRADE_${_corradeFlag} 1)
    endif()
endforeach()

# CMake module dir
find_path(_CORRADE_MODULE_DIR
    NAMES UseCorrade.cmake CorradeLibSuffix.cmake
    PATH_SUFFIXES share/cmake/Corrade)
mark_as_advanced(_CORRADE_MODULE_DIR)

set(CORRADE_USE_MODULE ${_CORRADE_MODULE_DIR}/UseCorrade.cmake)
set(CORRADE_LIB_SUFFIX_MODULE ${_CORRADE_MODULE_DIR}/CorradeLibSuffix.cmake)

# Component distinction (listing them explicitly to avoid mistakes with finding
# unknown components)
set(_CORRADE_LIBRARY_COMPONENTS
    Containers Interconnect Main PluginManager TestSuite Utility)
set(_CORRADE_HEADER_ONLY_COMPONENTS Containers)
if(NOT CORRADE_TARGET_WINDOWS)
    # CorradeMain is a real library only on windows, a dummy target elsewhere
    list(APPEND _CORRADE_HEADER_ONLY_COMPONENTS Main)
endif()
set(_CORRADE_EXECUTABLE_COMPONENTS rc)
# Currently everything is enabled implicitly. Keep in sync with Corrade's root
# CMakeLists.txt.
set(_CORRADE_IMPLICITLY_ENABLED_COMPONENTS
    Containers Interconnect Main PluginManager TestSuite Utility rc)

# Inter-component dependencies
set(_CORRADE_Containers_DEPENDENCIES Utility)
set(_CORRADE_Interconnect_DEPENDENCIES Containers Utility)
set(_CORRADE_PluginManager_DEPENDENCIES Containers Utility rc)
set(_CORRADE_TestSuite_DEPENDENCIES Containers Utility Main) # see below
set(_CORRADE_Utility_DEPENDENCIES Containers rc)

# Ensure that all inter-component dependencies are specified as well
foreach(_component ${Corrade_FIND_COMPONENTS})
    # Mark the dependencies as required if the component is also required
    if(Corrade_FIND_REQUIRED_${_component})
        foreach(_dependency ${_CORRADE_${_component}_DEPENDENCIES})
            set(Corrade_FIND_REQUIRED_${_dependency} TRUE)
        endforeach()
    endif()

    list(APPEND _CORRADE_ADDITIONAL_COMPONENTS ${_CORRADE_${_component}_DEPENDENCIES})
endforeach()

# Main is linked only in corrade_add_test(), not to everything that depends on
# TestSuite, so remove it from the list again once we filled the above
# variables
set(_CORRADE_TestSuite_DEPENDENCIES Containers Utility)

# Join the lists, remove duplicate components
set(_CORRADE_ORIGINAL_FIND_COMPONENTS ${Corrade_FIND_COMPONENTS})
if(_CORRADE_ADDITIONAL_COMPONENTS)
    list(INSERT Corrade_FIND_COMPONENTS 0 ${_CORRADE_ADDITIONAL_COMPONENTS})
endif()
if(Corrade_FIND_COMPONENTS)
    list(REMOVE_DUPLICATES Corrade_FIND_COMPONENTS)
endif()

# Find all components
foreach(_component ${Corrade_FIND_COMPONENTS})
    string(TOUPPER ${_component} _COMPONENT)

    # Create imported target in case the library is found. If the project is
    # added as subproject to CMake, the target already exists and all the
    # required setup is already done from the build tree.
    if(TARGET Corrade::${_component})
        set(Corrade_${_component}_FOUND TRUE)
    else()
        # Library (and not header-only) components
        if(_component IN_LIST _CORRADE_LIBRARY_COMPONENTS AND NOT _component IN_LIST _CORRADE_HEADER_ONLY_COMPONENTS)
            add_library(Corrade::${_component} UNKNOWN IMPORTED)

            # Try to find both debug and release version
            find_library(CORRADE_${_COMPONENT}_LIBRARY_DEBUG Corrade${_component}-d)
            find_library(CORRADE_${_COMPONENT}_LIBRARY_RELEASE Corrade${_component})
            mark_as_advanced(CORRADE_${_COMPONENT}_LIBRARY_DEBUG
                CORRADE_${_COMPONENT}_LIBRARY_RELEASE)

            if(CORRADE_${_COMPONENT}_LIBRARY_RELEASE)
                set_property(TARGET Corrade::${_component} APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS RELEASE)
                set_property(TARGET Corrade::${_component} PROPERTY
                    IMPORTED_LOCATION_RELEASE ${CORRADE_${_COMPONENT}_LIBRARY_RELEASE})
            endif()

            if(CORRADE_${_COMPONENT}_LIBRARY_DEBUG)
                set_property(TARGET Corrade::${_component} APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS DEBUG)
                set_property(TARGET Corrade::${_component} PROPERTY
                    IMPORTED_LOCATION_DEBUG ${CORRADE_${_COMPONENT}_LIBRARY_DEBUG})
            endif()
        endif()

        # Header-only library components
        if(_component IN_LIST _CORRADE_HEADER_ONLY_COMPONENTS)
            add_library(Corrade::${_component} INTERFACE IMPORTED)
        endif()

        # Default include path names to look for for library / header-only
        # components
        if(_component IN_LIST _CORRADE_LIBRARY_COMPONENTS)
            set(_CORRADE_${_COMPONENT}_INCLUDE_PATH_SUFFIX Corrade/${_component})
            set(_CORRADE_${_COMPONENT}_INCLUDE_PATH_NAMES ${_component}.h)
        endif()

        # Executable components
        if(_component IN_LIST _CORRADE_EXECUTABLE_COMPONENTS)
            add_executable(Corrade::${_component} IMPORTED)

            find_program(CORRADE_${_COMPONENT}_EXECUTABLE corrade-${_component})
            mark_as_advanced(CORRADE_${_COMPONENT}_EXECUTABLE)

            if(CORRADE_${_COMPONENT}_EXECUTABLE)
                set_property(TARGET Corrade::${_component} PROPERTY
                    IMPORTED_LOCATION ${CORRADE_${_COMPONENT}_EXECUTABLE})
            endif()
        endif()

        # No special setup for Containers library

        # Interconnect library
        if(_component STREQUAL Interconnect)
            # Disable /OPT:ICF on MSVC, which merges functions with identical
            # contents and thus breaks signal comparison
            if(CORRADE_TARGET_WINDOWS AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                if(CMAKE_VERSION VERSION_LESS 3.13)
                    set_property(TARGET Corrade::${_component} PROPERTY
                        INTERFACE_LINK_LIBRARIES "-OPT:NOICF,REF")
                else()
                    set_property(TARGET Corrade::${_component} PROPERTY
                        INTERFACE_LINK_OPTIONS "/OPT:NOICF,REF")
                endif()
            endif()

        # Main library
        elseif(_component STREQUAL Main)
            set(_CORRADE_${_COMPONENT}_INCLUDE_PATH_SUFFIX Corrade)
            set(_CORRADE_${_COMPONENT}_INCLUDE_PATH_NAMES Corrade.h)

            if(CORRADE_TARGET_WINDOWS)
                if(NOT MINGW)
                    # Abusing INTERFACE_LINK_LIBRARIES because
                    # INTERFACE_LINK_OPTIONS is only since 3.13. They treat
                    # things with `-` in front as linker flags and fortunately
                    # I can use `-ENTRY` instead of `/ENTRY`.
                    # https://gitlab.kitware.com/cmake/cmake/issues/16543
                    set_property(TARGET Corrade::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES "-ENTRY:$<$<NOT:$<BOOL:$<TARGET_PROPERTY:WIN32_EXECUTABLE>>>:wmainCRTStartup>$<$<BOOL:$<TARGET_PROPERTY:WIN32_EXECUTABLE>>:wWinMainCRTStartup>")
                else()
                    set_property(TARGET Corrade::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES "-municode")
                endif()
            endif()

        # PluginManager library
        elseif(_component STREQUAL PluginManager)
            # -ldl is handled by Utility now

        # TestSuite library has some additional files
        elseif(_component STREQUAL TestSuite)
            # XCTest runner file
            if(CORRADE_TESTSUITE_TARGET_XCTEST)
                find_file(CORRADE_TESTSUITE_XCTEST_RUNNER XCTestRunner.mm.in
                    PATH_SUFFIXES share/corrade/TestSuite)
                set(CORRADE_TESTSUITE_XCTEST_RUNNER_NEEDED CORRADE_TESTSUITE_XCTEST_RUNNER)

            # ADB runner file
            elseif(CORRADE_TARGET_ANDROID)
                find_file(CORRADE_TESTSUITE_ADB_RUNNER AdbRunner.sh
                    PATH_SUFFIXES share/corrade/TestSuite)
                set(CORRADE_TESTSUITE_ADB_RUNNER_NEEDED CORRADE_TESTSUITE_ADB_RUNNER)

            # Emscripten runner file
            elseif(CORRADE_TARGET_EMSCRIPTEN)
                find_file(CORRADE_TESTSUITE_EMSCRIPTEN_RUNNER EmscriptenRunner.html.in
                    PATH_SUFFIXES share/corrade/TestSuite)
                set(CORRADE_TESTSUITE_EMSCRIPTEN_RUNNER_NEEDED CORRADE_TESTSUITE_EMSCRIPTEN_RUNNER)
            endif()

        # Utility library (contains all setup that is used by others)
        elseif(_component STREQUAL Utility)
            # Top-level include directory
            set_property(TARGET Corrade::${_component} APPEND PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES ${CORRADE_INCLUDE_DIR})

            # Require (at least) C++11 for users
            set_property(TARGET Corrade::${_component} PROPERTY
                INTERFACE_CORRADE_CXX_STANDARD 11)
            set_property(TARGET Corrade::${_component} APPEND PROPERTY
                COMPATIBLE_INTERFACE_NUMBER_MAX CORRADE_CXX_STANDARD)

            # Directory::libraryLocation() needs this
            if(CORRADE_TARGET_UNIX)
                set_property(TARGET Corrade::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES ${CMAKE_DL_LIBS})
            endif()
            # AndroidLogStreamBuffer class needs to be linked to log library
            if(CORRADE_TARGET_ANDROID)
                set_property(TARGET Corrade::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES "log")
            endif()
        endif()

        # Find library includes
        if(_component IN_LIST _CORRADE_LIBRARY_COMPONENTS)
            find_path(_CORRADE_${_COMPONENT}_INCLUDE_DIR
                NAMES ${_CORRADE_${_COMPONENT}_INCLUDE_PATH_NAMES}
                HINTS ${CORRADE_INCLUDE_DIR}/${_CORRADE_${_COMPONENT}_INCLUDE_PATH_SUFFIX})
            mark_as_advanced(_CORRADE_${_COMPONENT}_INCLUDE_DIR)
        endif()

        # Add inter-library dependencies
        if(_component IN_LIST _CORRADE_LIBRARY_COMPONENTS OR _component IN_LIST _CORRADE_HEADER_ONLY_COMPONENTS)
            foreach(_dependency ${_CORRADE_${_component}_DEPENDENCIES})
                if(_dependency IN_LIST _CORRADE_LIBRARY_COMPONENTS OR _dependency IN_LIST _CORRADE_HEADER_ONLY_COMPONENTS)
                    set_property(TARGET Corrade::${_component} APPEND PROPERTY
                        INTERFACE_LINK_LIBRARIES Corrade::${_dependency})
                endif()
            endforeach()
        endif()

        # Decide if the component was found
        if((_component IN_LIST _CORRADE_LIBRARY_COMPONENTS AND _CORRADE_${_COMPONENT}_INCLUDE_DIR AND (_component IN_LIST _CORRADE_HEADER_ONLY_COMPONENTS OR CORRADE_${_COMPONENT}_LIBRARY_RELEASE OR CORRADE_${_COMPONENT}_LIBRARY_DEBUG)) OR (_component IN_LIST _CORRADE_EXECUTABLE_COMPONENTS AND CORRADE_${_COMPONENT}_EXECUTABLE))
            set(Corrade_${_component}_FOUND TRUE)
        else()
            set(Corrade_${_component}_FOUND FALSE)
        endif()
    endif()
endforeach()

# For CMake 3.16+ with REASON_FAILURE_MESSAGE, provide additional potentially
# useful info about the failed components.
if(NOT CMAKE_VERSION VERSION_LESS 3.16)
    set(_CORRADE_REASON_FAILURE_MESSAGE )
    # Go only through the originally specified find_package() components, not
    # the dependencies added by us afterwards
    foreach(_component ${_CORRADE_ORIGINAL_FIND_COMPONENTS})
        if(Corrade_${_component}_FOUND)
            continue()
        endif()

        # If it's not known at all, tell the user -- it might be a new library
        # and an old Find module, or something platform-specific.
        if(NOT _component IN_LIST _CORRADE_LIBRARY_COMPONENTS AND NOT _component IN_LIST _CORRADE_EXECUTABLE_COMPONENTS)
            list(APPEND _CORRADE_REASON_FAILURE_MESSAGE "${_component} is not a known component on this platform.")
        # Otherwise, if it's not among implicitly built components, hint that
        # the user may need to enable it.
        # TODO: currently, the _FOUND variable doesn't reflect if dependencies
        #   were found. When it will, this needs to be updated to avoid
        #   misleading messages.
        elseif(NOT _component IN_LIST _CORRADE_IMPLICITLY_ENABLED_COMPONENTS)
            string(TOUPPER ${_component} _COMPONENT)
            list(APPEND _CORRADE_REASON_FAILURE_MESSAGE "${_component} is not built by default. Make sure you enabled WITH_${_COMPONENT} when building Corrade.")
        # Otherwise we have no idea. Better be silent than to print something
        # misleading.
        else()
        endif()
    endforeach()

    string(REPLACE ";" " " _CORRADE_REASON_FAILURE_MESSAGE "${_CORRADE_REASON_FAILURE_MESSAGE}")
    set(_CORRADE_REASON_FAILURE_MESSAGE REASON_FAILURE_MESSAGE "${_CORRADE_REASON_FAILURE_MESSAGE}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Corrade REQUIRED_VARS
    CORRADE_INCLUDE_DIR
    _CORRADE_MODULE_DIR
    _CORRADE_CONFIGURE_FILE
    ${CORRADE_TESTSUITE_XCTEST_RUNNER_NEEDED}
    ${CORRADE_TESTSUITE_ADB_RUNNER_NEEDED}
    ${CORRADE_TESTSUITE_EMSCRIPTEN_RUNNER_NEEDED}
    HANDLE_COMPONENTS
    ${_CORRADE_REASON_FAILURE_MESSAGE})

# Finalize the finding process
include(${CORRADE_USE_MODULE})

# Installation dirs
set(CORRADE_INCLUDE_INSTALL_PREFIX "."
    CACHE STRING "Prefix where to put platform-independent include and other files")

set(CORRADE_INCLUDE_INSTALL_DIR ${CORRADE_INCLUDE_INSTALL_PREFIX}/include/Corrade)
