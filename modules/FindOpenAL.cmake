#.rst:
# FindOpenAL
# ----------
#
#
#
# Locate OpenAL This module defines OPENAL_LIBRARY OPENAL_FOUND, if
# false, do not try to link to OpenAL OPENAL_INCLUDE_DIR, where to find
# the headers
#
# $OPENALDIR is an environment variable that would correspond to the
# ./configure --prefix=$OPENALDIR used in building OpenAL.
#
# Created by Eric Wing.  This was influenced by the FindSDL.cmake
# module.

#=============================================================================
# CMake - Cross Platform Makefile Generator
# Copyright 2000-2016 Kitware, Inc.
# Copyright 2000-2011 Insight Software Consortium
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

# This makes the presumption that you are include al.h like
# #include "al.h"
# and not
# #include <AL/al.h>
# The reason for this is that the latter is not entirely portable.
# Windows/Creative Labs does not by default put their headers in AL/ and
# OS X uses the convention <OpenAL/al.h>.
#
# For Windows, Creative Labs seems to have added a registry key for their
# OpenAL 1.1 installer. I have added that key to the list of search paths,
# however, the key looks like it could be a little fragile depending on
# if they decide to change the 1.00.0000 number for bug fix releases.
# Also, they seem to have laid down groundwork for multiple library platforms
# which puts the library in an extra subdirectory. Currently there is only
# Win32 and I have hardcoded that here. This may need to be adjusted as
# platforms are introduced.
# The OpenAL 1.0 installer doesn't seem to have a useful key I can use.
# I do not know if the Nvidia OpenAL SDK has a registry key.
#
# For OS X, remember that OpenAL was added by Apple in 10.4 (Tiger).
# To support the framework, I originally wrote special framework detection
# code in this module which I have now removed with CMake's introduction
# of native support for frameworks.
# In addition, OpenAL is open source, and it is possible to compile on Panther.
# Furthermore, due to bugs in the initial OpenAL release, and the
# transition to OpenAL 1.1, it is common to need to override the built-in
# framework.
# Per my request, CMake should search for frameworks first in
# the following order:
# ~/Library/Frameworks/OpenAL.framework/Headers
# /Library/Frameworks/OpenAL.framework/Headers
# /System/Library/Frameworks/OpenAL.framework/Headers
#
# On OS X, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of
# OPENAL_LIBRARY to override this selection or set the CMake environment
# CMAKE_INCLUDE_PATH to modify the search paths.

# This version is modified for Magnum and was forked from
# https://github.com/Kitware/CMake/blob/v3.6.1/Modules/FindOpenAL.cmake
# The file was modified to add a new path suffix for finding OpenAL for
# Emscripten on macOS. Additionally, in case of Emscripten, there's no library
# to find but instead one says -lopenal (and if MINIMAL_RUNTIME is not
# specified, this is implicit).

find_path(OPENAL_INCLUDE_DIR al.h
  HINTS
    ENV OPENALDIR
  # The AL was added in order to make the module working for Emscripten on macOS.
  # Not sure why include/AL wasn't enough.
  PATH_SUFFIXES include/AL include/OpenAL include AL
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir]
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(_OpenAL_ARCH_DIR libs/Win64)
else()
  set(_OpenAL_ARCH_DIR libs/Win32)
endif()

if(CORRADE_TARGET_EMSCRIPTEN)
    set(OPENAL_LIBRARY openal CACHE STRING "Path to a library." FORCE)
else()
  find_library(OPENAL_LIBRARY
    NAMES OpenAL al openal OpenAL32
    HINTS
      ENV OPENALDIR
    PATH_SUFFIXES lib64 lib libs64 libs ${_OpenAL_ARCH_DIR}
    PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /sw
    /opt/local
    /opt/csw
    /opt
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir]
  )
endif()

unset(_OpenAL_ARCH_DIR)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenAL DEFAULT_MSG OPENAL_LIBRARY OPENAL_INCLUDE_DIR)

mark_as_advanced(OPENAL_LIBRARY OPENAL_INCLUDE_DIR)
