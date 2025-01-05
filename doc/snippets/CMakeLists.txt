#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/doc/snippets")

# On MSVC remove /W3, as we are replacing it with /W4
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set_directory_properties(PROPERTIES
    CORRADE_CXX_STANDARD 11
    CORRADE_USE_PEDANTIC_FLAGS ON)

set(snippets_Magnum_SRCS
    Magnum.cpp
    Animation.cpp
    Animation-custom.cpp
    Math.cpp
    Math-stl.cpp
    MathAlgorithms.cpp)
if(CORRADE_TARGET_EMSCRIPTEN)
    list(APPEND snippets_Magnum_SRCS platforms-html5.cpp)
endif()

add_library(snippets-Magnum STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} ${snippets_Magnum_SRCS})
target_link_libraries(snippets-Magnum PRIVATE Magnum)
if(MAGNUM_TARGET_GL)
    target_link_libraries(snippets-Magnum PRIVATE MagnumGL)
endif()
if(CORRADE_TESTSUITE_TEST_TARGET)
    add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Magnum)
endif()

# This is taken from corrade/src/Corrade/Test/CMakeLists.txt, keep in sync
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0") OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.9") OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.0") OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
    add_library(snippets-Magnum-cpp14 STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} Math-cpp14.cpp)
    target_link_libraries(snippets-Magnum-cpp14 PRIVATE Magnum)
    set_target_properties(snippets-Magnum-cpp14 PROPERTIES CORRADE_CXX_STANDARD 14)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Magnum-cpp14)
    endif()
endif()

if(MAGNUM_WITH_AUDIO)
    add_library(snippets-Audio STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} Audio.cpp)
    target_link_libraries(snippets-Audio PRIVATE MagnumAudio)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Audio)
    endif()

    if(MAGNUM_WITH_SCENEGRAPH)
        add_library(snippets-Audio-scenegraph STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
            Audio-scenegraph.cpp)
        target_link_libraries(snippets-Audio-scenegraph PRIVATE
            MagnumAudio
            MagnumSceneGraph)
        if(CORRADE_TESTSUITE_TEST_TARGET)
            add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Audio-scenegraph)
        endif()
    endif()
endif()

if(MAGNUM_WITH_GL)
    add_library(snippets-GL STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        GL.cpp
        MeshTools-gl.cpp
        Shaders-gl.cpp
        Text-gl.cpp)
    target_link_libraries(snippets-GL PRIVATE MagnumGL)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-GL)
    endif()
endif()

if(MAGNUM_WITH_MATERIALTOOLS)
    add_library(snippets-MaterialTools STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        MaterialTools.cpp)
    target_link_libraries(snippets-MaterialTools PRIVATE MagnumMaterialTools)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-MaterialTools)
    endif()
endif()

if(MAGNUM_WITH_MESHTOOLS)
    add_library(snippets-MeshTools STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        MeshTools.cpp)
    target_link_libraries(snippets-MeshTools PRIVATE MagnumMeshTools)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-MeshTools)
    endif()
endif()

if(MAGNUM_WITH_SHADERTOOLS)
    add_library(snippets-ShaderTools STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        ShaderTools.cpp)
    target_link_libraries(snippets-ShaderTools PRIVATE MagnumShaderTools)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-ShaderTools)
    endif()
endif()

if(MAGNUM_WITH_TEXT)
    add_library(snippets-Text STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        Text.cpp)
    target_link_libraries(snippets-Text PRIVATE MagnumText)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Text)
    endif()
endif()

if(MAGNUM_WITH_TEXTURETOOLS)
    add_library(snippets-TextureTools STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        TextureTools.cpp)
    target_link_libraries(snippets-TextureTools PRIVATE MagnumTextureTools)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-TextureTools)
    endif()
endif()

if(MAGNUM_WITH_TRADE)
    add_library(snippets-Trade STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        plugins.cpp
        Trade.cpp)
    target_link_libraries(snippets-Trade PRIVATE MagnumTrade)
    if(MAGNUM_TARGET_GL)
        target_link_libraries(snippets-Trade PRIVATE MagnumGL)
    endif()
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Trade)
    endif()
endif()

find_package(Corrade COMPONENTS TestSuite)

