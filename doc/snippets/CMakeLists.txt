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

# On MSVC remove /W3, as we are replacing it with /W4
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set_directory_properties(PROPERTIES
    CORRADE_CXX_STANDARD 11
    CORRADE_USE_PEDANTIC_FLAGS ON)

# Emscripten needs special flag to use WebGL 2
if(CORRADE_TARGET_EMSCRIPTEN AND NOT TARGET_GLES2)
    # TODO: give me INTERFACE_LINK_OPTIONS or something, please
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGL2=1")
endif()

set(snippets_Magnum_SRCS
    Magnum.cpp
    MagnumAnimation.cpp
    MagnumAnimation-custom.cpp
    MagnumMath.cpp
    MagnumMathAlgorithms.cpp)
if(CORRADE_TARGET_EMSCRIPTEN)
    list(APPEND snippets_Magnum_SRCS platforms-html5.cpp)
endif()

add_library(snippets-Magnum STATIC ${snippets_Magnum_SRCS})
target_link_libraries(snippets-Magnum PRIVATE Magnum)
if(MAGNUM_TARGET_GL)
    target_link_libraries(snippets-Magnum PRIVATE MagnumGL)
endif()
set_target_properties(snippets-Magnum PROPERTIES FOLDER "Magnum/doc/snippets")

# This is taken from corrade/src/Corrade/Test/CMakeLists.txt, keep in sync
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0") OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.9") OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.0") OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
    add_library(snippets-Magnum-cpp14 STATIC MagnumMath-cpp14.cpp)
    target_link_libraries(snippets-Magnum-cpp14 PRIVATE Magnum)
    set_target_properties(snippets-Magnum-cpp14 PROPERTIES
        CORRADE_CXX_STANDARD 14
        FOLDER "Magnum/doc/snippets")
endif()

if(WITH_AUDIO)
    add_library(snippets-MagnumAudio STATIC MagnumAudio.cpp)
    target_link_libraries(snippets-MagnumAudio PRIVATE MagnumAudio)
    set_target_properties(snippets-MagnumAudio PROPERTIES FOLDER "Magnum/doc/snippets")

    if(WITH_SCENEGRAPH)
        add_library(snippets-MagnumAudio-scenegraph STATIC
            MagnumAudio-scenegraph.cpp)
        target_link_libraries(snippets-MagnumAudio-scenegraph PRIVATE
            MagnumAudio
            MagnumSceneGraph)
        set_target_properties(snippets-MagnumAudio-scenegraph PROPERTIES FOLDER "Magnum/doc/snippets")
    endif()
endif()

