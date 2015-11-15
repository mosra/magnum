/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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

#include "Magnum/Audio/Extensions.h"
#include "Magnum/Audio/Context.h"

namespace Magnum { namespace Audio { namespace Test {

struct ContextTest: TestSuite::Tester {
    explicit ContextTest();

    void extensionsString();
    void isExtensionEnabled();
    void hrtfStatus();
    void hrtfs();

    Context _context;
};

ContextTest::ContextTest() {
    addTests({&ContextTest::extensionsString,
              &ContextTest::isExtensionEnabled,
              &ContextTest::hrtfStatus});
}

void ContextTest::extensionsString() {
    std::vector<std::string> extensions = _context.extensionStrings();

    CORRADE_VERIFY(extensions.size() > 0);
}

void ContextTest::isExtensionEnabled() {
    CORRADE_VERIFY(Context::current()->isExtensionSupported<Extensions::ALC::EXT::ENUMERATION>());
}

void ContextTest::hrtfStatus() {
    std::ostringstream out;
    Debug(&out) << Context::HrtfStatus::Denied;
    CORRADE_COMPARE(out.str(), "Audio::Context::HrtfStatus::Denied\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::ContextTest)
