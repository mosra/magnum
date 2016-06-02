#ifndef Magnum_Test_AbstractVulkanTester_h
#define Magnum_Test_AbstractVulkanTester_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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

using namespace Corrade;

namespace Magnum { namespace Vk { namespace Test {

class AbstractVulkanTester: public TestSuite::Tester {
    public:
        explicit AbstractVulkanTester();

    private:

};

AbstractVulkanTester::AbstractVulkanTester(): TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}.setSkippedArgumentPrefixes({"magnum"})} {

}

#define MAGNUM_VERIFY_NO_ERROR() CORRADE_COMPARE(Magnum::Renderer::error(), Magnum::Renderer::Error::NoError)

#if defined(CORRADE_TESTSUITE_TARGET_XCTEST)
#define MAGNUM_VK_TEST_MAIN(Class)                                          \
    int CORRADE_VISIBILITY_EXPORT corradeTestMain(int argc, char** argv) {  \
        Class t;                                                            \
        t.registerTest(__FILE__, #Class);                                   \
        return t.exec(argc, argv);                                          \
    }
#else
#define MAGNUM_VK_TEST_MAIN(Class)                                          \
    int main(int argc, char** argv) {                                       \
        Class t;                                                            \
        t.registerTest(__FILE__, #Class);                                   \
        return t.exec(argc, argv);                                          \
    }
#endif


}}}

#endif
