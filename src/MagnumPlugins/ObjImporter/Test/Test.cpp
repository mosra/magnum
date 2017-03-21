/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/MeshData3D.h"
#include "MagnumPlugins/ObjImporter/ObjImporter.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test {

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
}

void ObjImporterTest::pointMesh() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "pointMesh.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);

    const std::optional<MeshData3D> data = importer.mesh3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f},
        {2.0f, 3.0f, 5.0f}
    }));
    CORRADE_COMPARE(data->indices(), (std::vector<UnsignedInt>{
        0, 2, 1, 0
    }));
}

void ObjImporterTest::lineMesh() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "lineMesh.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);

    const std::optional<MeshData3D> data = importer.mesh3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f},
        {2.0f, 3.0f, 5.0f}
    }));
    CORRADE_COMPARE(data->indices(), (std::vector<UnsignedInt>{
        0, 1, 1, 2
    }));
}

void ObjImporterTest::triangleMesh() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "triangleMesh.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);

    const std::optional<MeshData3D> data = importer.mesh3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f},
        {2.0f, 3.0f, 5.0f},
        {2.5f, 0.0f, 1.0f}
    }));
    CORRADE_COMPARE(data->indices(), (std::vector<UnsignedInt>{
        0, 1, 2, 3, 1, 0
    }));
}

void ObjImporterTest::mixedPrimitives() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "mixedPrimitives.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(0));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): mixed primitive MeshPrimitive::Points and MeshPrimitive::Lines\n");
}

void ObjImporterTest::positionsOnly() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "triangleMesh.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);

    const std::optional<MeshData3D> data = importer.mesh3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_VERIFY(!data->hasNormals());
    CORRADE_VERIFY(!data->hasTextureCoords2D());
}

void ObjImporterTest::textureCoordinates() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "textureCoordinates.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);

    const std::optional<MeshData3D> data = importer.mesh3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_VERIFY(!data->hasNormals());
    CORRADE_COMPARE(data->textureCoords2DArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f},
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f}
    }));
    CORRADE_COMPARE(data->textureCoords2D(0), (std::vector<Vector2>{
        {1.0f, 0.5f},
        {1.0f, 0.5f},
        {0.5f, 1.0f},
        {0.5f, 1.0f}
    }));
    CORRADE_COMPARE(data->indices(), (std::vector<UnsignedInt>{
        0, 1, 2, 3, 1, 0
    }));
}

void ObjImporterTest::normals() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "normals.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);

    const std::optional<MeshData3D> data = importer.mesh3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_VERIFY(!data->hasTextureCoords2D());
    CORRADE_COMPARE(data->normalArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f},
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f}
    }));
    CORRADE_COMPARE(data->normals(0), (std::vector<Vector3>{
        {1.0f, 0.5f, 3.5f},
        {1.0f, 0.5f, 3.5f},
        {0.5f, 1.0f, 0.5f},
        {0.5f, 1.0f, 0.5f}
    }));
    CORRADE_COMPARE(data->indices(), (std::vector<UnsignedInt>{
        0, 1, 2, 3, 1, 0
    }));
}

void ObjImporterTest::textureCoordinatesNormals() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "textureCoordinatesNormals.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);

    const std::optional<MeshData3D> data = importer.mesh3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_COMPARE(data->textureCoords2DArrayCount(), 1);
    CORRADE_COMPARE(data->normalArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f},
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f},
        {0.0f, 1.5f, 1.0f}
    }));
    CORRADE_COMPARE(data->textureCoords2D(0), (std::vector<Vector2>{
        {1.0f, 0.5f},
        {1.0f, 0.5f},
        {0.5f, 1.0f},
        {0.5f, 1.0f},
        {0.5f, 1.0f}
    }));
    CORRADE_COMPARE(data->normals(0), (std::vector<Vector3>{
        {1.0f, 0.5f, 3.5f},
        {0.5f, 1.0f, 0.5f},
        {0.5f, 1.0f, 0.5f},
        {1.0f, 0.5f, 3.5f},
        {0.5f, 1.0f, 0.5f}
    }));
    CORRADE_COMPARE(data->indices(), (std::vector<UnsignedInt>{
        0, 1, 2, 3, 1, 0, 4, 2
    }));
}

