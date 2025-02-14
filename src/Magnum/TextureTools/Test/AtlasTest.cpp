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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Range.h"
#include "Magnum/TextureTools/Atlas.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#endif

namespace Magnum { namespace TextureTools { namespace Test { namespace {

struct AtlasTest: TestSuite::Tester {
    explicit AtlasTest();

    void debugLandfillFlag();
    void debugLandfillFlags();

    void landfillFullFit();
    void landfill();
    void landfillIncremental();
    void landfillPadded();
    void landfillNoFit();
    void landfillCopy();
    void landfillMove();

    void landfillArrayFullFit();
    void landfillArray();
    void landfillArrayIncremental();
    void landfillArrayPadded();
    void landfillArrayNoFit();

    void landfillInvalidSize();
    void landfillSetFlagsInvalid();
    void landfillAddMissingRotations();
    void landfillAddInvalidViewSizes();
    void landfillAddTwoComponentForArray();
    void landfillAddTooLargeElement();
    void landfillAddTooLargeElementPadded();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void deprecatedBasic();
    void deprecatedPadding();
    void deprecatedEmpty();
    void deprecatedTooSmall();
    #endif

    void arrayPowerOfTwoEmpty();
    void arrayPowerOfTwoSingleElement();
    void arrayPowerOfTwoAllSameElements();
    void arrayPowerOfTwoOneLayer();
    void arrayPowerOfTwoMoreLayers();
    void arrayPowerOfTwoInvalidViewSizes();
    void arrayPowerOfTwoWrongLayerSize();
    void arrayPowerOfTwoWrongSize();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void arrayPowerOfTwoDeprecated();
    #endif

