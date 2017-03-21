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
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Magnum/Trade/ImageData.h>

#include "MagnumPlugins/AnyImageImporter/AnyImageImporter.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test {

struct AnyImageImporterTest: TestSuite::Tester {
    explicit AnyImageImporterTest();

    void tga();
    void jpeg();
    void png();

    void unknown();

    private:
        PluginManager::Manager<AbstractImporter> _manager;
};

AnyImageImporterTest::AnyImageImporterTest(): _manager{MAGNUM_PLUGINS_IMPORTER_DIR} {
    addTests({&AnyImageImporterTest::tga,
              &AnyImageImporterTest::jpeg,
              &AnyImageImporterTest::png,

              &AnyImageImporterTest::unknown});
}

void AnyImageImporterTest::tga() {
    if(_manager.loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("TgaImporter plugin not found, cannot test");

    AnyImageImporter importer{_manager};
    CORRADE_VERIFY(importer.openFile(TGA_FILE));

    /* Check only size, as it is good enough proof that it is working */
    std::optional<ImageData2D> image = importer.image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
}

void AnyImageImporterTest::jpeg() {
    if(_manager.loadState("JpegImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("JpegImporter plugin not found, cannot test");

    AnyImageImporter importer{_manager};
    CORRADE_VERIFY(importer.openFile(JPEG_FILE));

    /* Check only size, as it is good enough proof that it is working */
    std::optional<ImageData2D> image = importer.image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));
}

void AnyImageImporterTest::png() {
    if(_manager.loadState("PngImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("PngImporter plugin not found, cannot test");

    AnyImageImporter importer{_manager};
    CORRADE_VERIFY(importer.openFile(PNG_FILE));

    /* Check only size, as it is good enough proof that it is working */
    std::optional<ImageData2D> image = importer.image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));
}

void AnyImageImporterTest::unknown() {
    std::ostringstream output;
    Error redirectError{&output};

    AnyImageImporter importer{_manager};
    CORRADE_VERIFY(!importer.openFile("image.xcf"));

    CORRADE_COMPARE(output.str(), "Trade::AnyImageImporter::openFile(): cannot determine type of file image.xcf\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnyImageImporterTest)
