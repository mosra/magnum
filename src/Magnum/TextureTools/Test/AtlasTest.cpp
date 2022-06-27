/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Math/Range.h"
#include "Magnum/TextureTools/Atlas.h"

namespace Magnum { namespace TextureTools { namespace Test { namespace {

struct AtlasTest: TestSuite::Tester {
    explicit AtlasTest();

    void basic();
    void padding();
    void empty();
    void tooSmall();

    void arrayPowerOfTwoEmpty();
    void arrayPowerOfTwoSingleElement();
    void arrayPowerOfTwoAllSameElements();
    void arrayPowerOfTwoOneLayer();
    void arrayPowerOfTwoMoreLayers();
    void arrayPowerOfTwoWrongLayerSize();
    void arrayPowerOfTwoWrongSize();
};

/* Could make order[15] and then Containers::arraySize(), but then it won't
   work on MSVC2015 and cause overly complicated code elsewhere */
constexpr std::size_t ArrayPowerOfTwoOneLayerImageCount = 15;
const struct {
    const char* name;
    std::size_t order[ArrayPowerOfTwoOneLayerImageCount];
} ArrayPowerOfTwoOneLayerData[]{
    {"sorted",
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
    {"",
        /* Because there are duplicate sizes, the shuffling needs to preserve
           the original order of duplicates to match the output */
        {0, 2, 7, 13, 11, 3, 4, 5, 8, 14, 1, 9, 6, 12, 10}},
};

const struct {
    const char* name;
    Vector2i size;
    const char* message;
} ArrayPowerOfTwoWrongSizeData[]{
    {"non-power-of-two", {128, 127}, "{128, 127}"},
    {"non-square", {128, 256}, "{128, 256}"},
    {"zero", {1024, 0}, "{1024, 0}"}
};

AtlasTest::AtlasTest() {
    addTests({&AtlasTest::basic,
              &AtlasTest::padding,
              &AtlasTest::empty,
              &AtlasTest::tooSmall,

              &AtlasTest::arrayPowerOfTwoEmpty,
              &AtlasTest::arrayPowerOfTwoSingleElement,
              &AtlasTest::arrayPowerOfTwoAllSameElements});

    addInstancedTests({&AtlasTest::arrayPowerOfTwoOneLayer},
        Containers::arraySize(ArrayPowerOfTwoOneLayerData));

    addTests({&AtlasTest::arrayPowerOfTwoMoreLayers});

    addInstancedTests({&AtlasTest::arrayPowerOfTwoWrongLayerSize,
                       &AtlasTest::arrayPowerOfTwoWrongSize},
        Containers::arraySize(ArrayPowerOfTwoWrongSizeData));
}

void AtlasTest::basic() {
    std::vector<Range2Di> atlas = TextureTools::atlas({64, 64}, {
        {12, 18},
        {32, 15},
        {23, 25}
    });

    CORRADE_COMPARE(atlas.size(), 3);
    CORRADE_COMPARE(atlas, (std::vector<Range2Di>{
        Range2Di::fromSize({0, 0}, {12, 18}),
        Range2Di::fromSize({32, 0}, {32, 15}),
        Range2Di::fromSize({0, 25}, {23, 25})}));
}

void AtlasTest::padding() {
    std::vector<Range2Di> atlas = TextureTools::atlas({64, 64}, {
        {8, 16},
        {28, 13},
        {19, 23}
    }, {2, 1});

    CORRADE_COMPARE(atlas.size(), 3);
    CORRADE_COMPARE(atlas, (std::vector<Range2Di>{
        Range2Di::fromSize({2, 1}, {8, 16}),
        Range2Di::fromSize({34, 1}, {28, 13}),
        Range2Di::fromSize({2, 26}, {19, 23})}));
}

void AtlasTest::empty() {
    std::vector<Range2Di> atlas = TextureTools::atlas({}, {});
    CORRADE_VERIFY(atlas.empty());
}

void AtlasTest::tooSmall() {
    std::ostringstream o;
    Error redirectError{&o};

    std::vector<Range2Di> atlas = TextureTools::atlas({64, 32}, {
        {8, 16},
        {21, 13},
        {19, 29}
    }, {2, 1});
    CORRADE_VERIFY(atlas.empty());
    CORRADE_COMPARE(o.str(), "TextureTools::atlas(): requested atlas size Vector(64, 32) is too small to fit 3 Vector(25, 31) textures. Generated atlas will be empty.\n");
}

void AtlasTest::arrayPowerOfTwoEmpty() {
    Containers::Pair<Int, Containers::Array<Vector3i>> out = atlasArrayPowerOfTwo({128, 128}, {});
    CORRADE_COMPARE(out.first(), 0);
    CORRADE_COMPARE_AS(out.second(), Containers::arrayView<Vector3i>({
    }), TestSuite::Compare::Container);
}

void AtlasTest::arrayPowerOfTwoSingleElement() {
    Containers::Pair<Int, Containers::Array<Vector3i>> out = atlasArrayPowerOfTwo({128, 128}, {{128, 128}});
    CORRADE_COMPARE(out.first(), 1);
    CORRADE_COMPARE_AS(out.second(), Containers::arrayView<Vector3i>({
        {0, 0, 0}
    }), TestSuite::Compare::Container);
}

void AtlasTest::arrayPowerOfTwoAllSameElements() {
    Containers::Pair<Int, Containers::Array<Vector3i>> out = atlasArrayPowerOfTwo({128, 128}, {
        {64, 64},
        {64, 64},
        {64, 64},
        {64, 64},
    });
    CORRADE_COMPARE(out.first(), 1);
    CORRADE_COMPARE_AS(out.second(), Containers::arrayView<Vector3i>({
        {0, 0, 0},
        {64, 0, 0},
        {0, 64, 0},
        {64, 64, 0}
    }), TestSuite::Compare::Container);
}

void AtlasTest::arrayPowerOfTwoOneLayer() {
    auto&& data = ArrayPowerOfTwoOneLayerData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const Vector2i inputSorted[ArrayPowerOfTwoOneLayerImageCount]{
        {1024, 1024},   /*  0 */
        {1024, 1024},   /*  1 */

        {512, 512},     /*  2 */
        {512, 512},     /*  3 */
        {512, 512},     /*  4 */
        {512, 512},     /*  5 */
        {512, 512},     /*  6 */

        {256, 256},     /*  7 */
        {256, 256},     /*  8 */
        {256, 256},     /*  9 */
        {256, 256},     /* 10 */

        {128, 128},     /* 11 */
        {128, 128},     /* 12 */

        {32, 32},       /* 13 */
        {32, 32}        /* 14 */
    };

    const Vector3i expectedSorted[ArrayPowerOfTwoOneLayerImageCount]{
        {0, 0, 0},
        {1024, 0, 0},

        {0, 1024, 0},
        {512, 1024, 0},
        {0, 1536, 0},
        {512, 1536, 0},
        {1024, 1024, 0},

        {1536, 1024, 0},
        {1792, 1024, 0},
        {1536, 1280, 0},
        {1792, 1280, 0},

        {1024, 1536, 0},
        {1152, 1536, 0},

        {1024, 1664, 0},
        {1056, 1664, 0}
    };

    Vector2i input[ArrayPowerOfTwoOneLayerImageCount];
    Vector3i expected[ArrayPowerOfTwoOneLayerImageCount];
    for(std::size_t i = 0; i != ArrayPowerOfTwoOneLayerImageCount; ++i) {
        input[i] = inputSorted[data.order[i]];
        expected[i] = expectedSorted[data.order[i]];
    }

    Containers::Pair<Int, Containers::Array<Vector3i>> out = atlasArrayPowerOfTwo({2048, 2048}, input);
    CORRADE_COMPARE(out.first(), 1);
    CORRADE_COMPARE_AS(out.second(),
        Containers::ArrayView<const Vector3i>{expected},
        TestSuite::Compare::Container);
}

void AtlasTest::arrayPowerOfTwoMoreLayers() {
    Containers::Pair<Int, Containers::Array<Vector3i>> out = atlasArrayPowerOfTwo({2048, 2048}, {
        {2048, 2048},

        {1024, 1024},
        {1024, 1024},
        {1024, 1024},
        {512, 512},
        {512, 512},
        {512, 512},
        {512, 512},

        {512, 512},
        {256, 256},
        {256, 256}
    });
    CORRADE_COMPARE(out.first(), 3);
    CORRADE_COMPARE_AS(out.second(), Containers::arrayView<Vector3i>({
        {0, 0, 0},

        {0, 0, 1},
        {1024, 0, 1},
        {0, 1024, 1},
        {1024, 1024, 1},
        {1536, 1024, 1},
        {1024, 1536, 1},
        {1536, 1536, 1},

        {0, 0, 2},
        {512, 0, 2},
        {768, 0, 2}
    }), TestSuite::Compare::Container);
}

void AtlasTest::arrayPowerOfTwoWrongLayerSize() {
    auto&& data = ArrayPowerOfTwoWrongSizeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    atlasArrayPowerOfTwo(data.size, {});
    CORRADE_COMPARE(out.str(), Utility::formatString("TextureTools::atlasArrayPowerOfTwo(): expected layer size to be a non-zero power-of-two square, got {}\n", data.message));
}

void AtlasTest::arrayPowerOfTwoWrongSize() {
    auto&& data = ArrayPowerOfTwoWrongSizeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    atlasArrayPowerOfTwo({256, 256}, {
        {64, 64},
        {128, 128},
        data.size
    });
    CORRADE_COMPARE(out.str(), Utility::formatString("TextureTools::atlasArrayPowerOfTwo(): expected size 2 to be a non-zero power-of-two square, got {}\n", data.message));
}

}}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::AtlasTest)
