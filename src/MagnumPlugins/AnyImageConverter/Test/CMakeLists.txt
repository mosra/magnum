#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
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

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(ANYIMAGECONVERTER_TEST_DIR "write")
else()
    set(ANYIMAGECONVERTER_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()

# CMake before 3.8 has broken $<TARGET_FILE*> expressions for iOS (see
# https://gitlab.kitware.com/cmake/cmake/merge_requests/404) and since Corrade
# doesn't support dynamic plugins on iOS, this sorta works around that. Should
# be revisited when updating Travis to newer Xcode (xcode7.3 has CMake 3.6).
if(NOT MAGNUM_ANYIMAGECONVERTER_BUILD_STATIC)
    set(ANYIMAGECONVERTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageConverter>)
    if(WITH_TGAIMAGECONVERTER)
        set(TGAIMAGECONVERTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImageConverter>)
    endif()
endif()

# First replace ${} variables, then $<> generator expressions
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

corrade_add_test(AnyImageConverterTest AnyImageConverterTest.cpp
    LIBRARIES MagnumTrade)
target_include_directories(AnyImageConverterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
if(MAGNUM_ANYIMAGECONVERTER_BUILD_STATIC)
    target_link_libraries(AnyImageConverterTest PRIVATE AnyImageConverter)
    if(WITH_TGAIMAGECONVERTER)
        target_link_libraries(AnyImageConverterTest PRIVATE TgaImageConverter)
    endif()
else()
    # So the plugins get properly built when building the test
    add_dependencies(AnyImageConverterTest AnyImageConverter)
    if(WITH_TGAIMAGECONVERTER)
        add_dependencies(AnyImageConverterTest TgaImageConverter)
    endif()
endif()
set_target_properties(AnyImageConverterTest PROPERTIES FOLDER "MagnumPlugins/AnyImageConverter/Test")
if(CORRADE_BUILD_STATIC AND NOT MAGNUM_ANYIMAGECONVERTER_BUILD_STATIC)
    # CMake < 3.4 does this implicitly, but 3.4+ not anymore (see CMP0065).
    # That's generally okay, *except if* the build is static, the executable
    # uses a plugin manager and needs to share globals with the plugins (such
    # as output redirection and so on).
    set_target_properties(AnyImageConverterTest PROPERTIES ENABLE_EXPORTS ON)
endif()
