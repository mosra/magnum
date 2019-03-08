/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/FileCallback.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AnimationData.h"
#include "Magnum/Trade/CameraData.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/LightData.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/MeshObjectData2D.h"
#include "Magnum/Trade/MeshObjectData3D.h"
#include "Magnum/Trade/PhongMaterialData.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/TextureData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

class AbstractImporterTest: public TestSuite::Tester {
    public:
        explicit AbstractImporterTest();

        void construct();
        void constructWithPluginManagerReference();

        void openFileAsData();
        void openFileAsDataNotFound();

        void openFileNotImplemented();
        void openDataNotSupported();
        void openDataNotImplemented();
        void openStateNotSupported();
        void openStateNotImplemented();

        void setFileCallback();
        void setFileCallbackTemplate();
        void setFileCallbackTemplateNull();
        void setFileCallbackFileOpened();
        void setFileCallbackNotImplemented();
        void setFileCallbackNotSupported();
        void setFileCallbackOpenFileDirectly();
        void setFileCallbackOpenFileThroughBaseImplementation();
        void setFileCallbackOpenFileThroughBaseImplementationFailed();
        void setFileCallbackOpenFileAsData();
        void setFileCallbackOpenFileAsDataFailed();

        void defaultScene();
        void defaultSceneNotImplemented();
        void defaultSceneNoFile();

        void scene();
        void sceneCountNotImplemented();
        void sceneCountNoFile();
        void sceneForNameNotImplemented();
        void sceneForNameNoFile();
        void sceneNameNotImplemented();
        void sceneNameNoFile();
        void sceneNameOutOfRange();
        void sceneNotImplemented();
        void sceneNoFile();
        void sceneOutOfRange();

        void animation();
        void animationCountNotImplemented();
        void animationCountNoFile();
        void animationForNameNotImplemented();
        void animationForNameNoFile();
        void animationNameNotImplemented();
        void animationNameNoFile();
        void animationNameOutOfRange();
        void animationNotImplemented();
        void animationNoFile();
        void animationOutOfRange();

        void light();
        void lightCountNotImplemented();
        void lightCountNoFile();
        void lightForNameNotImplemented();
        void lightForNameNoFile();
        void lightNameNotImplemented();
        void lightNameNoFile();
        void lightNameOutOfRange();
        void lightNotImplemented();
        void lightNoFile();
        void lightOutOfRange();

        void camera();
        void cameraCountNotImplemented();
        void cameraCountNoFile();
        void cameraForNameNotImplemented();
        void cameraForNameNoFile();
        void cameraNameNotImplemented();
        void cameraNameNoFile();
        void cameraNameOutOfRange();
        void cameraNotImplemented();
        void cameraNoFile();
        void cameraOutOfRange();

        void object2D();
        void object2DCountNotImplemented();
        void object2DCountNoFile();
        void object2DForNameNotImplemented();
        void object2DForNameNoFile();
        void object2DNameNotImplemented();
        void object2DNameNoFile();
        void object2DNameOutOfRange();
        void object2DNotImplemented();
        void object2DNoFile();
        void object2DOutOfRange();

        void object3D();
        void object3DCountNotImplemented();
        void object3DCountNoFile();
        void object3DForNameNotImplemented();
        void object3DForNameNoFile();
        void object3DNameNotImplemented();
        void object3DNameNoFile();
        void object3DNameOutOfRange();
        void object3DNotImplemented();
        void object3DNoFile();
        void object3DOutOfRange();

        void mesh2D();
        void mesh2DCountNotImplemented();
        void mesh2DCountNoFile();
        void mesh2DForNameNotImplemented();
        void mesh2DForNameNoFile();
        void mesh2DNameNotImplemented();
        void mesh2DNameNoFile();
        void mesh2DNameOutOfRange();
        void mesh2DNotImplemented();
        void mesh2DNoFile();
        void mesh2DOutOfRange();

        void mesh3D();
        void mesh3DCountNotImplemented();
        void mesh3DCountNoFile();
        void mesh3DForNameNotImplemented();
        void mesh3DForNameNoFile();
        void mesh3DNameNotImplemented();
        void mesh3DNameNoFile();
        void mesh3DNameOutOfRange();
        void mesh3DNotImplemented();
        void mesh3DNoFile();
        void mesh3DOutOfRange();

        void material();
        void materialCountNotImplemented();
        void materialCountNoFile();
        void materialForNameNotImplemented();
        void materialForNameNoFile();
        void materialNameNotImplemented();
        void materialNameNoFile();
        void materialNameOutOfRange();
        void materialNotImplemented();
        void materialNoFile();
        void materialOutOfRange();

        void texture();
        void textureCountNotImplemented();
        void textureCountNoFile();
        void textureForNameNotImplemented();
        void textureForNameNoFile();
        void textureNameNotImplemented();
        void textureNameNoFile();
        void textureNameOutOfRange();
        void textureNotImplemented();
        void textureNoFile();
        void textureOutOfRange();

        void image1D();
        void image1DCountNotImplemented();
        void image1DCountNoFile();
        void image1DForNameNotImplemented();
        void image1DForNameNoFile();
        void image1DNameNotImplemented();
        void image1DNameNoFile();
        void image1DNameOutOfRange();
        void image1DNotImplemented();
        void image1DNoFile();
        void image1DOutOfRange();

