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
#include <Corrade/TestSuite/Compare/FileToString.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AbstractSceneConverterTest: TestSuite::Tester {
    explicit AbstractSceneConverterTest();

    void featuresNone();

    void setFlags();
    void setFlagsNotImplemented();

    void thingNotSupported();

    void convertMesh();
    void convertMeshNotImplemented();
    void convertMeshNonOwningDeleters();
    void convertMeshGrowableDeleters();
    void convertMeshCustomIndexDataDeleter();
    void convertMeshCustomVertexDataDeleter();
    void convertMeshCustomAttributeDataDeleter();

    void convertMeshInPlace();
    void convertMeshInPlaceNotImplemented();

    void convertMeshToData();
    void convertMeshToDataNotImplemented();
    void convertMeshToDataCustomDeleter();

    void convertMeshToFile();
    void convertMeshToFileThroughData();
    void convertMeshToFileThroughDataFailed();
    void convertMeshToFileThroughDataNotWritable();
    void convertMeshToFileNotImplemented();

    void debugFeature();
    void debugFeatures();
    void debugFlag();
    void debugFlags();
};

AbstractSceneConverterTest::AbstractSceneConverterTest() {
    addTests({&AbstractSceneConverterTest::featuresNone,

              &AbstractSceneConverterTest::setFlags,
              &AbstractSceneConverterTest::setFlagsNotImplemented,

              &AbstractSceneConverterTest::thingNotSupported,

              &AbstractSceneConverterTest::convertMesh,
              &AbstractSceneConverterTest::convertMeshNotImplemented,
              &AbstractSceneConverterTest::convertMeshNonOwningDeleters,
              &AbstractSceneConverterTest::convertMeshGrowableDeleters,
              &AbstractSceneConverterTest::convertMeshCustomIndexDataDeleter,
              &AbstractSceneConverterTest::convertMeshCustomVertexDataDeleter,
              &AbstractSceneConverterTest::convertMeshCustomAttributeDataDeleter,

              &AbstractSceneConverterTest::convertMeshInPlace,
              &AbstractSceneConverterTest::convertMeshInPlaceNotImplemented,

              &AbstractSceneConverterTest::convertMeshToData,
              &AbstractSceneConverterTest::convertMeshToDataNotImplemented,
              &AbstractSceneConverterTest::convertMeshToDataCustomDeleter,

              &AbstractSceneConverterTest::convertMeshToFile,
              &AbstractSceneConverterTest::convertMeshToFileThroughData,
              &AbstractSceneConverterTest::convertMeshToFileThroughDataFailed,
              &AbstractSceneConverterTest::convertMeshToFileThroughDataNotWritable,
              &AbstractSceneConverterTest::convertMeshToFileNotImplemented,

              &AbstractSceneConverterTest::debugFeature,
              &AbstractSceneConverterTest::debugFeatures,
              &AbstractSceneConverterTest::debugFlag,
              &AbstractSceneConverterTest::debugFlags});

    /* Create testing dir */
    Utility::Directory::mkpath(TRADE_TEST_OUTPUT_DIR);
}

void AbstractSceneConverterTest::featuresNone() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return {}; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.features();
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::features(): implementation reported no features\n");
}

void AbstractSceneConverterTest::setFlags() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            /* Assuming this bit is unused */
            return SceneConverterFeature(1 << 7);
        }
        void doSetFlags(SceneConverterFlags flags) override {
            _flags = flags;
        }

        SceneConverterFlags _flags;
    } converter;

    CORRADE_COMPARE(converter.flags(), SceneConverterFlags{});
    CORRADE_COMPARE(converter._flags, SceneConverterFlags{});
    converter.setFlags(SceneConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), SceneConverterFlag::Verbose);
    CORRADE_COMPARE(converter._flags, SceneConverterFlag::Verbose);
}

void AbstractSceneConverterTest::setFlagsNotImplemented() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            /* Assuming this bit is unused */
            return SceneConverterFeature(1 << 7);
        }
    } converter;

    CORRADE_COMPARE(converter.flags(), SceneConverterFlags{});
    converter.setFlags(SceneConverterFlag::Verbose);
    CORRADE_COMPARE(converter.flags(), SceneConverterFlag::Verbose);
    /* Should just work, no need to implement the function */
}

