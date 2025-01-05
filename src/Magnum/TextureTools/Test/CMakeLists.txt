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
set(CMAKE_FOLDER "Magnum/TextureTools/Test")

if(MAGNUM_BUILD_GL_TESTS)
    # Otherwise CMake complains that Corrade::PluginManager is not found, wtf
    find_package(Corrade REQUIRED PluginManager)

    if(NOT MAGNUM_BUILD_PLUGINS_STATIC)
        if(MAGNUM_WITH_ANYIMAGEIMPORTER)
            set(ANYIMAGEIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:AnyImageImporter>)
        endif()
        if(MAGNUM_WITH_TGAIMPORTER)
            set(TGAIMPORTER_PLUGIN_FILENAME $<TARGET_FILE:TgaImporter>)
        endif()
    endif()
endif()

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(TEXTURETOOLS_TEST_DIR .)
else()
    set(TEXTURETOOLS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# First replace ${} variables, then $<> generator expressions
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

corrade_add_test(TextureToolsAtlasTest AtlasTest.cpp LIBRARIES MagnumTextureToolsTestLib)
corrade_add_test(TextureToolsAtlasBenchmark AtlasBenchmark.cpp
    LIBRARIES
        MagnumDebugTools
        MagnumTextureTools
        MagnumTrade
    FILES
        # ./extract-texture-sizes.py ~/Data/fp-scenes/scenes/original/102344349.glb fp-102344349-textures.bin --ratio 9
        AtlasTestFiles/fp-102344349-textures.bin
        # ./extract-texture-sizes.py ~/Data/fp-scenes/scenes/original/103997718_171030855.glb fp-103997718-171030855-textures.bin --ratio 8
        AtlasTestFiles/fp-103997718-171030855-textures.bin
        # ./extract-font-glyph-sizes.py /usr/share/fonts/noto/NotoSerifTangut-Regular.ttf noto-serif-tangut-glyphs.bin
        AtlasTestFiles/noto-serif-tangut-glyphs.bin
        # ./extract-font-glyph-sizes.py ~/Code/magnum-plugins/src/MagnumPlugins/FreeTypeFont/Test/Oxygen.ttf oxygen-glyphs.bin
        AtlasTestFiles/oxygen-glyphs.bin)
target_include_directories(TextureToolsAtlasBenchmark PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
if(CORRADE_TARGET_EMSCRIPTEN)
    if(CMAKE_VERSION VERSION_LESS 3.13)
        message(FATAL_ERROR "CMake 3.13+ is required in order to specify Emscripten linker options")
    endif()
    # It allocates an 8K image in order to verify no overlaps
    target_link_options(TextureToolsAtlasBenchmark PRIVATE "SHELL:-s ALLOW_MEMORY_GROWTH=1")
endif()
if(MAGNUM_BUILD_PLUGINS_STATIC)
    if(MAGNUM_WITH_TGAIMAGECONVERTER)
        target_link_libraries(TextureToolsAtlasBenchmark PRIVATE TgaImageConverter)
    endif()
else()
    # So the plugins get properly built when building the test
    if(MAGNUM_WITH_TGAIMAGECONVERTER)
        add_dependencies(TextureToolsAtlasBenchmark TgaImageConverter)
    endif()
endif()

if(MAGNUM_TARGET_GL)
    corrade_add_test(TextureToolsDistanceFieldGL_Test DistanceFieldGL_Test.cpp LIBRARIES MagnumTextureTools)

    if(MAGNUM_BUILD_GL_TESTS)
        # Otherwise CMake complains that Corrade::PluginManager is not found, wtf
        find_package(Corrade REQUIRED PluginManager)

        set(TextureToolsDistanceFieldGLTest_SRCS DistanceFieldGLTest.cpp)
        if(CORRADE_TARGET_IOS)
            # TODO: do this in a generic way in corrade_add_test()
            set_source_files_properties(DistanceFieldGLTestFiles PROPERTIES
                MACOSX_PACKAGE_LOCATION Resources)
            list(APPEND TextureToolsDistanceFieldGLTest_SRCS DistanceFieldGLTestFiles)
        endif()
        corrade_add_test(TextureToolsDistanceFieldGLTest ${TextureToolsDistanceFieldGLTest_SRCS}
            LIBRARIES
                MagnumDebugTools
                MagnumGL
                MagnumOpenGLTester
                MagnumTextureToolsTestLib
                MagnumTrade
            FILES
                DistanceFieldGLTestFiles/input.tga
                DistanceFieldGLTestFiles/output.tga)
        target_include_directories(TextureToolsDistanceFieldGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
        if(MAGNUM_BUILD_PLUGINS_STATIC)
            if(MAGNUM_WITH_ANYIMAGEIMPORTER)
                target_link_libraries(TextureToolsDistanceFieldGLTest PRIVATE AnyImageImporter)
            endif()
            if(MAGNUM_WITH_TGAIMPORTER)
                target_link_libraries(TextureToolsDistanceFieldGLTest PRIVATE TgaImporter)
            endif()
        else()
            # So the plugins get properly built when building the test
            if(MAGNUM_WITH_ANYIMAGEIMPORTER)
                add_dependencies(TextureToolsDistanceFieldGLTest AnyImageImporter)
            endif()
            if(MAGNUM_WITH_TGAIMPORTER)
                add_dependencies(TextureToolsDistanceFieldGLTest TgaImporter)
            endif()
        endif()
    endif()
endif()