if(WITH_GL)
    add_library(snippets-MagnumGL STATIC
        MagnumGL.cpp
        MagnumMeshTools-gl.cpp
        MagnumShaders.cpp
        MagnumText.cpp)
    target_link_libraries(snippets-MagnumGL PRIVATE MagnumGL)
    set_target_properties(snippets-MagnumGL PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if(WITH_MESHTOOLS)
    add_library(snippets-MagnumMeshTools STATIC
        MagnumMeshTools.cpp
        MagnumMeshTools-gl.cpp)
    target_link_libraries(snippets-MagnumMeshTools PRIVATE MagnumMeshTools)
    set_target_properties(snippets-MagnumMeshTools PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if(WITH_SHADERTOOLS)
    add_library(snippets-MagnumShaderTools STATIC
        MagnumShaderTools.cpp)
    target_link_libraries(snippets-MagnumShaderTools PRIVATE MagnumShaderTools)
    set_target_properties(snippets-MagnumShaderTools PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if(WITH_TRADE)
    add_library(snippets-MagnumTrade STATIC
        plugins.cpp
        MagnumTrade.cpp)
    target_link_libraries(snippets-MagnumTrade PRIVATE MagnumTrade)
    if(MAGNUM_TARGET_GL)
        target_link_libraries(snippets-MagnumTrade PRIVATE MagnumGL)
    endif()
    set_target_properties(snippets-MagnumTrade PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

find_package(Corrade COMPONENTS TestSuite)

if(WITH_DEBUGTOOLS)
    add_library(snippets-MagnumDebugTools STATIC
        MagnumDebugTools.cpp)
    target_link_libraries(snippets-MagnumDebugTools PRIVATE MagnumDebugTools)
    set_target_properties(snippets-MagnumDebugTools PROPERTIES FOLDER "Magnum/doc/snippets")

    if(BUILD_GL_TESTS AND NOT MAGNUM_TARGET_GLES)
        add_executable(debugtools-frameprofiler debugtools-frameprofiler.cpp)
        target_link_libraries(debugtools-frameprofiler PRIVATE
            MagnumDebugTools MagnumOpenGLTester)
    endif()

    # TODO: causes spurious linker errors on Travis iOS build, so I'm disabling it
    if(NOT CORRADE_TARGET_IOS)
        set(SNIPPETS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                       ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

        # CompareImage documentation snippet. I need it executable so I can
        # copy&paste the output to the documentation. Also not using
        # corrade_add_test() because it shouldn't be run as part of CTest as it
        # purposedly fails.
        add_executable(debugtools-compareimage debugtools-compareimage.cpp)
        target_link_libraries(debugtools-compareimage PRIVATE
            MagnumDebugTools
            MagnumTrade)
        target_include_directories(debugtools-compareimage PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
        set_target_properties(debugtools-compareimage PROPERTIES FOLDER "Magnum/doc/snippets")
    endif()

    if(TARGET_GL)
        add_library(snippets-MagnumDebugTools-gl STATIC
            MagnumDebugTools-gl.cpp)
        target_link_libraries(snippets-MagnumDebugTools-gl PRIVATE MagnumDebugTools)
        set_target_properties(snippets-MagnumDebugTools-gl PROPERTIES FOLDER "Magnum/doc/snippets")
    endif()
endif()

if(WITH_PRIMITIVES)
    add_library(snippets-MagnumPrimitives STATIC
        MagnumPrimitives.cpp)
    target_link_libraries(snippets-MagnumPrimitives PRIVATE MagnumPrimitives)
    set_target_properties(snippets-MagnumPrimitives
        PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if(WITH_SCENEGRAPH)
    add_library(snippets-MagnumSceneGraph STATIC
        MagnumSceneGraph.cpp)
    target_link_libraries(snippets-MagnumSceneGraph PRIVATE MagnumSceneGraph)
    set_target_properties(snippets-MagnumSceneGraph
        PROPERTIES FOLDER "Magnum/doc/snippets")

    if(WITH_SDL2APPLICATION AND WITH_GL AND WITH_PRIMITIVES AND WITH_SHADERS AND WITH_MESHTOOLS)
        add_library(snippets-MagnumSceneGraph-gl STATIC
            MagnumSceneGraph-gl.cpp)
        target_link_libraries(snippets-MagnumSceneGraph-gl PRIVATE
            MagnumGL
            MagnumMeshTools
            MagnumPrimitives
            MagnumSceneGraph
            MagnumSdl2Application
            MagnumShaders)
        set_target_properties(snippets-MagnumSceneGraph-gl
            PROPERTIES FOLDER "Magnum/doc/snippets")
    endif()
endif()

if(WITH_VK)
    add_library(snippets-MagnumVk STATIC MagnumVk.cpp)
    target_link_libraries(snippets-MagnumVk PRIVATE MagnumVk)
    set_target_properties(snippets-MagnumVk
        PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if(WITH_SDL2APPLICATION AND TARGET_GL)
    add_executable(getting-started getting-started.cpp)
    add_executable(getting-started-blue getting-started-blue.cpp)
    target_link_libraries(getting-started PRIVATE MagnumSdl2Application)
    target_link_libraries(getting-started-blue PRIVATE MagnumSdl2Application)

    add_library(snippets-MagnumPlatform STATIC
        MagnumPlatform.cpp
        MagnumGL-framebuffer.cpp)
    target_link_libraries(snippets-MagnumPlatform PRIVATE MagnumSdl2Application)

    set_target_properties(
        getting-started
        getting-started-blue
        snippets-MagnumPlatform
        PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if((NOT TARGET_GLES AND WITH_SDL2APPLICATION) OR (TARGET_GLES AND WITH_XEGLAPPLICATION))
    add_library(snippets-MagnumPlatform-portability STATIC MagnumPlatform-portability.cpp)
    if(TARGET_GLES)
        target_link_libraries(snippets-MagnumPlatform-portability PRIVATE MagnumXEglApplication)
    else()
        target_link_libraries(snippets-MagnumPlatform-portability PRIVATE MagnumSdl2Application)
    endif()

    set_target_properties(
        snippets-MagnumPlatform-portability
        PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if(WITH_WINDOWLESSEGLAPPLICATION)
    add_library(snippets-MagnumPlatform-custom STATIC MagnumPlatform-custom.cpp)
    add_library(snippets-MagnumPlatform-windowless STATIC MagnumPlatform-windowless.cpp)
    add_library(snippets-MagnumPlatform-windowless-custom STATIC MagnumPlatform-windowless-custom.cpp)
    add_library(snippets-MagnumPlatform-windowless-thread STATIC MagnumPlatform-windowless-thread.cpp)

    target_link_libraries(snippets-MagnumPlatform-custom PRIVATE MagnumWindowlessEglApplication)
    target_link_libraries(snippets-MagnumPlatform-windowless PRIVATE MagnumWindowlessEglApplication)
    target_link_libraries(snippets-MagnumPlatform-windowless-custom PRIVATE MagnumWindowlessEglApplication)
    target_link_libraries(snippets-MagnumPlatform-windowless-thread PRIVATE MagnumWindowlessEglApplication)

    set_target_properties(
        snippets-MagnumPlatform-windowless
        snippets-MagnumPlatform-windowless-custom
        snippets-MagnumPlatform-windowless-thread
        snippets-MagnumPlatform-custom
        PROPERTIES FOLDER "Magnum/doc/snippets")
endif()
