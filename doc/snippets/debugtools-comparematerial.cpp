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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MaterialData.h"
#include "Magnum/DebugTools/CompareMaterial.h"

using namespace Magnum;

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

struct MaterialTest: TestSuite::Tester {
    explicit MaterialTest();

    void conversion();
};

MaterialTest::MaterialTest() {
    addTests({&MaterialTest::conversion});
}

void MaterialTest::conversion() {
    Trade::MaterialData actual{Trade::MaterialType::PbrClearCoat|Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColor, Color4{0.851206f, 0.687386f, 0.416013f}},
        {Trade::MaterialAttribute::Metalness, 0.603401f},
        {Trade::MaterialAttribute::Roughness, 0.105112f},
        {Trade::MaterialAttribute::DoubleSided, true},
        {Trade::MaterialLayer::ClearCoat},
        {Trade::MaterialAttribute::LayerFactor, 0.02f},
        {Trade::MaterialAttribute::Roughness, 0.320856f},
    }, {4, 7}};

    Trade::MaterialData expected{Trade::MaterialType::PbrMetallicRoughness, {
        {Trade::MaterialAttribute::BaseColor, Color4{0.851206f, 0.687386f, 0.416013f}},
        {Trade::MaterialAttribute::Metalness, 0.603401f},
        {Trade::MaterialAttribute::Roughness, 0.105112f},
        {Trade::MaterialAttribute::DoubleSided, false}
    }};

/* [usage] */
CORRADE_COMPARE_AS(actual, expected, DebugTools::CompareMaterial);
/* [usage] */
}

CORRADE_TEST_MAIN(MaterialTest)
