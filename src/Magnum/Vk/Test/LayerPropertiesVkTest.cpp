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
#include <Corrade/Containers/StringStl.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/Version.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct LayerPropertiesVkTest: TestSuite::Tester {
    explicit LayerPropertiesVkTest();

    void constructMove();

    void enumerate();
    void outOfRange();
    void isSupported();
};

LayerPropertiesVkTest::LayerPropertiesVkTest() {
    addTests({&LayerPropertiesVkTest::constructMove,

              &LayerPropertiesVkTest::enumerate,
              &LayerPropertiesVkTest::outOfRange,
              &LayerPropertiesVkTest::isSupported});
}

using namespace Containers::Literals;

void LayerPropertiesVkTest::constructMove() {
    LayerProperties a = enumerateLayerProperties();
    const UnsignedInt count = a.count();
    if(!count) CORRADE_SKIP("No extensions reported, can't test");

    LayerProperties b = std::move(a);
    CORRADE_COMPARE(b.count(), count);

    LayerProperties c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.count(), count);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<LayerProperties>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<LayerProperties>::value);
}

void LayerPropertiesVkTest::enumerate() {
    LayerProperties properties = enumerateLayerProperties();

    if(!properties.count())
        CORRADE_SKIP("The driver reported no instance layers, can't test.");

    CORRADE_COMPARE(properties.count(), properties.names().size());
    Debug{} << "Available instance layers:" << properties.names();

    Containers::ArrayView<const Containers::StringView> names = properties.names();
    CORRADE_COMPARE_AS(names.size(), 0, TestSuite::Compare::Greater);
    /* The list should be sorted */
    for(std::size_t i = 1; i != names.size(); ++i) {
        CORRADE_COMPARE_AS(names[i - 1], names[i],
            TestSuite::Compare::Less);
    }

    CORRADE_COMPARE_AS(properties.name(0).size(), "VK_LAYER_"_s.size(),
        TestSuite::Compare::Greater);
    CORRADE_COMPARE_AS(properties.revision(0), 0,
        TestSuite::Compare::Greater);
    CORRADE_COMPARE_AS(properties.version(0), Version::Vk10,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(properties.description(0).size(), 10,
        TestSuite::Compare::Greater);
}

void LayerPropertiesVkTest::outOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    LayerProperties properties = enumerateLayerProperties();
    const UnsignedInt count = properties.count();

    std::ostringstream out;
    Error redirectError{&out};
    properties.name(count);
    properties.revision(count);
    properties.version(count);
    properties.description(count);
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Vk::LayerProperties::name(): index {0} out of range for {0} entries\n"
        "Vk::LayerProperties::revision(): index {0} out of range for {0} entries\n"
        "Vk::LayerProperties::version(): index {0} out of range for {0} entries\n"
        "Vk::LayerProperties::description(): index {0} out of range for {0} entries\n", count));
}

void LayerPropertiesVkTest::isSupported() {
    LayerProperties properties = enumerateLayerProperties();

    CORRADE_VERIFY(!properties.isSupported("this layer doesn't exist"));

    if(!properties.count())
        CORRADE_SKIP("The driver reported no instance layers, can't fully test.");

    for(UnsignedInt i = 0; i != properties.count(); ++i) {
        CORRADE_ITERATION(properties.name(i));
        CORRADE_VERIFY(properties.isSupported(properties.name(i)));
    }

    /* Verify that we're not just comparing a prefix */
    const std::string layer = std::string(properties.name(0)) + "_hello";
    CORRADE_VERIFY(!properties.isSupported(layer));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::LayerPropertiesVkTest)
