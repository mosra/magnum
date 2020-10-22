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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Trade/LightData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct LightDataTest: TestSuite::Tester {
    explicit LightDataTest();

    void construct();
    void constructAttenuation();
    void constructRange();
    void constructNone();

    void constructInvalid();

    void constructCopy();
    void constructMove();

    void debugType();
};

using namespace Math::Literals;

const struct {
    const char* name;
    LightData::Type type;
    Vector3 attenuation;
    Float range;
    Rad innerConeAngle;
    Rad outerConeAngle;
    const char* message;
} ConstructInvalidData[] {
    {"invalid ambient attenuation", LightData::Type::Ambient,
        {1.0f, 0.0f, 1.0f}, Constants::inf(), 360.0_degf, 360.0_degf,
        "attenuation has to be (1, 0, 0) for an ambient or directional light but got Vector(1, 0, 1)"},
    {"invalid directional attenuation", LightData::Type::Directional,
        {1.0f, 0.0f, 1.0f}, Constants::inf(), 360.0_degf, 360.0_degf,
        "attenuation has to be (1, 0, 0) for an ambient or directional light but got Vector(1, 0, 1)"},
    {"invalid ambient range", LightData::Type::Ambient,
        {1.0f, 0.0f, 0.0f}, 2.0f, 360.0_degf, 360.0_degf,
        "range has to be infinity for an ambient or directional light but got 2"},
    {"invalid directional range", LightData::Type::Directional,
        {1.0f, 0.0f, 0.0f}, 2.0f, 360.0_degf, 360.0_degf,
        "range has to be infinity for an ambient or directional light but got 2"},
    {"invalid point angles", LightData::Type::Point,
        {1.0f, 0.0f, 1.0f}, Constants::inf(), 15.0_degf, 90.0_degf,
        "cone angles have to be 360° for lights that aren't spot but got Deg(15) and Deg(90)"},
    {"negative inner spot angle", LightData::Type::Spot,
        {1.0f, 0.0f, 1.0f}, Constants::inf(), -1.0_degf, 90.0_degf,
        "spot light inner and outer cone angles have to be in range [0°, 360°] and inner not larger than outer but got Deg(-1) and Deg(90)"},
    {"too big outer spot angle", LightData::Type::Spot,
        {1.0f, 0.0f, 1.0f}, Constants::inf(), 0.0_degf, 361.0_degf,
        "spot light inner and outer cone angles have to be in range [0°, 360°] and inner not larger than outer but got Deg(0) and Deg(361)"},
    {"inner spot angle larger than outer", LightData::Type::Spot,
        {1.0f, 0.0f, 1.0f}, Constants::inf(), 35.0_degf, 30.0_degf,
        "spot light inner and outer cone angles have to be in range [0°, 360°] and inner not larger than outer but got Deg(35) and Deg(30)"}
};

LightDataTest::LightDataTest() {
    addTests({&LightDataTest::construct,
              &LightDataTest::constructAttenuation,
              &LightDataTest::constructRange,
              &LightDataTest::constructNone});

    addInstancedTests({&LightDataTest::constructInvalid},
        Containers::arraySize(ConstructInvalidData));

    addTests({&LightDataTest::constructCopy,
              &LightDataTest::constructMove,

              &LightDataTest::debugType});
}

void LightDataTest::construct() {
    {
        int a;
        LightData data{LightData::Type::Spot,
            0xccff33_rgbf, 0.8f,
            {0.1f, 0.5f, 0.7f}, 15.0f,
            15.0_degf, 35.0_degf,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Spot);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{0.1f, 0.5f, 0.7f}));
        CORRADE_COMPARE(data.range(), 15.0f);
        CORRADE_COMPARE(data.innerConeAngle(), 15.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 35.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit spot angles */
    } {
        int a;
        LightData data{LightData::Type::Spot,
            0xccff33_rgbf, 0.8f,
            {0.1f, 0.5f, 0.7f}, 15.0f,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Spot);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{0.1f, 0.5f, 0.7f}));
        CORRADE_COMPARE(data.range(), 15.0f);
        CORRADE_COMPARE(data.innerConeAngle(), 0.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 90.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit non-spot angles */
    } {
        int a;
        LightData data{LightData::Type::Point,
            0xccff33_rgbf, 0.8f,
            {0.1f, 0.5f, 0.7f}, 15.0f,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Point);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{0.1f, 0.5f, 0.7f}));
        CORRADE_COMPARE(data.range(), 15.0f);
        CORRADE_COMPARE(data.innerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);
    }
}

