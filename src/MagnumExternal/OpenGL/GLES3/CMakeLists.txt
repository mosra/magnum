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

# Desktop GLES on Windows still links to opengl32.dll so we have a special
# function loading code that queries everything above OpenGL 1.1
if(CORRADE_TARGET_WINDOWS AND MAGNUM_TARGET_DESKTOP_GLES)
    set(MagnumOpenGL_HEADERS flextGLWindowsDesktop.h)
    set(MagnumOpenGL_SRCS flextGLWindowsDesktop.cpp)

# Emscripten is special, provide only some extensions for it (and no extension
# loading)
elseif(CORRADE_TARGET_EMSCRIPTEN)
    set(MagnumOpenGL_HEADERS flextGLEmscripten.h)

# The common sane case
else()
    set(MagnumOpenGL_HEADERS flextGL.h)
    set(MagnumOpenGL_SRCS flextGL.cpp)
endif()

# Emscripten doesn't have the ability to load function pointers manually
if(NOT CORRADE_TARGET_EMSCRIPTEN)
    # flextGLPlatform*.cpp is compiled as part of Magnum*Context libraries in Platform
    add_library(MagnumFlextGLObjects OBJECT ${MagnumOpenGL_SRCS})
    target_include_directories(MagnumFlextGLObjects PUBLIC $<TARGET_PROPERTY:MagnumGL,INTERFACE_INCLUDE_DIRECTORIES>)
    if(NOT BUILD_STATIC)
        target_compile_definitions(MagnumFlextGLObjects PRIVATE "FlextGL_EXPORTS")
    endif()
    if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
        set_target_properties(MagnumFlextGLObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    set_target_properties(MagnumFlextGLObjects PROPERTIES FOLDER "MagnumExternal/OpenGL")
endif()

install(FILES ${MagnumOpenGL_HEADERS} DESTINATION ${MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR}/OpenGL/GLES3)