void AbstractSceneConverterTest::thingNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override {
            /* Assuming this bit is unused */
            return SceneConverterFeature(1 << 7);
        }
    } converter;

    MeshData mesh{MeshPrimitive::Triangles, 3};

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(mesh);
    converter.convertInPlace(mesh);
    converter.convertToData(mesh);
    converter.convertToFile(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "mesh.out"), mesh);
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::convert(): mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::convertInPlace(): mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::convertToData(): mesh conversion not supported\n"
        "Trade::AbstractSceneConverter::convertToFile(): mesh conversion not supported\n");
}

void AbstractSceneConverterTest::convertMesh() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData& mesh) override {
            if(mesh.primitive() == MeshPrimitive::Triangles)
                return MeshData{MeshPrimitive::Lines, mesh.vertexCount()*2};
            return {};
        }
    } converter;

    Containers::Optional<MeshData> out = converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(out->primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(out->vertexCount(), 12);
}

void AbstractSceneConverterTest::convertMeshNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convert(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::convertMeshNonOwningDeleters() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles,
                Containers::Array<char>{indexData, 1, Implementation::nonOwnedArrayDeleter}, MeshIndexData{MeshIndexType::UnsignedByte, indexData},
                Containers::Array<char>{nullptr, 0, Implementation::nonOwnedArrayDeleter},
                meshAttributeDataNonOwningArray(attributes)};
        }

        char indexData[1];
        MeshAttributeData attributes[1]{
            MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}
        };
    } converter;

    Containers::Optional<MeshData> out = converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(static_cast<const void*>(out->indexData()), converter.indexData);
}

void AbstractSceneConverterTest::convertMeshGrowableDeleters() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            Containers::Array<char> indexData;
            Containers::arrayAppend<ArrayAllocator>(indexData, '\xab');
            Containers::Array<Vector3> vertexData;
            Containers::arrayAppend<ArrayAllocator>(vertexData, Vector3{});
            MeshIndexData indices{MeshIndexType::UnsignedByte, indexData};
            MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};

            return MeshData{MeshPrimitive::Triangles,
                std::move(indexData), indices,
                Containers::arrayAllocatorCast<char, ArrayAllocator>(std::move(vertexData)), {positions}};
        }

        char indexData[1];
        MeshAttributeData attributes[1]{
            MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}
        };
    } converter;

    Containers::Optional<MeshData> out = converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_VERIFY(out);
    CORRADE_COMPARE(out->indexData()[0], '\xab');
    CORRADE_COMPARE(out->vertexData().size(), 12);
}

void AbstractSceneConverterTest::convertMeshCustomIndexDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshCustomVertexDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshCustomAttributeDataDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMesh; }

        Containers::Optional<MeshData> doConvert(const MeshData&) override {
            return MeshData{MeshPrimitive::Triangles, Containers::Array<char>{data, 1, [](char*, std::size_t) {}}, MeshIndexData{MeshIndexType::UnsignedByte, data}, 1};
        }

        char data[1];
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convert(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(),
        "Trade::AbstractSceneConverter::convert(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshInPlace() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshInPlace; }

        bool doConvertInPlace(MeshData& mesh) override {
            auto indices = mesh.mutableIndices<UnsignedInt>();
            for(std::size_t i = 0; i != indices.size()/2; ++i)
                std::swap(indices[i], indices[indices.size() - i -1]);
            return true;
        }
    } converter;

    UnsignedInt indices[]{1, 2, 3, 4, 2, 0};
    MeshData mesh{MeshPrimitive::Triangles,
        DataFlag::Mutable, indices, MeshIndexData{indices}, 5};
    CORRADE_VERIFY(converter.convertInPlace(mesh));
    CORRADE_COMPARE_AS(mesh.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 2, 4, 3, 2, 1}),
        TestSuite::Compare::Container);
}

void AbstractSceneConverterTest::convertMeshInPlaceNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshInPlace; }
    } converter;

    MeshData mesh{MeshPrimitive::Triangles, 3};

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertInPlace(mesh);
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convertInPlace(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::convertMeshToData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Array<char> doConvertToData(const MeshData& mesh) override {
            return Containers::Array<char>{nullptr, mesh.vertexCount()};
        }
    } converter;

    Containers::Array<char> data = converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(data.size(), 6);
}

