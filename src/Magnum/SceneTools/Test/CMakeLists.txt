#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
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
set(CMAKE_FOLDER "Magnum/SceneTools/Test")

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(SCENETOOLS_TEST_DIR ".")
else()
    set(SCENETOOLS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_test(SceneToolsCombineTest CombineTest.cpp LIBRARIES MagnumTrade)
corrade_add_test(SceneToolsConvertToSingleFun___Test ConvertToSingleFunctionObjectsTest.cpp LIBRARIES MagnumTrade)
corrade_add_test(SceneToolsFlattenMeshHierarchyTest FlattenMeshHierarchyTest.cpp LIBRARIES MagnumSceneToolsTestLib)
corrade_add_test(SceneToolsOrderClusterParentsTest OrderClusterParentsTest.cpp LIBRARIES MagnumSceneToolsTestLib)

corrade_add_test(SceneToolsSceneConverterTest SceneConverterTest.cpp
    LIBRARIES MagnumSceneTools
    FILES
        SceneConverterTestFiles/info-animations.txt
        SceneConverterTestFiles/info-images.txt
        SceneConverterTestFiles/info-lights.txt
        SceneConverterTestFiles/info-materials.txt
        SceneConverterTestFiles/info-meshes-bounds.txt
        SceneConverterTestFiles/info-meshes.txt
        SceneConverterTestFiles/info-objects.txt
        SceneConverterTestFiles/info-references.txt
        SceneConverterTestFiles/info-scenes-objects.txt
        SceneConverterTestFiles/info-scenes.txt
        SceneConverterTestFiles/info-skins.txt
        SceneConverterTestFiles/info-textures.txt)
target_include_directories(SceneToolsSceneConverterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
