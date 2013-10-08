/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Containers/Array.h>
#include <TestSuite/Tester.h>
#include <TestSuite/Compare/FileToString.h>
#include <Utility/Directory.h>

#include "ColorFormat.h"
#include "ImageReference.h"
#include "Trade/AbstractImageConverter.h"

#include "testConfigure.h"

namespace Magnum { namespace Trade { namespace Test {

class AbstractImageConverterTest: public TestSuite::Tester {
    public:
        explicit AbstractImageConverterTest();

        void exportToFile();
};

AbstractImageConverterTest::AbstractImageConverterTest() {
    addTests({&AbstractImageConverterTest::exportToFile});
}

void AbstractImageConverterTest::exportToFile() {
    class DataExporter: public Trade::AbstractImageConverter {
        private:
            Features doFeatures() const override { return Feature::ConvertData; }

            Containers::Array<unsigned char> doExportToData(const ImageReference2D& image) const override {
                Containers::Array<unsigned char> out(2);
                out[0] = image.size().x();
                out[1] = image.size().y();
                return out;
            };
    };

    /* Remove previous file */
    Utility::Directory::rm(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"));

    /* doExportToFile() should call doExportToData() */
    DataExporter exporter;
    ImageReference2D image(ColorFormat::RGBA, ColorType::UnsignedByte, {0xfe, 0xed}, nullptr);
    CORRADE_VERIFY(exporter.exportToFile(image, Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out")));
    CORRADE_COMPARE_AS(Utility::Directory::join(TRADE_TEST_OUTPUT_DIR, "image.out"),
        "\xFE\xED", TestSuite::Compare::FileToString);
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AbstractImageConverterTest)
