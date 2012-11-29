# - Find Corrade
#
# Basic usage:
#  find_package(Corrade [REQUIRED])
# This module tries to find Corrade library and then defines:
#  CORRADE_FOUND                    - True if Corrade library is found
#  CORRADE_INCLUDE_DIR              - Include dir for Corrade
#  CORRADE_UTILITY_LIBRARIES        - Corrade Utility library and dependent
#   libraries
#  CORRADE_PLUGINMANAGER_LIBRARIES  - Corrade Plugin manager library and
#   dependent libraries
#  CORRADE_TESTSUITE_LIBRARIES      - Corrade TestSuite library and dependent
#   libraries
#  CORRADE_RC_EXECUTABLE            - Corrade resource compiler executable
# Additionally these variables are defined for internal usage:
#  CORRADE_UTILITY_LIBRARY          - Corrade Utility library (w/o
#   dependencies)
#  CORRADE_PLUGINMANAGER_LIBRARY    - Corrade Plugin manager library (w/o
#   dependencies)
#  CORRADE_TESTSUITE_LIBRARY        - Corrade TestSuite library (w/o
#   dependencies)
# If Corrade library is found, these macros and functions are defined:
#
#
# Add unit test using Corrade's TestSuite.
#  corrade_add_test2(test_name
#                    sources...
#                    [LIBRARIES libraries...])
# Test name is also executable name. You can also specify libraries to link
# with instead of using target_link_libraries(). CORRADE_TESTSUITE_LIBRARIES
# are linked atuomatically to each test. Note that the enable_testing()
# function must be called explicitly.
#
#
# Add QtTest unit test.
#  corrade_add_test(test_name moc_header source_file
#                   [libraries...])
# These tests contain mainly from one source file and one header, which is
# processed by Qt meta-object compiler. The executable is then linked to QtCore
# and QtTest library, more libraries can be specified as another parameters.
# Test name is also executable name. Header file is processed with Qt's moc.
#
# Note: Before using this function you must find package Qt4. The
# enable_testing() function must be also called explicitly.
#
#
# Add QtTest unit test with multiple source files.
#  corrade_add_multifile_test(test_name
#                             moc_header_variable
#                             source_files_variable)
# Useful when there is need to compile more than one cpp/h file into the test.
#
# Example usage:
#  set(test_headers ComplexTest.h MyObject.h)
#  set(test_sources ComplexTest.cpp MyObject.cpp)
#  corrade_add_test(MyComplexTest test_headers test_sources
#                   CoreLibrary AnotherLibrary)
#
# Compile data resources into application binary.
#  corrade_add_resource(name group_name
#                       file [ALIAS alias]
#                       [file1 [ALIAS alias1]...])
# Depends on corrade-rc, which is part of Corrade utilities. This command
# generates resource file with group group_name from given files in current
# build directory. Argument name is name under which the resources can be
# explicitly loaded. Variable 'name' contains compiled resource filename,
# which is then used for compiling library / executable.
#
# Example usage:
#  corrade_add_resource(name group_name file1 ALIAS alias1 file2 file3)
#  add_executable(app source1 source2 ... ${name})
#
# Add dynamic plugin.
#  corrade_add_plugin(plugin_name install_dir metadata_file
#                     sources...)
# The macro adds preprocessor directive CORRADE_DYNAMIC_PLUGIN. Additional
# libraries can be linked in via target_link_libraries(plugin_name ...). If
# install_dir is set to CMAKE_CURRENT_BINARY_DIR (e.g. for testing purposes),
# the files are copied directly, without need to run 'make install'.
#
#
# Add static plugin.
#  corrade_add_static_plugin(static_plugins_variable
#                            plugin_name metadata_file
#                            sources...)
# The macro adds preprocessor directive CORRADE_STATIC_PLUGIN. Additional
# libraries can be linked in via target_link_libraries(plugin_name ...).
#
# Plugin library name will be added at the end of static_plugins_variable and
# the variable is meant to be used for linking plugins to main
# executable/library, e.g:
#  target_link_libraries(app lib1 lib2 ... ${static_plugins_variable})
#
# This variable is set with parent scope to be available in parent directory.
# If there are more intermediate directories between plugin directory and main
# executable directory, the variable can be propagated to parent scope like
# this:
#  set(static_plugins_variable ${static_plugins_variable} PARENT_SCOPE)
#
# Find and install DLLs for bundling with Windows build.
#  corrade_bundle_dlls(library_install_dir
#                      dlls...
#                      [PATHS paths...])
# It is possible to specify also additional paths for searching. DLL names can
# also contain paths, they will be installed into exact specified path. If an
# DLL is not found, fatal error message is printed.
#

# Libraries
find_library(CORRADE_UTILITY_LIBRARY CorradeUtility)
find_library(CORRADE_PLUGINMANAGER_LIBRARY CorradePluginManager)
find_library(CORRADE_TESTSUITE_LIBRARY CorradeTestSuite)

# RC executable
find_program(CORRADE_RC_EXECUTABLE corrade-rc)

# Paths
find_path(CORRADE_INCLUDE_DIR
    NAMES PluginManager Utility
    PATH_SUFFIXES Corrade)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Corrade DEFAULT_MSG
    CORRADE_UTILITY_LIBRARY
    CORRADE_PLUGINMANAGER_LIBRARY
    CORRADE_TESTSUITE_LIBRARY
    CORRADE_INCLUDE_DIR
    CORRADE_RC_EXECUTABLE)

if(NOT CORRADE_FOUND)
    return()
endif()

# Configuration
file(READ ${CORRADE_INCLUDE_DIR}/corradeConfigure.h _corradeConfigure)

# Compatibility?
string(FIND "${_corradeConfigure}" "#define CORRADE_GCC46_COMPATIBILITY" _GCC46_COMPATIBILITY)
if(NOT _GCC46_COMPATIBILITY EQUAL -1)
    set(CORRADE_GCC46_COMPATIBILITY 1)
endif()

set(CORRADE_UTILITY_LIBRARIES ${CORRADE_UTILITY_LIBRARY})
set(CORRADE_PLUGINMANAGER_LIBRARIES ${CORRADE_PLUGINMANAGER_LIBRARY} ${CORRADE_UTILITY_LIBRARIES})
set(CORRADE_TESTSUITE_LIBRARIES ${CORRADE_TESTSUITE_LIBRARY} ${CORRADE_UTILITY_LIBRARIES})
mark_as_advanced(CORRADE_UTILITY_LIBRARY CORRADE_PLUGINMANAGER_LIBRARY CORRADE_TESTSUITE_LIBRARY)

include(CorradeMacros)
include(CorradeLibSuffix)