    void textureCoordinateTransformation();
    void textureCoordinateTransformationOutOfBounds();
};

const Vector2i LandfillSizes[]{
    {3, 6}, /* 0 */
    {2, 5}, /* 1 */
    {4, 2}, /* 2 */
    {3, 3}, /* 3 */
    {2, 3}, /* 4 */
    {3, 3}, /* 5 */
    {2, 2}, /* 6 */
    {2, 1}, /* 7 */
    {2, 2}, /* 8 */
    {2, 2}, /* 9 */
    {2, 1}, /* a */
    {1, 2}, /* b */
    {1, 1}, /* c */
    {6, 0}, /* d */
    {0, 3}, /* e */
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    AtlasLandfillFlags flags;
    Vector2i size;
    Vector2i filledSize;
    Containers::Pair<Vector2i, bool> offsetsFlips[Containers::arraySize(LandfillSizes)];
} LandfillData[]{
    /* In all of these, rectangles with the same size should keep their order.
       5 after 3, 9 after 8 after 6 (and b after a after 7 if they're rotated
       to the same orientation) */
    {"no rotation, no width sorting", {}, {11, 12}, {11, 9}, {
        /* Here it discovers that item 8 is higher than 5 and so it begins from
           the opposite end in the same direction again, instead of flipping
           the direction at item 8.

              c
           8866aa77b99
           88662222b99
           000 2222555
           00011   555
           00011   555
           0001133344
           0001133344
           0001133344  */
        {{0, 0}, false},    /* 0 */
        {{3, 0}, false},    /* 1 */
        {{4, 5}, false},    /* 2 */
        {{5, 0}, false},    /* 3 */
        {{8, 0}, false},    /* 4 */
        {{8, 3}, false},    /* 5 */
        {{2, 6}, false},    /* 6 */
        {{6, 7}, false},    /* 7 */
        {{0, 6}, false},    /* 8 */
        {{9, 6}, false},    /* 9 */
        {{4, 7}, false},    /* a */
        {{8, 6}, false},    /* b */
        {{3, 8}, false},    /* c */
        {{5, 8}, false},    /* d (zero height, thus invisible) */
        {{8, 0}, false}}},  /* e (zero width, thus invisible) */
    /* No rotation with width sorting omitted, not interesting */
    {"portrait, no width sorting", AtlasLandfillFlag::RotatePortrait, {11, 12}, {11, 9}, {
        /* Here it should compare against the height of item 8, not item 0.
           Which is again higher than item 4 on the other side so it again
           begins from the opposite side.

                  ba
            88   cba99
            8876655599
           00076655544
           00011 55544
           0001122  44
           0001122333
           0001122333
           0001122333 */
        {{0, 0}, false},    /* 0 */
        {{3, 0}, false},    /* 1 */
        {{5, 0}, true},     /* 2 */
        {{7, 0}, false},    /* 3 */
        {{9, 3}, false},    /* 4 */
        {{6, 4}, false},    /* 5 */
        {{4, 5}, false},    /* 6 */
        {{3, 5}, true},     /* 7 */
        {{1, 6}, false},    /* 8 */
        {{9, 6}, false},    /* 9 */
        {{8, 7}, true},     /* a */
        {{7, 7}, false},    /* b */
        {{6, 7}, false},    /* c */
        {{3, 0}, true},     /* d (zero height, thus invisible) */
        {{6, 0}, false}}},  /* e (zero width, thus invisible) */
    {"portrait, widest first", AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::WidestFirst, {11, 12}, {11, 8}, {
        /* 9988   cba7
           99886644ba7
           000 6644555
           00011 44555
           0001122 555
           0001122333
           0001122333
           0001122333 */
        {{0, 0}, false},    /* 0 */
        {{3, 0}, false},    /* 1 */
        {{5, 0}, true},     /* 2 */
        {{7, 0}, false},    /* 3 */
        {{6, 4}, false},    /* 4 */
        {{8, 3}, false},    /* 5 */
        {{4, 5}, false},    /* 6 */
        {{10,6}, true},     /* 7 */
        {{2, 6}, false},    /* 8 */
        {{0, 6}, false},    /* 9 */
        {{9, 6}, true},     /* a */
        {{8, 6}, false},    /* b */
        {{7, 7}, false},    /* c */
        {{3, 0}, true},     /* d (zero height, thus invisible) */
        {{6, 0}, false}}},  /* e (zero width, thus invisible) */
    {"portrait, widest first, unbounded height", AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::WidestFirst, {11, 0}, {11, 8}, {
        /* Should have the same result as above.
         *
           9988   cba7
           99886644ba7
           000 6644555
           00011 44555
           0001122 555
           0001122333
           0001122333
           0001122333 */
        {{0, 0}, false},    /* 0 */
        {{3, 0}, false},    /* 1 */
        {{5, 0}, true},     /* 2 */
        {{7, 0}, false},    /* 3 */
        {{6, 4}, false},    /* 4 */
        {{8, 3}, false},    /* 5 */
        {{4, 5}, false},    /* 6 */
        {{10, 6}, true},    /* 7 */
        {{2, 6}, false},    /* 8 */
        {{0, 6}, false},    /* 9 */
        {{9, 6}, true},     /* a */
        {{8, 6}, false},    /* b */
        {{7, 7}, false},    /* c */
        {{3, 0}, true},     /* d (zero height, thus invisible) */
        {{6, 0}, false}}},  /* e (zero width, thus invisible) */
    {"portrait, widest first, reverse direction always", AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::WidestFirst|AtlasLandfillFlag::ReverseDirectionAlways, {11, 12}, {11, 10}, {
        /* Here it continues in reverse direction after placing item 9 even
           though it's higher than item 5 as it's forced to.

           7ab
           7abc
           9988
           99886644
           000 6644555
           00011 44555
           0001122 555
           0001122333
           0001122333
           0001122333 */
        {{0, 0}, false},    /* 0 */
        {{3, 0}, false},    /* 1 */
        {{5, 0}, true},     /* 2 */
        {{7, 0}, false},    /* 3 */
        {{6, 4}, false},    /* 4 */
        {{8, 3}, false},    /* 5 */
        {{4, 5}, false},    /* 6 */
        {{0, 8}, true},     /* 7 */
        {{2, 6}, false},    /* 8 */
        {{0, 6}, false},    /* 9 */
        {{1, 8}, true},     /* a */
        {{2, 8}, false},    /* b */
        {{3, 8}, false},    /* c */
        {{3, 0}, true},     /* d (zero height, thus invisible) */
        {{6, 0}, false}}},  /* e (zero width, thus invisible) */
    {"portrait, narrowest first", AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::NarrowestFirst, {11, 12}, {11, 9}, {
        /*        99
           66b   c9988
           66ba7555 88
           000a7555333
           00011555333
           0001122 333
           000112244
           000112244
           000112244 */
        {{0, 0}, false},    /* 0 */
        {{3, 0}, false},    /* 1 */
        {{5, 0}, true},     /* 2 */
        {{8, 3}, false},    /* 3 */
        {{7, 0}, false},    /* 4 */
        {{5, 4}, false},    /* 5 */
        {{0, 6}, false},    /* 6 */
        {{4, 5}, true},     /* 7 */
        {{9, 6}, false},    /* 8 */
        {{7, 7}, false},    /* 9 */
        {{3, 5}, true},     /* a */
        {{2, 6}, false},    /* b */
        {{6, 7}, false},    /* c */
        {{0, 0}, true},     /* d (zero height, thus invisible) */
        {{7, 0}, false}}},  /* e (zero width, thus invisible) */
    {"landscape, no width sorting", AtlasLandfillFlag::RotateLandscape, {11, 12}, {11, 9}, {
        /* After placing 3 it continues in reverse direction as 0 isn't lower
           (i.e., same behavior as if reversal was forced, and makes sense);
           after placing 1 it continues in reverse direction with 2 again;
           after placing 8 it however continues in the same direction again.

           99    bbc
           9977aa 6688
           22224446688
           2222444 555
              11111555
              11111555
           000000333
           000000333
           000000333   */
        {{0, 0}, true},     /* 0 */
        {{3, 3}, true},     /* 1 */
        {{0, 5}, false},    /* 2 */
        {{6, 0}, false},    /* 3 */
        {{4, 5}, true},     /* 4 */
        {{8, 3}, false},    /* 5 */
        {{7, 6}, false},    /* 6 */
        {{2, 7}, false},    /* 7 */
        {{9, 6}, false},    /* 8 */
        {{0, 7}, false},    /* 9 */
        {{4, 7}, false},    /* a */
        {{6, 8}, true},     /* b */
        {{8, 8}, false},    /* c */
        {{5, 9}, false},    /* d (zero height, thus invisible) */
        {{2, 8}, true}}},   /* e (zero width, thus invisible) */
    {"landscape, widest first", AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::WidestFirst, {11, 12}, {11, 9}, {
        /* No change compared to "no width sorting" in this case.

           99    bbc
           9977aa 6688
           22224446688
           2222444 555
              11111555
              11111555
           000000333
           000000333
           000000333   */
        {{0, 0}, true},     /* 0 */
        {{3, 3}, true},     /* 1 */
        {{0, 5}, false},    /* 2 */
        {{6, 0}, false},    /* 3 */
        {{4, 5}, true},     /* 4 */
        {{8, 3}, false},    /* 5 */
        {{7, 6}, false},    /* 6 */
        {{2, 7}, false},    /* 7 */
        {{9, 6}, false},    /* 8 */
        {{0, 7}, false},    /* 9 */
        {{4, 7}, false},    /* a */
        {{6, 8}, true},     /* b */
        {{8, 8}, false},    /* c */
        {{5, 9}, false},    /* d (zero height, thus invisible) */
        {{2, 8}, true}}},   /* e (zero width, thus invisible) */
    {"landscape, narrowest first", AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::NarrowestFirst, {11, 12}, {11, 10}, {
        /* No special behavior worth commenting on here. Flips direction after
           placing 5, after 8, and doesn't after placing 2.

                    bb
           11111c77aa
           111112222
           994442222
           99444000000
            8866000000
            8866000000
           333555
           333555
           333555      */
        {{5, 3}, true},     /* 0 */
        {{0, 7}, true},     /* 1 */
        {{5, 6}, false},    /* 2 */
        {{0, 0}, false},    /* 3 */
        {{2, 5}, true},     /* 4 */
        {{3, 0}, false},    /* 5 */
        {{3, 3}, false},    /* 6 */
        {{6, 8}, false},    /* 7 */
        {{1, 3}, false},    /* 8 */
        {{0, 5}, false},    /* 9 */
        {{8, 8}, false},    /* a */
        {{9, 9}, true},     /* b */
        {{5, 8}, false},    /* c */
        {{0, 9}, false},    /* d (zero height, thus invisible) */
        {{6, 9}, true}}},   /* e (zero width, thus invisible) */
};

const Vector2i LandfillArraySizes[]{
    {3, 6}, /* 0 */
    {2, 5}, /* 1 */
    {4, 2}, /* 2 */
    {3, 3}, /* 3 */
    {3, 3}, /* 4 */
    {2, 2}, /* 5 */
    {2, 2}, /* 6 */
    {2, 1}, /* 7 */
    {2, 2}, /* 8 */
    {2, 2}, /* 9 */
    {6, 0}, /* a */
    {0, 3}, /* b */
};

const struct {
    const char* name;
    AtlasLandfillFlags flags;
    Vector3i size;
    Vector3i filledSize;
    Containers::Pair<Vector3i, bool> offsetsFlips[Containers::arraySize(LandfillArraySizes)];
} LandfillArrayData[]{
    /* Various sorting aspects are tested in landfill() already, this just
       checks the array-specific behaviors and the rotation-less overload */
    {"no rotation", {}, {11, 6, 3}, {11, 6, 2}, {
        /* 000
           00011552222
           00011552222
           00011333444
           00011333444 668899
           00011333444 66889977 */
        {{0, 0, 0}, false},     /* 0 */
        {{3, 0, 0}, false},     /* 1 */
        {{7, 3, 0}, false},     /* 2 */
        {{5, 0, 0}, false},     /* 3 */
        {{8, 0, 0}, false},     /* 4 */
        {{5, 3, 0}, false},     /* 5 */
        {{0, 0, 1}, false},     /* 6 */
        {{6, 0, 1}, false},     /* 7 */
        {{2, 0, 1}, false},     /* 8 */
        {{4, 0, 1}, false},     /* 9 */
        {{5, 2, 1}, false},     /* a (zero height, thus invisible) */
        {{11,0, 0}, false}}},   /* b (zero height, thus invisible) */
    {"portrait, widest first", AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::WidestFirst, {11, 6, 3}, {11, 6, 2}, {
        /* 000   55444
           00011 55444
           0001122 444
           0001122333
           0001122333  6688997
           0001122333  6688997 */
        {{0, 0, 0}, false},     /* 0 */
        {{3, 0, 0}, false},     /* 1 */
        {{5, 0, 0}, true},      /* 2 */
        {{7, 0, 0}, false},     /* 3 */
        {{8, 3, 0}, false},     /* 4 */
        {{6, 4, 0}, false},     /* 5 */
        {{0, 0, 1}, false},     /* 6 */
        {{6, 0, 1}, true},      /* 7 */
        {{2, 0, 1}, false},     /* 8 */
        {{4, 0, 1}, false},     /* 9 */
        {{3, 0, 0}, true},      /* a (zero height, thus invisible) */
        {{8, 0, 0}, false}}},   /* b (zero height, thus invisible) */
    {"portrait, widest first, unbounded", AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::WidestFirst, {11, 6, 3}, {11, 6, 2}, {
        /* Should have the same result as above
           000   55444
           00011 55444
           0001122 444
           0001122333
           0001122333  6688997
           0001122333  6688997 */
        {{0, 0, 0}, false},     /* 0 */
        {{3, 0, 0}, false},     /* 1 */
        {{5, 0, 0}, true},      /* 2 */
        {{7, 0, 0}, false},     /* 3 */
        {{8, 3, 0}, false},     /* 4 */
        {{6, 4, 0}, false},     /* 5 */
        {{0, 0, 1}, false},     /* 6 */
        {{6, 0, 1}, true},      /* 7 */
        {{2, 0, 1}, false},     /* 8 */
        {{4, 0, 1}, false},     /* 9 */
        {{3, 0, 0}, true},      /* a (zero height, thus invisible) */
        {{8, 0, 0}, false}}},   /* b (zero height, thus invisible) */
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
} ArrayPowerOfTwoWrongLayerSizeData[]{
    {"non-power-of-two", {128, 127}, "{128, 127}"},
    {"non-square", {128, 256}, "{128, 256}"},
    {"zero", {1024, 0}, "{1024, 0}"},
};

const struct {
    const char* name;
    Vector2i size;
    const char* message;
} ArrayPowerOfTwoWrongSizeData[]{
    {"larger than size", {512, 512}, "{512, 512}"},
    {"non-power-of-two", {128, 127}, "{128, 127}"},
    {"non-square", {128, 256}, "{128, 256}"},
    {"zero", {1024, 0}, "{1024, 0}"},
};

AtlasTest::AtlasTest() {
    addTests({&AtlasTest::debugLandfillFlag,
              &AtlasTest::debugLandfillFlags,

              &AtlasTest::landfillFullFit});

    addInstancedTests({&AtlasTest::landfill},
        Containers::arraySize(LandfillData));

    addTests({&AtlasTest::landfillIncremental,
              &AtlasTest::landfillPadded,
              &AtlasTest::landfillNoFit,
              &AtlasTest::landfillCopy,
              &AtlasTest::landfillMove,

              &AtlasTest::landfillArrayFullFit});

    addInstancedTests({&AtlasTest::landfillArray},
        Containers::arraySize(LandfillArrayData));

    addTests({&AtlasTest::landfillArrayIncremental,
              &AtlasTest::landfillArrayPadded,
              &AtlasTest::landfillArrayNoFit,

              &AtlasTest::landfillInvalidSize,
              &AtlasTest::landfillSetFlagsInvalid,
              &AtlasTest::landfillAddMissingRotations,
              &AtlasTest::landfillAddInvalidViewSizes,
              &AtlasTest::landfillAddTwoComponentForArray,
              &AtlasTest::landfillAddTooLargeElement,
              &AtlasTest::landfillAddTooLargeElementPadded,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &AtlasTest::deprecatedBasic,
              &AtlasTest::deprecatedPadding,
              &AtlasTest::deprecatedEmpty,
              &AtlasTest::deprecatedTooSmall,
              #endif

              &AtlasTest::arrayPowerOfTwoEmpty,
              &AtlasTest::arrayPowerOfTwoSingleElement,
              &AtlasTest::arrayPowerOfTwoAllSameElements});

    addInstancedTests({&AtlasTest::arrayPowerOfTwoOneLayer},
        Containers::arraySize(ArrayPowerOfTwoOneLayerData));

    addTests({&AtlasTest::arrayPowerOfTwoMoreLayers,
              &AtlasTest::arrayPowerOfTwoInvalidViewSizes});

    addInstancedTests({&AtlasTest::arrayPowerOfTwoWrongLayerSize},
        Containers::arraySize(ArrayPowerOfTwoWrongLayerSizeData));

    addInstancedTests({&AtlasTest::arrayPowerOfTwoWrongSize},
        Containers::arraySize(ArrayPowerOfTwoWrongSizeData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addTests({&AtlasTest::arrayPowerOfTwoDeprecated});
    #endif

    addTests({&AtlasTest::textureCoordinateTransformation,
              &AtlasTest::textureCoordinateTransformationOutOfBounds});
}

void AtlasTest::debugLandfillFlag() {
    Containers::String out;
    Debug{&out} << AtlasLandfillFlag::RotatePortrait << AtlasLandfillFlag(0xcafedead);
    CORRADE_COMPARE(out, "TextureTools::AtlasLandfillFlag::RotatePortrait TextureTools::AtlasLandfillFlag(0xcafedead)\n");
}

void AtlasTest::debugLandfillFlags() {
    Containers::String out;
    Debug{&out} << (AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::NarrowestFirst|AtlasLandfillFlag(0xdead0000)) << AtlasLandfillFlags{};
    CORRADE_COMPARE(out, "TextureTools::AtlasLandfillFlag::RotateLandscape|TextureTools::AtlasLandfillFlag::NarrowestFirst|TextureTools::AtlasLandfillFlag(0xdead0000) TextureTools::AtlasLandfillFlags{}\n");
}

void AtlasTest::landfillFullFit() {
    /* Trivial case to verify there are no off-by-one errors that would prevent
       a tight fit */

    AtlasLandfill atlas{{4, 6}};
    CORRADE_COMPARE(atlas.size(), (Vector3i{4, 6, 1}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{4, 0, 1}));
    CORRADE_COMPARE(atlas.flags(), AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(atlas.padding(), Vector2i{});

    Vector2i offsets[4];
    UnsignedByte rotationData[1];
    Containers::MutableBitArrayView rotations{rotationData, 0, 4};
    /* Testing the init list overload here as all others test the view */
    CORRADE_COMPARE(atlas.add({
        {2, 4}, /* 0 */
        {2, 3}, /* 1 */
        {2, 3}, /* 2 */
        {2, 2}, /* 3 */
    }, offsets, rotations), (Range2Di{{}, {4, 6}}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{4, 6, 1}));
    CORRADE_COMPARE_AS(rotations, Containers::stridedArrayView({
        false, false, false, false
    }).sliceBit(0), TestSuite::Compare::Container);

    /* 3322
       3322
       0022
       0011
       0011
       0011 */
    CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector2i>({
        {0, 0}, /* 0 */
        {2, 0}, /* 1 */
        {2, 3}, /* 2 */
        {0, 4}, /* 3 */
    }), TestSuite::Compare::Container);
}

void AtlasTest::landfill() {
    auto&& data = LandfillData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    AtlasLandfill atlas{data.size};
    /* For unbounded sizes it should return 0 again */
    CORRADE_COMPARE(atlas.size(), (Vector3i{data.size, 1}));

    Vector2i offsets[Containers::arraySize(LandfillSizes)];
    /* In case rotations aren't enabled, this isn't zero-initialized by
       add() */
    UnsignedByte rotationData[2]{};
    Containers::MutableBitArrayView rotations{rotationData, 0, Containers::arraySize(LandfillSizes)};
    atlas.setFlags(data.flags);

    /* Test the rotations-less overload if no rotations are enabled */
    if(!(data.flags & (AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape)))
        CORRADE_COMPARE(atlas.add(LandfillSizes, offsets), (Range2Di{{}, data.filledSize}));
    else
        CORRADE_COMPARE(atlas.add(LandfillSizes, offsets, rotations), (Range2Di{{}, data.filledSize}));

    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{data.filledSize, 1}));
    CORRADE_COMPARE_AS(rotations,
        Containers::stridedArrayView(data.offsetsFlips)
            .slice(&Containers::Pair<Vector2i, bool>::second)
            .sliceBit(0),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(offsets),
        Containers::stridedArrayView(data.offsetsFlips)
            .slice(&Containers::Pair<Vector2i, bool>::first),
        TestSuite::Compare::Container);
}

void AtlasTest::landfillIncremental() {
    /* Same as landfill(portrait, widest first) (which is the default flags)
       but with the data split into three parts (0 to 4, 5 to 8, 9 to c), and
       shuffled to verify the sort works as it should */

    Vector2i sizeData[]{
        {4, 2}, /* 0, rotated */
        {3, 6}, /* 1 */
        {3, 3}, /* 2 */
        {5, 2}, /* 3, rotated */
        {3, 3}, /* 4 */
        {2, 2}, /* 5 */
        {2, 2}, /* 6 */
        {2, 2}, /* 7 */
        {3, 2}, /* 8, rotated */
        {1, 1}, /* 9 */
        {1, 2}, /* a */
        {2, 1}, /* b, rotated */
        {1, 2}, /* c */
    };
    auto sizes = Containers::arrayView(sizeData);

    Vector2i offsetData[Containers::arraySize(sizeData)];
    auto offsets = Containers::arrayView(offsetData);
    UnsignedByte rotationData[2];
    Containers::MutableBitArrayView rotations{rotationData, 0, Containers::arraySize(sizeData)};

    AtlasLandfill atlas{{11, 8}};
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{11, 0, 1}));