        void image2D();
        void image2DCountNotImplemented();
        void image2DCountNoFile();
        void image2DForNameNotImplemented();
        void image2DForNameNoFile();
        void image2DNameNotImplemented();
        void image2DNameNoFile();
        void image2DNameOutOfRange();
        void image2DNotImplemented();
        void image2DNoFile();
        void image2DOutOfRange();

        void image3D();
        void image3DCountNotImplemented();
        void image3DCountNoFile();
        void image3DForNameNotImplemented();
        void image3DForNameNoFile();
        void image3DNameNotImplemented();
        void image3DNameNoFile();
        void image3DNameOutOfRange();
        void image3DNotImplemented();
        void image3DNoFile();
        void image3DOutOfRange();

        void importerState();
        void importerStateNotImplemented();
        void importerStateNoFile();

        void debugFeature();
        void debugFeatures();
};

AbstractImporterTest::AbstractImporterTest() {
    addTests({&AbstractImporterTest::construct,
              &AbstractImporterTest::constructWithPluginManagerReference,

              &AbstractImporterTest::openFileAsData,
              &AbstractImporterTest::openFileAsDataNotFound,

              &AbstractImporterTest::openFileNotImplemented,
              &AbstractImporterTest::openDataNotSupported,
              &AbstractImporterTest::openDataNotImplemented,
              &AbstractImporterTest::openStateNotSupported,
              &AbstractImporterTest::openStateNotImplemented,

              &AbstractImporterTest::setFileCallback,
              &AbstractImporterTest::setFileCallbackTemplate,
              &AbstractImporterTest::setFileCallbackTemplateNull,
              &AbstractImporterTest::setFileCallbackFileOpened,
              &AbstractImporterTest::setFileCallbackNotImplemented,
              &AbstractImporterTest::setFileCallbackNotSupported,
              &AbstractImporterTest::setFileCallbackOpenFileDirectly,
              &AbstractImporterTest::setFileCallbackOpenFileThroughBaseImplementation,
              &AbstractImporterTest::setFileCallbackOpenFileThroughBaseImplementationFailed,
              &AbstractImporterTest::setFileCallbackOpenFileAsData,
              &AbstractImporterTest::setFileCallbackOpenFileAsDataFailed,

              &AbstractImporterTest::defaultScene,
              &AbstractImporterTest::defaultSceneNotImplemented,
              &AbstractImporterTest::defaultSceneNoFile,

              &AbstractImporterTest::scene,
              &AbstractImporterTest::sceneCountNotImplemented,
              &AbstractImporterTest::sceneCountNoFile,
              &AbstractImporterTest::sceneForNameNotImplemented,
              &AbstractImporterTest::sceneForNameNoFile,
              &AbstractImporterTest::sceneNameNotImplemented,
              &AbstractImporterTest::sceneNameNoFile,
              &AbstractImporterTest::sceneNameOutOfRange,
              &AbstractImporterTest::sceneNotImplemented,
              &AbstractImporterTest::sceneNoFile,
              &AbstractImporterTest::sceneOutOfRange,

              &AbstractImporterTest::animation,
              &AbstractImporterTest::animationCountNotImplemented,
              &AbstractImporterTest::animationCountNoFile,
              &AbstractImporterTest::animationForNameNotImplemented,
              &AbstractImporterTest::animationForNameNoFile,
              &AbstractImporterTest::animationNameNotImplemented,
              &AbstractImporterTest::animationNameNoFile,
              &AbstractImporterTest::animationNameOutOfRange,
              &AbstractImporterTest::animationNotImplemented,
              &AbstractImporterTest::animationNoFile,
              &AbstractImporterTest::animationOutOfRange,

              &AbstractImporterTest::light,
              &AbstractImporterTest::lightCountNotImplemented,
              &AbstractImporterTest::lightCountNoFile,
              &AbstractImporterTest::lightForNameNotImplemented,
              &AbstractImporterTest::lightForNameNoFile,
              &AbstractImporterTest::lightNameNotImplemented,
              &AbstractImporterTest::lightNameNoFile,
              &AbstractImporterTest::lightNameOutOfRange,
              &AbstractImporterTest::lightNotImplemented,
              &AbstractImporterTest::lightNoFile,
              &AbstractImporterTest::lightOutOfRange,

              &AbstractImporterTest::camera,
              &AbstractImporterTest::cameraCountNotImplemented,
              &AbstractImporterTest::cameraCountNoFile,
              &AbstractImporterTest::cameraForNameNotImplemented,
              &AbstractImporterTest::cameraForNameNoFile,
              &AbstractImporterTest::cameraNameNotImplemented,
              &AbstractImporterTest::cameraNameNoFile,
              &AbstractImporterTest::cameraNameOutOfRange,
              &AbstractImporterTest::cameraNotImplemented,
              &AbstractImporterTest::cameraNoFile,
              &AbstractImporterTest::cameraOutOfRange,

              &AbstractImporterTest::object2D,
              &AbstractImporterTest::object2DCountNotImplemented,
              &AbstractImporterTest::object2DCountNoFile,
              &AbstractImporterTest::object2DForNameNotImplemented,
              &AbstractImporterTest::object2DForNameNoFile,
              &AbstractImporterTest::object2DNameNotImplemented,
              &AbstractImporterTest::object2DNameNoFile,
              &AbstractImporterTest::object2DNameOutOfRange,
              &AbstractImporterTest::object2DNotImplemented,
              &AbstractImporterTest::object2DNoFile,
              &AbstractImporterTest::object2DOutOfRange,

              &AbstractImporterTest::object3D,
              &AbstractImporterTest::object3DCountNotImplemented,
              &AbstractImporterTest::object3DCountNoFile,
              &AbstractImporterTest::object3DForNameNotImplemented,
              &AbstractImporterTest::object3DForNameNoFile,
              &AbstractImporterTest::object3DNameNotImplemented,
              &AbstractImporterTest::object3DNameNoFile,
              &AbstractImporterTest::object3DNameOutOfRange,
              &AbstractImporterTest::object3DNotImplemented,
              &AbstractImporterTest::object3DNoFile,
              &AbstractImporterTest::object3DOutOfRange,

              &AbstractImporterTest::mesh2D,
              &AbstractImporterTest::mesh2DCountNotImplemented,
              &AbstractImporterTest::mesh2DCountNoFile,
              &AbstractImporterTest::mesh2DForNameNotImplemented,
              &AbstractImporterTest::mesh2DForNameNoFile,
              &AbstractImporterTest::mesh2DNameNotImplemented,
              &AbstractImporterTest::mesh2DNameNoFile,
              &AbstractImporterTest::mesh2DNameOutOfRange,
              &AbstractImporterTest::mesh2DNotImplemented,
              &AbstractImporterTest::mesh2DNoFile,
              &AbstractImporterTest::mesh2DOutOfRange,

              &AbstractImporterTest::mesh3D,
              &AbstractImporterTest::mesh3DCountNotImplemented,
              &AbstractImporterTest::mesh3DCountNoFile,
              &AbstractImporterTest::mesh3DForNameNotImplemented,
              &AbstractImporterTest::mesh3DForNameNoFile,
              &AbstractImporterTest::mesh3DNameNotImplemented,
              &AbstractImporterTest::mesh3DNameNoFile,
              &AbstractImporterTest::mesh3DNameOutOfRange,
              &AbstractImporterTest::mesh3DNotImplemented,
              &AbstractImporterTest::mesh3DNoFile,
              &AbstractImporterTest::mesh3DOutOfRange,

              &AbstractImporterTest::material,
              &AbstractImporterTest::materialCountNotImplemented,
              &AbstractImporterTest::materialCountNoFile,
              &AbstractImporterTest::materialForNameNotImplemented,
              &AbstractImporterTest::materialForNameNoFile,
              &AbstractImporterTest::materialNameNotImplemented,
              &AbstractImporterTest::materialNameNoFile,
              &AbstractImporterTest::materialNameOutOfRange,
              &AbstractImporterTest::materialNotImplemented,
              &AbstractImporterTest::materialNoFile,
              &AbstractImporterTest::materialOutOfRange,

              &AbstractImporterTest::texture,
              &AbstractImporterTest::textureCountNotImplemented,
              &AbstractImporterTest::textureCountNoFile,
              &AbstractImporterTest::textureForNameNotImplemented,
              &AbstractImporterTest::textureForNameNoFile,
              &AbstractImporterTest::textureNameNotImplemented,
              &AbstractImporterTest::textureNameNoFile,
              &AbstractImporterTest::textureNameOutOfRange,
              &AbstractImporterTest::textureNotImplemented,
              &AbstractImporterTest::textureNoFile,
              &AbstractImporterTest::textureOutOfRange,

              &AbstractImporterTest::image1D,
              &AbstractImporterTest::image1DCountNotImplemented,
              &AbstractImporterTest::image1DCountNoFile,
              &AbstractImporterTest::image1DForNameNotImplemented,
              &AbstractImporterTest::image1DForNameNoFile,
              &AbstractImporterTest::image1DNameNotImplemented,
              &AbstractImporterTest::image1DNameNoFile,
              &AbstractImporterTest::image1DNameOutOfRange,
              &AbstractImporterTest::image1DNotImplemented,
              &AbstractImporterTest::image1DNoFile,
              &AbstractImporterTest::image1DOutOfRange,

              &AbstractImporterTest::image2D,
              &AbstractImporterTest::image2DCountNotImplemented,
              &AbstractImporterTest::image2DCountNoFile,
              &AbstractImporterTest::image2DForNameNotImplemented,
              &AbstractImporterTest::image2DForNameNoFile,
              &AbstractImporterTest::image2DNameNotImplemented,
              &AbstractImporterTest::image2DNameNoFile,
              &AbstractImporterTest::image2DNameOutOfRange,
              &AbstractImporterTest::image2DNotImplemented,
              &AbstractImporterTest::image2DNoFile,
              &AbstractImporterTest::image2DOutOfRange,

              &AbstractImporterTest::image3D,
              &AbstractImporterTest::image3DCountNotImplemented,
              &AbstractImporterTest::image3DCountNoFile,
              &AbstractImporterTest::image3DForNameNotImplemented,
              &AbstractImporterTest::image3DForNameNoFile,
              &AbstractImporterTest::image3DNameNotImplemented,
              &AbstractImporterTest::image3DNameNoFile,
              &AbstractImporterTest::image3DNameOutOfRange,
              &AbstractImporterTest::image3DNotImplemented,
              &AbstractImporterTest::image3DNoFile,
              &AbstractImporterTest::image3DOutOfRange,

              &AbstractImporterTest::importerState,
              &AbstractImporterTest::importerStateNotImplemented,
              &AbstractImporterTest::importerStateNoFile,

              &AbstractImporterTest::debugFeature,
              &AbstractImporterTest::debugFeatures});
}