void ObjImporterTest::emptyFile() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "emptyFile.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);
}

void ObjImporterTest::unnamedMesh() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "emptyFile.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);
    CORRADE_COMPARE(importer.mesh3DName(0), "");
    CORRADE_COMPARE(importer.mesh3DForName(""), -1);
}

void ObjImporterTest::namedMesh() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "namedMesh.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 1);
    CORRADE_COMPARE(importer.mesh3DName(0), "MyMesh");
    CORRADE_COMPARE(importer.mesh3DForName("MyMesh"), 0);
}

void ObjImporterTest::moreMeshes() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "moreMeshes.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 3);

    CORRADE_COMPARE(importer.mesh3DName(0), "PointMesh");
    CORRADE_COMPARE(importer.mesh3DForName("PointMesh"), 0);
    const std::optional<MeshData3D> data = importer.mesh3D(0);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f}
    }));
    CORRADE_COMPARE(data->indices(), (std::vector<UnsignedInt>{
        0, 1
    }));

    CORRADE_COMPARE(importer.mesh3DName(1), "LineMesh");
    CORRADE_COMPARE(importer.mesh3DForName("LineMesh"), 1);
    const std::optional<MeshData3D> data1 = importer.mesh3D(1);
    CORRADE_VERIFY(data1);
    CORRADE_COMPARE(data1->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(data1->positionArrayCount(), 1);
    CORRADE_COMPARE(data1->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f}
    }));
    CORRADE_COMPARE(data1->indices(), (std::vector<UnsignedInt>{
        0, 1, 1, 0
    }));

    CORRADE_COMPARE(importer.mesh3DName(2), "TriangleMesh");
    CORRADE_COMPARE(importer.mesh3DForName("TriangleMesh"), 2);
    const std::optional<MeshData3D> data2 = importer.mesh3D(2);
    CORRADE_VERIFY(data2);
    CORRADE_COMPARE(data2->primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(data2->positionArrayCount(), 1);
    CORRADE_COMPARE(data2->positions(0), (std::vector<Vector3>{
        {0.5f, 2.0f, 3.0f},
        {0.0f, 1.5f, 1.0f},
        {2.0f, 3.0f, 5.5f}
    }));
    CORRADE_COMPARE(data2->indices(), (std::vector<UnsignedInt>{
        0, 1, 2, 2, 1, 0
    }));
}

void ObjImporterTest::unnamedFirstMesh() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "unnamedFirstMesh.obj")));
    CORRADE_COMPARE(importer.mesh3DCount(), 2);

    CORRADE_COMPARE(importer.mesh3DName(0), "");
    CORRADE_COMPARE(importer.mesh3DForName(""), -1);

    CORRADE_COMPARE(importer.mesh3DName(1), "SecondMesh");
    CORRADE_COMPARE(importer.mesh3DForName("SecondMesh"), 1);
}

void ObjImporterTest::wrongFloat() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer.mesh3DForName("WrongFloat");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): error while converting numeric data\n");
}

void ObjImporterTest::wrongInteger() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer.mesh3DForName("WrongInteger");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): error while converting numeric data\n");
}

void ObjImporterTest::unmergedIndexOutOfRange() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer.mesh3DForName("PositionIndexOutOfRange");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): index out of range\n");
}

void ObjImporterTest::mergedIndexOutOfRange() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer.mesh3DForName("TextureIndexOutOfRange");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): index out of range\n");
}

void ObjImporterTest::zeroIndex() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumbers.obj")));
    const Int id = importer.mesh3DForName("ZeroIndex");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): index out of range\n");
}

void ObjImporterTest::explicitOptionalPositionCoordinate() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "optionalCoordinates.obj")));
    const Int id = importer.mesh3DForName("SupportedPositionW");
    CORRADE_VERIFY(id > -1);

    const std::optional<MeshData3D> data = importer.mesh3D(id);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {1.5f, 2.0f, 3.0f}
    }));
}