    /* The first addition spans a range that begins at the origin and ends at
       filledSize() */
    CORRADE_COMPARE(atlas.add(
        sizes.prefix(5),
        offsets.prefix(5),
        rotations.prefix(5)), (Range2Di{{}, {11, 6}}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{11, 6, 1}));

    /* Following additions are just incremental */
    CORRADE_COMPARE(atlas.add(
        sizes.slice(5, 9),
        offsets.slice(5, 9),
        rotations.slice(5, 9)), (Range2Di{{0, 4}, {8, 8}}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{11, 8, 1}));

    CORRADE_COMPARE(atlas.add(
        sizes.exceptPrefix(9),
        offsets.exceptPrefix(9),
        rotations.exceptPrefix(9)), (Range2Di{{7, 6}, {11, 8}}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{11, 8, 1}));

    CORRADE_COMPARE_AS(rotations, Containers::stridedArrayView({
        true, false, false, true, false, false, false, false, true, false,
        false, true, false
    }).sliceBit(0), TestSuite::Compare::Container);

    /* 7766   9cba
       77665588cba
       111 5588444
       11133 88444
       1113300 444
       1113300222
       1113300222
       1113300222  */
    CORRADE_COMPARE_AS(offsets, Containers::arrayView<Vector2i>({
        {5, 0}, /* 0 */
        {0, 0}, /* 1 */
        {7, 0}, /* 2 */
        {3, 0}, /* 3 */
        {8, 3}, /* 4 */
        {4, 5}, /* 5 */
        {2, 6}, /* 6 */
        {0, 6}, /* 7 */
        {6, 4}, /* 8 */
        {7, 7}, /* 9 */
        {10,6}, /* a */
        {9, 6}, /* b */
        {8, 6}, /* c */
    }), TestSuite::Compare::Container);
}

void AtlasTest::landfillPadded() {
    AtlasLandfill atlas{{17, 14}};
    atlas.setPadding({1, 2});
    CORRADE_COMPARE(atlas.padding(), (Vector2i{1, 2}));

    Vector2i offsets[8];
    UnsignedByte rotationData[1];
    Containers::MutableBitArrayView rotations{rotationData, 0, 8};

    /* The filled size includes the padding as well, since that's what is
       likely desirable to get copied as well */
    CORRADE_COMPARE(atlas.add({
        {6, 2}, /* 0, padded to {8, 6}, flipped */
        {1, 3}, /* 1, padded to {3, 7} */
        {4, 1}, /* 2, padded to {6, 5}, flipped */
        {2, 2}, /* 3, padded to {4, 6} */
        {2, 1}, /* 4, padded to {4, 5}, not flipped as padded it's portrait */
        {1, 1}, /* 5, padded to {3, 5} */
        {3, 0}, /* 6 (zero height), padded to {5, 4}, flipped */
        {0, 2}, /* 7 (zero width), padded to {2, 6} */
    }, offsets, rotations), (Range2Di{{}, {17, 13}}));

    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{17, 13, 1}));
    CORRADE_COMPARE_AS(rotations, Containers::stridedArrayView({
        true, false, true, false, false, false, true, false
    }).sliceBit(0), TestSuite::Compare::Container);