void AbstractImporterTest::construct() {
    class Importer: public AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.features(), AbstractImporter::Features{});
    CORRADE_VERIFY(!importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::constructWithPluginManagerReference() {
    class Importer: public AbstractImporter {
        public:
            explicit Importer(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

        private:
            Features doFeatures() const override { return {}; }
            bool doIsOpened() const override { return false; }
            void doClose() override {}
    };

    PluginManager::Manager<AbstractImporter> manager;
    Importer importer{manager};
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::openFileAsData() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xa5');
        }

        bool _opened = false;
    };

    /* doOpenFile() should call doOpenData() */
    Importer importer;
    CORRADE_VERIFY(!importer.isOpened());
    importer.openFile(Utility::Directory::join(TRADE_TEST_DIR, "file.bin"));
    CORRADE_VERIFY(importer.isOpened());

    importer.close();
    CORRADE_VERIFY(!importer.isOpened());
}

void AbstractImporterTest::openFileAsDataNotFound() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenData(Containers::ArrayView<const char>) override {
            _opened = true;
        }

        bool _opened = false;
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.openFile("nonexistent.bin");
    CORRADE_VERIFY(!importer.isOpened());
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): cannot open file nonexistent.bin\n");
}

void AbstractImporterTest::openFileNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.openFile("file.dat");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): not implemented\n");
}

