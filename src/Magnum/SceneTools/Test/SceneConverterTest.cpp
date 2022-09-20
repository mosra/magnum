/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/TestSuite/Compare/StringToFile.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Trade/AbstractSceneConverter.h"

#include "Magnum/SceneTools/Implementation/sceneConverterUtilities.h"

#include "configure.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct SceneConverterTest: TestSuite::Tester {
    explicit SceneConverterTest();

    void infoImplementationEmpty();
    void infoImplementationScenesObjects();
    void infoImplementationAnimations();
    void infoImplementationSkins();
    void infoImplementationLights();
    void infoImplementationCameras();
    void infoImplementationMaterials();
    void infoImplementationMeshes();
    void infoImplementationMeshesBounds();
    void infoImplementationTextures();
    void infoImplementationImages();
    /* Image info further tested in ImageConverterTest */
    void infoImplementationReferenceCount();
    void infoImplementationError();

    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    void info();
    void convert();
    void error();
    #endif

    Utility::Arguments _infoArgs;
};

using namespace Containers::Literals;
using namespace Math::Literals;

const struct {
    const char* name;
    const char* arg;
    const char* expected;
    bool printVisualCheck;
} InfoImplementationScenesObjectsData[]{
    {"", "--info", "info-scenes-objects.txt", true},
    {"only scenes", "--info-scenes", "info-scenes.txt", false},
    {"only objects", "--info-objects", "info-objects.txt", false},
};

const struct {
    const char* name;
    bool oneOrAll;
    bool printVisualCheck;
} InfoImplementationOneOrAllData[]{
    {"", true, true},
    {"--info", false, false},
};

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
const struct {
    const char* name;
    Containers::Array<Containers::String> args;
    const char* expected;
} InfoData[]{
    {"", Containers::array<Containers::String>({}),
        "info.txt"},
    {"map", Containers::array<Containers::String>({
        "--map"}),
        /** @todo change to something else once we have a plugin that can
            zero-copy pass the imported data */
        "info.txt"},
    {"ignored output file", Containers::array<Containers::String>({
        "whatever.ply"}),
        "info-ignored-output.txt"},
};

