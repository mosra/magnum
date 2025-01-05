/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

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
    TestSuite::TestCaseDescriptionSourceLocation name;
    Containers::Array<Containers::String> args;
    const char* requiresImporter;
    const char* requiresConverter;
    const char* requiresImageConverter;
    const char* expected;
} InfoData[]{
    {"importer", {InPlaceInit, {
            "--info-importer", "-i", "someOption=yes"
        }},
        nullptr, nullptr, nullptr,
        "info-importer.txt"},
    {"converter", {InPlaceInit, {
            "-C", "AnySceneConverter", "--info-converter", "-c", "someOption=yes"
        }},
        nullptr, "AnySceneConverter", nullptr,
        "info-converter.txt"},
    {"converter, implicit", {InPlaceInit, {
            "--info-converter", "-c", "someOption=yes"
        }},
        nullptr, "AnySceneConverter", nullptr,
        "info-converter.txt"},
    {"image converter", {InPlaceInit, {
            "-P", "AnyImageConverter", "--info-image-converter", "-p", "someOption=yes"
        }},
        nullptr, nullptr, "AnyImageConverter",
        "info-image-converter.txt"},
    {"image converter, implicit", {InPlaceInit, {
        "--info-image-converter", "-p", "someOption=yes"
        }},
        nullptr, nullptr, "AnyImageConverter",
        "info-image-converter.txt"},
    {"importer, ignored input and output", {InPlaceInit, {
            "--info-importer", "input.obj", "output.ply"
        }},
        "AnySceneImporter", nullptr, nullptr,
        "info-importer-ignored-input-output.txt"},
    {"data", {InPlaceInit, {
            "-I", "ObjImporter", "--info", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj")
        }},
        "ObjImporter", nullptr, nullptr,
        "info-data.txt"},
    {"data, map", {InPlaceInit, {
            "--map", "-I", "ObjImporter", "--info", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj")
        }},
        "ObjImporter", nullptr, nullptr,
        /** @todo change to something else once we have a plugin that can
            zero-copy pass the imported data */
        "info-data.txt"},
    {"data, ignored output file", {InPlaceInit, {
            "-I", "ObjImporter", "--info", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"), "whatever.ply"
        }},
        "ObjImporter", nullptr, nullptr,
        "info-data-ignored-output.txt"},
    {"data, preferred importer plugin", {InPlaceInit, {
            "-I", "AnyImageImporter", "--info",
            /* Tested thoroughly in convert(preferred importer plugin), here it
               just verifies that the option has an effect on --info as well */
            "--prefer", "PngImporter:StbImageImporter", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/blue4x4.png")
        }},
        "StbImageImporter", nullptr, nullptr,
        "info-preferred-importer-plugin.txt"},
    {"data, global plugin options", {InPlaceInit, {
            "-I", "StbImageImporter", "--info",
            /* Tested thoroughly in convert(global importer options), here it
               just verifies that the option has an effect on --info as well */
            "--set", "StbImageImporter:forceChannelCount=1", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/blue4x4.png")
        }},
        "StbImageImporter", nullptr, nullptr,
        "info-global-plugin-options.txt"},
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
    {"one mesh", {InPlaceInit, {
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, whole scene converter", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as that's less context
               sensitive and thus shouldn't cause accidentally uncovered
               code paths. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf"),
        }},
        "ObjImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one mesh, explicit importer and converter", {InPlaceInit, {
            "-I", "ObjImporter", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, map", {InPlaceInit, {
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one mesh, options", {InPlaceInit, {
            /* It's silly, but since we have option propagation tested in
               AnySceneImporter / AnySceneConverter .cpp already, it's enough
               to just verify the (nonexistent) options arrive there */
            "-i", "nonexistentOption=13", "-c", "nonexistentConverterOption=26",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Trade::AnySceneImporter::openFile(): option nonexistentOption not recognized by ObjImporter\n"
        "Trade::AnySceneConverter::beginFile(): option nonexistentConverterOption not recognized by StanfordSceneConverter\n"},
    {"one mesh, options, explicit importer and converter", {InPlaceInit, {
            /* Same here, since we have option propagation tested in
               Magnum/Test/ConverterUtilitiesTest.cpp already, to verify it's
               getting called we can just supply nonexistent options */
            "-i", "nonexistentOption=13", "-c", "nonexistentConverterOption=26",
            "-I", "ObjImporter", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentOption not recognized by ObjImporter\n"
        "Option nonexistentConverterOption not recognized by StanfordSceneConverter\n"},
    {"two meshes + scene", {InPlaceInit, {
            /* Removing the generator identifier to have the file fully roundtrip */
            "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        /* There should be a minimal difference compared to the original */
        "two-quads.gltf", "two-quads.bin",
        {}},
    {"concatenate meshes without a scene", {InPlaceInit, {
            "--concatenate-meshes",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-duplicates.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad-duplicates.ply", nullptr,
        {}},
    {"concatenate meshes with a scene", {InPlaceInit, {
            "--concatenate-meshes",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles-transformed.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-duplicates.ply")
        }},
        "GltfImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad-duplicates.ply", nullptr,
        {}},
    {"concatenate meshes with a scene but no default scene", {InPlaceInit, {
            "--concatenate-meshes",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles-transformed-no-default-scene.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-duplicates.ply")
        }},
        "GltfImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad-duplicates.ply", nullptr,
        {}},
    /** @todo drop --mesh once it's not needed anymore again, then add a
        multi-mesh variant */
    {"one mesh, filter mesh attributes", {InPlaceInit, {
            /* Only 0 gets picked from here, others ignored */
            "--mesh", "0", "--only-mesh-attributes", "17,0,25-36",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-normals-texcoords.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"concatenate meshes, filter mesh attributes", {InPlaceInit, {
            "--concatenate-meshes", "--only-mesh-attributes", "17,0,25-36",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-normals-texcoords.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove vertex duplicates", {InPlaceInit, {
            "--remove-duplicate-vertices",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove duplicate vertices, verbose", {InPlaceInit, {
            /* Forcing the importer and converter to avoid AnySceneImporter /
               AnySceneConverter delegation messages */
            "--remove-duplicate-vertices", "-v", "-I", "ObjImporter", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Mesh 0 duplicate removal: 6 -> 4 vertices\n"},
    {"one selected mesh, remove duplicate vertices, verbose", {InPlaceInit, {
            /* Forcing the importer and converter to avoid AnySceneImporter /
               AnySceneConverter delegation messages */
            "--mesh", "1", "--remove-duplicate-vertices", "-v",
            "-I", "GltfImporter", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "GltfImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        /* The second mesh in the glTF is deliberately the same as in
           quad-duplicates.obj, so this produces the same file */
        "quad.ply", nullptr,
        "Duplicate removal: 6 -> 4 vertices\n"},
    {"two meshes + scene, remove duplicate vertices, verbose", {InPlaceInit, {
            /* Forcing the importer and converter to avoid AnySceneImporter /
               AnySceneConverter delegation messages */
            "--remove-duplicate-vertices", "-v", "-I", "GltfImporter", "-C", "GltfSceneConverter",
            /* Removing the generator identifier for a smaller file */
            "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        /* There should be a minimal difference compared to the original */
        "two-quads.gltf", "two-quads.bin",
        "Mesh 0 duplicate removal: 5 -> 4 vertices\n"
        "Mesh 1 duplicate removal: 6 -> 4 vertices\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding scene 0 out of 1\n"},
    {"one implicit mesh, remove duplicate vertices fuzzy", {InPlaceInit, {
            "--remove-duplicate-vertices-fuzzy", "1.0e-1",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates-fuzzy.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        {}},
    {"one implicit mesh, remove duplicate vertices fuzzy, verbose", {InPlaceInit, {
            /* Forcing the importer and converter to avoid AnySceneImporter /
               AnySceneConverter delegation messages */
            "--remove-duplicate-vertices-fuzzy", "1.0e-1", "-v",
            "-I", "ObjImporter", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates-fuzzy.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Mesh 0 fuzzy duplicate removal: 6 -> 4 vertices\n"},
    {"one selected mesh, remove duplicate vertices fuzzy, verbose", {InPlaceInit, {
            /* Forcing the importer and converter to avoid AnySceneImporter /
               AnySceneConverter delegation messages */
            "--mesh 1", "--remove-duplicate-vertices-fuzzy", "1.0e-1", "-v",
            "-I", "GltfImporter", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates-fuzzy.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "GltfImporter", nullptr, "StanfordSceneConverter", {}, nullptr,
        /* The second mesh in the glTF is deliberately the same as in
           quad-duplicates-fuzzy.obj, so this produces the same file */
        "quad.ply", nullptr,
        "Fuzzy duplicate removal: 6 -> 4 vertices\n"},
    {"two meshes + scene, remove duplicate vertices fuzzy, verbose", {InPlaceInit, {
            /* Forcing the importer and converter to avoid AnySceneImporter /
               AnySceneConverter delegation messages */
            "--remove-duplicate-vertices-fuzzy", "1.0e-1", "-v",
            "-I", "GltfImporter", "-C", "GltfSceneConverter",
            /* Removing the generator identifier for a smaller file */
            "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads-duplicates-fuzzy.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "two-quads.gltf", "two-quads.bin",
        "Mesh 0 fuzzy duplicate removal: 5 -> 4 vertices\n"
        "Mesh 1 fuzzy duplicate removal: 6 -> 4 vertices\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding scene 0 out of 1\n"},
    {"one implicit mesh, two converters", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as in this case as we
               want to test passing no -c options. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            "-C", "MeshOptimizerSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one implicit mesh, two converters, explicit last", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as in this case as we
               want to test passing no -c options. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            "-C", "MeshOptimizerSceneConverter", "-C", "GltfSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.gltf", "quad.bin",
        {}},
    {"one implicit mesh, two converters, verbose", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as in this case as we
               want to test passing no -c options. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            "-C", "MeshOptimizerSceneConverter", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")
        }},
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
    {"one implicit mesh, two converters, explicit last, verbose", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as in this case as we
               want to test passing no -c options. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            "-C", "MeshOptimizerSceneConverter", "-C", "GltfSceneConverter", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")
        }},
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
    {"one implicit mesh, two converters, options for the first only", {InPlaceInit, {
            "-C", "MeshOptimizerSceneConverter",
            "-c", "nonexistentMeshOptimizerOption=yes",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"},
    {"one implicit mesh, two converters, explicit last, options for the first only", {InPlaceInit, {
            "-C", "MeshOptimizerSceneConverter",
            "-c", "nonexistentMeshOptimizerOption=yes",
            "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"},
    {"one implicit mesh, two converters, options for both", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as -c is tested for
               something else. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            "-C", "MeshOptimizerSceneConverter",
            "-c", "nonexistentMeshOptimizerOption=yes",
            "-c", "nonexistentAnyConverterOption=no",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"
        "Trade::AnySceneConverter::beginFile(): option nonexistentAnyConverterOption not recognized by GltfSceneConverter\n"},
    {"one implicit mesh, two converters, explicit last, options for both", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as -c is tested for
               something else. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            "-C", "MeshOptimizerSceneConverter",
            "-c", "nonexistentMeshOptimizerOption=yes",
            "-C", "StanfordSceneConverter",
            "-c", "nonexistentStanfordConverterOption=no",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "quad.ply", nullptr,
        "Option nonexistentMeshOptimizerOption not recognized by MeshOptimizerSceneConverter\n"
        "Option nonexistentStanfordConverterOption not recognized by StanfordSceneConverter\n"},
    {"one mesh, remove duplicate vertices, two converters, verbose", {InPlaceInit, {
            "--remove-duplicate-vertices",
            "-C", "MeshOptimizerSceneConverter", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-duplicates.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
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
    {"implicit custom-processed mesh with a name and custom attributes", {InPlaceInit, {
            /* Removing the generator identifier to have the file closer to the
               original */
            "--remove-duplicate-vertices", "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-name-custom-attributes-duplicates.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-name-custom-attributes.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        /* The output should be mostly the same, except that there's now only 4
           vertices instead of 6. The code that adds meshes manually instead of
           using addSupportedImporterContents() should take care of propagating
           mesh names and custom attributes as well. */
        "quad-name-custom-attributes.gltf", "quad-name-custom-attributes.bin",
        {}},
    {"selected custom-processed mesh with a name and custom attributes", {InPlaceInit, {
            /* Removing the generator identifier to have the file closer to the
               original */
            "--mesh", "0", "--remove-duplicate-vertices", "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-name-custom-attributes-duplicates.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad-name-custom-attributes.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        /* The output should be mostly the same, except that there's now only 4
           vertices instead of 6. The code that adds meshes manually instead of
           using addSupportedImporterContents() should take care of propagating
           mesh names and custom attributes as well. */
        "quad-name-custom-attributes.gltf", "quad-name-custom-attributes.bin",
        {}},
    {"mesh converter", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as that's less context
               sensitive and thus shouldn't cause accidentally uncovered
               code paths. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            "-M", "MeshOptimizerSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter",
        {}, "MeshOptimizerSceneConverter",
        /* Converts a triangle strip to indexed triangles, which verifies that
           the output of MeshOptimizerSceneConverter got actually passed
           further and not discarded */
        "quad.gltf", "quad.bin",
        {}},
    {"mesh converter, two meshes, verbose", {InPlaceInit, {
            /* Removing the generator identifier for a smaller file */
            "-I", "GltfImporter", "-C", "GltfSceneConverter", "-c", "generator=",
            "-M", "MeshOptimizerSceneConverter", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-quads.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/two-quads.gltf")
        }},
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
    {"two mesh converters, two options, one mesh, verbose", {InPlaceInit, {
            /* Unfortunately *have to* use an option to make the output
               predictable. Using --set instead of -c as that's less context
               sensitive and thus shouldn't cause accidentally uncovered
               code paths. */
            "--set", "GltfSceneConverter:generator=\"Magnum GltfSceneConverter\"",
            "-I", "GltfImporter", "-C", "GltfSceneConverter",
            "-M", "MeshOptimizerSceneConverter",
            "-m", "nonexistentFirstOption=yes",
            "-M", "MeshOptimizerSceneConverter",
            "-m", "nonexistentSecondOption=yes", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/quad-strip.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.gltf")
        }},
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
    {"mesh converter, passthrough on failure", {InPlaceInit, {
            "-M", "MeshOptimizerSceneConverter",
            "--passthrough-on-mesh-converter-failure",
            /* Removing the generator identifier for a roundtrip */
            "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/mesh-passthrough-on-failure.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/mesh-passthrough-on-failure.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter",
        {}, "MeshOptimizerSceneConverter",
        /* The output is exactly the same as the input */
        "mesh-passthrough-on-failure.gltf", "mesh-passthrough-on-failure.bin",
        "Trade::MeshOptimizerSceneConverter::convert(): expected an indexed mesh\n"
        "Cannot process mesh 0 with MeshOptimizerSceneConverter, passing the original through\n"},
    {"2D image converter, two images", {InPlaceInit, {
            "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
            /* Removing the generator identifier for a smaller file, bundling
               the images to avoid having too many files */
            "-c", "bundleImages,generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d-1x1.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        "images-2d-1x1.gltf", "images-2d-1x1.bin",
        {}},
    {"2D image converter, two images, verbose", {InPlaceInit, {
            "-I", "GltfImporter", "-C", "GltfSceneConverter",
            "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
            /* Removing the generator identifier for a smaller file, bundling
               the images to avoid having too many files */
            "-c", "bundleImages,generator=", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d-1x1.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        "images-2d-1x1.gltf", "images-2d-1x1.bin",
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Processing 2D image 0 with StbResizeImageConverter...\n"
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Processing 2D image 1 with StbResizeImageConverter...\n"},
    {"two 2D image converters, two images, verbose", {InPlaceInit, {
            "-I", "GltfImporter", "-C", "GltfSceneConverter",
            "-P", "StbResizeImageConverter", "-p", "size=\"2 2\"",
            "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
            /* Removing the generator identifier for a smaller file, bundling
               the images to avoid having too many files */
            "-c", "bundleImages,generator=", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d-1x1.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        "images-2d-1x1.gltf", "images-2d-1x1.bin",
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Processing 2D image 0 (1/2) with StbResizeImageConverter...\n"
        "Processing 2D image 0 (2/2) with StbResizeImageConverter...\n"
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Processing 2D image 1 (1/2) with StbResizeImageConverter...\n"
        "Processing 2D image 1 (2/2) with StbResizeImageConverter...\n"},
    {"3D image converter, two images", {InPlaceInit, {
            /* Removing the KTX generator identifier for predictable output */
            "--set", "KtxImageConverter:generator=",
            "-i", "experimentalKhrTextureKtx",
            "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
            /* Removing the glTF generator identifier for predictable output,
               bundling the images to avoid having too many files */
            "-c", "experimentalKhrTextureKtx,imageConverter=KtxImageConverter,bundleImages,generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-3d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-3d-1x1x1.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "KtxImageConverter"}, nullptr,
        "images-3d-1x1x1.gltf", "images-3d-1x1x1.bin",
        {}},
    {"3D image converter, two images, verbose", {InPlaceInit, {
            /* Removing the KTX generator identifier for predictable output */
            "--set", "KtxImageConverter:generator=",
            "-I", "GltfImporter", "-C", "GltfSceneConverter",
            "-i", "experimentalKhrTextureKtx",
            "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
            /* Removing the generator identifier for predictable output,
               bundling the images to avoid having too many files */
            "-c", "experimentalKhrTextureKtx,imageConverter=KtxImageConverter,bundleImages,generator=", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-3d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-3d-1x1x1.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "KtxImageConverter"}, nullptr,
        "images-3d-1x1x1.gltf", "images-3d-1x1x1.bin",
        "Trade::AnyImageImporter::openFile(): using KtxImporter\n"
        "Processing 3D image 0 with StbResizeImageConverter...\n"
        "Trade::AnyImageImporter::openFile(): using KtxImporter\n"
        "Processing 3D image 1 with StbResizeImageConverter...\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding texture 0 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding texture 1 out of 2\n"},
    {"2D image converter, passthrough on failure", {InPlaceInit, {
            /* Size explicitly not set to trigger a failure */
            "-P", "StbResizeImageConverter",
            "--passthrough-on-image-converter-failure",
            /* Removing the generator identifier for a roundtrip */
            "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/image-passthrough-on-failure.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/image-passthrough-on-failure.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        /* The output is exactly the same as the input */
        "image-passthrough-on-failure.gltf", "image-passthrough-on-failure.0.png",
        "Trade::StbResizeImageConverter::convert(): output size was not specified\n"
        "Cannot process 2D image 0 with StbResizeImageConverter, passing the original through\n"},
    {"Phong to PBR", {InPlaceInit, {
            "-I", "UfbxImporter", "-C", "GltfSceneConverter", "--phong-to-pbr",
            /* We need the file as minimal as possible, so no index buffer.
               OTOH, dropping meshes altogether would also lose
               node/mesh/material assignment, which is important */
            "-i", "generateIndices=false",
            /* Removing the generator identifier for a smaller file */
            "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/materials-phong.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/materials-pbr.gltf")
        }},
        "UfbxImporter", "PngImporter", "GltfSceneConverter", {"PngImageConverter", nullptr}, nullptr,
        /* The file should contain also material names and everything */
        "materials-pbr.gltf", nullptr,
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::AmbientColor attribute, skipping\n"},
    {"Phong to PBR, verbose", {InPlaceInit, {
            /* Same as above, just with -v added */
            "-I", "UfbxImporter", "-C", "GltfSceneConverter", "--phong-to-pbr",
            "-i", "generateIndices=false", "-c", "generator=", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/materials-phong.obj"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/materials-pbr.gltf")
        }},
        "UfbxImporter", "PngImporter", "GltfSceneConverter", {"PngImageConverter", nullptr}, nullptr,
        "materials-pbr.gltf", nullptr,
        "Converting material 0 to PBR\n"
        "MaterialTools::phongToPbrMetallicRoughness(): unconvertible Trade::MaterialAttribute::AmbientColor attribute, skipping\n"
        "Converting material 1 to PBR\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 0 out of 2\n"
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 1 out of 2\n"
        "Trade::AnyImageImporter::openFile(): using PngImporter\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding texture 0 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding texture 1 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 1 out of 2\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding scene 0 out of 1\n"},
    {"remove duplicate materials", {InPlaceInit, {
            "-I" "GltfImporter", "-C", "GltfSceneConverter", "--remove-duplicate-materials", "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/materials-duplicate.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/materials-duplicate-removed.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "materials-duplicate-removed.gltf", nullptr,
        {}},
    {"remove duplicate materials, verbose", {InPlaceInit, {
            /* Same as above, just with -v added */
            "-I" "GltfImporter", "-C", "GltfSceneConverter", "--remove-duplicate-materials", "-c", "generator=", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/materials-duplicate.gltf"), Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/materials-duplicate-removed.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter", {}, nullptr,
        "materials-duplicate-removed.gltf", nullptr,
        "Duplicate material removal: 3 -> 2 materials\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 0 out of 3\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 1 out of 3\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding mesh 2 out of 3\n"},
    {"data unsupported by the converter", {InPlaceInit, {
            "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx",
            "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/ignoring-unsupported.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "GltfImporter", "KtxImporter", "StanfordSceneConverter",
        {"StbResizeImageConverter", nullptr}, nullptr,
        "quad.ply", nullptr,
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 2D images not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 3D images not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 2 textures not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 materials not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 scenes not supported by the converter\n"},
    {"per-image processed images unsupported by the converter", {InPlaceInit, {
            "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx",
            "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
            "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/ignoring-unsupported.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "GltfImporter", "KtxImporter", "StanfordSceneConverter",
        {"StbResizeImageConverter", nullptr}, nullptr,
        "quad.ply", nullptr,
        /* Compared to "data unsupported by the converter" these messages are
           printed by sceneconverter itself, not the converter interface */
        "Ignoring 1 2D images not supported by the converter\n"
        "Ignoring 1 3D images not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 2 textures not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 materials not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 scenes not supported by the converter\n"},
    {"per-material processed materials unsupported by the converter", {InPlaceInit, {
            "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx",
            "--phong-to-pbr", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/ignoring-unsupported.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "GltfImporter", "KtxImporter", "StanfordSceneConverter",
        {"StbResizeImageConverter", nullptr}, nullptr,
        "quad.ply", nullptr,
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 2D images not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 3D images not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 2 textures not supported by the converter\n"
        /* Compared to "data unsupported by the converter" this message is
           printed by sceneconverter itself, not the converter interface */
        "Ignoring 1 materials not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 scenes not supported by the converter\n"},
    {"per-scene processed scenes unsupported by the converter", {InPlaceInit, {
            "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx",
            "--remove-duplicate-materials", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/ignoring-unsupported.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/quad.ply")
        }},
        "GltfImporter", "KtxImporter", "StanfordSceneConverter",
        {"StbResizeImageConverter", nullptr}, nullptr,
        "quad.ply", nullptr,
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 2D images not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 3D images not supported by the converter\n"
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 2 textures not supported by the converter\n"
        /* Compared to "data unsupported by the converter" these messages are
           printed by sceneconverter itself, not the converter interface. Both
           scenes and materials get imported directly for duplicate material
           removal. */
        /** @todo switch to something that only touches scenes and not
            materials once it exists */
        "Ignoring 1 materials not supported by the converter\n"
        "Ignoring 1 scenes not supported by the converter\n"},
    {"preferred importer plugin", {InPlaceInit, {
            /* First is not found, second should be always found, third might
               be also but shouldn't be picked. The trailing comma should be allowed, using the plugin itself in the list should work too. */
            "--prefer", "PngImporter:Sdl3ImageImporter,StbImageImporter,SpngImporter,",
            "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d.gltf")
        }},
        "GltfImporter", "StbImageImporter", "GltfSceneConverter",
        {"PngImageConverter", nullptr}, nullptr,
        /* Not checking either of the files, the verbose output is enough to
           verify */
        nullptr, nullptr,
        "Trade::AnySceneImporter::openFile(): using GltfImporter\n"
        "Trade::AnySceneConverter::beginFile(): using GltfSceneConverter\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 0 out of 2\n"
        "Trade::AnyImageImporter::openFile(): using PngImporter (provided by StbImageImporter)\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 1 out of 2\n"
        "Trade::AnyImageImporter::openFile(): using PngImporter (provided by StbImageImporter)\n"},
    {"preferred image converter plugin", {InPlaceInit, {
            /* The main logic was tested in "preferred importer plugin" above,
               this just verifies that it works for image converters too. The
               converter doesn't use AnyImageConverter so we can't rely on
               verbose output, instead we convert a RGBA image to a JPEG
               (embedded in a glTF) and check the warning message. */
            "--prefer", "JpegImageConverter:StbImageConverter",
            "-I", "PngImporter",
            "-c", "imageConverter=JpegImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/rgba.png"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/rgba.gltf")
        }},
        "PngImporter", nullptr, "GltfSceneConverter",
        {"StbImageConverter", nullptr}, nullptr,
        /* Not checking either of the files, the warning output is enough to
           verify */
        nullptr, nullptr,
        "Trade::StbImageConverter::convertToData(): ignoring alpha channel for BMP/JPEG output\n"},
    {"preferred scene converter plugin", {InPlaceInit, {
            /* There aren't any alternative implementations for any scene
               converters so can only ensure the code doesn't blow up if scene
               converters are passed to --prefer */
            "--prefer", "GltfSceneConverter:",
            /* Removing the generator identifier for a roundtrip */
            "-c", "generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/empty.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter",
        {}, nullptr,
        /* It should give back the same file  */
        "empty.gltf", nullptr,
        {}},
    {"multiple --prefer options", {InPlaceInit, {
            /* Basically a combination of "preferred importer plugin" and
               "preferred image converter plugin" cases */
            "--prefer", "PngImporter:StbImageImporter",
            "--prefer", "JpegImageConverter:StbImageConverter",
            "-I", "AnyImageImporter",
            "-c", "imageConverter=JpegImageConverter", "-v",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/rgba.png"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/rgba.gltf")
        }},
        "PngImporter", nullptr, "GltfSceneConverter",
        {"StbImageConverter", nullptr}, nullptr,
        /* Not checking either of the files, the warning output is enough to
           verify */
        nullptr, nullptr,
        "Trade::AnyImageImporter::openFile(): using PngImporter (provided by StbImageImporter)\n"
        "Trade::AnySceneConverter::beginFile(): using GltfSceneConverter\n"
        "Trade::AbstractSceneConverter::addImporterContents(): adding 2D image 0 out of 1\n"
        "Trade::StbImageConverter::convertToData(): ignoring alpha channel for BMP/JPEG output\n"},
    {"global importer options", {InPlaceInit, {
            /* The image is RGB, but we import it as RGBA to trigger a warning
               inside the JPEG converter plugin */
            "--set", "StbImageImporter:forceChannelCount=4,unrecognizedOption=yes",
            "-I", "StbImageImporter",
            "-c", "imageConverter=StbJpegImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/blue4x4.png"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/rgba.gltf")
        }},
        "StbImageImporter", nullptr, "GltfSceneConverter",
        {"StbImageConverter", nullptr}, nullptr,
        /* Not checking either of the files, the warning output is enough to
           verify the option got properly set */
        nullptr, nullptr,
        "Option unrecognizedOption not recognized by StbImageImporter\n"
        "Trade::StbImageConverter::convertToData(): ignoring alpha channel for BMP/JPEG output\n"},
    {"global image converter options", {InPlaceInit, {
            /* This produces the same output as "2D image converter, two images"
               above, it's just that the options are specified through --set */
            "--set", "StbResizeImageConverter:size=\"1 1\"",
            "-P", "StbResizeImageConverter",
            /* Removing the generator identifier for a smaller file, bundling
               the images to avoid having too many files */
            "-c", "bundleImages,generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d-1x1.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        "images-2d-1x1.gltf", "images-2d-1x1.bin",
        {}},
    {"global scene converter options", {InPlaceInit, {
            /* Same as -c generator= */
            "--set", "GltfSceneConverter:generator=",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/empty.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter",
        {}, nullptr,
        "empty.gltf", nullptr,
        {}},
    {"multiple --set options", {InPlaceInit, {
            /* Another variant of "2D image converter, two images", this time
               with everything specified through --set */
            "--set", "StbResizeImageConverter:size=\"1 1\"",
            /* Removing the generator identifier for a smaller file, bundling
               the images to avoid having too many files */
            "--set", "GltfSceneConverter:bundleImages,generator=",
            "-P", "StbResizeImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/images-2d-1x1.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter",
        {"StbResizeImageConverter", "PngImageConverter"}, nullptr,
        "images-2d-1x1.gltf", "images-2d-1x1.bin",
        {}},
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Containers::Array<Containers::String> args;
    const char* requiresImporter;
    const char* requiresImageImporter;
    const char* requiresConverter;
    const char* requiresImageConverter;
    Containers::String message;
} ErrorData[]{
    {"missing output argument", {InPlaceInit, {
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj")
        }},
        nullptr, nullptr, nullptr, nullptr,
        /* The output should be optional only for --info, required otherwise.
           No need to test anything else as that's handled by Utility::Arguments
           already. Testing just a prefix of the message. */
        "Missing command-line argument output\nUsage:\n  "},
    {"--mesh and --concatenate-meshes", {InPlaceInit, {
            "--mesh", "0", "--concatenate-meshes", "a", "b"
        }},
        nullptr, nullptr, nullptr, nullptr,
        "The --mesh and --concatenate-meshes options are mutually exclusive\n"},
    {"--mesh-level but no --mesh", {InPlaceInit, {
            "--mesh-level", "0", "a", "b"
        }},
        nullptr, nullptr, nullptr, nullptr,
        "The --mesh-level option can only be used with --mesh\n"},
    {"--only-mesh-attributes but no --mesh", {InPlaceInit, {
            "--only-mesh-attributes", "0", "a", "b"
        }},
        nullptr, nullptr, nullptr, nullptr,
        "The --only-mesh-attributes option can only be used with --mesh or --concatenate-meshes\n"},
    {"--prefer without a colon", {InPlaceInit, {
            "--prefer", "PngImporter=StbImageImporter", "a", "b",
        }},
        nullptr, nullptr, nullptr, nullptr,
        "Invalid --prefer option PngImporter=StbImageImporter\n"},
    {"--prefer alias suffix unknown", {InPlaceInit, {
            "--prefer", "TrueTypeFont:HarfBuzzFont", "a", "b",
        }},
        nullptr, nullptr, nullptr, nullptr,
        "Alias TrueTypeFont not recognized for a --prefer option\n"},
    {"--prefer alias name not found", {InPlaceInit, {
            "--prefer", "FbxSceneConverter:UfbxSceneConverter", "a", "b",
        }},
        nullptr, nullptr, nullptr, nullptr,
        "Alias FbxSceneConverter not found for a --prefer option\n"},
    {"--prefer plugin doesn't provide alias", {InPlaceInit, {
            "--prefer", "GltfImporter:UfbxImporter", "a", "b",
        }},
        /* UfbxImporter is not really an image importer but it works here */
        "GltfImporter", "UfbxImporter", nullptr, nullptr,
        "UfbxImporter doesn't provide GltfImporter for a --prefer option\n"},
    {"--set without a colon", {InPlaceInit, {
            "--set", "StbImageImporter=forceChannelCount=3", "a", "b",
        }},
        nullptr, nullptr, nullptr, nullptr,
        "Invalid --set option StbImageImporter=forceChannelCount=3\n"},
    {"--set plugin suffix unknown", {InPlaceInit, {
            "--set", "TrueTypeFont:hinting=slight", "a", "b",
        }},
        nullptr, nullptr, nullptr, nullptr,
        "Plugin TrueTypeFont not recognized for a --set option\n"},
    {"--set plugin name not found", {InPlaceInit, {
            "--set", "FbxSceneConverter:binary=true", "a", "b",
        }},
        nullptr, nullptr, nullptr, nullptr,
        "Plugin FbxSceneConverter not found for a --set option\n"},
    {"can't load importer plugin", {InPlaceInit, {
            /* Override also the plugin directory for consistent output */
            "--plugin-dir", "nonexistent", "-I", "NonexistentImporter", "whatever.obj",
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        nullptr, nullptr, nullptr, nullptr,
        "PluginManager::Manager::load(): plugin NonexistentImporter is not static and was not found in nonexistent/importers\n"
        "Available importer plugins: "},
    {"can't open a file", {InPlaceInit, {
            "nonexistent.ffs",
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "AnySceneImporter", nullptr, nullptr, nullptr,
        "Trade::AnySceneImporter::openFile(): cannot determine the format of nonexistent.ffs\n"
        "Cannot open file nonexistent.ffs\n"},
    {"can't map a file", {InPlaceInit, {
            "nonexistent.ffs", "--map",
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "AnySceneImporter", nullptr, nullptr, nullptr,
        "Utility::Path::mapRead(): can't open nonexistent.ffs: error 2 (No such file or directory)\n"
        "Cannot memory-map file nonexistent.ffs\n"},
    {"no meshes found for concatenation", {InPlaceInit, {
            "--concatenate-meshes",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "GltfImporter", nullptr, nullptr, nullptr,
        Utility::format("No meshes found in {}\n", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"))},
    {"can't import a single mesh", {InPlaceInit, {
            "-I", "ObjImporter", "--mesh", "0",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, nullptr, nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import the mesh\n"},
    {"can't import a mesh for concatenation", {InPlaceInit, {
            "-I", "ObjImporter", "--concatenate-meshes",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, nullptr, nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import mesh 0\n"},
    {"can't import a scene for concatenation", {InPlaceInit, {
            /** @todo change to an OBJ once ObjImporter imports materials (and
                thus scenes) */
            "--concatenate-meshes",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-scene.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "GltfImporter", nullptr, nullptr, nullptr,
        "Trade::GltfImporter::scene(): mesh index 1 in node 0 out of range for 1 meshes\n"
        "Cannot import scene 0 for mesh concatenation\n"},
    {"can't import a mesh for per-mesh processing", {InPlaceInit, {
            "-I", "ObjImporter", "--remove-duplicate-vertices",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, nullptr, nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot import mesh 0\n"},
    {"invalid mesh attribute filter", {InPlaceInit, {
            /** @todo drop --mesh once it's not needed anymore again */
            "-I", "ObjImporter", "--mesh", "0", "--only-mesh-attributes", "LOLNEIN",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, nullptr, nullptr,
        "Utility::parseNumberSequence(): unrecognized character L in LOLNEIN\n"},
    {"can't load converter plugin", {InPlaceInit, {
            "-I", "ObjImporter",
            "-C", "NonexistentSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, nullptr, nullptr,
        Utility::format("PluginManager::Manager::load(): plugin NonexistentSceneConverter is not static and was not found in {}\n"
        "Available converter plugins: ", /* Just a prefix */
        MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR)},
    {"file coversion begin failed", {InPlaceInit, {
            "-I", "ObjImporter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.fbx")
        }},
        "ObjImporter", nullptr, "AnySceneConverter", nullptr,
        Utility::format("Trade::AnySceneConverter::beginFile(): cannot determine the format of {0}\n"
        "Cannot begin conversion of file {0}\n", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.fbx"))},
    {"file coversion end failed", {InPlaceInit, {
            "-I", "GltfImporter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "GltfImporter", nullptr, "StanfordSceneConverter", nullptr,
        Utility::format("Trade::AbstractSceneConverter::endFile(): the converter requires exactly one mesh, got 0\n"
        "Cannot end conversion of file {0}\n", Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply"))},
    /** @todo importer conversion begin failed, once there's a plugin for which
        begin() can fail */
    {"importer coversion end failed", {InPlaceInit, {
            "-I", "GltfImporter",
            "-C", "MeshOptimizerSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/empty.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "GltfImporter", nullptr, "MeshOptimizerSceneConverter", nullptr,
        "Trade::AbstractSceneConverter::end(): the converter requires exactly one mesh, got 0\n"
        "Cannot end importer conversion\n"},
    {"can't add importer contents", {InPlaceInit, {
            "-I", "ObjImporter", "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", nullptr,
        "Trade::ObjImporter::mesh(): wrong index count for point\n"
        "Cannot add importer contents\n"},
    {"can't add processed meshes", {InPlaceInit, {
            "-I", "ObjImporter", "-C", "StanfordSceneConverter",
            "--remove-duplicate-vertices",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-triangles.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", nullptr,
        "Trade::AbstractSceneConverter::add(): the converter requires exactly one mesh, got 2\n"
        "Cannot add mesh 1\n"},
    {"plugin doesn't support importer conversion", {InPlaceInit, {
            /* Pass the same plugin twice, which means the first instance
               should get used for a mesh-to-mesh conversion */
            "-I", "ObjImporter", "-C", "StanfordSceneConverter",
            "-C", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", nullptr,
        "StanfordSceneConverter doesn't support importer conversion, only ConvertMeshToData\n"},
    {"can't load mesh converter plugin", {InPlaceInit, {
            "-I", "ObjImporter",
            "-M", "NonexistentSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, nullptr, nullptr,
        Utility::format("PluginManager::Manager::load(): plugin NonexistentSceneConverter is not static and was not found in {}\n"
        "Available mesh converter plugins: ", /* Just a prefix */
        MAGNUM_PLUGINS_SCENECONVERTER_INSTALL_DIR)},
    {"plugin doesn't support mesh conversion", {InPlaceInit, {
            "-I", "ObjImporter", "-M", "StanfordSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, "StanfordSceneConverter", nullptr,
        "StanfordSceneConverter doesn't support mesh conversion, only ConvertMeshToData\n"},
    {"can't process a mesh", {InPlaceInit, {
            "-I", "ObjImporter", "-M", "MeshOptimizerSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/point.obj"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.ply")
        }},
        "ObjImporter", nullptr, "MeshOptimizerSceneConverter", nullptr,
        "Trade::MeshOptimizerSceneConverter::convert(): expected a triangle mesh, got MeshPrimitive::Points\n"
        "Cannot process mesh 0 with MeshOptimizerSceneConverter\n"},
    {"can't import a 2D image for per-image processing", {InPlaceInit, {
            "-I", "GltfImporter", "-P", "NonexistentImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-image-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "PngImporter", nullptr, nullptr,
        Utility::format("\n" /* Just a suffix */
        "Trade::AbstractImporter::openFile(): cannot open file {}\n"
        "Cannot import 2D image 0\n", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/nonexistent.png"))},
    {"can't import a 3D image for per-image processing", {InPlaceInit, {
            "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx",
            "-P", "NonexistentImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-image-3d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "KtxImporter", nullptr, nullptr,
        Utility::format("\n" /* Just a suffix */
        "Trade::AbstractImporter::openFile(): cannot open file {}\n"
        "Cannot import 3D image 0\n", Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/nonexistent.ktx2"))},
    {"can't load image converter plugin", {InPlaceInit, {
            "-P", "NonexistentImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "PngImporter", nullptr, nullptr,
        Utility::format("PluginManager::Manager::load(): plugin NonexistentImageConverter is not static and was not found in {}\n"
        "Available image converter plugins: ", /* Just a prefix */
        MAGNUM_PLUGINS_IMAGECONVERTER_INSTALL_DIR)},
    {"plugin doesn't support image conversion", {InPlaceInit, {
            "-I", "GltfImporter", "-P", "PngImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "PngImporter", nullptr, "PngImageConverter",
        "PngImageConverter doesn't support 2D image conversion, only Convert2DToData\n"},
    {"plugin doesn't support compressed image conversion", {InPlaceInit, {
            /* To not print warnings about Y flip for block-compressed data */
            "--set", "DdsImporter:assumeYUpZBackward=true",
            "-I", "GltfImporter", "-P", "StbResizeImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/image-dds.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "DdsImporter", nullptr, "StbResizeImageConverter",
        "StbResizeImageConverter doesn't support compressed 2D image conversion, only Convert2D|Convert3D\n"},
    {"can't process a 2D image", {InPlaceInit, {
            "-I", "GltfImporter", "-P", "StbResizeImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "PngImporter", nullptr, "StbResizeImageConverter",
        "Trade::StbResizeImageConverter::convert(): output size was not specified\n"
        "Cannot process 2D image 0 with StbResizeImageConverter\n"},
    {"can't process a 3D image", {InPlaceInit, {
            "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx",
            "-P", "StbResizeImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-3d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "KtxImporter", nullptr, "StbResizeImageConverter",
        "Trade::StbResizeImageConverter::convert(): output size was not specified\n"
        "Cannot process 3D image 0 with StbResizeImageConverter\n"},
    {"can't add processed 2D images", {InPlaceInit, {
            "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
            "-I", "GltfImporter", "-C", "GltfSceneConverter",
            "-c", "imageConverter=NonexistentImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-2d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "PngImporter", "GltfSceneConverter", "StbResizeImageConverter",
        "\n" /* Just a suffix */
        "Trade::GltfSceneConverter::add(): can't load NonexistentImageConverter for image conversion\n"
        "Cannot add 2D image 0\n"},
    {"can't add processed 3D images", {InPlaceInit, {
            "-P", "StbResizeImageConverter", "-p", "size=\"1 1\"",
            "-I", "GltfImporter", "-i", "experimentalKhrTextureKtx",
            "-C", "GltfSceneConverter",
            "-c", "experimentalKhrTextureKtx,imageConverter=NonexistentImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/images-3d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "KtxImporter", "GltfSceneConverter", "StbResizeImageConverter",
        "\n" /* Just a suffix */
        "Trade::GltfSceneConverter::add(): can't load NonexistentImageConverter for image conversion\n"
        "Cannot add 3D image 0\n"},
    {"1D image processing not implemented", {InPlaceInit, {
            /* Faking an image-only importer which is fine, it's failing right
               after anyway; also not supplying any size to the resize plugin */
            "-I" "KtxImporter", "-P" "StbResizeImageConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/1d.ktx2"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        nullptr, "KtxImporter", nullptr,
        nullptr,
        "Sorry, 1D image conversion is not implemented yet\n"},
    {"can't load a material for Phong to PBR conversion", {InPlaceInit, {
            "-I", "GltfImporter", "--phong-to-pbr",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-material.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", nullptr, nullptr,
        nullptr,
        "Trade::GltfImporter::material(): unrecognized alphaMode TOUGH\n"
        "Cannot import material 2\n"},
    {"can't add material dependencies", {InPlaceInit, {
            /* --phong-to-pbr is a no-op because the input is PBR already, we
               just need something that causes the materials to be added
               directly */
            "-I", "GltfImporter", "--phong-to-pbr",
            /* Not enabling experimentalKhrTextureKtx for the converter in
               order to trigger this error */
            "-i", "experimentalKhrTextureKtx", "-C", "GltfSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/materials-3d.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", "KtxImporter", "GltfSceneConverter",
        nullptr,
        "Trade::AbstractSceneConverter::addSupportedImporterContents(): ignoring 1 3D images not supported by the converter\n"
        /* Another way this could fail is that the texture is now referencing a
           3D image out of range (because adding it failed above) */
        "Trade::GltfSceneConverter::add(): 2D array textures require experimentalKhrTextureKtx to be enabled\n"
        "Cannot add material dependencies\n"},
    {"can't add processed material", {InPlaceInit, {
            /* --phong-to-pbr is a no-op because the input is PBR already, we
               just need something that causes the materials to be added
               directly */
            "-I", "UfbxImporter", "--phong-to-pbr",
            "-C", "GltfSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/materials-separate-metalness-roughness.mtl"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "UfbxImporter", "PngImporter", "GltfSceneConverter",
        nullptr,
        "Trade::GltfSceneConverter::add(): unsupported R/R packing of a metallic/roughness texture\n"
        "Cannot add material 1\n"},
    {"can't load a scene for material deduplication", {InPlaceInit, {
            "-I", "GltfImporter", "--remove-duplicate-materials",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-scene.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", nullptr, nullptr,
        nullptr,
        "Trade::GltfImporter::scene(): mesh index 1 in node 0 out of range for 1 meshes\n"
        "Cannot import scene 0\n"},
    {"can't add scene dependencies", {InPlaceInit, {
            /* --remove-duplicate-materials is a no-op because the input has no
               materials, we just need something that causes the scenes to be
               added directly */
            "-I", "GltfImporter", "--remove-duplicate-materials",
            "-C", "GltfSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/broken-mesh-with-scene.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter",
        nullptr,
        "Trade::GltfImporter::mesh(): accessor index 0 out of range for 0 accessors\n"
        "Cannot add scene dependencies\n"},
    {"can't add processed scene", {InPlaceInit, {
            /* --remove-duplicate-materials is a no-op because the input has no
               materials, we just need something that causes the scenes to be
               added directly */
            "-I", "GltfImporter", "--remove-duplicate-materials",
            "-C", "GltfSceneConverter",
            Utility::Path::join(SCENETOOLS_TEST_DIR, "SceneConverterTestFiles/two-scenes.gltf"),
            Utility::Path::join(SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles/whatever.gltf")
        }},
        "GltfImporter", nullptr, "GltfSceneConverter",
        nullptr,
        /** @todo find some better case for this, this will pass once
            GltfSceneConverter has multi-scene support */
        "Trade::GltfSceneConverter::add(): only one scene is supported at the moment\n"
        "Cannot add scene 1\n"},
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

    return {success, Utility::move(*output)};
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
    /* AnySceneImporter is required implicitly for simplicity */
    if(!(importerManager.load("AnySceneImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnySceneImporter plugin can't be loaded.");

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
    CORRADE_COMPARE_AS(output.second(),
        data.message,
        TestSuite::Compare::String);
    CORRADE_VERIFY(output.first());

    /* In some cases the test verifies only the printed output and doesn't
       check any file */
    if(data.expected) CORRADE_COMPARE_AS(Utility::Path::join({SCENETOOLS_TEST_OUTPUT_DIR, "SceneConverterTestFiles", data.expected}),
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
        CORRADE_COMPARE_AS(output.second(),
            data.message,
            TestSuite::Compare::String);
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