void AbstractImporterTest::openDataNotSupported() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.openData(nullptr);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openData(): feature not supported\n");
}

void AbstractImporterTest::openDataNotImplemented() {
    class DataImporter: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    DataImporter importer;
    importer.openData(nullptr);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openData(): feature advertised but not implemented\n");
}

void AbstractImporterTest::openStateNotSupported() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.openState(nullptr);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openState(): feature not supported\n");
}

void AbstractImporterTest::openStateNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenState; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.openState(nullptr);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openState(): feature advertised but not implemented\n");
}

void AbstractImporterTest::setFileCallback() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenData|Feature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            *static_cast<int*>(userData) = 1337;
        }
    };

    int a = 0;
    Importer importer;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    importer.setFileCallback(lambda, &a);
    CORRADE_COMPARE(importer.fileCallback(), lambda);
    CORRADE_COMPARE(importer.fileCallbackUserData(), &a);
    CORRADE_COMPARE(a, 1337);
}

void AbstractImporterTest::setFileCallbackTemplate() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenData|Feature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*), void*) override {
            called = true;
        }

        public: bool called = false;
    };

    int a = 0;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, int&) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    Importer importer;
    importer.setFileCallback(lambda, a);
    CORRADE_VERIFY(importer.fileCallback());
    CORRADE_VERIFY(importer.fileCallbackUserData());
    CORRADE_VERIFY(importer.called);

    /* The data pointers should be wrapped, thus not the same */
    CORRADE_VERIFY(reinterpret_cast<void(*)()>(importer.fileCallback()) != reinterpret_cast<void(*)()>(static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, int&)>(lambda)));
    CORRADE_VERIFY(importer.fileCallbackUserData() != &a);
}

void AbstractImporterTest::setFileCallbackTemplateNull() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenData|Feature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
        void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData) override {
            called = !callback && !userData;
        }

        public: bool called = false;
    };

    int a = 0;
    Importer importer;
    importer.setFileCallback(static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, int&)>(nullptr), a);
    CORRADE_VERIFY(!importer.fileCallback());
    CORRADE_VERIFY(!importer.fileCallbackUserData());
    CORRADE_VERIFY(importer.called);
}

void AbstractImporterTest::setFileCallbackFileOpened() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::setFileCallback(): can't be set while a file is opened\n");
}

void AbstractImporterTest::setFileCallbackNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::FileCallback; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    int a;
    auto lambda = [](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    };
    Importer importer;
    importer.setFileCallback(lambda, &a);
    CORRADE_COMPARE(importer.fileCallback(), lambda);
    CORRADE_COMPARE(importer.fileCallbackUserData(), &a);
    /* Should just work, no need to implement the function */
}

void AbstractImporterTest::setFileCallbackNotSupported() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    int a;
    Importer importer;
    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    }, &a);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::setFileCallback(): importer supports neither loading from data nor via callbacks, callbacks can't be used\n");
}

void AbstractImporterTest::setFileCallbackOpenFileDirectly() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::FileCallback|Feature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenFile(const std::string& filename) override {
            /* Called because FileCallback is supported */
            _opened = filename == "file.dat" && fileCallback() && fileCallbackUserData();
        }

        void doOpenData(Containers::ArrayView<const char>) override {
            /* Shouldn't be called because FileCallback is supported */
            openDataCalledNotSureWhy = true;
        }

        bool _opened = false;
        public: bool openDataCalledNotSureWhy = false;
    };

    bool calledNotSureWhy = false;
    Importer importer;
    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, bool& calledNotSureWhy) -> Containers::Optional<Containers::ArrayView<const char>> {
        calledNotSureWhy = true;
        return {};
    }, calledNotSureWhy);

    CORRADE_VERIFY(importer.openFile("file.dat"));
    CORRADE_VERIFY(!calledNotSureWhy);
    CORRADE_VERIFY(!importer.openDataCalledNotSureWhy);
}

void AbstractImporterTest::setFileCallbackOpenFileThroughBaseImplementation() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::FileCallback|Feature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenFile(const std::string& filename) override {
            openFileCalled = filename == "file.dat" && fileCallback() && fileCallbackUserData();
            AbstractImporter::doOpenFile(filename);
        }

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xb0');
        }

        bool _opened = false;
        public: bool openFileCalled = false;
    };

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
        bool calledNotSureWhy = false;
    } state;
    Importer importer;
    importer.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        state.calledNotSureWhy = true;
        return {};
    }, state);

    CORRADE_VERIFY(importer.openFile("file.dat"));
    CORRADE_VERIFY(importer.openFileCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_VERIFY(!state.calledNotSureWhy);
}