void AbstractSceneConverterTest::convertMeshToDataNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convertToData(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::convertMeshToDataCustomDeleter() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Array<char> doConvertToData(const MeshData&) override {
            return Containers::Array<char>{data, 1, [](char*, std::size_t) {}};
        }

        char data[1];
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToData(MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convertToData(): implementation is not allowed to use a custom Array deleter\n");
}

void AbstractSceneConverterTest::convertMeshToFile() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToFile; }

        bool doConvertToFile(const std::string& filename, const MeshData& mesh) override {
            return Utility::Directory::write(filename, Containers::arrayView(               {char(mesh.vertexCount())}));
        }
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    CORRADE_VERIFY(converter.convertToFile(filename, MeshData{MeshPrimitive::Triangles, 0xef}));
    CORRADE_COMPARE_AS(filename,
        "\xef", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::convertMeshToFileThroughData() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Array<char> doConvertToData(const MeshData& mesh) override {
            return Containers::array({char(mesh.vertexCount())});
        }
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* doConvertToFile() should call doConvertToData() */
    CORRADE_VERIFY(converter.convertToFile(filename, MeshData{MeshPrimitive::Triangles, 0xef}));
    CORRADE_COMPARE_AS(filename,
        "\xef", TestSuite::Compare::FileToString);
}

void AbstractSceneConverterTest::convertMeshToFileThroughDataFailed() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Array<char> doConvertToData(const MeshData&) override {
            return {};
        }
    } converter;

    const std::string filename = Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "mesh.out");

    /* Remove previous file, if any */
    Utility::Directory::rm(filename);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));

    /* Function should fail, no file should get written and no error output
       should be printed (the base implementation assumes the plugin does it) */
    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile(filename, MeshData{MeshPrimitive::Triangles, 0xef}));
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
    CORRADE_COMPARE(out.str(), "");
}

void AbstractSceneConverterTest::convertMeshToFileThroughDataNotWritable() {
    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToData; }

        Containers::Array<char> doConvertToData(const MeshData& mesh) override {
            return Containers::array({char(mesh.vertexCount())});
        }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter.convertToFile("/some/path/that/does/not/exist", MeshData{MeshPrimitive::Triangles, 0xef}));
    CORRADE_COMPARE(out.str(),
        "Utility::Directory::write(): can't open /some/path/that/does/not/exist\n"
        "Trade::AbstractSceneConverter::convertToFile(): cannot write to file /some/path/that/does/not/exist\n");
}

void AbstractSceneConverterTest::convertMeshToFileNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct: AbstractSceneConverter {
        SceneConverterFeatures doFeatures() const override { return SceneConverterFeature::ConvertMeshToFile; }
    } converter;

    std::ostringstream out;
    Error redirectError{&out};
    converter.convertToFile(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "mesh.out"), MeshData{MeshPrimitive::Triangles, 6});
    CORRADE_COMPARE(out.str(), "Trade::AbstractSceneConverter::convertToFile(): mesh conversion advertised but not implemented\n");
}

void AbstractSceneConverterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << SceneConverterFeature::ConvertMeshInPlace << SceneConverterFeature(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::SceneConverterFeature::ConvertMeshInPlace Trade::SceneConverterFeature(0xf0)\n");
}

void AbstractSceneConverterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (SceneConverterFeature::ConvertMesh|SceneConverterFeature::ConvertMeshToFile) << SceneConverterFeatures{};
    CORRADE_COMPARE(out.str(), "Trade::SceneConverterFeature::ConvertMesh|Trade::SceneConverterFeature::ConvertMeshToFile Trade::SceneConverterFeatures{}\n");
}

void AbstractSceneConverterTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << SceneConverterFlag::Verbose << SceneConverterFlag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::SceneConverterFlag::Verbose Trade::SceneConverterFlag(0xf0)\n");
}

void AbstractSceneConverterTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (SceneConverterFlag::Verbose|SceneConverterFlag(0xf0)) << SceneConverterFlags{};
    CORRADE_COMPARE(out.str(), "Trade::SceneConverterFlag::Verbose|Trade::SceneConverterFlag(0xf0) Trade::SceneConverterFlags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractSceneConverterTest)