    /*   ...6666
         ...6666----77....
      10 .5.6666----77....
       9 ...6666-44-77.33.
       8 ...6666----77.33.
         ______ ----77....
         __00__...  77....
         __00__..._____
         __00__.1.__2__
         __00__.1.__2__
       2 __00__.1.__2__
       1 __00__...__2__
         ______..._____

          12  5 78  12 4 */
    CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector2i>({
        { 2,  1}, /* 0 */
        { 7,  2}, /* 1 */
        {11,  1}, /* 2 */
        {14,  8}, /* 3 */
        { 8,  9}, /* 4 */
        { 1, 10}, /* 5 */
        { 5,  9}, /* 6 (zero height, flipped, pointing to the empty inside) */
        {12,  8}, /* 7 (zero width, pointing to the empty inside) */
    }), TestSuite::Compare::Container);
}

void AtlasTest::landfillNoFit() {
    /* Same as landfill(portrait, widest first) (which is the default flags)
       which fits into {11, 8} but limiting height to 7 */

    AtlasLandfill atlas{{11, 7}};

    Vector2i offsets[Containers::arraySize(LandfillSizes)];
    UnsignedByte rotationData[2];
    Containers::MutableBitArrayView rotations{rotationData, 0, Containers::arraySize(LandfillSizes)};
    CORRADE_COMPARE(atlas.add(LandfillSizes, offsets, rotations), Containers::NullOpt);
}

void AtlasTest::landfillCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<AtlasLandfill>{});
    CORRADE_VERIFY(!std::is_copy_assignable<AtlasLandfill>{});
}

void AtlasTest::landfillMove() {
    AtlasLandfill a{{16, 24, 8}};

    Vector3i offsets[2];
    UnsignedByte rotations[1];
    CORRADE_VERIFY(a.add({{12, 17}, {5, 12}}, offsets, Containers::MutableBitArrayView{rotations, 0, 2}));

    AtlasLandfill b = Utility::move(a);
    CORRADE_COMPARE(b.size(), (Vector3i{16, 24, 8}));
    CORRADE_COMPARE(b.filledSize(), (Vector3i{16, 24, 2}));

    AtlasLandfill c{{16, 12, 1}};
    c = Utility::move(b);
    CORRADE_COMPARE(c.size(), (Vector3i{16, 24, 8}));
    CORRADE_COMPARE(c.filledSize(), (Vector3i{16, 24, 2}));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<AtlasLandfill>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<AtlasLandfill>::value);
}

void AtlasTest::landfillArrayFullFit() {
    /* Trivial case to verify there are no off-by-one errors that would prevent
       a tight fit */

    AtlasLandfill atlas{{4, 5, 2}};
    CORRADE_COMPARE(atlas.size(), (Vector3i{4, 5, 2}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{4, 5, 0}));
    CORRADE_COMPARE(atlas.flags(), AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::WidestFirst);
    CORRADE_COMPARE(atlas.padding(), Vector2i{});

    Vector3i offsets[6];
    UnsignedByte rotationData[1];
    Containers::MutableBitArrayView rotations{rotationData, 0, 6};
    /* Testing the init list overload as all others test the view */
    CORRADE_COMPARE(atlas.add({
        {3, 5}, /* 0 */
        {1, 5}, /* 1 */
        {3, 3}, /* 2 */
        {1, 3}, /* 3 */
        {2, 2}, /* 4 */
        {2, 2}, /* 5 */
    }, offsets, rotations), (Range3Di{{}, {4, 5, 2}}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{4, 5, 2}));
    CORRADE_COMPARE_AS(rotations, Containers::stridedArrayView({
        false, false, false, false, false, false
    }).sliceBit(0), TestSuite::Compare::Container);

    /* 0001 5544
       0001 5544
       0001 2223
       0001 2223
       0001 2223 */
    CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector3i>({
        {0, 0, 0}, /* 0 */
        {3, 0, 0}, /* 1 */
        {0, 0, 1}, /* 2 */
        {3, 0, 1}, /* 3 */
        {2, 3, 1}, /* 4 */
        {0, 3, 1}, /* 5 */
    }), TestSuite::Compare::Container);
}

