/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test {

class AbstractImporterTest: public TestSuite::Tester {
    public:
        explicit AbstractImporterTest();

        void openFile();

        void debugFeature();
        void debugFeatures();
};

AbstractImporterTest::AbstractImporterTest() {
    addTests({&AbstractImporterTest::openFile,

              &AbstractImporterTest::debugFeature,
              &AbstractImporterTest::debugFeatures});
}

void AbstractImporterTest::openFile() {
    class DataImporter: public Trade::AbstractImporter {
        public:
            explicit DataImporter(): opened(false) {}

        private:
            Features doFeatures() const override { return Feature::OpenData; }
            bool doIsOpened() const override { return opened; }
            void doClose() override {}

            void doOpenData(Containers::ArrayView<const char> data) override {
                opened = (data.size() == 1 && data[0] == '\xa5');
            }

            bool opened;
    };

    /* doOpenFile() should call doOpenData() */
    DataImporter importer;
    CORRADE_VERIFY(!importer.isOpened());
    importer.openFile(Utility::Directory::join(TRADE_TEST_DIR, "file.bin"));
    CORRADE_VERIFY(importer.isOpened());
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

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractImporterTest)