void LightDataTest::constructAttenuation() {
    /* Implicit range */
    {
        int a;
        LightData data{LightData::Type::Spot,
            0xccff33_rgbf, 0.8f,
            {0.1f, 0.5f, 0.7f},
            15.0_degf, 35.0_degf,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Spot);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{0.1f, 0.5f, 0.7f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 15.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 35.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit range + spot angles */
    } {
        int a;
        LightData data{LightData::Type::Spot,
            0xccff33_rgbf, 0.8f,
            {0.1f, 0.5f, 0.7f},
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Spot);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{0.1f, 0.5f, 0.7f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 0.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 90.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit range + non-spot angles */
    } {
        int a;
        LightData data{LightData::Type::Point,
            0xccff33_rgbf, 0.8f,
            {0.1f, 0.5f, 0.7f},
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Point);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{0.1f, 0.5f, 0.7f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);
    }
}

void LightDataTest::constructRange() {
    /* Implicit attenuation for a spot */
    {
        int a;
        LightData data{LightData::Type::Spot,
            0xccff33_rgbf, 0.8f,
            15.0f,
            15.0_degf, 35.0_degf,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Spot);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 1.0f}));
        CORRADE_COMPARE(data.range(), 15.0f);
        CORRADE_COMPARE(data.innerConeAngle(), 15.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 35.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit attenuation for a spot + spot angles */
    } {
        int a;
        LightData data{LightData::Type::Spot,
            0xccff33_rgbf, 0.8f,
            15.0f,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Spot);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 1.0f}));
        CORRADE_COMPARE(data.range(), 15.0f);
        CORRADE_COMPARE(data.innerConeAngle(), 0.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 90.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit attenuation for a point + non-spot angles */
    } {
        int a;
        LightData data{LightData::Type::Point,
            0xccff33_rgbf, 0.8f,
            15.0f,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Point);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 1.0f}));
        CORRADE_COMPARE(data.range(), 15.0f);
        CORRADE_COMPARE(data.innerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit attenuation for an ambient + non-spot angles */
    } {
        int a;
        LightData data{LightData::Type::Ambient,
            0xccff33_rgbf, 0.8f,
            Constants::inf(),
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Ambient);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 0.0f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit attenuation for a directional + non-spot angles */
    } {
        int a;
        LightData data{LightData::Type::Directional,
            0xccff33_rgbf, 0.8f,
            Constants::inf(),
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Directional);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 0.0f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);
    }
}

void LightDataTest::constructNone() {
    /* Implicit attenuation + range for a spot */
    {
        int a;
        LightData data{LightData::Type::Spot,
            0xccff33_rgbf, 0.8f,
            15.0_degf, 35.0_degf,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Spot);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 1.0f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 15.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 35.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit attenuation + range for a spot + spot angles */
    } {
        int a;
        LightData data{LightData::Type::Spot,
            0xccff33_rgbf, 0.8f,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Spot);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 1.0f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 0.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 90.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit attenuation + range for a point + non-spot angles */
    } {
        int a;
        LightData data{LightData::Type::Point,
            0xccff33_rgbf, 0.8f,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Point);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 1.0f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit attenuation for an ambient + non-spot angles */
    } {
        int a;
        LightData data{LightData::Type::Ambient,
            0xccff33_rgbf, 0.8f,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Ambient);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 0.0f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);

    /* Implicit attenuation for a directional + non-spot angles */
    } {
        int a;
        LightData data{LightData::Type::Directional,
            0xccff33_rgbf, 0.8f,
            &a};

        CORRADE_COMPARE(data.type(), LightData::Type::Directional);
        CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
        CORRADE_COMPARE(data.intensity(), 0.8f);
        CORRADE_COMPARE(data.attenuation(), (Vector3{1.0f, 0.0f, 0.0f}));
        CORRADE_COMPARE(data.range(), Constants::inf());
        CORRADE_COMPARE(data.innerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.outerConeAngle(), 360.0_degf);
        CORRADE_COMPARE(data.importerState(), &a);
    }
}

void LightDataTest::constructInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    auto&& data = ConstructInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    std::ostringstream out;
    Error redirectError{&out};
    LightData{data.type, {}, {}, data.attenuation, data.range, data.innerConeAngle, data.outerConeAngle};
    CORRADE_COMPARE(out.str(), Utility::formatString("Trade::LightData: {}\n", data.message));
}

void LightDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<LightData, const LightData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<LightData, const LightData&>{}));
}

void LightDataTest::constructMove() {
    int state;
    LightData a{LightData::Type::Spot,
        0xccff33_rgbf, 0.8f,
        {0.1f, 0.5f, 0.7f}, 15.0f,
        15.0_degf, 35.0_degf,
        &state};

    LightData b{std::move(a)};
    CORRADE_COMPARE(b.type(), LightData::Type::Spot);
    CORRADE_COMPARE(b.color(), 0xccff33_rgbf);
    CORRADE_COMPARE(b.intensity(), 0.8f);
    CORRADE_COMPARE(b.attenuation(), (Vector3{0.1f, 0.5f, 0.7f}));
    CORRADE_COMPARE(b.range(), 15.0f);
    CORRADE_COMPARE(b.innerConeAngle(), 15.0_degf);
    CORRADE_COMPARE(b.outerConeAngle(), 35.0_degf);
    CORRADE_COMPARE(b.importerState(), &state);

    LightData c{{}, {}, {}};
    c = std::move(a);
    CORRADE_COMPARE(c.type(), LightData::Type::Spot);
    CORRADE_COMPARE(c.color(), 0xccff33_rgbf);
    CORRADE_COMPARE(c.intensity(), 0.8f);
    CORRADE_COMPARE(c.attenuation(), (Vector3{0.1f, 0.5f, 0.7f}));
    CORRADE_COMPARE(c.range(), 15.0f);
    CORRADE_COMPARE(c.innerConeAngle(), 15.0_degf);
    CORRADE_COMPARE(c.outerConeAngle(), 35.0_degf);
    CORRADE_COMPARE(c.importerState(), &state);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<LightData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<LightData>::value);
}

void LightDataTest::debugType() {
    std::ostringstream out;

    Debug(&out) << LightData::Type::Spot << LightData::Type(0xbe);
    CORRADE_COMPARE(out.str(), "Trade::LightData::Type::Spot Trade::LightData::Type(0xbe)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::LightDataTest)