void AtlasTest::landfillArray() {
    auto&& data = LandfillArrayData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    AtlasLandfill atlas{data.size};
    /* For unbounded sizes it should return 0 again */
    CORRADE_COMPARE(atlas.size(), data.size);

    Vector3i offsets[Containers::arraySize(LandfillArraySizes)];
    /* In case rotations aren't enabled, this isn't zero-initialized by
       add() */
    UnsignedByte rotationData[2]{};
    Containers::MutableBitArrayView rotations{rotationData, 0, Containers::arraySize(LandfillArraySizes)};
    atlas.setFlags(data.flags);

    /* Test the rotations-less overload if no rotations are enabled */
    if(!(data.flags & (AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape)))
        CORRADE_COMPARE(atlas.add(LandfillArraySizes, offsets), (Range3Di{{}, data.filledSize}));
    else
        CORRADE_COMPARE(atlas.add(LandfillArraySizes, offsets, rotations), (Range3Di{{}, data.filledSize}));

    CORRADE_COMPARE(atlas.filledSize(), data.filledSize);
    CORRADE_COMPARE_AS(rotations,
        Containers::stridedArrayView(data.offsetsFlips)
            .slice(&Containers::Pair<Vector3i, bool>::second)
            .sliceBit(0),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(offsets),
        Containers::stridedArrayView(data.offsetsFlips)
            .slice(&Containers::Pair<Vector3i, bool>::first),
        TestSuite::Compare::Container);
}

void AtlasTest::landfillArrayIncremental() {
    /* Variant of landfillIncremental(), with less elements and different item
       4 to test sorting across slices */

    Vector2i sizeData[]{
        {4, 2}, /* 0, rotated */
        {3, 6}, /* 1 */
        {3, 3}, /* 2 */
        {5, 2}, /* 3, rotated */
        {2, 2}, /* 4 */
        {2, 2}, /* 5 */
        {3, 3}, /* 6 */
        {2, 2}, /* 7 */
        {2, 1}, /* 8, rotated */
        {2, 2}, /* 9 */
    };
    auto sizes = Containers::arrayView(sizeData);

    Vector3i offsetData[Containers::arraySize(sizeData)];
    auto offsets = Containers::arrayView(offsetData);
    UnsignedByte rotationData[2];
    Containers::MutableBitArrayView rotations{rotationData, 0, Containers::arraySize(sizeData)};

    AtlasLandfill atlas{{11, 6, 2}};
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{11, 6, 0}));

    /* The first addition spans a range that begins at the origin and ends at
       filledSize(). Well, almost, because the first four items don't make use
       of the rightmost column. */
    CORRADE_COMPARE(atlas.add(
        sizes.prefix(4),
        offsets.prefix(4),
        rotations.prefix(4)), (Range3Di{{}, {10, 6, 1}}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{11, 6, 1}));

    /* Following additions are incremental ... well, in this case it overflows
       to the next slice, which means it covers basically the whole area */
    CORRADE_COMPARE(atlas.add(
        sizes.slice(4, 7),
        offsets.slice(4, 7),
        rotations.slice(4, 7)), (Range3Di{{}, {11, 6, 2}}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{11, 6, 2}));

    /* The last addition is then just a tiny bit of the second slice */
    CORRADE_COMPARE(atlas.add(
        sizes.exceptPrefix(7),
        offsets.exceptPrefix(7),
        rotations.exceptPrefix(7)), (Range3Di{{2, 0, 1}, {7, 2, 2}}));
    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{11, 6, 2}));

    CORRADE_COMPARE_AS(rotations, Containers::stridedArrayView({
        true, false, false, true, false, false, false, false, true, false
    }).sliceBit(0), TestSuite::Compare::Container);

    /* 111   44666
       11133 44666
       1113300 666
       1113300222
       1113300222  5577998
       1113300222  5577998 */
    CORRADE_COMPARE_AS(offsets, Containers::arrayView<Vector3i>({
        {5, 0, 0}, /* 0 */
        {0, 0, 0}, /* 1 */
        {7, 0, 0}, /* 2 */
        {3, 0, 0}, /* 3 */
        {6, 4, 0}, /* 4 */
        {0, 0, 1}, /* 5 */
        {8, 3, 0}, /* 6 */
        {2, 0, 1}, /* 7 */
        {6, 0, 1}, /* 8 */
        {4, 0, 1}, /* 9 */
    }), TestSuite::Compare::Container);
}

void AtlasTest::landfillArrayPadded() {
    /* Like landfillPadded(), but item 5 and 6 overflowing to the next slice */

    AtlasLandfill atlas{{16, 12, 3}};
    atlas.setPadding({1, 2});
    CORRADE_COMPARE(atlas.padding(), (Vector2i{1, 2}));

    Vector3i offsets[8];
    UnsignedByte rotationData[1];
    Containers::MutableBitArrayView rotations{rotationData, 0, 8};
    CORRADE_COMPARE(atlas.add({
        {6, 2}, /* 0, padded to {8, 6}, flipped */
        {1, 3}, /* 1, padded to {3, 7} */
        {4, 1}, /* 2, padded to {6, 5}, flipped */
        {2, 2}, /* 3, padded to {4, 6} */
        {2, 1}, /* 4, padded to {4, 5}, not flipped as padded it's portrait */
        {1, 1}, /* 5, padded to {3, 5} */
        {3, 0}, /* 6 (zero height), padded to {5, 4}, flipped */
        {0, 2}, /* 7 (zero width), padded to {2, 6} */
    }, offsets, rotations), (Range3Di{{}, {16, 12, 2}}));

    CORRADE_COMPARE(atlas.filledSize(), (Vector3i{16, 12, 2}));
    CORRADE_COMPARE_AS(rotations, Containers::stridedArrayView({
        true, false, true, false, false, false, true, false
    }).sliceBit(0), TestSuite::Compare::Container);

    /*         ----77....
               ----77....
       9       -44-77.33.
       8       ----77.33.
         _____ ----77....
         __00__... 77....
         __00__..._____
         __00__.1.__2__   6666...
         __00__.1.__2__   6666...
       2 __00__.1.__2__   6666.5.
       1 __00__...__2__   6666...
         ______..._____   6666...

           2  5 7   1 3     2  5 */
    CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector3i>({
        { 2, 1, 0}, /* 0 */
        { 7, 2, 0}, /* 1 */
        {11, 1, 0}, /* 2 */
        {13, 8, 0}, /* 3 */
        { 7, 9, 0}, /* 4 */
        { 5, 2, 1}, /* 5 */
        { 2, 1, 1}, /* 6 (zero height, flipped, pointing to the empty inside) */
        {11, 8, 0}, /* 7 (zero width, pointing to the empty inside) */
    }), TestSuite::Compare::Container);
}

void AtlasTest::landfillArrayNoFit() {
    /* It should fail in the second slice, not first, to test also that the
       recursion failure is propagated */
    {
        AtlasLandfill atlas{{6, 6, 2}};
        Vector3i offsets[Containers::arraySize(LandfillArraySizes)];
        UnsignedByte rotationData[2];
        Containers::MutableBitArrayView rotations{rotationData, 0, Containers::arraySize(LandfillArraySizes)};
        CORRADE_COMPARE(atlas.add(LandfillArraySizes, offsets, rotations), Containers::NullOpt);

    /* Sanity check that with one more slice it works */
    } {
        AtlasLandfill atlas{{6, 6, 3}};
        Vector3i offsets[Containers::arraySize(LandfillArraySizes)];
        UnsignedByte rotationData[2];
        Containers::MutableBitArrayView rotations{rotationData, 0, Containers::arraySize(LandfillArraySizes)};
        CORRADE_COMPARE(atlas.add(LandfillArraySizes, offsets, rotations), (Range3Di{{}, {6, 6, 3}}));
    }
}

