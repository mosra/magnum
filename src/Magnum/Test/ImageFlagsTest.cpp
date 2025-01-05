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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/ImageFlags.h"

namespace Magnum { namespace Test { namespace {

struct ImageFlagsTest: TestSuite::Tester {
    explicit ImageFlagsTest();

    void matchingValues();

    void debugFlag1D();
    void debugFlag2D();
    void debugFlag3D();
    void debugFlag1DPacked();
    void debugFlag2DPacked();
    void debugFlag3DPacked();

    void debugFlags1D();
    void debugFlags2D();
    void debugFlags3D();
    void debugFlags1DPacked();
    void debugFlags2DPacked();
    void debugFlags3DPacked();
};

ImageFlagsTest::ImageFlagsTest() {
    addTests({&ImageFlagsTest::matchingValues,

              &ImageFlagsTest::debugFlag1D,
              &ImageFlagsTest::debugFlag2D,
              &ImageFlagsTest::debugFlag3D,
              &ImageFlagsTest::debugFlag1DPacked,
              &ImageFlagsTest::debugFlag2DPacked,
              &ImageFlagsTest::debugFlag3DPacked,

              &ImageFlagsTest::debugFlags1D,
              &ImageFlagsTest::debugFlags2D,
              &ImageFlagsTest::debugFlags3D,
              &ImageFlagsTest::debugFlags1DPacked,
              &ImageFlagsTest::debugFlags2DPacked,
              &ImageFlagsTest::debugFlags3DPacked});
}

void ImageFlagsTest::matchingValues() {
    CORRADE_COMPARE(UnsignedShort(ImageFlag3D::Array), UnsignedShort(ImageFlag2D::Array));
}

void ImageFlagsTest::debugFlag1D() {
    Containers::String out;
    /** @todo use a real flag once it exists */
    Debug{&out} << ImageFlag1D(0xcafe);
    CORRADE_COMPARE(out, "ImageFlag1D(0xcafe)\n");
}

void ImageFlagsTest::debugFlag2D() {
    Containers::String out;
    Debug{&out} << ImageFlag2D::Array << ImageFlag2D(0xcafe);
    CORRADE_COMPARE(out, "ImageFlag2D::Array ImageFlag2D(0xcafe)\n");
}

void ImageFlagsTest::debugFlag3D() {
    Containers::String out;
    Debug{&out} << ImageFlag3D::CubeMap << ImageFlag3D(0xcafe);
    CORRADE_COMPARE(out, "ImageFlag3D::CubeMap ImageFlag3D(0xcafe)\n");
}

void ImageFlagsTest::debugFlag1DPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    /** @todo use real flags once they exist */
    Debug{&out} << Debug::packed << ImageFlag1D(0xcafe) << ImageFlag1D(0xbeef);
    CORRADE_COMPARE(out, "0xcafe ImageFlag1D(0xbeef)\n");
}

void ImageFlagsTest::debugFlag2DPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << ImageFlag2D::Array << Debug::packed << ImageFlag2D(0xcafe) << ImageFlag2D::Array;
    CORRADE_COMPARE(out, "Array 0xcafe ImageFlag2D::Array\n");
}

void ImageFlagsTest::debugFlag3DPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << ImageFlag3D::CubeMap << Debug::packed << ImageFlag3D(0xcafe) << ImageFlag3D::Array;
    CORRADE_COMPARE(out, "CubeMap 0xcafe ImageFlag3D::Array\n");
}

void ImageFlagsTest::debugFlags1D() {
    Containers::String out;
    /** @todo use real flags once they exist */
    Debug{&out} << (ImageFlag1D{}|ImageFlag1D(0xcafe)) << ImageFlags1D{};
    CORRADE_COMPARE(out, "ImageFlag1D(0xcafe) ImageFlags1D{}\n");
}

void ImageFlagsTest::debugFlags2D() {
    Containers::String out;
    Debug{&out} << (ImageFlag2D::Array|ImageFlag2D(0xcafe)) << ImageFlags2D{};
    CORRADE_COMPARE(out, "ImageFlag2D::Array|ImageFlag2D(0xcafe) ImageFlags2D{}\n");
}

void ImageFlagsTest::debugFlags3D() {
    Containers::String out;
    Debug{&out} << (ImageFlag3D::Array|ImageFlag3D::CubeMap|ImageFlag3D(0xcaf0)) << ImageFlags3D{};
    CORRADE_COMPARE(out, "ImageFlag3D::Array|ImageFlag3D::CubeMap|ImageFlag3D(0xcaf0) ImageFlags3D{}\n");
}

void ImageFlagsTest::debugFlags1DPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    /** @todo use real flags once they exist */
    Debug{&out} << Debug::packed << (ImageFlag1D{}|ImageFlag1D(0xcafe)) << Debug::packed << ImageFlags1D{} << ImageFlag1D(0xbeef);
    CORRADE_COMPARE(out, "0xcafe {} ImageFlag1D(0xbeef)\n");
}

void ImageFlagsTest::debugFlags2DPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (ImageFlag2D::Array|ImageFlag2D(0xcaf0)) << Debug::packed << ImageFlags2D{} << ImageFlag2D::Array;
    CORRADE_COMPARE(out, "Array|0xcaf0 {} ImageFlag2D::Array\n");
}

void ImageFlagsTest::debugFlags3DPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (ImageFlag3D::Array|ImageFlag3D::CubeMap |ImageFlag3D(0xcaf0)) << Debug::packed << ImageFlags3D{} << ImageFlag3D::Array;
    CORRADE_COMPARE(out, "Array|CubeMap|0xcaf0 {} ImageFlag3D::Array\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::ImageFlagsTest)