if(MAGNUM_WITH_DEBUGTOOLS)
    add_library(snippets-DebugTools STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        DebugTools.cpp)
    target_link_libraries(snippets-DebugTools PRIVATE MagnumDebugTools)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-DebugTools)
    endif()

    if(MAGNUM_BUILD_GL_TESTS AND NOT MAGNUM_TARGET_GLES)
        add_executable(debugtools-frameprofiler ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} debugtools-frameprofiler.cpp)
        target_link_libraries(debugtools-frameprofiler PRIVATE
            MagnumDebugTools MagnumOpenGLTester)
        if(CORRADE_TESTSUITE_TEST_TARGET)
            add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} debugtools-frameprofiler)
        endif()
    endif()

    # TODO: causes spurious linker errors on Travis iOS build, so I'm disabling it
    if(NOT CORRADE_TARGET_IOS)
        set(SNIPPETS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                       ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

        # CompareImage / CompareMaterial documentation snippets. I need them
        # executable so I can include the colored output in the documentation.
        # Also not using corrade_add_test() because it shouldn't be run as part
        # of CTest as it purposely fails.
        add_executable(debugtools-compareimage ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} debugtools-compareimage.cpp)
        add_executable(debugtools-comparematerial ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} debugtools-comparematerial.cpp)
        target_link_libraries(debugtools-compareimage PRIVATE MagnumDebugTools)
        target_link_libraries(debugtools-comparematerial PRIVATE MagnumDebugTools)
        target_include_directories(debugtools-compareimage PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
        if(CORRADE_TESTSUITE_TEST_TARGET)
            add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET}
                debugtools-compareimage
                debugtools-comparematerial)
        endif()
    endif()

    if(MAGNUM_TARGET_GL)
        add_library(snippets-DebugTools-gl STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
            DebugTools-gl.cpp)
        target_link_libraries(snippets-DebugTools-gl PRIVATE MagnumDebugTools)
        if(CORRADE_TESTSUITE_TEST_TARGET)
            add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-DebugTools-gl)
        endif()
    endif()
endif()

if(MAGNUM_WITH_PRIMITIVES)
    add_library(snippets-Primitives STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        Primitives.cpp)
    target_link_libraries(snippets-Primitives PRIVATE MagnumPrimitives)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Primitives)
    endif()
endif()

if(MAGNUM_WITH_SCENEGRAPH)
    add_library(snippets-SceneGraph STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        SceneGraph.cpp)
    target_link_libraries(snippets-SceneGraph PRIVATE MagnumSceneGraph)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-SceneGraph)
    endif()

    if(MAGNUM_WITH_SDL2APPLICATION AND MAGNUM_WITH_GL AND MAGNUM_WITH_PRIMITIVES AND MAGNUM_WITH_SHADERS AND MAGNUM_WITH_MESHTOOLS)
        add_library(snippets-SceneGraph-gl STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
            SceneGraph-gl.cpp)
        target_link_libraries(snippets-SceneGraph-gl PRIVATE
            MagnumGL
            MagnumMeshTools
            MagnumPrimitives
            MagnumSceneGraph
            MagnumSdl2Application
            MagnumShaders)
        if(CORRADE_TESTSUITE_TEST_TARGET)
            add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-SceneGraph-gl)
        endif()
    endif()
endif()

if(MAGNUM_WITH_SCENETOOLS)
    add_library(snippets-SceneTools STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        SceneTools.cpp)
    target_link_libraries(snippets-SceneTools PRIVATE MagnumSceneTools)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-SceneTools)
    endif()
endif()

if(MAGNUM_WITH_VK)
    add_library(snippets-Vk STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} Vk.cpp)
    target_link_libraries(snippets-Vk PRIVATE MagnumVk)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Vk)
    endif()
endif()

if(MAGNUM_WITH_SDL2APPLICATION AND MAGNUM_TARGET_GL)
    add_executable(getting-started ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} getting-started.cpp)
    add_executable(getting-started-blue ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} getting-started-blue.cpp)
    target_link_libraries(getting-started PRIVATE MagnumSdl2Application)
    target_link_libraries(getting-started-blue PRIVATE MagnumSdl2Application)

    add_library(snippets-Platform STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET}
        Magnum-application.cpp
        Platform.cpp
        GL-application.cpp)
    target_link_libraries(snippets-Platform PRIVATE MagnumSdl2Application)

    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET}
            getting-started
            getting-started-blue
            snippets-Platform)
    endif()
endif()

if((NOT MAGNUM_TARGET_GLES AND MAGNUM_WITH_SDL2APPLICATION) OR (MAGNUM_TARGET_GLES AND MAGNUM_WITH_XEGLAPPLICATION))
    add_library(snippets-Platform-portability STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} Platform-portability.cpp)
    if(MAGNUM_TARGET_GLES)
        target_link_libraries(snippets-Platform-portability PRIVATE MagnumXEglApplication)
    else()
        target_link_libraries(snippets-Platform-portability PRIVATE MagnumSdl2Application)
    endif()
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-Platform-portability)
    endif()
endif()

if(MAGNUM_WITH_WINDOWLESSEGLAPPLICATION)
    add_library(snippets-Platform-custom STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} Platform-custom.cpp)
    add_library(snippets-Platform-windowless STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} Platform-windowless.cpp)
    add_library(snippets-Platform-windowless-custom STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} Platform-windowless-custom.cpp)
    add_library(snippets-Platform-windowless-thread STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} Platform-windowless-thread.cpp)

    target_link_libraries(snippets-Platform-custom PRIVATE MagnumWindowlessEglApplication)
    target_link_libraries(snippets-Platform-windowless PRIVATE MagnumWindowlessEglApplication)
    target_link_libraries(snippets-Platform-windowless-custom PRIVATE MagnumWindowlessEglApplication)
    target_link_libraries(snippets-Platform-windowless-thread PRIVATE MagnumWindowlessEglApplication)

    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET}
            snippets-Platform-custom
            snippets-Platform-windowless
            snippets-Platform-windowless-custom
            snippets-Platform-windowless-thread)
    endif()
endif()