const struct {
    const char* name;
    Containers::Array<Containers::String> args;
    const char* requiresImporter;
    const char* requiresConverter;
    const char* requiresMeshConverter;
    const char* expected;
    const char* expected2;
    Containers::String message;
} ConvertData[]{
    {"one mesh", Containers::array<Containers::String>({
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, whole scene converter", Containers::array<Containers::String>({
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "ObjImporter", "GltfSceneConverter", nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one mesh, explicit importer and converter", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, map", Containers::array<Containers::String>({
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, options", Containers::array<Containers::String>({
        /* It's silly, but since we have option propagation tested in
           AnySceneImporter / AnySceneConverter .cpp already, it's enough to
           just verify the (nonexistent) options arrive there */
        "-i", "nonexistentOption=13", "-c", "nonexistentConverterOption=26",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Trade::AnySceneImporter::openFile(): option nonexistentOption not recognized by ObjImporter\n"
        "Trade::AnySceneConverter::beginFile(): option nonexistentConverterOption not recognized by StanfordSceneConverter\n"},
    {"one mesh, options, explicit importer and converter", Containers::array<Containers::String>({
        /* Same here, since we have option propagation tested in
           Magnum/Test/ConverterUtilitiesTest.cpp already, to verify it's
           getting called we can just supply nonexistent options */
        "-i", "nonexistentOption=13", "-c", "nonexistentConverterOption=26",
        "-I", "ObjImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Option nonexistentOption not recognized by ObjImporter\n"
        "Option nonexistentConverterOption not recognized by StanfordSceneConverter\n"},
    {"two meshes + scene", Containers::array<Containers::String>({
        /* Removing the generator identifier to have the file fully roundtrip */
        "-c", "generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        /* There should be a minimal difference compared to the original */
        "two-quads.gltf", "two-quads.bin",
        {}},
    {"concatenate meshes without a scene", Containers::array<Containers::String>({
        "--concatenate-meshes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-duplicates.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad-duplicates.ply", nullptr,
        {}},
    {"concatenate meshes with a scene", Containers::array<Containers::String>({
        "--concatenate-meshes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles-transformed.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-duplicates.ply")}),
        "GltfImporter", "StanfordSceneConverter", nullptr,
        "quad-duplicates.ply", nullptr,
        {}},
    /** @todo drop --mesh once it's not needed anymore again, then add a
        multi-mesh variant */
    {"one mesh, filter mesh attributes", Containers::array<Containers::String>({
        /* Only 0 gets picked from here, others ignored */
        "--mesh", "0", "--only-mesh-attributes", "17,0,25-36",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-normals-texcoords.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        {}},
    {"concatenate meshes, filter mesh attributes", Containers::array<Containers::String>({
        "--concatenate-meshes", "--only-mesh-attributes", "17,0,25-36",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-normals-texcoords.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove vertex duplicates", Containers::array<Containers::String>({
        "--remove-duplicate-vertices",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove duplicate vertices, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--remove-duplicate-vertices", "-v", "-I", "ObjImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Mesh 0 duplicate removal: 6 -> 4 vertices\n"},
    {"one selected mesh, remove duplicate vertices, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--mesh", "1", "--remove-duplicate-vertices", "-v", "-I", "GltfImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "GltfImporter", "StanfordSceneConverter", nullptr,
        /* The second mesh in the glTF is deliberately the same as in
           quad-duplicates.obj, so this produces the same file */
        "quad.ply", nullptr,
        "Duplicate removal: 6 -> 4 vertices\n"},
    {"two meshes + scene, remove duplicate vertices, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--remove-duplicate-vertices", "-v", "-I", "GltfImporter", "-C", "GltfSceneConverter",
        /* Removing the generator identifier for a smaller file */
        "-c", "generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        /* There should be a minimal difference compared to the original */
        "two-quads.gltf", "two-quads.bin",
        "Mesh 0 duplicate removal: 5 -> 4 vertices\n"
        "Mesh 1 duplicate removal: 6 -> 4 vertices\n"},
    {"one implicit mesh, remove duplicate vertices fuzzy", Containers::array<Containers::String>({
        "--remove-duplicate-vertices-fuzzy", "1.0e-1",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates-fuzzy.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove duplicate vertices fuzzy, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--remove-duplicate-vertices-fuzzy", "1.0e-1", "-v", "-I", "ObjImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates-fuzzy.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Mesh 0 fuzzy duplicate removal: 6 -> 4 vertices\n"},
    {"one selected mesh, remove duplicate vertices fuzzy, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--mesh 1", "--remove-duplicate-vertices-fuzzy", "1.0e-1", "-v", "-I", "GltfImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates-fuzzy.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "GltfImporter", "StanfordSceneConverter", nullptr,
        /* The second mesh in the glTF is deliberately the same as in
           quad-duplicates-fuzzy.obj, so this produces the same file */
        "quad.ply", nullptr,
        "Fuzzy duplicate removal: 6 -> 4 vertices\n"},
    {"two meshes + scene, remove duplicate vertices fuzzy, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--remove-duplicate-vertices-fuzzy", "1.0e-1", "-v", "-I", "GltfImporter", "-C", "GltfSceneConverter",
        /* Removing the generator identifier for a smaller file */
        "-c", "generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates-fuzzy.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "two-quads.gltf", "two-quads.bin",
        "Mesh 0 fuzzy duplicate removal: 5 -> 4 vertices\n"
        "Mesh 1 fuzzy duplicate removal: 6 -> 4 vertices\n"},
    {"one implicit mesh, two converters", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one implicit mesh, two converters, explicit last", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter", "-C", "GltfSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one implicit mesh, two converters, verbose", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "quad.gltf", "quad.bin",
        /* While this looks like a no-op in the output, it converts a
           triangle strip to indexed triangles, which verifies that the output
           of MeshOptimizerSceneConverter got actually passed further and not
           discarded */
        "Trade::AnySceneImporter::openFile(): using GltfImporter\n"
        "Trade::MeshOptimizerSceneConverter::convert(): processing stats:\n"
        "  vertex cache:\n"
        "    4 -> 4 transformed vertices\n"
        "    1 -> 1 executed warps\n"
        "    ACMR 2 -> 2\n"
        "    ATVR 1 -> 1\n"
        "  vertex fetch:\n"
        "    64 -> 64 bytes fetched\n"
        "    overfetch 1.33333 -> 1.33333\n"
        "  overdraw:\n"
        "    65536 -> 65536 shaded pixels\n"
        "    65536 -> 65536 covered pixels\n"
        "    overdraw 1 -> 1\n"
        "Trade::AnySceneConverter::beginFile(): using GltfSceneConverter\n"},
    {"one implicit mesh, two converters, explicit last, verbose", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter", "-C", "GltfSceneConverter", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "quad.gltf", "quad.bin",
        /* As the importers and converters are specified explicitly, there's
           no messages from AnySceneConverter, OTOH as we have more than one -C
           option the verbose output includes a progress info */
        "Trade::AnySceneImporter::openFile(): using GltfImporter\n"
        "Processing (1/2) with MeshOptimizerSceneConverter...\n"
        "Trade::MeshOptimizerSceneConverter::convert(): processing stats:\n"
        "  vertex cache:\n"
        "    4 -> 4 transformed vertices\n"
        "    1 -> 1 executed warps\n"
        "    ACMR 2 -> 2\n"
        "    ATVR 1 -> 1\n"
        "  vertex fetch:\n"
        "    64 -> 64 bytes fetched\n"
        "    overfetch 1.33333 -> 1.33333\n"
        "  overdraw:\n"
        "    65536 -> 65536 shaded pixels\n"
        "    65536 -> 65536 covered pixels\n"
        "    overdraw 1 -> 1\n"
        "Saving output (2/2) with GltfSceneConverter...\n"},
    {"one implicit mesh, two converters, options for the first only", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        "-c", "nonexistentMeshOptimizerOption=yes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"},
    {"one implicit mesh, two converters, explicit last, options for the first only", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        "-c", "nonexistentMeshOptimizerOption=yes",
        "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"},
    {"one implicit mesh, two converters, options for both", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        "-c", "nonexistentMeshOptimizerOption=yes",
        "-c", "nonexistentAnyConverterOption=no",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"
        "Trade::AnySceneConverter::beginFile(): option nonexistentAnyConverterOption not recognized by GltfSceneConverter\n"},
    {"one implicit mesh, two converters, explicit last, options for both", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        "-c", "nonexistentMeshOptimizerOption=yes",
        "-C", "StanfordSceneConverter",
        "-c", "nonexistentStanfordConverterOption=no",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"
        "Option nonexistentStanfordConverterOption not recognized by StanfordSceneConverter\n"},
    {"one mesh, remove duplicate vertices, two converters, verbose", Containers::array<Containers::String>({
        "--remove-duplicate-vertices",
        "-C", "MeshOptimizerSceneConverter", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", "StanfordSceneConverter", nullptr,
        "quad.ply", nullptr,
        "Trade::AnySceneImporter::openFile(): using ObjImporter\n"
        "Mesh 0 duplicate removal: 6 -> 4 vertices\n"
        /** @todo this only verifies that the result of duplicate removal is
            properly passed to MeshOptimizer, but not that the MeshOptimizer
            output is properly passed to StanfordSceneConverter -- needs to
            wait until there's a plugin which can verify that with a small
            data amount */
        "Trade::MeshOptimizerSceneConverter::convert(): processing stats:\n"
        "  vertex cache:\n"
        "    4 -> 4 transformed vertices\n"
        "    1 -> 1 executed warps\n"
        "    ACMR 2 -> 2\n"
        "    ATVR 1 -> 1\n"
        "  vertex fetch:\n"
        "    64 -> 64 bytes fetched\n"
        "    overfetch 1.33333 -> 1.33333\n"
        "  overdraw:\n"
        "    65536 -> 65536 shaded pixels\n"
        "    65536 -> 65536 covered pixels\n"
        "    overdraw 1 -> 1\n"
        "Trade::AnySceneConverter::beginFile(): using StanfordSceneConverter\n"},
    {"implicit custom-processed mesh with a name and custom attributes", Containers::array<Containers::String>({
        /* Removing the generator identifier to have the file closer to the
           original */
        "--remove-duplicate-vertices", "-c", "generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-name-custom-attributes-duplicates.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-name-custom-attributes.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        /* The output should be mostly the same, except that there's now only 4
           vertices instead of 6. The code that adds meshes manually instead of
           using addSupportedImporterContents() should take care of propagating
           mesh names and custom attributes as well. */
        "quad-name-custom-attributes.gltf", "quad-name-custom-attributes.bin",
        {}},
    {"selected custom-processed mesh with a name and custom attributes", Containers::array<Containers::String>({
        /* Removing the generator identifier to have the file closer to the
           original */
        "--mesh", "0", "--remove-duplicate-vertices", "-c", "generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-name-custom-attributes-duplicates.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-name-custom-attributes.gltf")}),
        "GltfImporter", "GltfSceneConverter", nullptr,
        /* The output should be mostly the same, except that there's now only 4
           vertices instead of 6. The code that adds meshes manually instead of
           using addSupportedImporterContents() should take care of propagating
           mesh names and custom attributes as well. */
        "quad-name-custom-attributes.gltf", "quad-name-custom-attributes.bin",
        {}},
    {"mesh converter", Containers::array<Containers::String>({
        "-M", "MeshOptimizerSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", "GltfSceneConverter", "MeshOptimizerSceneConverter",
        /* Converts a triangle strip to indexed triangles, which verifies that
           the output of MeshOptimizerSceneConverter got actually passed
           further and not discarded */
        "quad.gltf", "quad.bin",
        {}},
    {"mesh converter, two meshes, verbose", Containers::array<Containers::String>({
        /* Removing the generator identifier for a smaller file */
        "-I", "GltfImporter", "-C", "GltfSceneConverter", "-c", "generator=",
        "-M", "MeshOptimizerSceneConverter", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")}),
        "GltfImporter", "GltfSceneConverter", "MeshOptimizerSceneConverter",
        "two-quads.gltf", "two-quads.bin",
        "Processing mesh 0 with MeshOptimizerSceneConverter...\n"
        "Trade::MeshOptimizerSceneConverter::convert(): processing stats:\n"
        "  vertex cache:\n"
        "    4 -> 4 transformed vertices\n"
        "    1 -> 1 executed warps\n"
        "    ACMR 2 -> 2\n"
        "    ATVR 1 -> 1\n"
        "  vertex fetch:\n"
        "    64 -> 64 bytes fetched\n"
        "    overfetch 1.33333 -> 1.33333\n"
        "  overdraw:\n"
        "    65536 -> 65536 shaded pixels\n"
        "    65536 -> 65536 covered pixels\n"
        "    overdraw 1 -> 1\n"
        "Processing mesh 1 with MeshOptimizerSceneConverter...\n"
        "Trade::MeshOptimizerSceneConverter::convert(): processing stats:\n"
        "  vertex cache:\n"
        "    4 -> 4 transformed vertices\n"
        "    1 -> 1 executed warps\n"
        "    ACMR 2 -> 2\n"
        "    ATVR 1 -> 1\n"
        "  vertex fetch:\n"
        "    64 -> 64 bytes fetched\n"
        "    overfetch 1.33333 -> 1.33333\n"
        "  overdraw:\n"
        "    65536 -> 65536 shaded pixels\n"
        "    65536 -> 65536 covered pixels\n"
        "    overdraw 1 -> 1\n"},
    {"two mesh converters, two options, one mesh, verbose", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-C", "GltfSceneConverter",
        "-M", "MeshOptimizerSceneConverter",
        "-m", "nonexistentFirstOption=yes",
        "-M", "MeshOptimizerSceneConverter",
        "-m", "nonexistentSecondOption=yes", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", "GltfSceneConverter", "MeshOptimizerSceneConverter",
        "quad.gltf", "quad.bin",
        "Processing mesh 0 (1/2) with MeshOptimizerSceneConverter...\n"
        "Option nonexistentFirstOption not recognized by MeshOptimizerSceneConverter\n"
        "Trade::MeshOptimizerSceneConverter::convert(): processing stats:\n"
        "  vertex cache:\n"
        "    4 -> 4 transformed vertices\n"
        "    1 -> 1 executed warps\n"
        "    ACMR 2 -> 2\n"
        "    ATVR 1 -> 1\n"
        "  vertex fetch:\n"
        "    64 -> 64 bytes fetched\n"
        "    overfetch 1.33333 -> 1.33333\n"
        "  overdraw:\n"
        "    65536 -> 65536 shaded pixels\n"
        "    65536 -> 65536 covered pixels\n"
        "    overdraw 1 -> 1\n"
        "Processing mesh 0 (2/2) with MeshOptimizerSceneConverter...\n"
        "Option nonexistentSecondOption not recognized by MeshOptimizerSceneConverter\n"
        "Trade::MeshOptimizerSceneConverter::convert(): processing stats:\n"
        "  vertex cache:\n"
        "    4 -> 4 transformed vertices\n"
        "    1 -> 1 executed warps\n"
        "    ACMR 2 -> 2\n"
        "    ATVR 1 -> 1\n"
        "  vertex fetch:\n"
        "    64 -> 64 bytes fetched\n"
        "    overfetch 1.33333 -> 1.33333\n"
        "  overdraw:\n"
        "    65536 -> 65536 shaded pixels\n"
        "    65536 -> 65536 covered pixels\n"
        "    overdraw 1 -> 1\n"},
};

const struct {
    const char* name;
    Containers::Array<Containers::String> args;
    const char* requiresImporter;
    const char* requiresConverter;
    Containers::String message;
} ErrorData[]{
    {"missing output argument", Containers::array<Containers::String>({
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj")}),
        nullptr, nullptr,
        /* The output should be optional only for --info, required otherwise.
           No need to test anything else as that's handled by Utility::Arguments
           already. Testing just a prefix of the message. */
        "Missing command-line argument output\nUsage:\n  "},
    {"--mesh and --concatenate-meshes", Containers::array<Containers::String>({
        "--mesh", "0", "--concatenate-meshes", "a", "b"}),
        nullptr, nullptr,
        "The --mesh and --concatenate-meshes options are mutually exclusive\n"},
    {"--mesh-level but no --mesh", Containers::array<Containers::String>({
        "--mesh-level", "0", "a", "b"}),
        nullptr, nullptr,
        "The --mesh-level option can only be used with --mesh\n"},
    {"--only-mesh-attributes but no --mesh", Containers::array<Containers::String>({
        "--only-mesh-attributes", "0", "a", "b"}),
        nullptr, nullptr,
        "The --only-mesh-attributes option can only be used with --mesh or --concatenate-meshes\n"},
    {"can't load importer plugin", Containers::array<Containers::String>({
        /* Override also the plugin directory for consistent output */
        "--plugin-dir", "nonexistent", "-I", "NonexistentImporter", "whatever.obj", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        nullptr, nullptr,
        "PluginManager::Manager::load(): plugin NonexistentImporter is not static and was not found in nonexistent/importers\n"
        "Available importer plugins: "},
    {"can't open a file", Containers::array<Containers::String>({
        "noexistent.ffs", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "AnySceneImporter", nullptr,
        "Trade::AnySceneImporter::openFile(): cannot determine the format of noexistent.ffs\n"
        "Cannot open file noexistent.ffs\n"},
    {"can't map a file", Containers::array<Containers::String>({
        "noexistent.ffs", "--map", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "AnySceneImporter", nullptr,
        "Utility::Path::mapRead(): can't open noexistent.ffs: error 2 (No such file or directory)\n"
        "Cannot memory-map file noexistent.ffs\n"},
    {"no meshes found for concatenation", Containers::array<Containers::String>({
        "--concatenate-meshes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "GltfImporter", nullptr,
        Utility::format("No meshes found in {}\n", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"))},
    {"can't import a single mesh", Containers::array<Containers::String>({
        "-I", "ObjImporter", "--mesh", "0", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import the mesh\n"},
    {"can't import a mesh for concatenation", Containers::array<Containers::String>({
        "-I", "ObjImporter", "--concatenate-meshes", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import mesh 0\n"},
    {"can't import a scene for concatenation", Containers::array<Containers::String>({
        /** @todo change to an OBJ once ObjImporter imports materials (and thus
            scenes) */
        "--concatenate-meshes", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-scene.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "GltfImporter", nullptr,
        "Trade::GltfImporter::scene(): mesh index 1 in node 0 out of range for 1 meshes\n"
        "Cannot import scene 0 for mesh concatenation\n"},
    {"can't import a mesh for per-mesh processing", Containers::array<Containers::String>({
        "-I", "ObjImporter", "--remove-duplicate-vertices", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import mesh 0\n"},
    {"invalid mesh attribute filter", Containers::array<Containers::String>({
        /** @todo drop --mesh once it's not needed anymore again */
        "-I", "ObjImporter", "--mesh", "0", "--only-mesh-attributes", "LOLNEIN", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr,
        "Utility::parseNumberSequence(): unrecognized character L in LOLNEIN\n"},
    {"can't load converter plugin", Containers::array<Containers::String>({
        "-C", "NonexistentSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr,
        Utility::format("PluginManager::Manager::load(): plugin NonexistentSceneConverter is not static and was not found in {}\n"
        "Available converter plugins: ", /* Just a prefix */
        MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR)},
    {"file coversion begin failed", Containers::array<Containers::String>({
        "-I", "ObjImporter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.fbx")}),
        "ObjImporter", "AnySceneConverter",
        Utility::format("Trade::AnySceneConverter::beginFile(): cannot determine the format of {0}\n"
        "Cannot begin conversion of file {0}\n", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.fbx"))},
    {"file coversion end failed", Containers::array<Containers::String>({
        "-I", "GltfImporter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "GltfImporter", "StanfordSceneConverter",
        Utility::format("Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh, got 0\n"
        "Cannot end conversion of file {0}\n", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply"))},
    /** @todo importer conversion begin failed, once there's a plugin for which
        begin() can fail */
    {"importer coversion end failed", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-C", "MeshOptimizerSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "GltfImporter", "MeshOptimizerSceneConverter",
        "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh, got 0\n"
        "Cannot end importer conversion\n"},
    {"can't add importer contents", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-C", "StanfordSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", "StanfordSceneConverter",
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot add importer contents\n"},
    {"can't add processed meshes", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-C", "StanfordSceneConverter", "--remove-duplicate-vertices", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", "StanfordSceneConverter",
        "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh, got 2\n"
        "Cannot add mesh 1\n"},
    {"plugin doesn't support importer conversion", Containers::array<Containers::String>({
        /* Pass the same plugin twice, which means the first instance should
           get used for a mesh-to-mesh conversion */
        "-I", "ObjImporter", "-C", "StanfordSceneConverter", "-C", "StanfordSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", "StanfordSceneConverter",
        "StanfordSceneConverter doesn't support importer conversion, only ConvertMeshToData\n"},
    {"can't load mesh converter plugin", Containers::array<Containers::String>({
        "-M", "NonexistentSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr,
        Utility::format("PluginManager::Manager::load(): plugin NonexistentSceneConverter is not static and was not found in {}\n"
        "Available mesh converter plugins: ", /* Just a prefix */
        MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR)},
    {"plugin doesn't support mesh conversion", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-M", "StanfordSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", "StanfordSceneConverter",
        "StanfordSceneConverter doesn't support mesh conversion, only ConvertMeshToData\n"},
    {"can't process a mesh", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-M", "MeshOptimizerSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", "MeshOptimizerSceneConverter",
        "Trade::MeshOptimizerSceneConverter::convert(): expected a triangle mesh, got MeshPrimitive::Points\n"
        "Cannot process mesh 0 with MeshOptimizerSceneConverter\n"},
};
#endif

SceneConverterTest::SceneConverterTest() {
    addTests({&SceneConverterTest::infoImplementationEmpty});

    addInstancedTests({&SceneConverterTest::infoImplementationScenesObjects},
        Containers::arraySize(InfoImplementationScenesObjectsData));

    addInstancedTests({&SceneConverterTest::infoImplementationAnimations,
                       &SceneConverterTest::infoImplementationSkins,
                       &SceneConverterTest::infoImplementationLights,
                       &SceneConverterTest::infoImplementationCameras,
                       &SceneConverterTest::infoImplementationMaterials,
                       &SceneConverterTest::infoImplementationMeshes},
        Containers::arraySize(InfoImplementationOneOrAllData));

    addTests({&SceneConverterTest::infoImplementationMeshesBounds});

    addInstancedTests({&SceneConverterTest::infoImplementationTextures,
                       &SceneConverterTest::infoImplementationImages},
        Containers::arraySize(InfoImplementationOneOrAllData));

    addTests({&SceneConverterTest::infoImplementationReferenceCount,
              &SceneConverterTest::infoImplementationError});

    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    addInstancedTests({&SceneConverterTest::info},
        Containers::arraySize(InfoData));

    addInstancedTests({&SceneConverterTest::convert},
        Containers::arraySize(ConvertData));

    addInstancedTests({&SceneConverterTest::error},
        Containers::arraySize(ErrorData));
    #endif

    /* A subset of arguments needed by the info printing code */
    _infoArgs.addBooleanOption("info")
             .addBooleanOption("info-scenes")
             .addBooleanOption("info-objects")
             .addBooleanOption("info-animations")
             .addBooleanOption("info-skins")
             .addBooleanOption("info-lights")
             .addBooleanOption("info-cameras")
             .addBooleanOption("info-materials")
             .addBooleanOption("info-meshes")
             .addBooleanOption("info-textures")
             .addBooleanOption("info-images")
             .addBooleanOption("bounds");

    /* Create output dir, if doesn't already exist */
    Utility::Path::make(Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles"));
}

void SceneConverterTest::infoImplementationEmpty() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    } importer;

    const char* argv[]{"", "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == false);
    CORRADE_COMPARE(out.str(), "");
}

void SceneConverterTest::infoImplementationScenesObjects() {
    auto&& data = InfoImplementationScenesObjectsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        /* First scene has 4, second 7, the last three are not in any scene and
           thus not listed. Object 5 has no fields and thus not listed either. */
        UnsignedLong doObjectCount() const override { return 10; }
        UnsignedInt doSceneCount() const override { return 2; }
        Containers::String doSceneName(UnsignedInt id) override {
            return id == 0 ? "A simple scene" : "";
        }
        Containers::String doObjectName(UnsignedLong id) override {
            if(id == 0) return "Parent-less mesh";
            if(id == 2) return "Two meshes, shared among two scenes";
            if(id == 4) return "Two custom arrays";
            if(id == 6) return "Only in the second scene, but no fields, thus same as unreferenced";
            if(id == 8) return "Not in any scene";
            return "";
        }
        Containers::String doSceneFieldName(UnsignedInt name) override {
            if(name == 1337) return "DirectionVector";
            return "";
        }
        Containers::Optional<Trade::SceneData> doScene(UnsignedInt id) override {
            /* Builtin fields, some duplicated, one marked as ordered */
            if(id == 0) {
                Containers::ArrayView<UnsignedInt> parentMapping;
                Containers::ArrayView<Int> parents;
                Containers::ArrayView<UnsignedInt> meshMapping;
                Containers::ArrayView<UnsignedInt> meshes;
                Containers::ArrayTuple data{
                    {NoInit, 3, parentMapping},
                    {ValueInit, 3, parents},
                    {NoInit, 4, meshMapping},
                    {ValueInit, 4, meshes},
                };
                Utility::copy({1, 3, 2}, parentMapping);
                Utility::copy({2, 0, 2, 1}, meshMapping);
                /* No need to fill the data, zero-init is fine */
                return Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 4, std::move(data), {
                    Trade::SceneFieldData{Trade::SceneField::Parent, parentMapping, parents},
                    Trade::SceneFieldData{Trade::SceneField::Mesh, meshMapping, meshes, Trade::SceneFieldFlag::OrderedMapping},
                }};
            }

            /* Two custom fields, one array. Stored as an external memory. */
            if(id == 1) {
                return Trade::SceneData{Trade::SceneMappingType::UnsignedByte, 8, Trade::DataFlag::ExternallyOwned|Trade::DataFlag::Mutable, scene2Data, {
                    Trade::SceneFieldData{Trade::sceneFieldCustom(42), Containers::arrayView(scene2Data->customMapping), Containers::arrayView(scene2Data->custom)},
                    Trade::SceneFieldData{Trade::sceneFieldCustom(1337), Trade::SceneMappingType::UnsignedByte, scene2Data->customArrayMapping, Trade::SceneFieldType::Short, scene2Data->customArray, 3},
                }};
            }

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        struct {
            UnsignedByte customMapping[2];
            Double custom[2];
            UnsignedByte customArrayMapping[3];
            Vector3s customArray[3];
        } scene2Data[1]{{
            /* No need to fill the data, zero-init is fine */
            {7, 3}, {}, {2, 4, 4}, {}
        }};
    } importer;

    const char* argv[]{"", data.arg};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join({SCENETOOLS_TEST_DIR, "SceneConverterTestFiles", data.expected}),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationAnimations() {
    auto&& data = InfoImplementationOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 2; }
        Containers::String doAnimationName(UnsignedInt id) override {
            return id == 1 ? "Custom track duration and interpolator function" : "";
        }
        Containers::Optional<Trade::AnimationData> doAnimation(UnsignedInt id) override {
            /* First has two tracks with a shared time and implicit duration,
               one with a different result type. */
            if(id == 0) {
                Containers::ArrayView<Float> time;
                Containers::ArrayView<Vector2> translation;
                Containers::ArrayView<CubicHermite2D> rotation;
                Containers::ArrayTuple data{
                    {ValueInit, 3, time},
                    {ValueInit, 3, translation},
                    {ValueInit, 3, rotation}
                };
                Utility::copy({0.5f, 1.0f, 1.25f}, time);
                return Trade::AnimationData{std::move(data), {
                    /** @todo cleanup once AnimationTrackData has sane
                        constructors */
                    Trade::AnimationTrackData{Trade::AnimationTrackTargetType::Translation2D, 17, Animation::TrackView<const Float, const Vector2>{time, translation, Animation::Interpolation::Linear, Animation::Extrapolation::DefaultConstructed, Animation::Extrapolation::Constant}},
                    Trade::AnimationTrackData{Trade::AnimationTrackTargetType::Rotation2D, 17, Animation::TrackView<const Float, const CubicHermite2D>{time, rotation, Animation::Interpolation::Constant, Animation::Extrapolation::Extrapolated}},
                }};
            }

            /* Second has track duration different from animation duration and
               a custom interpolator. Stored as an external memory. */
            if(id == 1) {
                return Trade::AnimationData{Trade::DataFlag::ExternallyOwned, animation2Data, {
                    /** @todo cleanup once AnimationTrackData has sane
                        constructors */
                    Trade::AnimationTrackData{Trade::AnimationTrackTargetType::Scaling3D, 666, Animation::TrackView<const Float, const Vector3>{animation2Data->time, animation2Data->scaling, Math::lerp, Animation::Extrapolation::DefaultConstructed, Animation::Extrapolation::Constant}},
                }, {0.1f, 1.3f}};
            }

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        struct {
            Float time[5];
            Vector3 scaling[5];
        } animation2Data[1]{{
            {0.75f, 0.75f, 1.0f, 1.0f, 1.25f},
            {}
        }};
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-animations" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-animations.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationSkins() {
    auto&& data = InfoImplementationOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSkin2DCount() const override { return 2; }
        Containers::String doSkin2DName(UnsignedInt id) override {
            return id == 1 ? "Second 2D skin, external data" : "";
        }
        Containers::Optional<Trade::SkinData2D> doSkin2D(UnsignedInt id) override {
            /* First a regular skin, second externally owned */
            if(id == 0) return Trade::SkinData2D{
                {3, 6, 7, 12, 22},
                {{}, {}, {}, {}, {}}
            };

            if(id == 1) return Trade::SkinData2D{Trade::DataFlag::ExternallyOwned, skin2JointData, Trade::DataFlag::ExternallyOwned, skin2MatrixData};

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doSkin3DCount() const override { return 3; }
        Containers::String doSkin3DName(UnsignedInt id) override {
            return id == 0 ? "First 3D skin, external data" : "";
        }
        Containers::Optional<Trade::SkinData3D> doSkin3D(UnsignedInt id) override {
            /* Reverse order in 3D, plus one more to ensure the count isn't
               mismatched between 2D and 3D */
            if(id == 0) return Trade::SkinData3D{Trade::DataFlag::ExternallyOwned, skin3JointData, Trade::DataFlag::ExternallyOwned, skin3MatrixData};

            if(id == 1) return Trade::SkinData3D{
                {3, 22},
                {{}, {}}
            };

            if(id == 2) return Trade::SkinData3D{
                {3},
                {{}}
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt skin2JointData[15];
        Matrix3 skin2MatrixData[15];
        UnsignedInt skin3JointData[12];
        Matrix4 skin3MatrixData[12];
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-skins" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-skins.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationLights() {
    auto&& data = InfoImplementationOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 2; }
        Containers::String doLightName(UnsignedInt id) override {
            return id == 1 ?  "Directional light with always-implicit attenuation and range" : "";
        }
        Containers::Optional<Trade::LightData> doLight(UnsignedInt id) override {
            /* First a blue spot light */
            if(id == 0) return Trade::LightData{
                Trade::LightData::Type::Spot,
                0x3457ff_rgbf,
                15.0f,
                {1.2f, 0.3f, 0.04f},
                100.0f,
                55.0_degf,
                85.0_degf
            };

            /* Second a yellow directional light with infinite range */
            if(id == 1) return Trade::LightData{
                Trade::LightData::Type::Directional,
                0xff5734_rgbf,
                5.0f
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-lights" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-lights.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationCameras() {
    auto&& data = InfoImplementationOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 3; }
        Containers::String doCameraName(UnsignedInt id) override {
            return id == 0 ? "Orthographic 2D" : "";
        }
        Containers::Optional<Trade::CameraData> doCamera(UnsignedInt id) override {
            /* First 2D ortho camera, where near/far will get omited */
            if(id == 0) return Trade::CameraData{
                Trade::CameraType::Orthographic2D,
                {5.0f, 6.0f},
                0.0f, 0.0f
            };

            /* 3D ortho camera */
            if(id == 1) return Trade::CameraData{
                Trade::CameraType::Orthographic3D,
                {2.0f, 3.0f},
                -1.0f, 0.5f
            };

            /* Third a perspective camera, specified with size, but printed
               with FoV */
            if(id == 2) return Trade::CameraData{
                Trade::CameraType::Perspective3D,
                35.0_degf, 4.0f/3.0f, 0.01f, 100.0f
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-cameras" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-cameras.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationMaterials() {
    auto&& data = InfoImplementationOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 2; }
        Containers::String doMaterialName(UnsignedInt id) override {
            return id == 1 ? "Lots o' laierz" : "";
        }
        Containers::Optional<Trade::MaterialData> doMaterial(UnsignedInt id) override {
            /* First has custom attributes */
            if(id == 0) return Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, {
                {Trade::MaterialAttribute::BaseColor, 0x3bd26799_rgbaf},
                {Trade::MaterialAttribute::DoubleSided, true},
                {Trade::MaterialAttribute::EmissiveColor, 0xe9eca_rgbf},
                {Trade::MaterialAttribute::RoughnessTexture, 67u},
                {Trade::MaterialAttribute::RoughnessTextureMatrix, Matrix3::translation({0.25f, 0.75f})},
                {Trade::MaterialAttribute::RoughnessTextureSwizzle, Trade::MaterialTextureSwizzle::B},
                {"reflectionAngle", 35.0_degf},
                /* These shouldn't have a color swatch rendered */
                {"notAColour4", Vector4{0.1f, 0.2f, 0.3f, 0.4f}},
                {"notAColour3", Vector3{0.2f, 0.3f, 0.4f}},
                {"deadBeef", reinterpret_cast<const void*>(0xdeadbeef)},
                {"undeadBeef", reinterpret_cast<void*>(0xbeefbeef)},
            }};

            /* Second has layers, custom layers, unnamed layers and a name */
            if(id == 1) return Trade::MaterialData{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::Phong, {
                {Trade::MaterialAttribute::DiffuseColor, 0xc7cf2f99_rgbaf},
                {Trade::MaterialLayer::ClearCoat},
                {Trade::MaterialAttribute::LayerFactor, 0.5f},
                {Trade::MaterialAttribute::LayerFactorTexture, 3u},
                {Trade::MaterialAttribute::LayerName, "anEmptyLayer"},
                {Trade::MaterialAttribute::LayerFactor, 0.25f},
                {Trade::MaterialAttribute::LayerFactorTexture, 2u},
                {"yes", "a string"},
            }, {1, 4, 5, 8}};

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-materials" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-materials.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationMeshes() {
    auto&& data = InfoImplementationOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 3; }
        UnsignedInt doMeshLevelCount(UnsignedInt id) override {
            return id == 1 ? 2 : 1;
        }
        Containers::String doMeshName(UnsignedInt id) override {
            return id == 1 ? "LODs? No, meshets." : "";
        }
        Containers::String doMeshAttributeName(UnsignedShort name) override {
            if(name == 25) return "vertices";
            if(name == 26) return "triangles";
            /* 37 (triangleCount) deliberately not named */
            if(name == 116) return "vertexCount";

            return "";
        }
        Containers::Optional<Trade::MeshData> doMesh(UnsignedInt id, UnsignedInt level) override {
            /* First is indexed & externally owned */
            if(id == 0 && level == 0) return Trade::MeshData{MeshPrimitive::Points,
                Trade::DataFlag::ExternallyOwned, indices,
                Trade::MeshIndexData{indices},
                Trade::DataFlag::ExternallyOwned|Trade::DataFlag::Mutable, points, {
                    Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(points)}
                }};

            /* Second is multi-level, with second level being indexed meshlets
               with custom (array) attributes */
            if(id == 1 && level == 0) {
                Containers::ArrayView<Vector3> positions;
                Containers::ArrayView<Vector4> tangents;
                Containers::ArrayTuple data{
                    {NoInit, 250, positions},
                    {NoInit, 250, tangents},
                };
                return Trade::MeshData{MeshPrimitive::Triangles, std::move(data), {
                    Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, tangents},
                }};
            }
            if(id == 1 && level == 1) {
                Containers::StridedArrayView2D<UnsignedInt> vertices;
                Containers::StridedArrayView2D<Vector3ub> indices;
                Containers::ArrayView<UnsignedByte> triangleCount;
                Containers::ArrayView<UnsignedByte> vertexCount;
                Containers::ArrayTuple data{
                    {NoInit, {135, 64}, vertices},
                    {NoInit, {135, 126}, indices},
                    {NoInit, 135, triangleCount},
                    {NoInit, 135, vertexCount},
                };
                return Trade::MeshData{MeshPrimitive::Meshlets, std::move(data), {
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(25), vertices},
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(26), indices},
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(37), triangleCount},
                    Trade::MeshAttributeData{Trade::meshAttributeCustom(116), vertexCount},
                }};
            }

            /* Third is an empty instance mesh */
            if(id == 2 && level == 0) return Trade::MeshData{MeshPrimitive::Instances, 15};

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedShort indices[70];
        Vector3 points[50];
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-meshes" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-meshes.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationMeshesBounds() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMeshCount() const override { return 1; }
        Containers::Optional<Trade::MeshData> doMesh(UnsignedInt, UnsignedInt) override {
            return Trade::MeshData{MeshPrimitive::Lines,
                {}, indexData, Trade::MeshIndexData{indexData},
                {}, vertexData, {
                    Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(vertexData->positions)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, Containers::arrayView(vertexData->tangent)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, Containers::arrayView(vertexData->bitangent)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(vertexData->objectId)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertexData->normal)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::arrayView(vertexData->textureCoordinates)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::Color, Containers::arrayView(vertexData->color)},
                    Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(vertexData->objectIdSecondary)},
                }};
        }

        UnsignedByte indexData[3]{15, 3, 176};

        struct {
            Vector3 positions[2];
            Vector3 tangent[2];
            Vector3 bitangent[2];
            UnsignedShort objectId[2];
            Vector3 normal[2];
            Vector2 textureCoordinates[2];
            Vector4 color[2];
            UnsignedInt objectIdSecondary[2];
        } vertexData[1]{{
            {{0.1f, -0.1f, 0.2f}, {0.2f, 0.0f, -0.2f}},
            {{0.2f, -0.2f, 0.8f}, {0.3f, 0.8f, 0.2f}},
            {{0.4f, 0.2f, 1.0f}, {0.3f, 0.9f, 0.0f}},
            {155, 12},
            {{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}},
            {{0.5f, 0.5f}, {1.5f, 0.5f}},
            {0x99336600_rgbaf, 0xff663333_rgbaf},
            {15, 337},
        }};
    } importer;

    const char* argv[]{"", "--info-meshes", "--bounds"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-meshes-bounds.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationTextures() {
    auto&& data = InfoImplementationOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 2; }
        Containers::String doTextureName(UnsignedInt id) override {
            return id == 1 ? "Name!" : "";
        }
        Containers::Optional<Trade::TextureData> doTexture(UnsignedInt id) override {
            /* First a 1D texture */
            if(id == 0) return Trade::TextureData{
                Trade::TextureType::Texture1D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                666
            };

            /* Second a 2D array texture */
            if(id == 1) return Trade::TextureData{
                Trade::TextureType::Texture2DArray,
                SamplerFilter::Linear,
                SamplerFilter::Nearest,
                SamplerMipmap::Linear,
                {SamplerWrapping::MirroredRepeat, SamplerWrapping::ClampToEdge, SamplerWrapping::MirrorClampToEdge},
                3
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-textures" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-textures.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationImages() {
    auto&& data = InfoImplementationOneOrAllData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Just the very basics to ensure image info *is* printed. Tested in full
       in ImageConverterTest. */
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 1; }
        Containers::Optional<Trade::ImageData1D> doImage1D(UnsignedInt, UnsignedInt) override {
            return Trade::ImageData1D{PixelFormat::R32F, 1024, Containers::Array<char>{NoInit, 4096}};
        }
    } importer;

    const char* argv[]{"", data.oneOrAll ? "--info-images" : "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    if(data.printVisualCheck) {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-images.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationReferenceCount() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        /* One data of each kind should be always referenced twice+, one once,
           one not at all, and one reference should be OOB */

        UnsignedLong doObjectCount() const override { return 4; }
        Containers::String doObjectName(UnsignedLong id) override {
            return id == 2 ? "Not referenced" : "";
        }
        UnsignedInt doSceneCount() const override { return 2; }
        Containers::Optional<Trade::SceneData> doScene(UnsignedInt id) override {
            if(id == 0) return Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 2, {}, sceneData3D, {
                /* To mark the scene as 3D */
                Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix4x4, nullptr},
                Trade::SceneFieldData{Trade::SceneField::Mesh,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->meshes)},
                Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->materials)},
                Trade::SceneFieldData{Trade::SceneField::Light,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->lights)},
                Trade::SceneFieldData{Trade::SceneField::Camera,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->cameras)},
                Trade::SceneFieldData{Trade::SceneField::Skin,
                    Containers::arrayView(sceneData3D->mapping),
                    Containers::arrayView(sceneData3D->skins)},
            }};
            if(id == 1) return Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 4, {}, sceneData2D, {
                /* To mark the scene as 2D */
                Trade::SceneFieldData{Trade::SceneField::Transformation, Trade::SceneMappingType::UnsignedInt, nullptr, Trade::SceneFieldType::Matrix3x3, nullptr},
                Trade::SceneFieldData{Trade::SceneField::Mesh,
                    Containers::arrayView(sceneData2D->mapping),
                    Containers::arrayView(sceneData2D->meshes)},
                Trade::SceneFieldData{Trade::SceneField::Skin,
                    Containers::arrayView(sceneData2D->mapping),
                    Containers::arrayView(sceneData2D->skins)},
            }};

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doSkin2DCount() const override { return 3; }
        Containers::String doSkin2DName(UnsignedInt id) override {
            return id == 2 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::SkinData2D> doSkin2D(UnsignedInt id) override {
            if(id == 0) return Trade::SkinData2D{
                {35, 22},
                {{}, {}}
            };
            if(id == 1) return Trade::SkinData2D{
                {33, 10, 100},
                {{}, {}, {}}
            };
            if(id == 2) return Trade::SkinData2D{
                {66},
                {{}}
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doSkin3DCount() const override { return 3; }
        Containers::String doSkin3DName(UnsignedInt id) override {
            return id == 0 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::SkinData3D> doSkin3D(UnsignedInt id) override {
            if(id == 0) return Trade::SkinData3D{
                {35, 22},
                {{}, {}}
            };
            if(id == 1) return Trade::SkinData3D{
                {37},
                {{}}
            };
            if(id == 2) return Trade::SkinData3D{
                {300, 10, 1000},
                {{}, {}, {}}
            };

            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doLightCount() const override { return 3; }
        Containers::String doLightName(UnsignedInt id) override {
            return id == 1 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::LightData> doLight(UnsignedInt id) override {
            if(id == 0) return Trade::LightData{
                Trade::LightData::Type::Directional,
                0x57ff34_rgbf,
                5.0f
            };
            if(id == 1) return Trade::LightData{
                Trade::LightData::Type::Ambient,
                0xff5734_rgbf,
                0.1f
            };
            if(id == 2) return Trade::LightData{
                Trade::LightData::Type::Directional,
                0x3457ff_rgbf,
                1.0f
            };
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doCameraCount() const override { return 3; }
        Containers::String doCameraName(UnsignedInt id) override {
            return id == 0 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::CameraData> doCamera(UnsignedInt id) override {
            if(id == 0) return Trade::CameraData{
                Trade::CameraType::Orthographic3D,
                {2.0f, 3.0f},
                -1.0f, 0.5f
            };
            if(id == 1) return Trade::CameraData{
                Trade::CameraType::Orthographic3D,
                {2.0f, 2.0f},
                0.0f, 1.0f
            };
            if(id == 2) return Trade::CameraData{
                Trade::CameraType::Orthographic2D,
                {2.0f, 2.0f},
                0.0f, 0.0f
            };
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doMaterialCount() const override { return 3; }
        Containers::String doMaterialName(UnsignedInt id) override {
            return id == 2 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::MaterialData> doMaterial(UnsignedInt id) override {
            if(id == 0) return Trade::MaterialData{{}, {
                {Trade::MaterialAttribute::DiffuseTexture, 2u},
                {Trade::MaterialAttribute::BaseColorTexture, 2u},
            }};
            if(id == 1) return Trade::MaterialData{{}, {
                {"lookupTexture", 0u},
                {"volumeTexture", 3u},
                {Trade::MaterialAttribute::NormalTexture, 17u},
                {Trade::MaterialAttribute::EmissiveTexture, 4u},
            }};
            if(id == 2) return Trade::MaterialData{{}, {}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doMeshCount() const override { return 3; }
        Containers::String doMeshName(UnsignedInt id) override {
            return id == 1 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::MeshData> doMesh(UnsignedInt id, UnsignedInt) override {
            if(id == 0) return Trade::MeshData{MeshPrimitive::Points, 5};
            if(id == 1) return Trade::MeshData{MeshPrimitive::Lines, 4};
            if(id == 2) return Trade::MeshData{MeshPrimitive::TriangleFan, 4};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doTextureCount() const override { return 5; }
        Containers::String doTextureName(UnsignedInt id) override {
            return id == 1 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::TextureData> doTexture(UnsignedInt id) override {
            if(id == 0) return Trade::TextureData{
                Trade::TextureType::Texture1D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                1
            };
            if(id == 1) return Trade::TextureData{
                Trade::TextureType::Texture1DArray,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                225
            };
            if(id == 2) return Trade::TextureData{
                Trade::TextureType::Texture2D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                0
            };
            if(id == 3) return Trade::TextureData{
                Trade::TextureType::Texture3D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                1
            };
            if(id == 4) return Trade::TextureData{
                Trade::TextureType::Texture2D,
                SamplerFilter::Nearest,
                SamplerFilter::Linear,
                SamplerMipmap::Nearest,
                SamplerWrapping::Repeat,
                0
            };
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doImage1DCount() const override { return 2; }
        Containers::String doImage1DName(UnsignedInt id) override {
            return id == 0 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::ImageData1D> doImage1D(UnsignedInt id, UnsignedInt) override {
            if(id == 0)
                return Trade::ImageData1D{PixelFormat::RGBA8I, 1, Containers::Array<char>{NoInit, 4}};
            if(id == 1)
                return Trade::ImageData1D{PixelFormat::R8I, 4, Containers::Array<char>{NoInit, 4}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doImage2DCount() const override { return 2; }
        Containers::String doImage2DName(UnsignedInt id) override {
            return id == 1 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::ImageData2D> doImage2D(UnsignedInt id, UnsignedInt) override {
            if(id == 0)
                return Trade::ImageData2D{PixelFormat::RGBA8I, {1, 2}, Containers::Array<char>{NoInit, 8}};
            if(id == 1)
                return Trade::ImageData2D{PixelFormat::R8I, {4, 1}, Containers::Array<char>{NoInit, 4}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        UnsignedInt doImage3DCount() const override { return 2; }
        Containers::String doImage3DName(UnsignedInt id) override {
            return id == 0 ? "Not referenced" : "";
        }
        Containers::Optional<Trade::ImageData3D> doImage3D(UnsignedInt id, UnsignedInt) override {
            if(id == 0)
                return Trade::ImageData3D{PixelFormat::RGBA8I, {1, 2, 1}, Containers::Array<char>{NoInit, 8}};
            if(id == 1)
                return Trade::ImageData3D{PixelFormat::R8I, {4, 1, 1}, Containers::Array<char>{NoInit, 4}};
            CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        struct {
            UnsignedInt mapping[4];
            UnsignedInt meshes[4];
            Int materials[4];
            UnsignedInt lights[4];
            UnsignedInt cameras[4];
            UnsignedInt skins[4];
        } sceneData3D[1]{{
            {0, 1, 1, 25},
            {2, 0, 2, 67},
            {0, 1, 23, 0},
            {0, 17, 0, 2},
            {166, 1, 2, 1},
            {1, 1, 22, 2}
        }};

        struct {
            UnsignedInt mapping[3];
            UnsignedInt meshes[3];
            UnsignedInt skins[3];
        } sceneData2D[1]{{
            {3, 116, 1},
            {2, 0, 23},
            {177, 0, 1}
        }};
    } importer;

    const char* argv[]{"", "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    /* Print to visually verify coloring */
    {
        Debug{} << "======================== visual color verification start =======================";
        Implementation::printInfo(Debug::isTty() ? Debug::Flags{} : Debug::Flag::DisableColors, Debug::isTty(), _infoArgs, importer, time);
        Debug{} << "======================== visual color verification end =========================";
    }

    std::ostringstream out;
    Debug redirectOutput{&out};
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, false, _infoArgs, importer, time) == false);
    CORRADE_COMPARE_AS(out.str(),
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/info-references.txt"),
        TestSuite::Compare::StringToFile);
}

void SceneConverterTest::infoImplementationError() {
    struct Importer: Trade::AbstractImporter {
        Trade::ImporterFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        /* The one single object is named, and that name should be printed
           after all error messages */
        UnsignedLong doObjectCount() const override { return 1; }
        Containers::String doObjectName(UnsignedLong) override { return "A name"; }

        UnsignedInt doSceneCount() const override { return 2; }
        Containers::Optional<Trade::SceneData> doScene(UnsignedInt id) override {
            Error{} << "Scene" << id << "error!";
            return {};
        }

        UnsignedInt doAnimationCount() const override { return 2; }
        Containers::Optional<Trade::AnimationData> doAnimation(UnsignedInt id) override {
            Error{} << "Animation" << id << "error!";
            return {};
        }

        UnsignedInt doSkin2DCount() const override { return 2; }
        Containers::Optional<Trade::SkinData2D> doSkin2D(UnsignedInt id) override {
            Error{} << "2D skin" << id << "error!";
            return {};
        }

        UnsignedInt doSkin3DCount() const override { return 2; }
        Containers::Optional<Trade::SkinData3D> doSkin3D(UnsignedInt id) override {
            Error{} << "3D skin" << id << "error!";
            return {};
        }

        UnsignedInt doLightCount() const override { return 2; }
        Containers::Optional<Trade::LightData> doLight(UnsignedInt id) override {
            Error{} << "Light" << id << "error!";
            return {};
        }

        UnsignedInt doCameraCount() const override { return 2; }
        Containers::Optional<Trade::CameraData> doCamera(UnsignedInt id) override {
            Error{} << "Camera" << id << "error!";
            return {};
        }

        UnsignedInt doMaterialCount() const override { return 2; }
        Containers::Optional<Trade::MaterialData> doMaterial(UnsignedInt id) override {
            Error{} << "Material" << id << "error!";
            return {};
        }

        UnsignedInt doMeshCount() const override { return 2; }
        Containers::Optional<Trade::MeshData> doMesh(UnsignedInt id, UnsignedInt) override {
            Error{} << "Mesh" << id << "error!";
            return {};
        }

        UnsignedInt doTextureCount() const override { return 2; }
        Containers::Optional<Trade::TextureData> doTexture(UnsignedInt id) override {
            Error{} << "Texture" << id << "error!";
            return {};
        }

        /* Errors for all image types tested in ImageConverterTest */
        UnsignedInt doImage2DCount() const override { return 2; }
        Containers::Optional<Trade::ImageData2D> doImage2D(UnsignedInt id, UnsignedInt) override {
            Error{} << "Image" << id << "error!";
            return {};
        }
    } importer;

    const char* argv[]{"", "--info"};
    CORRADE_VERIFY(_infoArgs.tryParse(Containers::arraySize(argv), argv));

    std::chrono::high_resolution_clock::duration time;

    std::ostringstream out;
    Debug redirectOutput{&out};
    Error redirectError{&out};
    /* It should return a failure */
    CORRADE_VERIFY(Implementation::printInfo(Debug::Flag::DisableColors, {}, _infoArgs, importer, time) == true);
    CORRADE_COMPARE(out.str(),
        /* It should not exit after first error... */
        "Scene 0 error!\n"
        "Scene 1 error!\n"
        "Animation 0 error!\n"
        "Animation 1 error!\n"
        "2D skin 0 error!\n"
        "2D skin 1 error!\n"
        "3D skin 0 error!\n"
        "3D skin 1 error!\n"
        "Light 0 error!\n"
        "Light 1 error!\n"
        "Camera 0 error!\n"
        "Camera 1 error!\n"
        "Material 0 error!\n"
        "Material 1 error!\n"
        "Mesh 0 error!\n"
        "Mesh 1 error!\n"
        "Texture 0 error!\n"
        "Texture 1 error!\n"
        "Image 0 error!\n"
        "Image 1 error!\n"
        /* ... and it should print all info output after the errors */
        "Object 0: A name\n");
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
namespace {

#ifdef SCENECONVERTER_EXECUTABLE_FILENAME
/** @todo take a StringIterable once it exists */
Containers::Pair<bool, Containers::String> call(Containers::ArrayView<const Containers::String> arguments) {
    /* Create a string view array for the arguments, implicitly pass the
       application name and plugin directory override */
    /** @todo drop once StringIterable exists */
    Containers::Array<Containers::StringView> argumentViews{ValueInit, arguments.size() + 3};
    argumentViews[0] = ""_s;
    argumentViews[1] = "--plugin-dir"_s;
    argumentViews[2] = MAGNUM_PLUGINS_INSTALL_DIR;
    for(std::size_t i = 0; i != arguments.size(); ++i)
        argumentViews[i + 3] = arguments[i];

    const Containers::String outputFilename = Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/output.txt");
    /** @todo clean up once Utility::System::execute() with output redirection
        exists */
    const bool success = std::system(Utility::format("{} {} > {} 2>&1",
        SCENECONVERTER_EXECUTABLE_FILENAME,
        " "_s.join(argumentViews), /** @todo handle space escaping here? */
        outputFilename
    ).data()) == 0;

    const Containers::Optional<Containers::String> output = Utility::Path::readString(outputFilename);
    CORRADE_VERIFY(output);

    return {success, std::move(*output)};
}
#endif

}

void SceneConverterTest::info() {
    auto&& data = InfoData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef SCENECONVERTER_EXECUTABLE_FILENAME
    #ifdef CORRADE_TARGET_UNIX
    CORRADE_SKIP("magnum-sceneconverter not built, can't test");
    #else
    CORRADE_SKIP("Executable testing implemented only on Unix platforms");
    #endif
    #else
    /* Check if required plugins can be loaded. Catches also ABI and interface
       mismatch errors. */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    if(!(importerManager.load("ObjImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("ObjImporter plugin can't be loaded.");

    Containers::Array<Containers::String> args{InPlaceInit,
        {"-I", "ObjImporter", "--info", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj")}};
    arrayAppend(args, data.args);

    CORRADE_VERIFY(true); /* capture correct function name */

    Containers::Pair<bool, Containers::String> output = call(args);
    CORRADE_COMPARE_AS(output.second(),
        Utility::Path::join({SCENETOOLS_TEST_DIR, "SceneConverterTestFiles", data.expected}),
        TestSuite::Compare::StringToFile);
    CORRADE_VERIFY(output.first());
    #endif
}

void SceneConverterTest::convert() {
    auto&& data = ConvertData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef SCENECONVERTER_EXECUTABLE_FILENAME
    #ifdef CORRADE_TARGET_UNIX
    CORRADE_SKIP("magnum-sceneconverter not built, can't test");
    #else
    CORRADE_SKIP("Executable testing implemented only on Unix platforms");
    #endif
    #else
    /* Check if required plugins can be loaded. Catches also ABI and interface
       mismatch errors. */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    if(data.requiresImporter && !(importerManager.load(data.requiresImporter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImporter << "plugin can't be loaded.");
    if(data.requiresConverter && !(converterManager.load(data.requiresConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresConverter << "plugin can't be loaded.");
    if(data.requiresMeshConverter && !(converterManager.load(data.requiresMeshConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresMeshConverter << "plugin can't be loaded.");
    /* AnySceneImporter & AnySceneConverter are required implicitly for
       simplicity */
    if(!(importerManager.load("AnySceneImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnySceneImporter plugin can't be loaded.");
    if(!(converterManager.load("AnySceneConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnySceneConverter plugin can't be loaded.");

    CORRADE_VERIFY(true); /* capture correct function name */

    Containers::Pair<bool, Containers::String> output = call(data.args);
    CORRADE_COMPARE(output.second(), data.message);
    CORRADE_VERIFY(output.first());

    CORRADE_COMPARE_AS(Utility::Path::join({SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles", data.expected}),
        Utility::Path::join({SCENETOOLS_TEST_DIR, "SceneConverterTestFiles", data.expected}),
        TestSuite::Compare::File);
    if(data.expected2) CORRADE_COMPARE_AS(Utility::Path::join({SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles", data.expected2}),
        Utility::Path::join({SCENETOOLS_TEST_DIR, "SceneConverterTestFiles", data.expected2}),
        TestSuite::Compare::File);
    #endif
}

void SceneConverterTest::error() {
    auto&& data = ErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef SCENECONVERTER_EXECUTABLE_FILENAME
    #ifdef CORRADE_TARGET_UNIX
    CORRADE_SKIP("magnum-sceneconverter not built, can't test");
    #else
    CORRADE_SKIP("Executable testing implemented only on Unix platforms");
    #endif
    #else
    /* Check if required plugins can be loaded. Catches also ABI and interface
       mismatch errors. */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    if(data.requiresImporter && !(importerManager.load(data.requiresImporter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImporter << "plugin can't be loaded.");
    if(data.requiresConverter && !(converterManager.load(data.requiresConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresConverter << "plugin can't be loaded.");

    CORRADE_VERIFY(true); /* capture correct function name */

    Containers::Pair<bool, Containers::String> output = call(data.args);
    /* If the message ends with a \n, assume it's the whole message. Otherwise
       it's just a prefix. */
    if(data.message.hasSuffix('\n'))
        CORRADE_COMPARE(output.second(), data.message);
    else
        CORRADE_COMPARE_AS(output.second(),
        data.message,
        TestSuite::Compare::StringHasPrefix);
    /* It should return a non-zero code */
    CORRADE_VERIFY(!output.first());
    #endif
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::SceneConverterTest)