void AbstractImporterTest::setFileCallbackOpenFileThroughBaseImplementationFailed() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::FileCallback|Feature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenFile(const std::string& filename) override {
            openFileCalled = true;
            AbstractImporter::doOpenFile(filename);
        }

        public: bool openFileCalled = false;
    };

    Importer importer;
    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) -> Containers::Optional<Containers::ArrayView<const char>> {
        return {};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("file.dat"));
    CORRADE_VERIFY(importer.openFileCalled);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): cannot open file file.dat\n");
}

void AbstractImporterTest::setFileCallbackOpenFileAsData() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        void doOpenFile(const std::string&) override {
            openFileCalled = true;
        }

        void doOpenData(Containers::ArrayView<const char> data) override {
            _opened = (data.size() == 1 && data[0] == '\xb0');
        }

        bool _opened = false;
        public: bool openFileCalled = false;
    };

    struct State {
        const char data = '\xb0';
        bool loaded = false;
        bool closed = false;
        bool calledNotSureWhy = false;
    } state;
    Importer importer;
    importer.setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy, State& state) -> Containers::Optional<Containers::ArrayView<const char>> {
        if(filename == "file.dat" && policy == InputFileCallbackPolicy::LoadTemporary) {
            state.loaded = true;
            return Containers::arrayView(&state.data, 1);
        }

        if(filename == "file.dat" && policy == InputFileCallbackPolicy::Close) {
            state.closed = true;
            return {};
        }

        state.calledNotSureWhy = true;
        return {};
    }, state);

    CORRADE_VERIFY(importer.openFile("file.dat"));
    CORRADE_VERIFY(!importer.openFileCalled);
    CORRADE_VERIFY(state.loaded);
    CORRADE_VERIFY(state.closed);
    CORRADE_VERIFY(!state.calledNotSureWhy);
}

void AbstractImporterTest::setFileCallbackOpenFileAsDataFailed() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        void doOpenFile(const std::string&) override {
            openFileCalled = true;
        }

        public: bool openFileCalled = false;
    };

    Importer importer;
    importer.setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
        return Containers::Optional<Containers::ArrayView<const char>>{};
    });

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!importer.openFile("file.dat"));
    CORRADE_VERIFY(!importer.openFileCalled);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): cannot open file file.dat\n");
}

void AbstractImporterTest::defaultScene() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        Int doDefaultScene() override { return 42; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.defaultScene(), 42);
}

void AbstractImporterTest::defaultSceneNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.defaultScene(), -1);
}

void AbstractImporterTest::defaultSceneNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.defaultScene();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::defaultScene(): no file opened\n");
}

int state;

void AbstractImporterTest::scene() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
        Int doSceneForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doSceneName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<SceneData> doScene(UnsignedInt id) override {
            if(id == 7) return SceneData{{}, {}, &state};
            else return SceneData{{}, {}};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.sceneCount(), 8);
    CORRADE_COMPARE(importer.sceneForName("eighth"), 7);
    CORRADE_COMPARE(importer.sceneName(7), "eighth");

    auto data = importer.scene(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::sceneCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.sceneCount(), 0);
}

void AbstractImporterTest::sceneCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.sceneCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::sceneCount(): no file opened\n");
}

void AbstractImporterTest::sceneForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.sceneForName(""), -1);
}

void AbstractImporterTest::sceneForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.sceneForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::sceneForName(): no file opened\n");
}

void AbstractImporterTest::sceneNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.sceneName(7), "");
}

void AbstractImporterTest::sceneNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.sceneName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::sceneName(): no file opened\n");
}

void AbstractImporterTest::sceneNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.sceneName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::sceneName(): index out of range\n");
}

void AbstractImporterTest::sceneNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doSceneCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.scene(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::scene(): not implemented\n");
}

void AbstractImporterTest::sceneNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.scene(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::scene(): no file opened\n");
}

void AbstractImporterTest::sceneOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.scene(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::scene(): index out of range\n");
}

void AbstractImporterTest::animation() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
        Int doAnimationForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doAnimationName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<AnimationData> doAnimation(UnsignedInt id) override {
            if(id == 7) return AnimationData{{}, {}, &state};
            else return AnimationData{{}, {}};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.animationCount(), 8);
    CORRADE_COMPARE(importer.animationForName("eighth"), 7);
    CORRADE_COMPARE(importer.animationName(7), "eighth");

    auto data = importer.animation(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::animationCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.animationCount(), 0);
}

void AbstractImporterTest::animationCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.animationCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animationCount(): no file opened\n");
}

void AbstractImporterTest::animationForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.animationForName(""), -1);
}

void AbstractImporterTest::animationForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.animationForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animationForName(): no file opened\n");
}

void AbstractImporterTest::animationNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.animationName(7), "");
}

void AbstractImporterTest::animationNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.animationName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animationName(): no file opened\n");
}

void AbstractImporterTest::animationNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.animationName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animationName(): index out of range\n");
}

void AbstractImporterTest::animationNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doAnimationCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.animation(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animation(): not implemented\n");
}

void AbstractImporterTest::animationNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.animation(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animation(): no file opened\n");
}

