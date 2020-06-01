/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct ObjImporterTest: TestSuite::Tester {
    explicit ObjImporterTest();

    void pointMesh();
    void lineMesh();
    void triangleMesh();
    void mixedPrimitives();

    void positionsOnly();
    void textureCoordinates();
    void normals();
    void textureCoordinatesNormals();

    void emptyFile();
    void unnamedMesh();
    void namedMesh();
    void moreMeshes();
    void unnamedFirstMesh();

    void wrongFloat();
    void wrongInteger();
    void unmergedIndexOutOfRange();
    void mergedIndexOutOfRange();
    void zeroIndex();

    void explicitOptionalPositionCoordinate();
    void explicitOptionalTextureCoordinate();
    void unsupportedOptionalPositionCoordinate();
    void unsupportedOptionalTextureCoordinate();

    void shortFloatData();
    void longFloatData();
    void longOptionalFloatData();

    void longIndexData();
    void wrongPointIndexData();
    void wrongLineIndexData();
    void wrongTriangleIndexData();
    void polygonIndexData();

    void missingPositionData();
    void missingNormalData();
    void missingTextureCoordinateData();
    void missingPositionIndices();
    void missingNormalIndices();
    void missingTextureCoordinateIndices();

    void wrongTextureCoordinateIndexCount();
    void wrongNormalIndexCount();

    void unsupportedKeyword();
    void unknownKeyword();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

ObjImporterTest::ObjImporterTest() {
    addTests({&ObjImporterTest::pointMesh,
              &ObjImporterTest::lineMesh,
              &ObjImporterTest::triangleMesh,
              &ObjImporterTest::mixedPrimitives,

              &ObjImporterTest::positionsOnly,
              &ObjImporterTest::textureCoordinates,
              &ObjImporterTest::normals,
              &ObjImporterTest::textureCoordinatesNormals,

              &ObjImporterTest::emptyFile,
              &ObjImporterTest::unnamedMesh,
              &ObjImporterTest::namedMesh,
              &ObjImporterTest::moreMeshes,
              &ObjImporterTest::unnamedFirstMesh,

              &ObjImporterTest::wrongFloat,
              &ObjImporterTest::wrongInteger,
              &ObjImporterTest::unmergedIndexOutOfRange,
              &ObjImporterTest::mergedIndexOutOfRange,
              &ObjImporterTest::zeroIndex,

              &ObjImporterTest::explicitOptionalPositionCoordinate,
              &ObjImporterTest::explicitOptionalTextureCoordinate,
              &ObjImporterTest::unsupportedOptionalPositionCoordinate,
              &ObjImporterTest::unsupportedOptionalTextureCoordinate,

              &ObjImporterTest::shortFloatData,
              &ObjImporterTest::longFloatData,
              &ObjImporterTest::longOptionalFloatData,

              &ObjImporterTest::longIndexData,
              &ObjImporterTest::wrongPointIndexData,
              &ObjImporterTest::wrongLineIndexData,
              &ObjImporterTest::wrongTriangleIndexData,
              &ObjImporterTest::polygonIndexData,

              &ObjImporterTest::missingPositionData,
              &ObjImporterTest::missingNormalData,
              &ObjImporterTest::missingTextureCoordinateData,
              &ObjImporterTest::missingPositionIndices,
              &ObjImporterTest::missingNormalIndices,
              &ObjImporterTest::missingTextureCoordinateIndices,

              &ObjImporterTest::wrongTextureCoordinateIndexCount,
              &ObjImporterTest::wrongNormalIndexCount,

              &ObjImporterTest::unsupportedKeyword,
              &ObjImporterTest::unknownKeyword});

    #ifdef OBJIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(OBJIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void ObjImporterTest::pointMesh() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "pointMesh.obj")));
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

void ObjImporterTest::lineMesh() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "lineMesh.obj")));
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

void ObjImporterTest::triangleMesh() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "triangleMesh.obj")));
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

void ObjImporterTest::mixedPrimitives() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "mixedPrimitives.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(0));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): mixed primitive MeshPrimitive::Points and MeshPrimitive::Lines\n");
}

void ObjImporterTest::positionsOnly() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "triangleMesh.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);

    const Containers::Optional<MeshData> data = importer->mesh(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->attributeCount(), 1);
    CORRADE_VERIFY(data->hasAttribute(MeshAttribute::Position));
}

void ObjImporterTest::textureCoordinates() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "textureCoordinates.obj")));
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

void ObjImporterTest::normals() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "normals.obj")));
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

void ObjImporterTest::textureCoordinatesNormals() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "textureCoordinatesNormals.obj")));
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

void ObjImporterTest::emptyFile() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "emptyFile.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);
}

void ObjImporterTest::unnamedMesh() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "emptyFile.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);
    CORRADE_COMPARE(importer->meshName(0), "");
    CORRADE_COMPARE(importer->meshForName(""), -1);
}

void ObjImporterTest::namedMesh() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "namedMesh.obj")));
    CORRADE_COMPARE(importer->meshCount(), 1);
    CORRADE_COMPARE(importer->meshName(0), "MyMesh");
    CORRADE_COMPARE(importer->meshForName("MyMesh"), 0);
}

void ObjImporterTest::moreMeshes() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "moreMeshes.obj")));
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

