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

#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Version.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ExtensionPropertiesVkTest: TestSuite::Tester {
    explicit ExtensionPropertiesVkTest();

    void constructMove();
    /* Device extension move tested in DevicePropertiesVkTest */

    void enumerateInstance();
    void enumerateInstanceWithKhronosValidationLayer();
    void enumerateInstanceNonexistentLayer();
    void instanceExtensionIsSupported();

    /* Device extensions tested in DevicePropertiesVkTest */
    void outOfRange();
    void namedRevision();
};

ExtensionPropertiesVkTest::ExtensionPropertiesVkTest() {
    addTests({&ExtensionPropertiesVkTest::constructMove,

              &ExtensionPropertiesVkTest::enumerateInstance,
              &ExtensionPropertiesVkTest::enumerateInstanceWithKhronosValidationLayer,
              &ExtensionPropertiesVkTest::enumerateInstanceNonexistentLayer,
              &ExtensionPropertiesVkTest::instanceExtensionIsSupported,

              &ExtensionPropertiesVkTest::outOfRange,
              &ExtensionPropertiesVkTest::namedRevision});
}

using namespace Containers::Literals;

void ExtensionPropertiesVkTest::constructMove() {
    InstanceExtensionProperties a = enumerateInstanceExtensionProperties();
    const UnsignedInt count = a.count();
    if(!count) CORRADE_SKIP("No extensions reported, can't test");

    InstanceExtensionProperties b = std::move(a);
    CORRADE_COMPARE(b.count(), count);

    InstanceExtensionProperties c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.count(), count);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<InstanceExtensionProperties>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<InstanceExtensionProperties>::value);
}

void ExtensionPropertiesVkTest::enumerateInstance() {
    InstanceExtensionProperties properties = enumerateInstanceExtensionProperties();
    Debug{} << "Available instance extension count:" << properties.names().size();

    CORRADE_COMPARE_AS(properties.count(), 0, TestSuite::Compare::Greater);
    for(std::size_t i = 0; i != properties.count(); ++i) {
        using namespace Containers::Literals;
        CORRADE_ITERATION(properties.name(i));
        CORRADE_COMPARE_AS(properties.name(i).size(), "VK_"_s.size(),
            TestSuite::Compare::Greater);
        CORRADE_COMPARE_AS(properties.revision(i), 0,
            TestSuite::Compare::Greater);
        /* All extensions are from the global layer */
        CORRADE_COMPARE(properties.layer(i), 0);
    }

    /* The extension list should be sorted and unique (so Less, not
       LessOrEqual) */
    Containers::ArrayView<const Containers::StringView> extensions = properties.names();
    for(std::size_t i = 1; i != extensions.size(); ++i) {
        CORRADE_COMPARE_AS(extensions[i - 1], extensions[i],
            TestSuite::Compare::Less);
    }
}

void ExtensionPropertiesVkTest::enumerateInstanceWithKhronosValidationLayer() {
    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");

    /* There should be more extensions with this layer enabled */
    InstanceExtensionProperties global = enumerateInstanceExtensionProperties();
    InstanceExtensionProperties withKhronosValidation = enumerateInstanceExtensionProperties({"VK_LAYER_KHRONOS_validation"});
    CORRADE_COMPARE_AS(global.count(),
        withKhronosValidation.count(),
        TestSuite::Compare::Less);

    /* The extension list should be sorted even including the extra layers, and
       unique (so Less, not LessOrEqual) */
    Containers::ArrayView<const Containers::StringView> extensions = withKhronosValidation.names();
    for(std::size_t i = 1; i != extensions.size(); ++i) {
        CORRADE_COMPARE_AS(extensions[i - 1], extensions[i],
            TestSuite::Compare::Less);
    }

    /* The VK_LAYER_KHRONOS_validation adds extensions that are supported
       globally, which means extensionCount() should be larger than
       extensions.size() as it has some duplicates */
    CORRADE_COMPARE_AS(withKhronosValidation.count(), extensions.size(),
        TestSuite::Compare::Greater);

    /* The last extension should be from the validation layer */
    CORRADE_COMPARE(withKhronosValidation.layer(0), 0);
    CORRADE_COMPARE(withKhronosValidation.layer(withKhronosValidation.count() - 1), 1);

    /* VK_EXT_validation_features is only in the layer */
    CORRADE_VERIFY(!global.isSupported("VK_EXT_validation_features"));
    CORRADE_VERIFY(withKhronosValidation.isSupported("VK_EXT_validation_features"));
}