void AtlasTest::landfillInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* These are fine */
    AtlasLandfill{{16, 0}};
    AtlasLandfill{{16, 65536}};
    AtlasLandfill{{16, 16, 0}};
    AtlasLandfill{{16, 65536, 16}};

    Containers::String out;
    Error redirectError{&out};
    AtlasLandfill{{0, 16}};
    AtlasLandfill{{16, 65537}};
    AtlasLandfill{{0, 16, 16}};
    AtlasLandfill{{16, 0, 16}};
    AtlasLandfill{{16, 65537, 16}};
    CORRADE_COMPARE_AS(out,
        "TextureTools::AtlasLandfill: expected non-zero width, got {0, 16, 1}\n"
        "TextureTools::AtlasLandfill: expected height to fit into 16 bits, got {16, 65537, 1}\n"
        "TextureTools::AtlasLandfill: expected non-zero width, got {0, 16, 16}\n"
        "TextureTools::AtlasLandfill: expected a single array slice for unbounded height, got {16, 0, 16}\n"
        "TextureTools::AtlasLandfill: expected height to fit into 16 bits, got {16, 65537, 16}\n",
        TestSuite::Compare::String);
}

void AtlasTest::landfillSetFlagsInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    AtlasLandfill atlas{{16, 16}};

    Containers::String out;
    Error redirectError{&out};
    atlas.setFlags(AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape);
    atlas.setFlags(AtlasLandfillFlag::WidestFirst|AtlasLandfillFlag::NarrowestFirst);
    CORRADE_COMPARE_AS(out,
        "TextureTools::AtlasLandfill::setFlags(): only one of RotatePortrait and RotateLandscape can be set\n"
        "TextureTools::AtlasLandfill::setFlags(): only one of WidestFirst and NarrowestFirst can be set\n",
        TestSuite::Compare::String);
}

void AtlasTest::landfillAddMissingRotations() {
    CORRADE_SKIP_IF_NO_ASSERT();

    AtlasLandfill portrait{{16, 23}};
    AtlasLandfill landscape{{16, 23}};
    portrait.setFlags(AtlasLandfillFlag::RotatePortrait);
    landscape.setFlags(AtlasLandfillFlag::RotateLandscape);
    Vector2i sizes[2];
    Vector2i offsets[2];
    Vector3i offsets3[2];

    Containers::String out;
    Error redirectError{&out};
    portrait.add(sizes, offsets);
    portrait.add(sizes, offsets3);
    /* "Testing" the rotation-less init list variants too */
    landscape.add({{}, {}}, offsets);
    landscape.add({{}, {}}, offsets3);
    CORRADE_COMPARE(out,
        "TextureTools::AtlasLandfill::add(): TextureTools::AtlasLandfillFlag::RotatePortrait set, expected a rotations view\n"
        "TextureTools::AtlasLandfill::add(): TextureTools::AtlasLandfillFlag::RotatePortrait set, expected a rotations view\n"
        "TextureTools::AtlasLandfill::add(): TextureTools::AtlasLandfillFlag::RotateLandscape set, expected a rotations view\n"
        "TextureTools::AtlasLandfill::add(): TextureTools::AtlasLandfillFlag::RotateLandscape set, expected a rotations view\n");
}

void AtlasTest::landfillAddInvalidViewSizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    AtlasLandfill atlas{{16, 23}};
    Vector2i sizes[2];
    Vector2i offsets[2];
    Vector2i offsetsInvalid[3];
    UnsignedByte rotationsData[1];
    Containers::MutableBitArrayView rotations{rotationsData, 0, 2};
    Containers::MutableBitArrayView rotationsInvalid{rotationsData, 0, 3};

    Containers::String out;
    Error redirectError{&out};
    atlas.add(sizes, offsetsInvalid, rotations);
    atlas.add(sizes, offsets, rotationsInvalid);
    CORRADE_COMPARE(out,
        "TextureTools::AtlasLandfill::add(): expected sizes and offsets views to have the same size, got 2 and 3\n"
        "TextureTools::AtlasLandfill::add(): expected sizes and rotations views to have the same size, got 2 and 3\n");
}

void AtlasTest::landfillAddTwoComponentForArray() {
    CORRADE_SKIP_IF_NO_ASSERT();

    AtlasLandfill atlas{{16, 23, 3}};
    atlas.clearFlags(AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape);
    Vector2i sizes[2];
    Vector2i offsets[2];
    UnsignedByte rotationsData[1];
    Containers::MutableBitArrayView rotations{rotationsData, 0, 2};

    Containers::String out;
    Error redirectError{&out};
    atlas.add(sizes, offsets, rotations);
    atlas.add(sizes, offsets);
    atlas.add({}, offsets, rotations);
    atlas.add({}, offsets);
    CORRADE_COMPARE(out,
        "TextureTools::AtlasLandfill::add(): use the three-component overload for an array atlas\n"
        "TextureTools::AtlasLandfill::add(): use the three-component overload for an array atlas\n"
        "TextureTools::AtlasLandfill::add(): use the three-component overload for an array atlas\n"
        "TextureTools::AtlasLandfill::add(): use the three-component overload for an array atlas\n");
}

void AtlasTest::landfillAddTooLargeElement() {
    CORRADE_SKIP_IF_NO_ASSERT();

    AtlasLandfill portrait{{16, 23}};
    AtlasLandfill portrait2{{16, 13}};
    AtlasLandfill landscape{{23, 16}};
    AtlasLandfill landscape2{{13, 16}};
    landscape.setFlags(AtlasLandfillFlag::RotateLandscape);
    landscape2.setFlags(AtlasLandfillFlag::RotateLandscape);
    Vector2i offsets[2];
    Vector3i offsets3[2];
    UnsignedByte rotationsData[1];
    Containers::MutableBitArrayView rotations{rotationsData, 0, 2};

    Containers::String out;
    Error redirectError{&out};
    /* Zero-size elements should still be checked against bounds in the other
       dimension */
    portrait.add({{16, 23}, {0, 24}}, offsets, rotations);
    landscape.add({{23, 16}, {24, 0}}, offsets3, rotations);
    portrait.add({{16, 23}, {17, 23}}, offsets, rotations);
    landscape.add({{23, 16}, {23, 17}}, offsets3, rotations);
    /* Sizes that fit but don't after a flip */
    portrait2.add({{13, 13}, {15, 13}}, offsets, rotations);
    landscape2.add({{13, 13}, {13, 15}}, offsets3, rotations);
    CORRADE_COMPARE_AS(out,
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {16, 23} but got {0, 24}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {23, 16} but got {24, 0}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {16, 23} but got {17, 23}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {23, 16} but got {23, 17}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {16, 13} but got {13, 15}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {13, 16} but got {15, 13}\n",
        TestSuite::Compare::String);
}

void AtlasTest::landfillAddTooLargeElementPadded() {
    /* Sizes (except for zeros) are same as above minus padding */

    CORRADE_SKIP_IF_NO_ASSERT();

    AtlasLandfill portrait{{16, 23}};
    AtlasLandfill portrait2{{16, 13}};
    AtlasLandfill landscape{{23, 16}};
    AtlasLandfill landscape2{{13, 16}};
    portrait.setPadding({2, 1});
    portrait2.setPadding({2, 1});
    landscape.setPadding({1, 2})
        .setFlags(AtlasLandfillFlag::RotateLandscape);
    landscape2.setPadding({1, 2})
        .setFlags(AtlasLandfillFlag::RotateLandscape);
    Vector2i offsets[2];
    Vector3i offsets3[2];
    UnsignedByte rotationsData[1];
    Containers::MutableBitArrayView rotations{rotationsData, 0, 2};

    Containers::String out;
    Error redirectError{&out};
    /* Zero-size elements should still be checked against bounds in the other
       dimension */
    portrait.add({{12, 21}, {0, 22}}, offsets, rotations);
    landscape.add({{21, 12}, {22, 0}}, offsets3, rotations);
    portrait.add({{12, 21}, {13, 21}}, offsets, rotations);
    landscape.add({{21, 12}, {21, 13}}, offsets3, rotations);
    /* Sizes that fit but don't after a flip */
    portrait2.add({{9, 11}, {12, 11}}, offsets, rotations);
    landscape2.add({{11, 9}, {11, 12}}, offsets3, rotations);
    CORRADE_COMPARE_AS(out,
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {16, 23} but got {0, 22} and padding {2, 1}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {23, 16} but got {22, 0} and padding {1, 2}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {16, 23} but got {13, 21} and padding {2, 1}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {23, 16} but got {21, 13} and padding {1, 2}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {16, 13} but got {11, 12} and padding {1, 2}\n"
        "TextureTools::AtlasLandfill::add(): expected size 1 to be not larger than {13, 16} but got {12, 11} and padding {2, 1}\n",
        TestSuite::Compare::String);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AtlasTest::deprecatedBasic() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::vector<Range2Di> atlas = TextureTools::atlas({64, 64}, {
        {12, 18},
        {32, 15},
        {23, 25}
    });
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(atlas.size(), 3);
    CORRADE_COMPARE(atlas, (std::vector<Range2Di>{
        Range2Di::fromSize({0, 0}, {12, 18}),
        Range2Di::fromSize({32, 0}, {32, 15}),
        Range2Di::fromSize({0, 25}, {23, 25})}));
}