void AbstractImporterTest::animationOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.animation(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::animation(): index out of range\n");
}

void AbstractImporterTest::light() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
        Int doLightForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doLightName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<LightData> doLight(UnsignedInt id) override {
            if(id == 7) return LightData{{}, {}, {}, &state};
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.lightCount(), 8);
    CORRADE_COMPARE(importer.lightForName("eighth"), 7);
    CORRADE_COMPARE(importer.lightName(7), "eighth");

    auto data = importer.light(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::lightCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.lightCount(), 0);
}

void AbstractImporterTest::lightCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.lightCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::lightCount(): no file opened\n");
}

void AbstractImporterTest::lightForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.lightForName(""), -1);
}

void AbstractImporterTest::lightForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.lightForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::lightForName(): no file opened\n");
}

void AbstractImporterTest::lightNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.lightName(7), "");
}

void AbstractImporterTest::lightNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.lightName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::lightName(): no file opened\n");
}

void AbstractImporterTest::lightNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.lightName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::lightName(): index out of range\n");
}

void AbstractImporterTest::lightNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doLightCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.light(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::light(): not implemented\n");
}

void AbstractImporterTest::lightNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.light(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::light(): no file opened\n");
}

void AbstractImporterTest::lightOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.light(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::light(): index out of range\n");
}

void AbstractImporterTest::camera() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
        Int doCameraForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doCameraName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<CameraData> doCamera(UnsignedInt id) override {
            if(id == 7) return CameraData{{}, Vector2{}, {}, {}, &state};
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.cameraCount(), 8);
    CORRADE_COMPARE(importer.cameraForName("eighth"), 7);
    CORRADE_COMPARE(importer.cameraName(7), "eighth");

    auto data = importer.camera(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::cameraCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.cameraCount(), 0);
}

void AbstractImporterTest::cameraCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.cameraCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::cameraCount(): no file opened\n");
}

void AbstractImporterTest::cameraForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.cameraForName(""), -1);
}

void AbstractImporterTest::cameraForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.cameraForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::cameraForName(): no file opened\n");
}

void AbstractImporterTest::cameraNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.cameraName(7), "");
}

void AbstractImporterTest::cameraNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.cameraName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::cameraName(): no file opened\n");
}

void AbstractImporterTest::cameraNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.cameraName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::cameraName(): index out of range\n");
}

void AbstractImporterTest::cameraNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doCameraCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.camera(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::camera(): not implemented\n");
}

void AbstractImporterTest::cameraNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.camera(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::camera(): no file opened\n");
}

void AbstractImporterTest::cameraOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.camera(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::camera(): index out of range\n");
}

void AbstractImporterTest::object2D() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
        Int doObject2DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doObject2DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Pointer<ObjectData2D> doObject2D(UnsignedInt id) override {
            if(id == 7) return Containers::pointer(new ObjectData2D{{}, {}, &state});
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.object2DCount(), 8);
    CORRADE_COMPARE(importer.object2DForName("eighth"), 7);
    CORRADE_COMPARE(importer.object2DName(7), "eighth");

    auto data = importer.object2D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::object2DCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.object2DCount(), 0);
}

void AbstractImporterTest::object2DCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object2DCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DCount(): no file opened\n");
}

void AbstractImporterTest::object2DForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.object2DForName(""), -1);
}

void AbstractImporterTest::object2DForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object2DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DForName(): no file opened\n");
}

void AbstractImporterTest::object2DNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.object2DName(7), "");
}

void AbstractImporterTest::object2DNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object2DName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DName(): no file opened\n");
}

void AbstractImporterTest::object2DNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object2DName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2DName(): index out of range\n");
}

void AbstractImporterTest::object2DNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject2DCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object2D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2D(): not implemented\n");
}

void AbstractImporterTest::object2DNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object2D(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2D(): no file opened\n");
}

void AbstractImporterTest::object2DOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object2D(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object2D(): index out of range\n");
}

void AbstractImporterTest::object3D() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
        Int doObject3DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doObject3DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Pointer<ObjectData3D> doObject3D(UnsignedInt id) override {
            if(id == 7) return Containers::pointer(new ObjectData3D{{}, {}, &state});
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.object3DCount(), 8);
    CORRADE_COMPARE(importer.object3DForName("eighth"), 7);
    CORRADE_COMPARE(importer.object3DName(7), "eighth");

    auto data = importer.object3D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::object3DCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.object3DCount(), 0);
}

void AbstractImporterTest::object3DCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object3DCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3DCount(): no file opened\n");
}

void AbstractImporterTest::object3DForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.object3DForName(""), -1);
}

void AbstractImporterTest::object3DForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object3DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3DForName(): no file opened\n");
}

void AbstractImporterTest::object3DNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.object3DName(7), "");
}

void AbstractImporterTest::object3DNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object3DName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3DName(): no file opened\n");
}

void AbstractImporterTest::object3DNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object3DName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3DName(): index out of range\n");
}

void AbstractImporterTest::object3DNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doObject3DCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object3D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3D(): not implemented\n");
}

void AbstractImporterTest::object3DNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object3D(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3D(): no file opened\n");
}

void AbstractImporterTest::object3DOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.object3D(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::object3D(): index out of range\n");
}

