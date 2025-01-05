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

#include <set>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Instance.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct ExtensionsTest: TestSuite::Tester {
    explicit ExtensionsTest();

    void isInstanceExtension();
    void isExtension();

    void constructInstanceExtensionFromCompileTimeExtension();
    void constructExtensionFromCompileTimeExtension();

    void instanceExtensions();
    void extensions();
};

ExtensionsTest::ExtensionsTest() {
    addTests({&ExtensionsTest::isInstanceExtension,
              &ExtensionsTest::isExtension,

              &ExtensionsTest::constructInstanceExtensionFromCompileTimeExtension,
              &ExtensionsTest::constructExtensionFromCompileTimeExtension,

              &ExtensionsTest::instanceExtensions,
              &ExtensionsTest::extensions});
}

void ExtensionsTest::isInstanceExtension() {
    CORRADE_VERIFY(Implementation::IsInstanceExtension<Extensions::KHR::get_physical_device_properties2>::value);
    CORRADE_VERIFY(!Implementation::IsInstanceExtension<Extensions::KHR::external_memory>::value);
    CORRADE_VERIFY(!Implementation::IsInstanceExtension<int>::value);

    /* Variadic check (used in variadic addEnabledExtensions()), check that it
       properly fails for each occurrence of a device extension */
    CORRADE_VERIFY(Implementation::IsInstanceExtension<
        Extensions::KHR::get_physical_device_properties2,
        Extensions::KHR::external_memory_capabilities,
        Extensions::KHR::external_fence_capabilities>::value);
    CORRADE_VERIFY(!Implementation::IsInstanceExtension<
        Extensions::KHR::draw_indirect_count, /* not */
        Extensions::KHR::external_memory_capabilities,
        Extensions::KHR::external_fence_capabilities>::value);
    CORRADE_VERIFY(!Implementation::IsInstanceExtension<
        Extensions::KHR::get_physical_device_properties2,
        Extensions::KHR::external_memory, /* not */
        Extensions::KHR::external_fence_capabilities>::value);
    CORRADE_VERIFY(!Implementation::IsInstanceExtension<
        Extensions::KHR::get_physical_device_properties2,
        Extensions::KHR::external_memory_capabilities,
        Extensions::KHR::external_fence>::value); /* not */

    /* Empty variadic list should return true */
    CORRADE_VERIFY(Implementation::IsInstanceExtension<>::value);
}

void ExtensionsTest::isExtension() {
    CORRADE_VERIFY(Implementation::IsExtension<Extensions::KHR::external_memory>::value);
    CORRADE_VERIFY(!Implementation::IsExtension<Extensions::KHR::external_memory_capabilities>::value);
    CORRADE_VERIFY(!Implementation::IsExtension<int>::value);

    {
        /* Not really a problem right now, but once people hit this we might
           want to guard against this (especially because the Index might be
           out of range) */
        struct GLExtension {
            enum: std::size_t { Index };
        };
        CORRADE_EXPECT_FAIL("GL/AL extensions are not rejected right now.");
        CORRADE_VERIFY(!Implementation::IsExtension<GLExtension>::value);
    }

    /* Variadic check (used in variadic addEnabledExtensions()), check that it
       properly fails for each occurrence of a device extension */
    CORRADE_VERIFY(Implementation::IsExtension<
        Extensions::KHR::external_memory,
        Extensions::KHR::depth_stencil_resolve,
        Extensions::KHR::external_fence>::value);
    CORRADE_VERIFY(!Implementation::IsExtension<
        Extensions::KHR::external_memory_capabilities, /* not */
        Extensions::KHR::depth_stencil_resolve,
        Extensions::KHR::external_fence>::value);
    CORRADE_VERIFY(!Implementation::IsExtension<
        Extensions::KHR::external_memory,
        Extensions::EXT::debug_report, /* not */
        Extensions::KHR::external_fence>::value);
    CORRADE_VERIFY(!Implementation::IsExtension<
        Extensions::KHR::external_memory,
        Extensions::KHR::depth_stencil_resolve,
        Extensions::KHR::external_fence_capabilities>::value); /* not */

    /* Empty variadic list should return true */
    CORRADE_VERIFY(Implementation::IsExtension<>::value);
}

