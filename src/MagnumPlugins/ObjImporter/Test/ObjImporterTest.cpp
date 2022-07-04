/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct ObjImporterTest: TestSuite::Tester {
    explicit ObjImporterTest();

    void empty();

    void meshPrimitivePoints();
    void meshPrimitiveLines();
    void meshPrimitiveTriangles();

    /* Positions alone are tested above */
    void meshPositionsOptionalCoordinate();
    void meshTextureCoordinates();
    void meshTextureCoordinatesOptionalCoordinate();
    void meshNormals();
    void meshTextureCoordinatesNormals();

    void meshIgnoredKeyword();

    void meshNamed();
    void meshNamedFirstUnnamed();

    void moreMeshes();

    /* Technically, all invalid cases could be put into a single file, but
       because the indexing is global, it would get increasingly hard to
       maintain. So it's instead grouped into files by a common error scenario
       with each case testing one file and just the invalid() case testing
       separate files. */
    void invalidKeywords();
    void invalidMixedPrimitives();
    void invalidNumbers();
    void invalidNumberCount();
    void invalidInconsistentIndexTuple();
    void invalidIncompleteData();
    void invalidOptionalCoordinate();

    void whitespace();

    void openTwice();
    void importTwice();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

const struct {
    const char* name;
    const char* filename;
} MeshNamedFirstUnnamedData[]{
    {"", "mesh-named-first-unnamed.obj"},
    {"index first", "mesh-named-first-unnamed-index-first.obj"},
};

const struct {
    const char* name;
    const char* message;
} InvalidKeywordsData[]{
    {"unknown keyword", "unknown keyword bleh"},
    {"no space between vertex keyword and number", "unknown keyword vt3"},
    {"no space between index keyword and number", "unknown keyword p6"}
};

const struct {
    const char* name;
    const char* message;
} InvalidMixedPrimitivesData[]{
    {"points after some other",
        "mixed primitive MeshPrimitive::Triangles and MeshPrimitive::Points"},
    {"lines after some other",
        "mixed primitive MeshPrimitive::Points and MeshPrimitive::Lines"},
    {"triangles after some other",
        "mixed primitive MeshPrimitive::Lines and MeshPrimitive::Triangles"},
};

const struct {
    const char* name;
    const char* message;
} InvalidNumbersData[]{
    {"too long float literal", "too long numeric literal 1234.567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567"},
    {"too long integer literal", "too long numeric literal 12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"},
    {"too large integer literal", "too large integer literal 4294967296"},
    {"invalid float literal", "invalid floating-point literal bleh"},
    {"invalid integer literal", "invalid integer literal bleh"},
    {"position index out of range", "index 3 out of range for 1 vertices"},
    {"texture index out of range", "index 4 out of range for 3 vertices"},
    {"normal index out of range", "index 3 out of range for 2 vertices"},
    {"zero index", "index 0 out of range for 1 vertices"}
};

const struct {
    const char* name;
    const char* message;
} InvalidNumberCountData[]{
    {"no position component at all", "expected 3 or 4 position coordinates, got "},
    {"two-component position", "expected 3 or 4 position coordinates, got 0.5 1.0"},
    {"five-component position with optional fourth component", "expected 3 or 4 position coordinates, got 0.5 1 2 1.0 3.5"},
    {"no texture coordinate component at all", "expected 2 or 3 texture coordinates, got "},
    {"one-component texture coordinate", "expected 2 or 3 texture coordinates, got 0.5"},
    {"four-component texture coordinate with optional third component", "expected 2 or 3 texture coordinates, got 0.5 1.0 0.0 7.4"},
    {"no normal component at all", "expected 3 normal coordinates, got "},
    {"two-component normal", "expected 3 normal coordinates, got 0.5 0.0"},
    {"four-component normal", "expected 3 normal coordinates, got 0.5 1.0 2.3 7.4"},
    {"no index at all", "expected exactly 1 position index tuple for a point, got "},
    /** @todo better error message (the literal is empty) */
    {"no index before first slash", "invalid integer literal "},
    {"no index after first slash", "invalid integer literal "},
    {"no index after second slash", "invalid integer literal "},
    {"four-component index tuple", "invalid integer literal 1/1"},
    {"point with two indices", "expected exactly 1 position index tuple for a point, got 9 9"},
    {"line with one index", "expected exactly 2 position index tuples for a line, got 10"},
    {"triangle with two indices", "expected exactly 3 position index tuples for a triangle, got 11 11"},
    {"quad", "expected exactly 3 position index tuples for a triangle, got 12 12 12 12"}
};

const struct {
    const char* name;
    const char* message;
} InvalidInconsistentIndexTupleData[]{
    {"missing normal reference", "some normal indices are missing"},
    {"missing texture reference", "some texture coordinate indices are missing"},
};

const struct {
    const char* name;
    const char* message;
} InvalidIncompleteDataData[]{
    {"missing position data", "incomplete position data"},
    {"missing position indices", "incomplete position data"},
    {"missing normal data", "incomplete normal data"},
    {"missing normal indices", "incomplete normal data"},
    {"missing texture coordinate data", "incomplete texture coordinate data"},
    {"missing texture coordinate indices", "incomplete texture coordinate data"},
};

const struct {
    const char* name;
    const char* message;
} InvalidOptionalCoordinateData[]{
    {"position with optional fourth component not one", "homogeneous coordinates are not supported"},
    {"texture with optional third component not zero", "3D texture coordinates are not supported"}
};

ObjImporterTest::ObjImporterTest() {
    addTests({&ObjImporterTest::empty,

              &ObjImporterTest::meshPrimitivePoints,
              &ObjImporterTest::meshPrimitiveLines,
              &ObjImporterTest::meshPrimitiveTriangles,

              &ObjImporterTest::meshPositionsOptionalCoordinate,
              &ObjImporterTest::meshTextureCoordinates,
              &ObjImporterTest::meshTextureCoordinatesOptionalCoordinate,
              &ObjImporterTest::meshNormals,
              &ObjImporterTest::meshTextureCoordinatesNormals,

              &ObjImporterTest::meshIgnoredKeyword,

              &ObjImporterTest::meshNamed});

    addInstancedTests({&ObjImporterTest::meshNamedFirstUnnamed},
        Containers::arraySize(MeshNamedFirstUnnamedData));

    addTests({&ObjImporterTest::moreMeshes});

    addInstancedTests({&ObjImporterTest::invalidKeywords},
        Containers::arraySize(InvalidKeywordsData));

    addInstancedTests({&ObjImporterTest::invalidMixedPrimitives},
        Containers::arraySize(InvalidMixedPrimitivesData));

    addInstancedTests({&ObjImporterTest::invalidNumbers},
        Containers::arraySize(InvalidNumbersData));

    addInstancedTests({&ObjImporterTest::invalidNumberCount},
        Containers::arraySize(InvalidNumberCountData));

    addInstancedTests({&ObjImporterTest::invalidInconsistentIndexTuple},
        Containers::arraySize(InvalidInconsistentIndexTupleData));

    addInstancedTests({&ObjImporterTest::invalidIncompleteData},
        Containers::arraySize(InvalidIncompleteDataData));

    addInstancedTests({&ObjImporterTest::invalidOptionalCoordinate},
        Containers::arraySize(InvalidOptionalCoordinateData));

    addTests({&ObjImporterTest::whitespace,

              &ObjImporterTest::openTwice,
              &ObjImporterTest::importTwice});

    #ifdef OBJIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(OBJIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void ObjImporterTest::empty() {
    /* Duplicates what's in invalidIncompleteData(MissingPositionData), but
       it's good to have such case explicit. It also tests for empty naming. */

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "empty.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);
    CORRADE_COMPARE(importer->meshName(0), "");
    CORRADE_COMPARE(importer->meshForName(""), -1);

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(0));
    CORRADE_COMPARE(out, "Trade::ObjImporter::mesh(): incomplete position data\n");
}