void AtlasTest::deprecatedPadding() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::vector<Range2Di> atlas = TextureTools::atlas({64, 64}, {
        {8, 16},
        {28, 13},
        {19, 23}
    }, {2, 1});
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(atlas.size(), 3);
    CORRADE_COMPARE(atlas, (std::vector<Range2Di>{
        Range2Di::fromSize({2, 1}, {8, 16}),
        Range2Di::fromSize({34, 1}, {28, 13}),
        Range2Di::fromSize({2, 26}, {19, 23})}));
}

void AtlasTest::deprecatedEmpty() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::vector<Range2Di> atlas = TextureTools::atlas({}, {});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_VERIFY(atlas.empty());
}

void AtlasTest::deprecatedTooSmall() {
    Containers::String out;
    Error redirectError{&out};

    CORRADE_IGNORE_DEPRECATED_PUSH
    std::vector<Range2Di> atlas = TextureTools::atlas({64, 32}, {
        {8, 16},
        {21, 13},
        {19, 29}
    }, {2, 1});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_VERIFY(atlas.empty());
    CORRADE_COMPARE(out, "TextureTools::atlas(): requested atlas size Vector(64, 32) is too small to fit 3 Vector(25, 31) textures. Generated atlas will be empty.\n");
}
#endif

void AtlasTest::arrayPowerOfTwoEmpty() {
    Containers::ArrayView<Vector3i> offsets;
    CORRADE_COMPARE(atlasArrayPowerOfTwo({128, 128}, {}, offsets), 0);
}

void AtlasTest::arrayPowerOfTwoSingleElement() {
    Vector3i offsets[1];
    CORRADE_COMPARE(atlasArrayPowerOfTwo({128, 128}, {{128, 128}}, offsets), 1);
    CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector3i>({
        {0, 0, 0}
    }), TestSuite::Compare::Container);
}

