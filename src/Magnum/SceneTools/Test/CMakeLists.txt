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
set(CMAKE_FOLDER "Magnum/SceneTools/Test")

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(SCENETOOLS_TEST_DIR ".")
    set(SCENETOOLS_TEST_OUTPUT_DIR "write")
else()
    set(SCENETOOLS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(SCENETOOLS_TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Executable testing is implemented on Unix platforms only at the moment, so
# don't even provide the filename elsewhere.
if(MAGNUM_WITH_SCENECONVERTER AND CORRADE_TARGET_UNIX)
    set(SCENECONVERTER_EXECUTABLE_FILENAME $<TARGET_FILE:magnum-sceneconverter>)
endif()
if(MAGNUM_WITH_ANYSCENECONVERTER AND NOT MAGNUM_BUILD_PLUGINS_STATIC AND NOT MAGNUM_ANYSCENECONVERTER_BUILD_STATIC)
    set(ANYSCENECONVERTER_PLUGIN_FILENAME $<TARGET_FILE:AnySceneConverter>)
endif()

# First replace ${} variables, then $<> generator expressions
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/configure.h
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.h.in)

corrade_add_test(SceneToolsCombineTest CombineTest.cpp LIBRARIES MagnumSceneToolsTestLib)
corrade_add_test(SceneToolsCopyTest CopyTest.cpp LIBRARIES MagnumSceneToolsTestLib)
corrade_add_test(SceneToolsConvertToSingleFunc___Test ConvertToSingleFunctionObjectsTest.cpp LIBRARIES MagnumSceneToolsTestLib)
corrade_add_test(SceneToolsFilterTest FilterTest.cpp LIBRARIES MagnumSceneToolsTestLib)
corrade_add_test(SceneToolsHierarchyTest HierarchyTest.cpp LIBRARIES MagnumSceneToolsTestLib)
corrade_add_test(SceneToolsMapTest MapTest.cpp LIBRARIES MagnumSceneToolsTestLib)

corrade_add_test(SceneToolsSceneConverterImple___Test SceneConverterImplementationTest.cpp
    LIBRARIES MagnumSceneTools
    FILES
        SceneConverterImplementationTestFiles/info-animations.txt
        SceneConverterImplementationTestFiles/info-cameras.txt
        SceneConverterImplementationTestFiles/info-images.txt
        SceneConverterImplementationTestFiles/info-lights.txt
        SceneConverterImplementationTestFiles/info-materials.txt
        SceneConverterImplementationTestFiles/info-meshes-bounds.txt
        SceneConverterImplementationTestFiles/info-meshes.txt
        SceneConverterImplementationTestFiles/info-objects.txt
        SceneConverterImplementationTestFiles/info-object-hierarchy.txt
        SceneConverterImplementationTestFiles/info-object-hierarchy-no-parents.txt
        SceneConverterImplementationTestFiles/info-object-hierarchy-only-objects.txt
        SceneConverterImplementationTestFiles/info-object-hierarchy-only-objects-no-parents.txt
        SceneConverterImplementationTestFiles/info-references.txt
        SceneConverterImplementationTestFiles/info-scenes.txt
        SceneConverterImplementationTestFiles/info-scenes-no-default.txt
        SceneConverterImplementationTestFiles/info-scenes-objects.txt
        SceneConverterImplementationTestFiles/info-skins.txt
        SceneConverterImplementationTestFiles/info-textures.txt)
target_include_directories(SceneToolsSceneConverterImple___Test PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
if(MAGNUM_WITH_ANYSCENECONVERTER)
    if(MAGNUM_BUILD_PLUGINS_STATIC OR MAGNUM_ANYSCENECONVERTER_BUILD_STATIC)
        target_link_libraries(SceneToolsSceneConverterImple___Test PRIVATE AnySceneConverter)
    else()
        # So the plugins get properly built when building the test
        add_dependencies(SceneToolsSceneConverterImple___Test AnySceneConverter)
    endif()
endif()

if(MAGNUM_BUILD_DEPRECATED)
    corrade_add_test(SceneToolsFlattenMeshHierarchyTest FlattenMeshHierarchyTest.cpp LIBRARIES MagnumSceneToolsTestLib)
endif()

# Executable testing is implemented on Unix platforms only at the moment
if(CORRADE_TARGET_UNIX AND NOT CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT)
    corrade_add_test(SceneToolsSceneConverterTest SceneConverterTest.cpp
        LIBRARIES
            MagnumSceneTools
            # Link the same static plugins as for the magnum-sceneconverter
            # executable so plugin existence checks are consistent between the two
            ${MAGNUM_SCENECONVERTER_STATIC_PLUGINS}
        FILES
            SceneConverterTestFiles/1d.ktx2
            SceneConverterTestFiles/blue4x4.png
            # magnum-imageconverter --layers blue4x4.png --array blue4x4x1.ktx2 -c writerName=
            SceneConverterTestFiles/blue4x4x1.ktx2
            SceneConverterTestFiles/broken-image-2d.gltf
            SceneConverterTestFiles/broken-image-3d.gltf
            SceneConverterTestFiles/broken-material.gltf
            SceneConverterTestFiles/broken-mesh.obj
            SceneConverterTestFiles/broken-mesh-with-scene.gltf
            SceneConverterTestFiles/broken-scene.gltf
            SceneConverterTestFiles/dxt1.dds
            SceneConverterTestFiles/empty.gltf
            # Same as blue4x4.png, just named like this to have the file
            # roundtrip on conversion
            SceneConverterTestFiles/image-passthrough-on-failure.0.png
            SceneConverterTestFiles/image-passthrough-on-failure.gltf
            SceneConverterTestFiles/image-dds.gltf
            SceneConverterTestFiles/images-2d.gltf
            SceneConverterTestFiles/images-2d-1x1.bin
            SceneConverterTestFiles/images-2d-1x1.gltf
            SceneConverterTestFiles/images-3d.gltf
            SceneConverterTestFiles/images-3d-1x1x1.bin
            SceneConverterTestFiles/images-3d-1x1x1.gltf
            SceneConverterTestFiles/ignoring-unsupported.gltf
            SceneConverterTestFiles/info-data.txt
            SceneConverterTestFiles/info-data-ignored-output.txt
            SceneConverterTestFiles/info-converter.txt
            SceneConverterTestFiles/info-global-plugin-options.txt
            SceneConverterTestFiles/info-image-converter.txt
            SceneConverterTestFiles/info-importer.txt
            SceneConverterTestFiles/info-importer-ignored-input-output.txt
            SceneConverterTestFiles/info-preferred-importer-plugin.txt
            SceneConverterTestFiles/materials-3d.gltf
            SceneConverterTestFiles/materials-duplicate.gltf
            SceneConverterTestFiles/materials-duplicate-removed.gltf
            SceneConverterTestFiles/materials-pbr.gltf
            SceneConverterTestFiles/materials-phong.mtl
            SceneConverterTestFiles/materials-phong.obj
            SceneConverterTestFiles/materials-separate-metalness-roughness.mtl
            SceneConverterTestFiles/mesh-passthrough-on-failure.bin
            SceneConverterTestFiles/mesh-passthrough-on-failure.gltf
            SceneConverterTestFiles/point.obj
            SceneConverterTestFiles/quad-duplicates-fuzzy.obj
            SceneConverterTestFiles/quad-duplicates.obj
            SceneConverterTestFiles/quad-duplicates.ply
            SceneConverterTestFiles/quad-name-custom-attributes-duplicates.bin
            SceneConverterTestFiles/quad-name-custom-attributes-duplicates.gltf
            SceneConverterTestFiles/quad-name-custom-attributes.bin
            SceneConverterTestFiles/quad-name-custom-attributes.gltf
            SceneConverterTestFiles/quad-normals-texcoords.obj
            SceneConverterTestFiles/quad-strip.bin
            SceneConverterTestFiles/quad-strip.gltf
            SceneConverterTestFiles/quad.bin # generated from quad.obj
            SceneConverterTestFiles/quad.gltf
            SceneConverterTestFiles/quad.obj
            SceneConverterTestFiles/quad.ply
            SceneConverterTestFiles/red2x2.png
            # magnum-imageconverter --layers red2x2.png --array red2x2x1.ktx2 -c writerName=
            SceneConverterTestFiles/red2x2x1.ktx2
            SceneConverterTestFiles/rgba.png # copied from PngImporter tests
            SceneConverterTestFiles/two-quads-duplicates-fuzzy.bin
            SceneConverterTestFiles/two-quads-duplicates-fuzzy.gltf
            SceneConverterTestFiles/two-quads-duplicates.bin
            SceneConverterTestFiles/two-quads-duplicates.gltf
            SceneConverterTestFiles/two-quads.bin
            SceneConverterTestFiles/two-quads.gltf
            SceneConverterTestFiles/two-triangles-transformed.bin
            SceneConverterTestFiles/two-triangles-transformed.gltf
            SceneConverterTestFiles/two-triangles-transformed-no-default-scene.gltf
            SceneConverterTestFiles/two-triangles.obj
            SceneConverterTestFiles/two-scenes.gltf)
    target_include_directories(SceneToolsSceneConverterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    if(MAGNUM_WITH_SCENECONVERTER)
        add_dependencies(SceneToolsSceneConverterTest magnum-sceneconverter)
    endif()
endif()