void ObjImporterTest::meshPrimitivePoints() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-points.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(data->attributeCount(), 1);
    /* The points get reordered according to the index buffer. Might not be a
       problem in general but it is when relying on the order */
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {2.0f, 3.0f, 5.0f},
            {0.0f, 1.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data->isIndexed());
    CORRADE_COMPARE(data->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 0}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::meshPrimitiveLines() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-lines.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data->attributeCount(), 1);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f},
            {2.0f, 3.0f, 5.0f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data->isIndexed());
    CORRADE_COMPARE(data->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 1, 2}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::meshPrimitiveTriangles() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-triangles.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(data->attributeCount(), 1);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f},
            {2.0f, 3.0f, 5.0f},
            {2.5f, 0.0f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data->isIndexed());
    CORRADE_COMPARE(data->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 3, 1, 0}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::meshPositionsOptionalCoordinate() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-positions-optional-coordinate.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->attributeCount(), 1);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {1.5f, 2.0f, 3.0f}
        }), TestSuite::Compare::Container);
}

void ObjImporterTest::meshTextureCoordinates() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-texture-coordinates.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data->attributeCount(), 2);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f},
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data->attribute<Vector2>(MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {1.0f, 0.5f},
            {1.0f, 0.5f},
            {0.5f, 1.0f},
            {0.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data->isIndexed());
    CORRADE_COMPARE(data->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 3, 1, 0}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::meshTextureCoordinatesOptionalCoordinate() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-texture-coordinates-optional-coordinate.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->attributeCount(MeshAttribute::TextureCoordinates), 1);
    CORRADE_COMPARE_AS(data->attribute<Vector2>(MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.5f, 0.7f}
        }), TestSuite::Compare::Container);
}