void ObjImporterTest::unnamedFirstMesh() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "unnamedFirstMesh.obj")));
    CORRADE_COMPARE(importer->meshCount(), 2);

    CORRADE_COMPARE(importer->meshName(0), "");
    CORRADE_COMPARE(importer->meshForName(""), -1);

    CORRADE_COMPARE(importer->meshName(1), "SecondMesh");
    CORRADE_COMPARE(importer->meshForName("SecondMesh"), 1);
}

void ObjImporterTest::wrongFloat() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer->meshForName("WrongFloat");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): error while converting numeric data\n");
}

void ObjImporterTest::wrongInteger() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer->meshForName("WrongInteger");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): error while converting numeric data\n");
}

void ObjImporterTest::unmergedIndexOutOfRange() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer->meshForName("PositionIndexOutOfRange");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): index 1 out of range for 1 vertices\n");
}

void ObjImporterTest::mergedIndexOutOfRange() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer->meshForName("TextureIndexOutOfRange");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): index 2 out of range for 1 vertices\n");
}

void ObjImporterTest::zeroIndex() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer->meshForName("ZeroIndex");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): index 0 out of range for 1 vertices\n");
}

void ObjImporterTest::explicitOptionalPositionCoordinate() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "optionalCoordinates.obj")));
    const Int id = importer->meshForName("SupportedPositionW");
    CORRADE_VERIFY(id > -1);

    const Containers::Optional<MeshData> data = importer->mesh(id);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->attributeCount(), 1);
    CORRADE_COMPARE_AS(data->attribute<Vector3>(MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {1.5f, 2.0f, 3.0f}
        }), TestSuite::Compare::Container);
}

void ObjImporterTest::explicitOptionalTextureCoordinate() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "optionalCoordinates.obj")));
    const Int id = importer->meshForName("SupportedTextureW");
    CORRADE_VERIFY(id > -1);

    const Containers::Optional<MeshData> data = importer->mesh(id);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->attributeCount(MeshAttribute::TextureCoordinates), 1);
    CORRADE_COMPARE_AS(data->attribute<Vector2>(MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.5f, 0.7f}
        }), TestSuite::Compare::Container);
}

void ObjImporterTest::unsupportedOptionalPositionCoordinate() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "optionalCoordinates.obj")));
    const Int id = importer->meshForName("UnsupportedPositionW");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): homogeneous coordinates are not supported\n");
}

void ObjImporterTest::unsupportedOptionalTextureCoordinate() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "optionalCoordinates.obj")));
    const Int id = importer->meshForName("UnsupportedTextureW");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): 3D texture coordinates are not supported\n");
}

void ObjImporterTest::shortFloatData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer->meshForName("ShortFloat");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): invalid float array size\n");
}

void ObjImporterTest::longFloatData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer->meshForName("LongFloat");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): invalid float array size\n");
}

void ObjImporterTest::longOptionalFloatData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer->meshForName("LongOptionalFloat");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): invalid float array size\n");
}

void ObjImporterTest::longIndexData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer->meshForName("InvalidIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): invalid index data\n");
}

void ObjImporterTest::wrongPointIndexData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer->meshForName("WrongPointIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): wrong index count for point\n");
}

void ObjImporterTest::wrongLineIndexData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer->meshForName("WrongLineIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): wrong index count for line\n");
}

void ObjImporterTest::wrongTriangleIndexData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer->meshForName("WrongTriangleIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): wrong index count for triangle\n");
}

void ObjImporterTest::polygonIndexData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer->meshForName("PolygonIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): polygons are not supported\n");
}

void ObjImporterTest::missingPositionData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer->meshForName("MissingPositionData");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): incomplete position data\n");
}

void ObjImporterTest::missingPositionIndices() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer->meshForName("MissingPositionIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): incomplete position data\n");
}

void ObjImporterTest::missingNormalData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer->meshForName("MissingNormalData");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): incomplete normal data\n");
}

void ObjImporterTest::missingNormalIndices() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer->meshForName("MissingNormalIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): incomplete normal data\n");
}

void ObjImporterTest::missingTextureCoordinateData() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer->meshForName("MissingTextureData");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): incomplete texture coordinate data\n");
}

void ObjImporterTest::missingTextureCoordinateIndices() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer->meshForName("MissingTextureIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): incomplete texture coordinate data\n");
}

void ObjImporterTest::wrongNormalIndexCount() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongIndexCount.obj")));
    const Int id = importer->meshForName("ShortNormalIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): some normal indices are missing\n");
}

void ObjImporterTest::wrongTextureCoordinateIndexCount() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongIndexCount.obj")));
    const Int id = importer->meshForName("ShortTextureIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): some texture coordinate indices are missing\n");
}

void ObjImporterTest::unsupportedKeyword() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "keywords.obj")));
    const Int id = importer->meshForName("UnsupportedKeyword");
    CORRADE_VERIFY(id > -1);

    /* Everything should be parsed properly */
    const Containers::Optional<MeshData> data = importer->mesh(id);
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

void ObjImporterTest::unknownKeyword() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("ObjImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "keywords.obj")));
    const Int id = importer->meshForName("UnknownKeyword");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->mesh(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh(): unknown keyword bleh\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ObjImporterTest)
