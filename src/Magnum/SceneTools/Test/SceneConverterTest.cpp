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

#include <cstdlib>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/TestSuite/Compare/StringToFile.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractSceneConverter.h"

#include "configure.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct SceneConverterTest: TestSuite::Tester {
    explicit SceneConverterTest();

    void info();
    void convert();
    void error();
};

using namespace Containers::Literals;

const struct {
    const char* name;
    Containers::Array<Containers::String> args;
    const char* requiresImporter;
    const char* requiresConverter;
    const char* requiresImageConverter;
    const char* expected;
} InfoData[]{
    {"importer", Containers::array<Containers::String>({
        "--info-importer", "-i", "someOption=yes"}),
        "AnySceneImporter", nullptr, nullptr,
        "info-importer.txt"},
    {"converter", Containers::array<Containers::String>({
        "-C", "AnySceneConverter", "--info-converter", "-c", "someOption=yes"}),
        nullptr, "AnySceneConverter", nullptr,
        "info-converter.txt"},
    {"converter, implicit", Containers::array<Containers::String>({
        "--info-converter", "-c", "someOption=yes"}),
        nullptr, "AnySceneConverter", nullptr,
        "info-converter.txt"},
    {"image converter", Containers::array<Containers::String>({
        "-P", "AnyImageConverter", "--info-image-converter", "-p", "someOption=yes"}),
        nullptr, nullptr, "AnyImageConverter",
        "info-image-converter.txt"},
    {"image converter, implicit", Containers::array<Containers::String>({
        "--info-image-converter", "-p", "someOption=yes"}),
        nullptr, nullptr, "AnyImageConverter",
        "info-image-converter.txt"},
    {"importer, ignored input and output", Containers::array<Containers::String>({
        "--info-importer", "input.obj", "output.ply"}),
        "AnySceneImporter", nullptr, nullptr,
        "info-importer-ignored-input-output.txt"},
    {"data", Containers::array<Containers::String>({
        "-I", "ObjImporter", "--info", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj")}),
        "ObjImporter", nullptr, nullptr,
        "info-data.txt"},
    {"data, map", Containers::array<Containers::String>({
        "--map", "-I", "ObjImporter", "--info", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj")}),
        "ObjImporter", nullptr, nullptr,
        /** @todo change to something else once we have a plugin that can
            zero-copy pass the imported data */
        "info-data.txt"},
    {"data, ignored output file", Containers::array<Containers::String>({
        "-I", "ObjImporter", "--info", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), "whatever.ply"}),
        "ObjImporter", nullptr, nullptr,
        "info-data-ignored-output.txt"}
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Containers::Array<Containers::String> args;
    const char* requiresImporter;
    const char* requiresImporter2;
    const char* requiresConverter;
    /* One for image, one for file conversion */
    const char* requiresImageConverter[2];
    const char* requiresMeshConverter;
    const char* expected;
    const char* expected2;
    Containers::String message;
} ConvertData[]{
    {"one mesh", Containers::array<Containers::String>({
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, whole scene converter", Containers::array<Containers::String>({
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "ObjImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one mesh, explicit importer and converter", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, map", Containers::array<Containers::String>({
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, options", Containers::array<Containers::String>({
        /* It's silly, but since we have option propagation tested in
           AnySceneImporter / AnySceneConverter .cpp already, it's enough to
           just verify the (nonexistent) options arrive there */
        "-i", "nonexistentOption=13", "-c", "nonexistentConverterOption=26",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
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
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentOption not recognized by ObjImporter\n"
        "Option nonexistentConverterOption not recognized by StanfordSceneConverter\n"},
    {"two meshes + scene", Containers::array<Containers::String>({
        /* Removing the generator identifier to have the file fully roundtrip */
        "-c", "generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        /* There should be a minimal difference compared to the original */
        "two-quads.gltf", "two-quads.bin",
        {}},
    {"concatenate meshes without a scene", Containers::array<Containers::String>({
        "--concatenate-meshes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-duplicates.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad-duplicates.ply", nullptr,
        {}},
    {"concatenate meshes with a scene", Containers::array<Containers::String>({
        "--concatenate-meshes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles-transformed.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-duplicates.ply")}),
        "GltfImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad-duplicates.ply", nullptr,
        {}},
    {"concatenate meshes with a scene but no default scene", Containers::array<Containers::String>({
        "--concatenate-meshes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles-transformed-no-default-scene.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-duplicates.ply")}),
        "GltfImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad-duplicates.ply", nullptr,
        {}},
    /** @todo drop --mesh once it's not needed anymore again, then add a
        multi-mesh variant */
    {"one mesh, filter mesh attributes", Containers::array<Containers::String>({
        /* Only 0 gets picked from here, others ignored */
        "--mesh", "0", "--only-mesh-attributes", "17,0,25-36",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-normals-texcoords.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"concatenate meshes, filter mesh attributes", Containers::array<Containers::String>({
        "--concatenate-meshes", "--only-mesh-attributes", "17,0,25-36",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-normals-texcoords.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove vertex duplicates", Containers::array<Containers::String>({
        "--remove-duplicate-vertices",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove duplicate vertices, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--remove-duplicate-vertices", "-v", "-I", "ObjImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Mesh 0 duplicate removal: 6 -> 4 vertices\n"},
    {"one selected mesh, remove duplicate vertices, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--mesh", "1", "--remove-duplicate-vertices", "-v", "-I", "GltfImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "GltfImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
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
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        /* There should be a minimal difference compared to the original */
        "two-quads.gltf", "two-quads.bin",
        "Mesh 0 duplicate removal: 5 -> 4 vertices\n"
        "Mesh 1 duplicate removal: 6 -> 4 vertices\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding scene 0 out of 1\n"},
    {"one implicit mesh, remove duplicate vertices fuzzy", Containers::array<Containers::String>({
        "--remove-duplicate-vertices-fuzzy", "1.0e-1",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates-fuzzy.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove duplicate vertices fuzzy, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--remove-duplicate-vertices-fuzzy", "1.0e-1", "-v", "-I", "ObjImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates-fuzzy.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Mesh 0 fuzzy duplicate removal: 6 -> 4 vertices\n"},
    {"one selected mesh, remove duplicate vertices fuzzy, verbose", Containers::array<Containers::String>({
        /* Forcing the importer and converter to avoid AnySceneImporter /
           AnySceneConverter delegation messages */
        "--mesh 1", "--remove-duplicate-vertices-fuzzy", "1.0e-1", "-v", "-I", "GltfImporter", "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates-fuzzy.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "GltfImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
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
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "two-quads.gltf", "two-quads.bin",
        "Mesh 0 fuzzy duplicate removal: 5 -> 4 vertices\n"
        "Mesh 1 fuzzy duplicate removal: 6 -> 4 vertices\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding scene 0 out of 1\n"},
    {"one implicit mesh, two converters", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one implicit mesh, two converters, explicit last", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter", "-C", "GltfSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one implicit mesh, two converters, verbose", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.gltf", "quad.bin",
        /* While this looks like a no-op in the output, it converts a
           triangle strip to indexed triangles, which verifies that the output
           of MeshOptimizerSceneConverter got actually passed further and not
           discarded */
        "Trade::AnySceneImporter::openFile(): using GltfImporter\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 1\n"
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
        "Trade::AnySceneConverter::beginFile(): using GltfSceneConverter\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 1\n"},
    {"one implicit mesh, two converters, explicit last, verbose", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter", "-C", "GltfSceneConverter", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.gltf", "quad.bin",
        /* As the importers and converters are specified explicitly, there's
           no messages from AnySceneConverter, OTOH as we have more than one -C
           option the verbose output includes a progress info */
        "Trade::AnySceneImporter::openFile(): using GltfImporter\n"
        "Processing (1/2) with MeshOptimizerSceneConverter...\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 1\n"
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
        "Saving output (2/2) with GltfSceneConverter...\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 1\n"},
    {"one implicit mesh, two converters, options for the first only", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        "-c", "nonexistentMeshOptimizerOption=yes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"},
    {"one implicit mesh, two converters, explicit last, options for the first only", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        "-c", "nonexistentMeshOptimizerOption=yes",
        "-C", "StanfordSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"},
    {"one implicit mesh, two converters, options for both", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        "-c", "nonexistentMeshOptimizerOption=yes",
        "-c", "nonexistentAnyConverterOption=no",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"
        "Trade::AnySceneConverter::beginFile(): option nonexistentAnyConverterOption not recognized by GltfSceneConverter\n"},
    {"one implicit mesh, two converters, explicit last, options for both", Containers::array<Containers::String>({
        "-C", "MeshOptimizerSceneConverter",
        "-c", "nonexistentMeshOptimizerOption=yes",
        "-C", "StanfordSceneConverter",
        "-c", "nonexistentStanfordConverterOption=no",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"
        "Option nonexistentStanfordConverterOption not recognized by StanfordSceneConverter\n"},
    {"one mesh, remove duplicate vertices, two converters, verbose", Containers::array<Containers::String>({
        "--remove-duplicate-vertices",
        "-C", "MeshOptimizerSceneConverter", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
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
        "Trade::AnySceneConverter::beginFile(): using StanfordSceneConverter\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 1\n"},
    {"implicit custom-processed mesh with a name and custom attributes", Containers::array<Containers::String>({
        /* Removing the generator identifier to have the file closer to the
           original */
        "--remove-duplicate-vertices", "-c", "generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-name-custom-attributes-duplicates.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-name-custom-attributes.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
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
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        /* The output should be mostly the same, except that there's now only 4
           vertices instead of 6. The code that adds meshes manually instead of
           using addSupportedImporterContents() should take care of propagating
           mesh names and custom attributes as well. */
        "quad-name-custom-attributes.gltf", "quad-name-custom-attributes.bin",
        {}},
    {"mesh converter", Containers::array<Containers::String>({
        "-M", "MeshOptimizerSceneConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter",
        {}, "MeshOptimizerSceneConverter",
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
        "GltfImporter", nullptr, "GltfSceneConverter",
        {}, "MeshOptimizerSceneConverter",
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
        "    overdraw 1 -> 1\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding scene 0 out of 1\n"},
    {"two mesh converters, two options, one mesh, verbose", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-C", "GltfSceneConverter",
        "-M", "MeshOptimizerSceneConverter",
        "-m", "nonexistentFirstOption=yes",
        "-M", "MeshOptimizerSceneConverter",
        "-m", "nonexistentSecondOption=yes", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")}),
        "GltfImporter", nullptr, "GltfSceneConverter",
        {}, "MeshOptimizerSceneConverter",
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
    {"2D image converter, two images", Containers::array<Containers::String>({
        "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
        /* Removing the generator identifier for a smaller file, bundling the
           images to avoid having too many files */
        "-c", "bundleImages,generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d-1x1.gltf")}),
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        "images-2d-1x1.gltf", "images-2d-1x1.bin",
        {}},
    {"2D image converter, two images, verbose", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-C", "GltfSceneConverter",
        "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
        /* Removing the generator identifier for a smaller file, bundling the
           images to avoid having too many files */
        "-c", "bundleImages,generator=", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d-1x1.gltf")}),
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        "images-2d-1x1.gltf", "images-2d-1x1.bin",
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Processing 2D image 0 with StbResizeImageConverter...\n"
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Processing 2D image 1 with StbResizeImageConverter...\n"},
    {"two 2D image converters, two images, verbose", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-C", "GltfSceneConverter",
        "-P", "StbResizeImageConverter", "-p", "size=\"2 2\"",
        "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
        /* Removing the generator identifier for a smaller file, bundling the
           images to avoid having too many files */
        "-c", "bundleImages,generator=", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d-1x1.gltf")}),
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        "images-2d-1x1.gltf", "images-2d-1x1.bin",
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Processing 2D image 0 (1/2) with StbResizeImageConverter...\n"
        "Processing 2D image 0 (2/2) with StbResizeImageConverter...\n"
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Processing 2D image 1 (1/2) with StbResizeImageConverter...\n"
        "Processing 2D image 1 (2/2) with StbResizeImageConverter...\n"},
    {"3D image converter, two images", Containers::array<Containers::String>({
        "-i", "experimentalKhrTextureKtx",
        "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
        /* Removing the generator identifier for a smaller file, bundling the
           images to avoid having too many files */
        "-c", "experimentalKhrTextureKtx,imageConverter=KtxImageConverter,bundleImages,generator=",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-3d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-3d-1x1x1.gltf")}),
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "KtxImageConverter"}, nullptr,
        "images-3d-1x1x1.gltf", "images-3d-1x1x1.bin",
        {}},
    {"3D image converter, two images, verbose", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-C", "GltfSceneConverter",
        "-i", "experimentalKhrTextureKtx",
        "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
        /* Removing the generator identifier for a smaller file, bundling the
           images to avoid having too many files */
        "-c", "experimentalKhrTextureKtx,imageConverter=KtxImageConverter,bundleImages,generator=", "-v",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-3d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-3d-1x1x1.gltf")}),
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "KtxImageConverter"}, nullptr,
        "images-3d-1x1x1.gltf", "images-3d-1x1x1.bin",
        "Trade::AnyImageImporter::openFile(): using KtxImporter\n"
        "Processing 3D image 0 with StbResizeImageConverter...\n"
        "Trade::AnyImageImporter::openFile(): using KtxImporter\n"
        "Processing 3D image 1 with StbResizeImageConverter...\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding texture 0 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding texture 1 out of 2\n"},
};

const struct {
    const char* name;
    Containers::Array<Containers::String> args;
    const char* requiresImporter;
    const char* requiresImageImporter;
    const char* requiresConverter;
    const char* requiresImageConverter;
    Containers::String message;
} ErrorData[]{
    {"missing output argument", Containers::array<Containers::String>({
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj")}),
        nullptr, nullptr, nullptr, nullptr,
        /* The output should be optional only for --info, required otherwise.
           No need to test anything else as that's handled by Utility::Arguments
           already. Testing just a prefix of the message. */
        "Missing command-line argument output\nUsage:\n  "},
    {"--mesh and --concatenate-meshes", Containers::array<Containers::String>({
        "--mesh", "0", "--concatenate-meshes", "a", "b"}),
        nullptr, nullptr, nullptr, nullptr,
        "The --mesh and --concatenate-meshes options are mutually exclusive\n"},
    {"--mesh-level but no --mesh", Containers::array<Containers::String>({
        "--mesh-level", "0", "a", "b"}),
        nullptr, nullptr, nullptr, nullptr,
        "The --mesh-level option can only be used with --mesh\n"},
    {"--only-mesh-attributes but no --mesh", Containers::array<Containers::String>({
        "--only-mesh-attributes", "0", "a", "b"}),
        nullptr, nullptr, nullptr, nullptr,
        "The --only-mesh-attributes option can only be used with --mesh or --concatenate-meshes\n"},
    {"can't load importer plugin", Containers::array<Containers::String>({
        /* Override also the plugin directory for consistent output */
        "--plugin-dir", "nonexistent", "-I", "NonexistentImporter", "whatever.obj", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        nullptr, nullptr, nullptr, nullptr,
        "PluginManager::Manager::load(): plugin NonexistentImporter is not static and was not found in nonexistent/importers\n"
        "Available importer plugins: "},
    {"can't open a file", Containers::array<Containers::String>({
        "noexistent.ffs", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "AnySceneImporter", nullptr, nullptr, nullptr,
        "Trade::AnySceneImporter::openFile(): cannot determine the format of noexistent.ffs\n"
        "Cannot open file noexistent.ffs\n"},
    {"can't map a file", Containers::array<Containers::String>({
        "noexistent.ffs", "--map", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "AnySceneImporter", nullptr, nullptr, nullptr,
        "Utility::Path::mapRead(): can't open noexistent.ffs: error 2 (No such file or directory)\n"
        "Cannot memory-map file noexistent.ffs\n"},
    {"no meshes found for concatenation", Containers::array<Containers::String>({
        "--concatenate-meshes",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "GltfImporter", nullptr, nullptr, nullptr,
        Utility::format("No meshes found in {}\n", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"))},
    {"can't import a single mesh", Containers::array<Containers::String>({
        "-I", "ObjImporter", "--mesh", "0", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, nullptr, nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import the mesh\n"},
    {"can't import a mesh for concatenation", Containers::array<Containers::String>({
        "-I", "ObjImporter", "--concatenate-meshes", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, nullptr, nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import mesh 0\n"},
    {"can't import a scene for concatenation", Containers::array<Containers::String>({
        /** @todo change to an OBJ once ObjImporter imports materials (and thus
            scenes) */
        "--concatenate-meshes", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-scene.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "GltfImporter", nullptr, nullptr, nullptr,
        "Trade::GltfImporter::scene(): mesh index 1 in node 0 out of range for 1 meshes\n"
        "Cannot import scene 0 for mesh concatenation\n"},
    {"can't import a mesh for per-mesh processing", Containers::array<Containers::String>({
        "-I", "ObjImporter", "--remove-duplicate-vertices", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, nullptr, nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import mesh 0\n"},
    {"invalid mesh attribute filter", Containers::array<Containers::String>({
        /** @todo drop --mesh once it's not needed anymore again */
        "-I", "ObjImporter", "--mesh", "0", "--only-mesh-attributes", "LOLNEIN", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, nullptr, nullptr,
        "Utility::parseNumberSequence(): unrecognized character L in LOLNEIN\n"},
    {"can't load converter plugin", Containers::array<Containers::String>({
        "-C", "NonexistentSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, nullptr, nullptr,
        Utility::format("PluginManager::Manager::load(): plugin NonexistentSceneConverter is not static and was not found in {}\n"
        "Available converter plugins: ", /* Just a prefix */
        MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR)},
    {"file coversion begin failed", Containers::array<Containers::String>({
        "-I", "ObjImporter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.fbx")}),
        "ObjImporter", nullptr, "AnySceneConverter", nullptr,
        Utility::format("Trade::AnySceneConverter::beginFile(): cannot determine the format of {0}\n"
        "Cannot begin conversion of file {0}\n", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.fbx"))},
    {"file coversion end failed", Containers::array<Containers::String>({
        "-I", "GltfImporter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "GltfImporter", nullptr, "StanfordSceneConverter", nullptr,
        Utility::format("Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh, got 0\n"
        "Cannot end conversion of file {0}\n", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply"))},
    /** @todo importer conversion begin failed, once there's a plugin for which
        begin() can fail */
    {"importer coversion end failed", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-C", "MeshOptimizerSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "GltfImporter", nullptr, "MeshOptimizerSceneConverter", nullptr,
        "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh, got 0\n"
        "Cannot end importer conversion\n"},
    {"can't add importer contents", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-C", "StanfordSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot add importer contents\n"},
    {"can't add processed meshes", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-C", "StanfordSceneConverter", "--remove-duplicate-vertices", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", nullptr,
        "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh, got 2\n"
        "Cannot add mesh 1\n"},
    {"plugin doesn't support importer conversion", Containers::array<Containers::String>({
        /* Pass the same plugin twice, which means the first instance should
           get used for a mesh-to-mesh conversion */
        "-I", "ObjImporter", "-C", "StanfordSceneConverter", "-C", "StanfordSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", nullptr,
        "StanfordSceneConverter doesn't support importer conversion, only ConvertMeshToData\n"},
    {"can't load mesh converter plugin", Containers::array<Containers::String>({
        "-M", "NonexistentSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, nullptr, nullptr,
        Utility::format("PluginManager::Manager::load(): plugin NonexistentSceneConverter is not static and was not found in {}\n"
        "Available mesh converter plugins: ", /* Just a prefix */
        MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR)},
    {"plugin doesn't support mesh conversion", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-M", "StanfordSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, "StanfordSceneConverter", nullptr,
        "StanfordSceneConverter doesn't support mesh conversion, only ConvertMeshToData\n"},
    {"can't process a mesh", Containers::array<Containers::String>({
        "-I", "ObjImporter", "-M", "MeshOptimizerSceneConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")}),
        "ObjImporter", nullptr, "MeshOptimizerSceneConverter", nullptr,
        "Trade::MeshOptimizerSceneConverter::convert(): expected a triangle mesh, got MeshPrimitive::Points\n"
        "Cannot process mesh 0 with MeshOptimizerSceneConverter\n"},
    {"can't import a 2D image for per-image processing", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-P", "NonexistentImageConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-image-2d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "GltfImporter", "PngImporter", nullptr, nullptr,
        Utility::format("\n" /* Just a suffix */
        "Trade::AbstractImporter::openFile(): cannot open file {}\n"
        "Cannot import 2D image 0\n", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/nonexistent.png"))},
    {"can't import a 3D image for per-image processing", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx", "-P", "NonexistentImageConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-image-3d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "GltfImporter", "KtxImporter", nullptr, nullptr,
        Utility::format("\n" /* Just a suffix */
        "Trade::AbstractImporter::openFile(): cannot open file {}\n"
        "Cannot import 3D image 0\n", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/nonexistent.ktx2"))},
    {"can't load image converter plugin", Containers::array<Containers::String>({
        "-P", "NonexistentImageConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "GltfImporter", "PngImporter", nullptr, nullptr,
        Utility::format("PluginManager::Manager::load(): plugin NonexistentImageConverter is not static and was not found in {}\n"
        "Available image converter plugins: ", /* Just a prefix */
        MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR)},
    {"plugin doesn't support image conversion", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-P", "PngImageConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "GltfImporter", "PngImporter", nullptr, "PngImageConverter",
        "PngImageConverter doesn't support 2D image conversion, only Convert2DToData\n"},
    {"plugin doesn't support compressed image conversion", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-P", "StbResizeImageConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/image-dds.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "GltfImporter", "DdsImporter", nullptr, "StbResizeImageConverter",
        /** @todo add an ability to pass options to AnyImageImporter to
            suppress this */
        "Trade::DdsImporter::openData(): block-compressed image is assumed to be encoded with Y down and Z forward, imported data will have wrong orientation. Enable assumeYUpZBackward to suppress this warning.\n"
        "StbResizeImageConverter doesn't support compressed 2D image conversion, only Convert2D|Convert3D\n"},
    {"can't process a 2D image", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-P", "StbResizeImageConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "GltfImporter", "PngImporter", nullptr, "StbResizeImageConverter",
        "Trade::StbResizeImageConverter::convert(): output size was not specified\n"
        "Cannot process 2D image 0 with StbResizeImageConverter\n"},
    {"can't process a 3D image", Containers::array<Containers::String>({
        "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx", "-P", "StbResizeImageConverter", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-3d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "GltfImporter", "KtxImporter", nullptr, "StbResizeImageConverter",
        "Trade::StbResizeImageConverter::convert(): output size was not specified\n"
        "Cannot process 3D image 0 with StbResizeImageConverter\n"},
    {"can't add processed 2D images", Containers::array<Containers::String>({
        "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
        "-I", "GltfImporter", "-C", "GltfSceneConverter",
        "-c", "imageConverter=NonexistentImageConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "ObjImporter", "PngImporter", "GltfSceneConverter", "StbResizeImageConverter",
        "\n" /* Sust a suffix */
        "Trade::GltfSceneConverter::add(): can't load NonexistentImageConverter for image conversion\n"
        "Cannot add 2D image 0\n"},
    {"can't add processed 3D images", Containers::array<Containers::String>({
        "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
        "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx",
        "-C", "GltfSceneConverter",
        "-c", "experimentalKhrTextureKtx,imageConverter=NonexistentImageConverter",
        Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-3d.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")}),
        "ObjImporter", "PngImporter", "GltfSceneConverter", "StbResizeImageConverter",
        "\n" /* Sust a suffix */
        "Trade::GltfSceneConverter::add(): can't load NonexistentImageConverter for image conversion\n"
        "Cannot add 3D image 0\n"},
};

SceneConverterTest::SceneConverterTest() {
    addInstancedTests({&SceneConverterTest::info},
        Containers::arraySize(InfoData));

    addInstancedTests({&SceneConverterTest::convert},
        Containers::arraySize(ConvertData));

    addInstancedTests({&SceneConverterTest::error},
        Containers::arraySize(ErrorData));

    /* Create output dir, if doesn't already exist */
    Utility::Path::make(Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles"));
}

namespace {

#ifdef SCENECONVERTER_EXECUTABLE_FILENAME
Containers::Pair<bool, Containers::String> call(const Containers::StringIterable& arguments) {
    const Containers::String outputFilename = Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/output.txt");
    /** @todo clean up once Utility::System::execute() with output redirection
        exists */
    /* Implicitly pass the plugin directory override */
    const bool success = std::system(Utility::format("{} --plugin-dir {} {} > {} 2>&1",
        SCENECONVERTER_EXECUTABLE_FILENAME,
        MAGNUM_PLUGINS_INSTALL_DIR,
        " "_s.join(arguments), /** @todo handle space escaping here? */
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
    CORRADE_SKIP("magnum-sceneconverter not built, can't test");
    #else
    /* Check if required plugins can be loaded. Catches also ABI and interface
       mismatch errors. */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractImageConverter> imageConverterManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    if(data.requiresImporter && !(importerManager.load(data.requiresImporter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImporter << "plugin can't be loaded.");
    if(data.requiresConverter && !(converterManager.load(data.requiresConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresConverter << "plugin can't be loaded.");
    if(data.requiresImageConverter && !(imageConverterManager.load(data.requiresImageConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImageConverter << "plugin can't be loaded.");

    CORRADE_VERIFY(true); /* capture correct function name */

    Containers::Pair<bool, Containers::String> output = call(data.args);
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
    CORRADE_SKIP("magnum-sceneconverter not built, can't test");
    #else
    /* Check if required plugins can be loaded. Catches also ABI and interface
       mismatch errors. */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractImageConverter> imageConverterManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    if(data.requiresImporter && !(importerManager.load(data.requiresImporter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImporter << "plugin can't be loaded.");
    if(data.requiresImporter2 && !(importerManager.load(data.requiresImporter2) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImporter2 << "plugin can't be loaded.");
    if(data.requiresConverter && !(converterManager.load(data.requiresConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresConverter << "plugin can't be loaded.");
    if(data.requiresImageConverter[0] && !(imageConverterManager.load(data.requiresImageConverter[0]) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImageConverter[0] << "plugin can't be loaded.");
    if(data.requiresImageConverter[1] && !(imageConverterManager.load(data.requiresImageConverter[1]) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImageConverter[1] << "plugin can't be loaded.");
    if(data.requiresMeshConverter && !(converterManager.load(data.requiresMeshConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresMeshConverter << "plugin can't be loaded.");
    /* AnySceneImporter, AnyImageImporter & AnySceneConverter are required
       implicitly for simplicity */
    if(!(importerManager.load("AnySceneImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnySceneImporter plugin can't be loaded.");
    if(!(importerManager.load("AnyImageImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter plugin can't be loaded.");
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
    CORRADE_SKIP("magnum-sceneconverter not built, can't test");
    #else
    /* Check if required plugins can be loaded. Catches also ABI and interface
       mismatch errors. */
    PluginManager::Manager<Trade::AbstractImporter>  importerManager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractImageConverter> imageConverterManager{MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR};
    PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR};
    if(data.requiresImporter && !(importerManager.load(data.requiresImporter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImporter << "plugin can't be loaded.");
    if(data.requiresImageImporter && !(importerManager.load(data.requiresImageImporter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImageImporter << "plugin can't be loaded.");
    if(data.requiresConverter && !(converterManager.load(data.requiresConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresConverter << "plugin can't be loaded.");
    if(data.requiresImageConverter && !(imageConverterManager.load(data.requiresImageConverter) & PluginManager::LoadState::Loaded))
        CORRADE_SKIP(data.requiresImageConverter << "plugin can't be loaded.");
    /* AnyImageImporter is required implicitly for simplicity if any importer
       is required */
    if(data.requiresImageImporter && !(importerManager.load("AnyImageImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter plugin can't be loaded.");

    CORRADE_VERIFY(true); /* capture correct function name */

    Containers::Pair<bool, Containers::String> output = call(data.args);
    /* If the message begins with a \n, assume it's just a suffix */
    if(data.message.hasPrefix('\n'))
        CORRADE_COMPARE_AS(output.second(),
            data.message,
            TestSuite::Compare::StringHasSuffix);
    /* If it ends with a \n, assume it's the whole message */
    else if(data.message.hasSuffix('\n'))
        CORRADE_COMPARE(output.second(), data.message);
    /* Otherwise it's just a prefix */
    else
        CORRADE_COMPARE_AS(output.second(),
            data.message,
            TestSuite::Compare::StringHasPrefix);
    /* It should return a non-zero code */
    CORRADE_VERIFY(!output.first());
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::SceneConverterTest)