void ObjImporterTest::meshNormals() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-normals.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data->attributeCount(), 2);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f},
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Normal),
        Containers::arrayView<Vector3>({
            {1.0f, 0.5f, 3.5f},
            {1.0f, 0.5f, 3.5f},
            {0.5f, 1.0f, 0.5f},
            {0.5f, 1.0f, 0.5f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data->isIndexed());
    CORRADE_COMPARE(data->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 3, 1, 0}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::meshTextureCoordinatesNormals() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-texture-coordinates-normals.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data->attributeCount(), 3);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f},
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f},
            {0.0f, 1.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data->attribute<Vector2>(MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {1.0f, 0.5f},
            {1.0f, 0.5f},
            {0.5f, 1.0f},
            {0.5f, 1.0f},
            {0.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Normal),
        Containers::arrayView<Vector3>({
            {1.0f, 0.5f, 3.5f},
            {0.5f, 1.0f, 0.5f},
            {0.5f, 1.0f, 0.5f},
            {1.0f, 0.5f, 3.5f},
            {0.5f, 1.0f, 0.5f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data->isIndexed());
    CORRADE_COMPARE(data->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 3, 1, 0, 4, 2}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::meshIgnoredKeyword() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-ignored-keyword.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    /* Everything should be parsed properly */
    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(data->attributeCount(), 1);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.0f, 1.0f, 2.0f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data->isIndexed());
    CORRADE_COMPARE(data->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::meshNamed() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-named.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);
    CORRADE_COMPARE(importer->meshName(0), "MyMesh");
    CORRADE_COMPARE(importer->meshForName("MyMesh"), 0);
}

void ObjImporterTest::meshNamedFirstUnnamed() {
    auto&& data = MeshNamedFirstUnnamedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, data.filename)));
    CORRADE_COMPARE(importer->meshCount(), 2);

    CORRADE_COMPARE(importer->meshName(0), "");
    CORRADE_COMPARE(importer->meshForName(""), -1);

    CORRADE_COMPARE(importer->meshName(1), "SecondMesh");
    CORRADE_COMPARE(importer->meshForName("SecondMesh"), 1);
}

void ObjImporterTest::moreMeshes() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-multiple.obj")));
    CORRADE_COMPARE(importer->meshCount(), 3);

    CORRADE_COMPARE(importer->meshName(0), "PointMesh");
    CORRADE_COMPARE(importer->meshForName("PointMesh"), 0);
    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(data->attributeCount(), 2);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Normal),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data->isIndexed());
    CORRADE_COMPARE(data->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(importer->meshName(1), "LineMesh");
    CORRADE_COMPARE(importer->meshForName("LineMesh"), 1);
    const Containers::Optional<MeshData> data1 = importer->mesh(1);
    CORRADE_VERIFY(data1);
    CORRADE_COMPARE(data1->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data1->attributeCount(), 2);
    CORRADE_COMPARE_AS(data1->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data1->attribute<Vector2>(MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.5f, 2.0f},
            {0.0f, 1.5f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data1->isIndexed());
    CORRADE_COMPARE(data1->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data1->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 1, 0}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(importer->meshName(2), "TriangleMesh");
    CORRADE_COMPARE(importer->meshForName("TriangleMesh"), 2);
    const Containers::Optional<MeshData> data2 = importer->mesh(2);
    CORRADE_VERIFY(data2);
    CORRADE_COMPARE(data2->primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(data2->attributeCount(), 3);
    CORRADE_COMPARE_AS(data2->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.0f, 1.5f, 1.0f},
            {2.0f, 3.0f, 5.5f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data2->attribute<Vector3>(MeshAttribute::Normal),
        Containers::arrayView<Vector3>({
            {0.5f, 2.0f, 3.0f},
            {0.5f, 2.0f, 3.0f},
            {0.5f, 2.0f, 3.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data2->attribute<Vector2>(MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.5f, 2.0f},
            {0.5f, 2.0f},
            {0.5f, 2.0f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(data2->isIndexed());
    CORRADE_COMPARE(data2->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(data2->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 2, 1, 0}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::invalidKeywords() {
    auto&& data = InvalidKeywordsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "invalid-keywords.obj")));

    /* Ensure we didn't forget to test any case */
    CORRADE_COMPARE(importer->meshCount(), Containers::arraySize(InvalidKeywordsData));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(data.name));
    CORRADE_COMPARE(out, Utility::format("Trade::ObjImporter::mesh(): {}\n", data.message));
}

void ObjImporterTest::invalidMixedPrimitives() {
    auto&& data = InvalidMixedPrimitivesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "invalid-mixed-primitives.obj")));

    /* Ensure we didn't forget to test any case */
    CORRADE_COMPARE(importer->meshCount(), Containers::arraySize(InvalidMixedPrimitivesData));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(data.name));
    CORRADE_COMPARE(out, Utility::format("Trade::ObjImporter::mesh(): {}\n", data.message));
}

void ObjImporterTest::invalidNumbers() {
    auto&& data = InvalidNumbersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "invalid-numbers.obj")));

    /* Ensure we didn't forget to test any case */
    CORRADE_COMPARE(importer->meshCount(), Containers::arraySize(InvalidNumbersData));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(data.name));
    CORRADE_COMPARE(out, Utility::format("Trade::ObjImporter::mesh(): {}\n", data.message));
}