void AbstractImporterTest::mesh2D() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh2DCount() const override { return 8; }
        Int doMesh2DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doMesh2DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<MeshData2D> doMesh2D(UnsignedInt id) override {
            if(id == 7) return MeshData2D{{}, {}, {{}}, {}, {}, &state};
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.mesh2DCount(), 8);
    CORRADE_COMPARE(importer.mesh2DForName("eighth"), 7);
    CORRADE_COMPARE(importer.mesh2DName(7), "eighth");

    auto data = importer.mesh2D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::mesh2DCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.mesh2DCount(), 0);
}

void AbstractImporterTest::mesh2DCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh2DCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2DCount(): no file opened\n");
}

void AbstractImporterTest::mesh2DForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.mesh2DForName(""), -1);
}

void AbstractImporterTest::mesh2DForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh2DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2DForName(): no file opened\n");
}

void AbstractImporterTest::mesh2DNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh2DCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.mesh2DName(7), "");
}

void AbstractImporterTest::mesh2DNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh2DName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2DName(): no file opened\n");
}

void AbstractImporterTest::mesh2DNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh2DName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2DName(): index out of range\n");
}

void AbstractImporterTest::mesh2DNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh2DCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh2D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2D(): not implemented\n");
}

void AbstractImporterTest::mesh2DNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh2D(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2D(): no file opened\n");
}

void AbstractImporterTest::mesh2DOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh2D(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh2D(): index out of range\n");
}

void AbstractImporterTest::mesh3D() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh3DCount() const override { return 8; }
        Int doMesh3DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doMesh3DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<MeshData3D> doMesh3D(UnsignedInt id) override {
            if(id == 7) return MeshData3D{{}, {}, {{}}, {}, {}, {}, &state};
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.mesh3DCount(), 8);
    CORRADE_COMPARE(importer.mesh3DForName("eighth"), 7);
    CORRADE_COMPARE(importer.mesh3DName(7), "eighth");

    auto data = importer.mesh3D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::mesh3DCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.mesh3DCount(), 0);
}

void AbstractImporterTest::mesh3DCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh3DCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3DCount(): no file opened\n");
}

void AbstractImporterTest::mesh3DForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.mesh3DForName(""), -1);
}

void AbstractImporterTest::mesh3DForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh3DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3DForName(): no file opened\n");
}

void AbstractImporterTest::mesh3DNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh3DCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.mesh3DName(7), "");
}

void AbstractImporterTest::mesh3DNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh3DName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3DName(): no file opened\n");
}

void AbstractImporterTest::mesh3DNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh3DName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3DName(): index out of range\n");
}

void AbstractImporterTest::mesh3DNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMesh3DCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh3D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3D(): not implemented\n");
}

void AbstractImporterTest::mesh3DNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh3D(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3D(): no file opened\n");
}

void AbstractImporterTest::mesh3DOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.mesh3D(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::mesh3D(): index out of range\n");
}

void AbstractImporterTest::material() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
        Int doMaterialForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doMaterialName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Pointer<AbstractMaterialData> doMaterial(UnsignedInt id) override {
            if(id == 7) return Containers::pointer(new PhongMaterialData{{}, {}, {}, {}, &state});
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.materialCount(), 8);
    CORRADE_COMPARE(importer.materialForName("eighth"), 7);
    CORRADE_COMPARE(importer.materialName(7), "eighth");

    auto data = importer.material(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::materialCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.materialCount(), 0);
}

void AbstractImporterTest::materialCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.materialCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::materialCount(): no file opened\n");
}

void AbstractImporterTest::materialForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.materialForName(""), -1);
}

void AbstractImporterTest::materialForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.materialForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::materialForName(): no file opened\n");
}

void AbstractImporterTest::materialNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.materialName(7), "");
}

void AbstractImporterTest::materialNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.materialName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::materialName(): no file opened\n");
}

void AbstractImporterTest::materialNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.materialName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::materialName(): index out of range\n");
}

void AbstractImporterTest::materialNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doMaterialCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.material(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::material(): not implemented\n");
}

void AbstractImporterTest::materialNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.material(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::material(): no file opened\n");
}

void AbstractImporterTest::materialOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.material(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::material(): index out of range\n");
}

void AbstractImporterTest::texture() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
        Int doTextureForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doTextureName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<TextureData> doTexture(UnsignedInt id) override {
            if(id == 7) return TextureData{{}, {}, {}, {}, {}, {}, &state};
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.textureCount(), 8);
    CORRADE_COMPARE(importer.textureForName("eighth"), 7);
    CORRADE_COMPARE(importer.textureName(7), "eighth");

    auto data = importer.texture(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::textureCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.textureCount(), 0);
}

void AbstractImporterTest::textureCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.textureCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::textureCount(): no file opened\n");
}

void AbstractImporterTest::textureForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.textureForName(""), -1);
}

void AbstractImporterTest::textureForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.textureForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::textureForName(): no file opened\n");
}

void AbstractImporterTest::textureNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.textureName(7), "");
}

void AbstractImporterTest::textureNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.textureName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::textureName(): no file opened\n");
}

void AbstractImporterTest::textureNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.textureName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::textureName(): index out of range\n");
}

void AbstractImporterTest::textureNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doTextureCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.texture(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::texture(): not implemented\n");
}

void AbstractImporterTest::textureNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.texture(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::texture(): no file opened\n");
}

void AbstractImporterTest::textureOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.texture(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::texture(): index out of range\n");
}