void AtlasTest::arrayPowerOfTwoAllSameElements() {
    Vector3i offsets[4];
    CORRADE_COMPARE(atlasArrayPowerOfTwo({128, 128}, {
        {64, 64},
        {64, 64},
        {64, 64},
        {64, 64},
    }, offsets), 1);
    CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector3i>({
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

    Vector3i offsets[ArrayPowerOfTwoOneLayerImageCount];
    CORRADE_COMPARE(atlasArrayPowerOfTwo({2048, 2048}, input, offsets), 1);
    CORRADE_COMPARE_AS(Containers::arrayView(offsets),
        Containers::arrayView(expected),
        TestSuite::Compare::Container);
}

void AtlasTest::arrayPowerOfTwoMoreLayers() {
    Vector3i offsets[11];
    CORRADE_COMPARE(atlasArrayPowerOfTwo({2048, 2048}, {
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
    }, offsets), 3);
    CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector3i>({
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

void AtlasTest::arrayPowerOfTwoInvalidViewSizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2i sizes[2];
    Vector3i offsetsInvalid[3];

    Containers::String out;
    Error redirectError{&out};
    atlasArrayPowerOfTwo({}, sizes, offsetsInvalid);
    CORRADE_COMPARE(out,
        "TextureTools::atlasArrayPowerOfTwo(): expected sizes and offsets views to have the same size, got 2 and 3\n");
}

void AtlasTest::arrayPowerOfTwoWrongLayerSize() {
    auto&& data = ArrayPowerOfTwoWrongLayerSizeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    atlasArrayPowerOfTwo(data.size, {}, {});
    CORRADE_COMPARE(out, Utility::format("TextureTools::atlasArrayPowerOfTwo(): expected layer size to be a non-zero power-of-two square, got {}\n", data.message));
}

void AtlasTest::arrayPowerOfTwoWrongSize() {
    auto&& data = ArrayPowerOfTwoWrongSizeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3i offsets[3];

    Containers::String out;
    Error redirectError{&out};
    atlasArrayPowerOfTwo({256, 256}, {
        {64, 64},
        {128, 128},
        data.size
    }, offsets);
    CORRADE_COMPARE(out, Utility::format("TextureTools::atlasArrayPowerOfTwo(): expected size 2 to be a non-zero power-of-two square not larger than {{256, 256}} but got {}\n", data.message));
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AtlasTest::arrayPowerOfTwoDeprecated() {
    /* Same as arrayPowerOfTwoAllSameElements(), but with the deprecated API */

    CORRADE_IGNORE_DEPRECATED_PUSH
    Containers::Pair<Int, Containers::Array<Vector3i>> out = atlasArrayPowerOfTwo({128, 128}, {
        {64, 64},
        {64, 64},
        {64, 64},
        {64, 64},
    });
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.first(), 1);
    CORRADE_COMPARE_AS(out.second(), Containers::arrayView<Vector3i>({
        {0, 0, 0},
        {64, 0, 0},
        {0, 64, 0},
        {64, 64, 0}
    }), TestSuite::Compare::Container);
}
#endif

void AtlasTest::textureCoordinateTransformation() {
    const Vector2i atlasSize{4, 5};
    const Vector2i size{2, 1};
    const Vector2i offset{1, 2};
    const Vector2 a{0.0f, 0.0f};
    const Vector2 b{1.0f, 0.0f};
    const Vector2 c{0.0f, 1.0f};
    const Vector2 d{1.0f, 1.0f};

    /* Trivial rotation cases with no scaling or offset should return in exact
       corner positions
        c--d    d--b    a--c
        |  |    |  |    |  |
        a--b    c--a    b--d */
    {
        const Matrix3 transformation = atlasTextureCoordinateTransformation(atlasSize, atlasSize, {});
        CORRADE_COMPARE(transformation.transformPoint(a), (Vector2{0.0f, 0.0f}));
        CORRADE_COMPARE(transformation.transformPoint(b), (Vector2{1.0f, 0.0f}));
        CORRADE_COMPARE(transformation.transformPoint(c), (Vector2{0.0f, 1.0f}));
        CORRADE_COMPARE(transformation.transformPoint(d), (Vector2{1.0f, 1.0f}));
        CORRADE_COMPARE(transformation, (Matrix3{
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }));
    } {
        /* The item size is flipped, as otherwise with the rotation it'd mean
           we want to put a {5, 4} item into an atlas of size {4, 5} */
        const Matrix3 transformation = atlasTextureCoordinateTransformationRotatedCounterClockwise(atlasSize, atlasSize.flipped(), {});
        CORRADE_COMPARE(transformation.transformPoint(a), (Vector2{1.0f, 0.0f}));
        CORRADE_COMPARE(transformation.transformPoint(b), (Vector2{1.0f, 1.0f}));
        CORRADE_COMPARE(transformation.transformPoint(c), (Vector2{0.0f, 0.0f}));
        CORRADE_COMPARE(transformation.transformPoint(d), (Vector2{0.0f, 1.0f}));
        CORRADE_COMPARE(transformation, (Matrix3{
            {0.0f, 1.0f, 0.0f},
            {-1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 1.0f}
        }));
    } {
        /* The item size is flipped, as otherwise with the rotation it'd mean
           we want to put a {5, 4} item into an atlas of size {4, 5} */
        const Matrix3 transformation = atlasTextureCoordinateTransformationRotatedClockwise(atlasSize, atlasSize.flipped(), {});
        CORRADE_COMPARE(transformation.transformPoint(a), (Vector2{0.0f, 1.0f}));
        CORRADE_COMPARE(transformation.transformPoint(b), (Vector2{0.0f, 0.0f}));
        CORRADE_COMPARE(transformation.transformPoint(c), (Vector2{1.0f, 1.0f}));
        CORRADE_COMPARE(transformation.transformPoint(d), (Vector2{1.0f, 0.0f}));
        CORRADE_COMPARE(transformation, (Matrix3{
            {0.0f, -1.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 1.0f}
        }));

    /* 5 +--------+
         |        |
       3 | c----d |
         | |    | |
       2 | a----b |
         |        |
       0 +--------+
         0 1    3 4 */
    } {
        const Matrix3 transformation = atlasTextureCoordinateTransformation(atlasSize, size, offset);
        CORRADE_COMPARE(transformation.transformPoint(a)*atlasSize, (Vector2i{1, 2}));
        CORRADE_COMPARE(transformation.transformPoint(b)*atlasSize, (Vector2i{3, 2}));
        CORRADE_COMPARE(transformation.transformPoint(c)*atlasSize, (Vector2i{1, 3}));
        CORRADE_COMPARE(transformation.transformPoint(d)*atlasSize, (Vector2i{3, 3}));
        CORRADE_COMPARE(transformation, (Matrix3{
            {0.5f, 0.0f, 0.0f},
            {0.0f, 0.2f, 0.0f},
            {0.25f, 0.4f, 1.0f}
        }));

    /* 5 +--------+
       4 | d--b   |
         | |  |   |
         | |  |   |
       2 | c--a   |
         |        |
       0 +--------+
         0 1  2   4 */
    } {
        const Matrix3 transformation = atlasTextureCoordinateTransformationRotatedCounterClockwise(atlasSize, size, offset);
        CORRADE_COMPARE(transformation.transformPoint(a)*atlasSize, (Vector2i{2, 2}));
        CORRADE_COMPARE(transformation.transformPoint(b)*atlasSize, (Vector2i{2, 4}));
        CORRADE_COMPARE(transformation.transformPoint(c)*atlasSize, (Vector2i{1, 2}));
        CORRADE_COMPARE(transformation.transformPoint(d)*atlasSize, (Vector2i{1, 4}));
        CORRADE_COMPARE(transformation, (Matrix3{
            {0.0f, 0.4f, 0.0f},
            {-0.25f, 0.0f, 0.0f},
            {0.5f, 0.4f, 1.0f}
        }));

    /* 5 +--------+
       4 | a--c   |
         | |  |   |
         | |  |   |
       2 | b--d   |
         |        |
       0 +--------+
         0 1  2   4 */
    } {
        const Matrix3 transformation = atlasTextureCoordinateTransformationRotatedClockwise(atlasSize, size, offset);
        CORRADE_COMPARE(transformation.transformPoint(a)*atlasSize, (Vector2i{1, 4}));
        CORRADE_COMPARE(transformation.transformPoint(b)*atlasSize, (Vector2i{1, 2}));
        CORRADE_COMPARE(transformation.transformPoint(c)*atlasSize, (Vector2i{2, 4}));
        CORRADE_COMPARE(transformation.transformPoint(d)*atlasSize, (Vector2i{2, 2}));
        CORRADE_COMPARE(transformation, (Matrix3{
            {0.0f, -0.4f, 0.0f},
            {0.25f, 0.0f, 0.0f},
            {0.25f, 0.8f, 1.0f}
        }));
    }
}

void AtlasTest::textureCoordinateTransformationOutOfBounds() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* These should be fine */
    atlasTextureCoordinateTransformation({5, 4}, {5, 4}, {});
    atlasTextureCoordinateTransformationRotatedCounterClockwise({5, 4}, {4, 5}, {});
    atlasTextureCoordinateTransformationRotatedClockwise({5, 4}, {4, 5}, {});
    atlasTextureCoordinateTransformation({5, 4}, {3, 1}, {2, 3});
    atlasTextureCoordinateTransformationRotatedCounterClockwise({5, 4}, {1, 3}, {2, 3});
    atlasTextureCoordinateTransformationRotatedClockwise({5, 4}, {1, 3}, {2, 3});

    Containers::String out;
    Error redirectError{&out};
    /* Size too large in either dimension */
    atlasTextureCoordinateTransformation({5, 4}, {3, 5}, {});
    atlasTextureCoordinateTransformation({4, 5}, {5, 3}, {});
    atlasTextureCoordinateTransformationRotatedCounterClockwise({5, 4}, {5, 3}, {});
    atlasTextureCoordinateTransformationRotatedCounterClockwise({4, 5}, {3, 5}, {});
    atlasTextureCoordinateTransformationRotatedClockwise({5, 4}, {5, 3}, {});
    atlasTextureCoordinateTransformationRotatedClockwise({4, 5}, {3, 5}, {});
    /* Size + offset too large */
    atlasTextureCoordinateTransformation({5, 4}, {1, 2}, {2, 3});
    atlasTextureCoordinateTransformation({4, 5}, {2, 1}, {3, 2});
    atlasTextureCoordinateTransformationRotatedCounterClockwise({5, 4}, {2, 1}, {2, 3});
    atlasTextureCoordinateTransformationRotatedCounterClockwise({4, 5}, {1, 2}, {3, 2});
    atlasTextureCoordinateTransformationRotatedClockwise({5, 4}, {2, 1}, {2, 3});
    atlasTextureCoordinateTransformationRotatedClockwise({4, 5}, {1, 2}, {3, 2});
    CORRADE_COMPARE_AS(out,
       "TextureTools::atlasTextureCoordinateTransformation(): size {3, 5} and offset {0, 0} doesn't fit into {5, 4}\n"
       "TextureTools::atlasTextureCoordinateTransformation(): size {5, 3} and offset {0, 0} doesn't fit into {4, 5}\n"
       "TextureTools::atlasTextureCoordinateTransformationRotatedCounterClockwise(): (rotated) size {3, 5} and offset {0, 0} doesn't fit into {5, 4}\n"
       "TextureTools::atlasTextureCoordinateTransformationRotatedCounterClockwise(): (rotated) size {5, 3} and offset {0, 0} doesn't fit into {4, 5}\n"
       "TextureTools::atlasTextureCoordinateTransformationRotatedClockwise(): (rotated) size {3, 5} and offset {0, 0} doesn't fit into {5, 4}\n"
       "TextureTools::atlasTextureCoordinateTransformationRotatedClockwise(): (rotated) size {5, 3} and offset {0, 0} doesn't fit into {4, 5}\n"

       "TextureTools::atlasTextureCoordinateTransformation(): size {1, 2} and offset {2, 3} doesn't fit into {5, 4}\n"
       "TextureTools::atlasTextureCoordinateTransformation(): size {2, 1} and offset {3, 2} doesn't fit into {4, 5}\n"
       "TextureTools::atlasTextureCoordinateTransformationRotatedCounterClockwise(): (rotated) size {1, 2} and offset {2, 3} doesn't fit into {5, 4}\n"
       "TextureTools::atlasTextureCoordinateTransformationRotatedCounterClockwise(): (rotated) size {2, 1} and offset {3, 2} doesn't fit into {4, 5}\n"
       "TextureTools::atlasTextureCoordinateTransformationRotatedClockwise(): (rotated) size {1, 2} and offset {2, 3} doesn't fit into {5, 4}\n"
       "TextureTools::atlasTextureCoordinateTransformationRotatedClockwise(): (rotated) size {2, 1} and offset {3, 2} doesn't fit into {4, 5}\n",
        TestSuite::Compare::String);
}

}}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::AtlasTest)