void ObjImporterTest::invalidNumberCount() {
    auto&& data = InvalidNumberCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "invalid-number-count.obj")));

    /* Ensure we didn't forget to test any case */
    CORRADE_COMPARE(importer->meshCount(), Containers::arraySize(InvalidNumberCountData));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(data.name));
    CORRADE_COMPARE(out, Utility::format("Trade::ObjImporter::mesh(): {}\n", data.message));
}

void ObjImporterTest::invalidInconsistentIndexTuple() {
    auto&& data = InvalidInconsistentIndexTupleData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "invalid-inconsistent-index-tuple.obj")));

    /* Ensure we didn't forget to test any case */
    CORRADE_COMPARE(importer->meshCount(), Containers::arraySize(InvalidInconsistentIndexTupleData));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(data.name));
    CORRADE_COMPARE(out, Utility::format("Trade::ObjImporter::mesh(): {}\n", data.message));
}

void ObjImporterTest::invalidIncompleteData() {
    auto&& data = InvalidIncompleteDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "invalid-incomplete-data.obj")));

    /* Ensure we didn't forget to test any case */
    CORRADE_COMPARE(importer->meshCount(), Containers::arraySize(InvalidIncompleteDataData));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(data.name));
    CORRADE_COMPARE(out, Utility::format("Trade::ObjImporter::mesh(): {}\n", data.message));
}

void ObjImporterTest::invalidOptionalCoordinate() {
    auto&& data = InvalidOptionalCoordinateData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "invalid-optional-coordinate.obj")));

    /* Ensure we didn't forget to test any case */
    CORRADE_COMPARE(importer->meshCount(), Containers::arraySize(InvalidOptionalCoordinateData));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(data.name));
    CORRADE_COMPARE(out, Utility::format("Trade::ObjImporter::mesh(): {}\n", data.message));
}

void ObjImporterTest::whitespace() {
    /* Using an embedded string to have the whitespace visualized clearer */

    Containers::StringView data =
        " \t\r  # This file has various \t\n"
        "  #whitespace that should be treated properly \n"
        " \r\t o\t \tObject \t name   \r\n"
        " \t  v\r  \t 3  5 \t7  \t1\n"
        " \rvt  8\t 9\t\t\r0\n"
        "\n  \r \n"
        "     vn \t  2  \t 4 \r6\n"
        "\n \t   \r\n"
        /* There's no space expected inside the index tuple */
        "  f\t  1/1/1  \r1/1/1 \t1/1/1  \t\r\n"
        "\n\n"
        "\n";

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openData(data));

    CORRADE_COMPARE(importer->meshCount(), 1);
    /* The name is trimmed from both ends, but internal whitespace is left
       intact */
    CORRADE_COMPARE(importer->meshName(0), "Object \t name");

    const Containers::Optional<MeshData> mesh = importer->mesh(0);
    CORRADE_VERIFY(mesh);
    CORRADE_COMPARE(mesh->primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(mesh->attributeCount(), 3);
    CORRADE_COMPARE_AS(mesh->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {3.0f, 5.0f, 7.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh->attribute<Vector2>(MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {8.0f, 9.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh->attribute<Vector3>(MeshAttribute::Normal),
        Containers::arrayView<Vector3>({
            {2.0f, 4.0f, 6.0f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(mesh->isIndexed());
    CORRADE_COMPARE(mesh->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(mesh->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 0, 0}),
        TestSuite::Compare::Container);
}

void ObjImporterTest::openTwice() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-points.obj")));
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-points.obj")));

    /* Shouldn't crash, leak or anything */
}

void ObjImporterTest::importTwice() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(OBJIMPORTER_TEST_DIR, "mesh-primitive-points.obj")));

    /* Verify that everything is working the same way on second use */
    {
        Containers::Optional<Trade::MeshData> mesh = importer->mesh(0);
        CORRADE_VERIFY(mesh);
        CORRADE_COMPARE(mesh->vertexCount(), 3);
    } {
        Containers::Optional<Trade::MeshData> mesh = importer->mesh(0);
        CORRADE_VERIFY(mesh);
        CORRADE_COMPARE(mesh->vertexCount(), 3);
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ObjImporterTest)