void ExtensionsTest::constructInstanceExtensionFromCompileTimeExtension() {
    InstanceExtension a{Extensions::KHR::get_physical_device_properties2{}};
    CORRADE_COMPARE(a.index(), Extensions::KHR::get_physical_device_properties2::InstanceIndex);
    CORRADE_COMPARE(a.requiredVersion(), Extensions::KHR::get_physical_device_properties2::requiredVersion());
    CORRADE_COMPARE(a.coreVersion(), Extensions::KHR::get_physical_device_properties2::coreVersion());
    CORRADE_COMPARE(a.string(), Extensions::KHR::get_physical_device_properties2::string());

    /* Should be convertible from device extensions, but not instance exts */
    CORRADE_VERIFY(std::is_convertible<Extensions::KHR::get_physical_device_properties2, InstanceExtension>::value);
    CORRADE_VERIFY(!std::is_convertible<Extensions::KHR::external_memory, InstanceExtension>::value);

    /* Shouldn't be convertible from strings to avoid ambiguity in APIs that
       have string/extension overloads */
    CORRADE_VERIFY(!std::is_convertible<Containers::StringView, InstanceExtension>::value);
}

void ExtensionsTest::constructExtensionFromCompileTimeExtension() {
    Extension a{Extensions::KHR::external_memory{}};
    CORRADE_COMPARE(a.index(), Extensions::KHR::external_memory::Index);
    CORRADE_COMPARE(a.requiredVersion(), Extensions::KHR::external_memory::requiredVersion());
    CORRADE_COMPARE(a.coreVersion(), Extensions::KHR::external_memory::coreVersion());
    CORRADE_COMPARE(a.string(), Extensions::KHR::external_memory::string());

    /* Should be convertible from device extensions, but not instance exts */
    CORRADE_VERIFY(std::is_convertible<Extensions::KHR::external_memory, Extension>::value);
    CORRADE_VERIFY(!std::is_convertible<Extensions::KHR::get_physical_device_properties2, Extension>::value);

    /* Shouldn't be convertible from strings to avoid ambiguity in APIs that
       have string/extension overloads */
    CORRADE_VERIFY(!std::is_convertible<Containers::StringView, Extension>::value);
}

void ExtensionsTest::instanceExtensions() {
    Containers::StringView used[Implementation::InstanceExtensionCount]{};

    std::set<Containers::StringView> unique;

    /* Check that all extension indices are unique, are in correct lists, are
       listed just once etc. */
    for(Version version: {
        Version::Vk10,
        Version::Vk11,
        Version::Vk12,
        Version::None})
    {
        Containers::StringView previous;
        for(const InstanceExtension& e: InstanceExtension::extensions(version)) {
            CORRADE_ITERATION(e.string());

            /* Binary search is performed on each list to find known
               extensions, so the exts have to be sorted */
            CORRADE_FAIL_IF(!previous.isEmpty() && previous >= e.string(),
                "Extension not sorted after" << previous);

            CORRADE_FAIL_IF(e.index() >= Implementation::InstanceExtensionCount,
                "Index" << e.index() << "larger than" << Implementation::InstanceExtensionCount);

            CORRADE_FAIL_IF(used[e.index()] != nullptr,
                "Index" << e.index() << "already used by" << used[e.index()]);

            used[e.index()] = e.string();
            CORRADE_FAIL_IF(!unique.insert(e.string()).second,
                "Extension listed more than once");

            CORRADE_COMPARE_AS(e.coreVersion(), e.requiredVersion(), TestSuite::Compare::GreaterOrEqual);
            CORRADE_FAIL_IF(e.coreVersion() != version,
                "Extension should have core version" << version << "but has" << e.coreVersion());

            previous = e.string();
        }
    }
}

void ExtensionsTest::extensions() {
    Containers::StringView used[Implementation::ExtensionCount]{};

    std::set<Containers::StringView> unique;

    /* Check that all extension indices are unique, are in correct lists, are
       not compiled on versions that shouldn't have them, are listed just once
       etc. */
    for(Version version: {
        Version::Vk10,
        Version::Vk11,
        Version::Vk12,
        Version::None})
    {
        Containers::StringView previous;
        for(const Extension& e: Extension::extensions(version)) {
            CORRADE_ITERATION(e.string());

            /* Binary search is performed on each list to find known
               extensions, so the exts have to be sorted */
            CORRADE_FAIL_IF(!previous.isEmpty() && previous >= e.string(),
                "Extension not sorted after" << previous);

            CORRADE_FAIL_IF(e.index() >= Implementation::ExtensionCount,
                "Index" << e.index() << "larger than" << Implementation::ExtensionCount);

            CORRADE_FAIL_IF(used[e.index()] != nullptr,
                "Index" << e.index() << "already used by" << used[e.index()]);

            used[e.index()] = e.string();
            CORRADE_FAIL_IF(!unique.insert(e.string()).second,
                "Extension listed more than once");

            CORRADE_COMPARE_AS(e.coreVersion(), e.requiredVersion(), TestSuite::Compare::GreaterOrEqual);
            CORRADE_FAIL_IF(e.coreVersion() != version,
                "Extension should have core version" << version << "but has" << e.coreVersion());

            previous = e.string();
        }
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::ExtensionsTest)
