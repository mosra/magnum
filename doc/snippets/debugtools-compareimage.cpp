/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/DebugTools/CompareImage.h"

#include "configure.h"

using namespace Magnum;

#define DOXYGEN_IGNORE(...) __VA_ARGS__

namespace {

Image2D doProcessing() {
    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
    Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("TgaImporter");
    importer->openFile(Utility::Directory::join(SNIPPETS_DIR, "image2.tga"));
    auto image = importer->image2D(0);
    CORRADE_INTERNAL_ASSERT(image);
    return Image2D{image->storage(), image->format(), image->size(), image->release()};
}

Image2D loadExpectedImage() {
    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
    Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("TgaImporter");
    importer->openFile(Utility::Directory::join(SNIPPETS_DIR, "image1.tga"));
    auto image = importer->image2D(0);
    CORRADE_INTERNAL_ASSERT(image);
    return Image2D{image->storage(), image->format(), image->size(), image->release()};
}

}

struct ProcessingTest: TestSuite::Tester {
    explicit ProcessingTest();

    void process();
};

ProcessingTest::ProcessingTest() {
    addTests({&ProcessingTest::process});

    if(false) {
/* [basic] */
Image2D actual{DOXYGEN_IGNORE(doProcessing())}, expected{DOXYGEN_IGNORE(loadExpectedImage())};
CORRADE_COMPARE_AS(actual, expected, DebugTools::CompareImage);
/* [basic] */
    }
}

void ProcessingTest::process() {
    Image2D actual = doProcessing();
    Image2D expected = loadExpectedImage();
/* [delta] */
CORRADE_COMPARE_WITH(actual, expected, (DebugTools::CompareImage{170.0f, 96.0f}));
/* [delta] */
}

CORRADE_TEST_MAIN(ProcessingTest)