void ObjImporterTest::explicitOptionalTextureCoordinate() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "optionalCoordinates.obj")));
    const Int id = importer.mesh3DForName("SupportedTextureW");
    CORRADE_VERIFY(id > -1);

    const std::optional<MeshData3D> data = importer.mesh3D(id);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->textureCoords2DArrayCount(), 1);
    CORRADE_COMPARE(data->textureCoords2D(0), (std::vector<Vector2>{
        {0.5f, 0.7f}
    }));
}

void ObjImporterTest::unsupportedOptionalPositionCoordinate() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "optionalCoordinates.obj")));
    const Int id = importer.mesh3DForName("UnsupportedPositionW");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): homogeneous coordinates are not supported\n");
}

void ObjImporterTest::unsupportedOptionalTextureCoordinate() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "optionalCoordinates.obj")));
    const Int id = importer.mesh3DForName("UnsupportedTextureW");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): 3D texture coordinates are not supported\n");
}

void ObjImporterTest::shortFloatData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer.mesh3DForName("ShortFloat");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): invalid float array size\n");
}

void ObjImporterTest::longFloatData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer.mesh3DForName("LongFloat");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): invalid float array size\n");
}

void ObjImporterTest::longOptionalFloatData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer.mesh3DForName("LongOptionalFloat");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): invalid float array size\n");
}

void ObjImporterTest::longIndexData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer.mesh3DForName("InvalidIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): invalid index data\n");
}

void ObjImporterTest::wrongPointIndexData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer.mesh3DForName("WrongPointIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): wrong index count for point\n");
}

void ObjImporterTest::wrongLineIndexData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer.mesh3DForName("WrongLineIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): wrong index count for line\n");
}

void ObjImporterTest::wrongTriangleIndexData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer.mesh3DForName("WrongTriangleIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): wrong index count for triangle\n");
}

void ObjImporterTest::polygonIndexData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongNumberCount.obj")));
    const Int id = importer.mesh3DForName("PolygonIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): polygons are not supported\n");
}

void ObjImporterTest::missingPositionData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer.mesh3DForName("MissingPositionData");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): incomplete position data\n");
}

void ObjImporterTest::missingPositionIndices() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer.mesh3DForName("MissingPositionIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): incomplete position data\n");
}

void ObjImporterTest::missingNormalData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer.mesh3DForName("MissingNormalData");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): incomplete normal data\n");
}

void ObjImporterTest::missingNormalIndices() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer.mesh3DForName("MissingNormalIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): incomplete normal data\n");
}

void ObjImporterTest::missingTextureCoordinateData() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer.mesh3DForName("MissingTextureData");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): incomplete texture coordinate data\n");
}

void ObjImporterTest::missingTextureCoordinateIndices() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "missingData.obj")));
    const Int id = importer.mesh3DForName("MissingTextureIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): incomplete texture coordinate data\n");
}

void ObjImporterTest::wrongNormalIndexCount() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongIndexCount.obj")));
    const Int id = importer.mesh3DForName("ShortNormalIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): some normal indices are missing\n");
}

void ObjImporterTest::wrongTextureCoordinateIndexCount() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "wrongIndexCount.obj")));
    const Int id = importer.mesh3DForName("ShortTextureIndices");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): some texture coordinate indices are missing\n");
}

void ObjImporterTest::unsupportedKeyword() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "keywords.obj")));
    const Int id = importer.mesh3DForName("UnsupportedKeyword");
    CORRADE_VERIFY(id > -1);

    /* Everything should be parsed properly */
    const std::optional<MeshData3D> data = importer.mesh3D(id);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(data->positionArrayCount(), 1);
    CORRADE_COMPARE(data->positions(0), (std::vector<Vector3>{
        {0.0f, 1.0f, 2.0f}
    }));
    CORRADE_COMPARE(data->indices(), std::vector<UnsignedInt>{0});
}

void ObjImporterTest::unknownKeyword() {
    ObjImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(OBJIMPORTER_TEST_DIR, "keywords.obj")));
    const Int id = importer.mesh3DForName("UnknownKeyword");
    CORRADE_VERIFY(id > -1);

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer.mesh3D(id));
    CORRADE_COMPARE(out.str(), "Trade::ObjImporter::mesh3D(): unknown keyword bleh\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ObjImporterTest)
