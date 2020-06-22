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
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct VersionTest: TestSuite::Tester {
    explicit VersionTest();

    void packing();
    void packingMagnumVersion();
    void comparison();

    void debug();

    void configuration();
};

VersionTest::VersionTest() {
    addTests({&VersionTest::packing,
              &VersionTest::packingMagnumVersion,
              &VersionTest::comparison,

              &VersionTest::debug,

              &VersionTest::configuration});
}

void VersionTest::packing() {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
    #endif
    constexpr Version packed = version(1, 5, 789);
    constexpr UnsignedInt major = versionMajor(packed);
    constexpr UnsignedInt minor = versionMinor(packed);
    constexpr UnsignedInt patch = versionPatch(packed);
    CORRADE_COMPARE(packed, Version(VK_MAKE_VERSION(1, 5, 789)));
    CORRADE_COMPARE(major, VK_VERSION_MAJOR(packed));
    CORRADE_COMPARE(minor, VK_VERSION_MINOR(packed));
    CORRADE_COMPARE(patch, VK_VERSION_PATCH(packed));
    CORRADE_COMPARE(major, 1);
    CORRADE_COMPARE(minor, 5);
    CORRADE_COMPARE(patch, 789);
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif
}

void VersionTest::packingMagnumVersion() {
    Version a = version(2019, 10, 1506);
    {
        CORRADE_EXPECT_FAIL("Vulkan version encoding can't fit full years.");
        CORRADE_COMPARE(versionMajor(a), 2019);
    }
    CORRADE_COMPARE(versionMinor(a), 10);
    CORRADE_COMPARE(versionPatch(a), 1506);
}

void VersionTest::comparison() {
    CORRADE_VERIFY(!(version(1, 5, 3) < version(1, 5, 3)));
    CORRADE_VERIFY(!(version(1, 5, 3) > version(1, 5, 3)));

    CORRADE_VERIFY(version(1, 5, 3) < version(1, 5, 4));
    CORRADE_VERIFY(version(1, 5, 4) > version(1, 5, 3));
    CORRADE_VERIFY(!(version(1, 5, 3) > version(1, 5, 4)));
    CORRADE_VERIFY(!(version(1, 5, 4) < version(1, 5, 3)));

    CORRADE_VERIFY(version(1, 5, 3) < version(1, 6, 3));
    CORRADE_VERIFY(version(1, 6, 3) > version(1, 5, 3));
    CORRADE_VERIFY(!(version(1, 5, 3) > version(1, 6, 3)));
    CORRADE_VERIFY(!(version(1, 6, 3) < version(1, 5, 3)));

    CORRADE_VERIFY(version(1, 5, 3) < version(2, 5, 3));
    CORRADE_VERIFY(version(2, 5, 3) > version(1, 5, 3));
    CORRADE_VERIFY(!(version(1, 5, 3) > version(2, 5, 3)));
    CORRADE_VERIFY(!(version(2, 5, 3) < version(1, 5, 3)));

    CORRADE_VERIFY(version(1, 5, 3) <= version(1, 5, 3));
    CORRADE_VERIFY(version(1, 5, 3) >= version(1, 5, 3));

    CORRADE_VERIFY(version(1, 5, 3) <= version(1, 5, 4));
    CORRADE_VERIFY(version(1, 5, 4) >= version(1, 5, 3));
    CORRADE_VERIFY(!(version(1, 5, 3) >= version(1, 5, 4)));
    CORRADE_VERIFY(!(version(1, 5, 4) <= version(1, 5, 3)));

    CORRADE_VERIFY(version(1, 5, 3) <= version(1, 6, 3));
    CORRADE_VERIFY(version(1, 6, 3) >= version(1, 5, 3));
    CORRADE_VERIFY(!(version(1, 5, 3) >= version(1, 6, 3)));
    CORRADE_VERIFY(!(version(1, 6, 3) <= version(1, 5, 3)));

    CORRADE_VERIFY(version(1, 5, 3) <= version(2, 5, 3));
    CORRADE_VERIFY(version(2, 5, 3) >= version(1, 5, 3));
    CORRADE_VERIFY(!(version(1, 5, 3) >= version(2, 5, 3)));
    CORRADE_VERIFY(!(version(2, 5, 3) <= version(1, 5, 3)));
}

void VersionTest::debug() {
    std::ostringstream out;
    Debug{&out} << Version::Vk12 << version(1, 5, 789) << Version::None
        /* packed should omit "Vulkan" */
        << Debug::packed << version(20, 6);
    CORRADE_COMPARE(out.str(), "Vulkan 1.2 Vulkan 1.5.789 Vulkan 1023.1023.4095 20.6\n");
}

void VersionTest::configuration() {
    Utility::Configuration c;

    /* The ideal thing */
    c.setValue("version", "1.1");
    CORRADE_COMPARE(c.value<Version>("version"), Version::Vk11);

    /* Errors */
    c.setValue("version", "");
    CORRADE_COMPARE(c.value<Version>("version"), Version::None);
    c.setValue("version", "1");
    CORRADE_COMPARE(c.value<Version>("version"), Version::None);
    c.setValue("version", "1.");
    CORRADE_COMPARE(c.value<Version>("version"), Version::None);
    c.setValue("version", ".1");
    CORRADE_COMPARE(c.value<Version>("version"), Version::None);

    /* Leading spaces */
    c.setValue("version", "   12.  5");
    CORRADE_COMPARE(c.value<Version>("version"), version(12, 5));

    /* Trailing spaces */
    {
        CORRADE_EXPECT_FAIL("Parsing of trailing spaces not implemented yet.");
        c.setValue("version", "12  .5");
        CORRADE_COMPARE(c.value<Version>("version"), version(12, 5));
        c.setValue("version", "12.5  ");
        CORRADE_COMPARE(c.value<Version>("version"), version(12, 5));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::VersionTest)