void ExtensionPropertiesVkTest::enumerateInstanceNonexistentLayer() {
    CORRADE_SKIP("Currently this hits an internal assert, which can't be tested.");

    std::ostringstream out;
    Error redirectError{&out};
    enumerateInstanceExtensionProperties({"VK_LAYER_this_doesnt_exist"});
    CORRADE_COMPARE(out.str(), "TODO");
}

void ExtensionPropertiesVkTest::instanceExtensionIsSupported() {
    InstanceExtensionProperties properties = enumerateInstanceExtensionProperties();
    CORRADE_COMPARE_AS(properties.count(), 0, TestSuite::Compare::Greater);

    for(UnsignedInt i = 0; i != properties.count(); ++i) {
        CORRADE_ITERATION(properties.name(i));
        CORRADE_VERIFY(properties.isSupported(properties.name(i)));
    }

    CORRADE_VERIFY(!properties.isSupported("VK_this_doesnt_exist"));

    /* Verify that we're not just comparing a prefix */
    const std::string extension = std::string(properties.name(0)) + "_hello";
    CORRADE_VERIFY(!properties.isSupported(extension));

    /* This extension should be available almost always */
    if(!properties.isSupported("VK_KHR_get_physical_device_properties2"))
        CORRADE_SKIP("VK_KHR_get_physical_device_properties2 not supported, can't fully test");

    /* Verify the overloads that take our extension wrappers work as well */
    CORRADE_VERIFY(properties.isSupported<Extensions::KHR::get_physical_device_properties2>());
    CORRADE_VERIFY(properties.isSupported(Extensions::KHR::get_physical_device_properties2{}));
}

void ExtensionPropertiesVkTest::outOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    InstanceExtensionProperties properties = enumerateInstanceExtensionProperties();
    const UnsignedInt count = properties.count();

    std::ostringstream out;
    Error redirectError{&out};
    properties.name(count);
    properties.revision(count);
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Vk::ExtensionProperties::name(): index {0} out of range for {0} entries\n"
        "Vk::ExtensionProperties::revision(): index {0} out of range for {0} entries\n", count));
}

void ExtensionPropertiesVkTest::namedRevision() {
    InstanceExtensionProperties properties = enumerateInstanceExtensionProperties();
    /** @todo use Extensions::KHR::surface once the extension is recognized */
    if(!properties.isSupported("VK_KHR_surface"))
        CORRADE_SKIP("VK_KHR_surface not supported, can't test");
    if(!properties.isSupported<Extensions::KHR::get_physical_device_properties2>())
        CORRADE_SKIP("VK_KHR_get_physical_device_properties2 not supported, can't test");

    /* It was at revision 25 in January 2016, which is four months before
       Vulkan first came out, so it's safe to assume all drivers have this
       revision by now */
    CORRADE_COMPARE_AS(properties.revision("VK_KHR_surface"), 25,
        TestSuite::Compare::GreaterOrEqual);

    /* Unknown extensions return 0 */
    CORRADE_COMPARE(properties.revision("VK_this_doesnt_exist"), 0);

    /* Verify the overloads that take our extension wrappers work as well */
    CORRADE_COMPARE_AS(properties.revision<Extensions::KHR::get_physical_device_properties2>(), 0,
        TestSuite::Compare::Greater);
    CORRADE_COMPARE_AS(properties.revision(Extensions::KHR::get_physical_device_properties2{}), 0,
        TestSuite::Compare::Greater);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ExtensionPropertiesVkTest)