void AbstractImporterTest::image1D() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
        Int doImage1DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doImage1DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<ImageData1D> doImage1D(UnsignedInt id) override {
            if(id == 7) return ImageData1D{PixelStorage{}, {}, {}, {}, &state};
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.image1DCount(), 8);
    CORRADE_COMPARE(importer.image1DForName("eighth"), 7);
    CORRADE_COMPARE(importer.image1DName(7), "eighth");

    auto data = importer.image1D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::image1DCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.image1DCount(), 0);
}

void AbstractImporterTest::image1DCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image1DCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1DCount(): no file opened\n");
}

void AbstractImporterTest::image1DForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.image1DForName(""), -1);
}

void AbstractImporterTest::image1DForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image1DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1DForName(): no file opened\n");
}

void AbstractImporterTest::image1DNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.image1DName(7), "");
}

void AbstractImporterTest::image1DNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image1DName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1DName(): no file opened\n");
}

void AbstractImporterTest::image1DNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image1DName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1DName(): index out of range\n");
}

void AbstractImporterTest::image1DNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage1DCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image1D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1D(): not implemented\n");
}

void AbstractImporterTest::image1DNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image1D(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1D(): no file opened\n");
}

void AbstractImporterTest::image1DOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image1D(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image1D(): index out of range\n");
}

void AbstractImporterTest::image2D() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
        Int doImage2DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doImage2DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<ImageData2D> doImage2D(UnsignedInt id) override {
            if(id == 7) return ImageData2D{PixelStorage{}, {}, {}, {}, &state};
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.image2DCount(), 8);
    CORRADE_COMPARE(importer.image2DForName("eighth"), 7);
    CORRADE_COMPARE(importer.image2DName(7), "eighth");

    auto data = importer.image2D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::image2DCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.image2DCount(), 0);
}

void AbstractImporterTest::image2DCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image2DCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2DCount(): no file opened\n");
}

void AbstractImporterTest::image2DForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.image2DForName(""), -1);
}

void AbstractImporterTest::image2DForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image2DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2DForName(): no file opened\n");
}

void AbstractImporterTest::image2DNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.image2DName(7), "");
}

void AbstractImporterTest::image2DNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image2DName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2DName(): no file opened\n");
}

void AbstractImporterTest::image2DNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image2DName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2DName(): index out of range\n");
}

void AbstractImporterTest::image2DNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage2DCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image2D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2D(): not implemented\n");
}

void AbstractImporterTest::image2DNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image2D(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2D(): no file opened\n");
}

void AbstractImporterTest::image2DOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image2D(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image2D(): index out of range\n");
}

void AbstractImporterTest::image3D() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
        Int doImage3DForName(const std::string& name) override {
            if(name == "eighth") return 7;
            else return -1;
        }
        std::string doImage3DName(UnsignedInt id) override {
            if(id == 7) return "eighth";
            else return {};
        }
        Containers::Optional<ImageData3D> doImage3D(UnsignedInt id) override {
            if(id == 7) return ImageData3D{PixelStorage{}, {}, {}, {}, &state};
            else return {};
        }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    CORRADE_COMPARE(importer.image3DCount(), 8);
    CORRADE_COMPARE(importer.image3DForName("eighth"), 7);
    CORRADE_COMPARE(importer.image3DName(7), "eighth");

    auto data = importer.image3D(7);
    CORRADE_VERIFY(data);
    CORRADE_COMPARE(data->importerState(), &state);
}

void AbstractImporterTest::image3DCountNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.image3DCount(), 0);
}

void AbstractImporterTest::image3DCountNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image3DCount();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3DCount(): no file opened\n");
}

void AbstractImporterTest::image3DForNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.image3DForName(""), -1);
}

void AbstractImporterTest::image3DForNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image3DForName("");
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3DForName(): no file opened\n");
}

void AbstractImporterTest::image3DNameNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.image3DName(7), "");
}

void AbstractImporterTest::image3DNameNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image3DName(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3DName(): no file opened\n");
}

void AbstractImporterTest::image3DNameOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image3DName(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3DName(): index out of range\n");
}

void AbstractImporterTest::image3DNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doImage3DCount() const override { return 8; }
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image3D(7);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3D(): not implemented\n");
}

void AbstractImporterTest::image3DNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image3D(42);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3D(): no file opened\n");
}

void AbstractImporterTest::image3DOutOfRange() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.image3D(0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::image3D(): index out of range\n");
}

void AbstractImporterTest::importerState() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        const void* doImporterState() const override { return &state; }
    };

    Importer importer;
    CORRADE_COMPARE(importer.importerState(), &state);
}

void AbstractImporterTest::importerStateNotImplemented() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}
    };

    Importer importer;
    CORRADE_COMPARE(importer.importerState(), nullptr);
}

void AbstractImporterTest::importerStateNoFile() {
    class Importer: public Trade::AbstractImporter {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}
    };

    std::ostringstream out;
    Error redirectError{&out};

    Importer importer;
    importer.importerState();
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::importerState(): no file opened\n");
}

void AbstractImporterTest::debugFeature() {
    std::ostringstream out;

    Debug{&out} << AbstractImporter::Feature::OpenData << AbstractImporter::Feature(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::Feature::OpenData Trade::AbstractImporter::Feature(0xf0)\n");
}

void AbstractImporterTest::debugFeatures() {
    std::ostringstream out;

    Debug{&out} << (AbstractImporter::Feature::OpenData|AbstractImporter::Feature::OpenState) << AbstractImporter::Features{};
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::Feature::OpenData|Trade::AbstractImporter::Feature::OpenState Trade::AbstractImporter::Features{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractImporterTest)
