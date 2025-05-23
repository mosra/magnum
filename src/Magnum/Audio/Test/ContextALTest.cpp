/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Audio/Extensions.h"
#include "Magnum/Audio/Context.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

struct ContextALTest: TestSuite::Tester {
    explicit ContextALTest();

    void constructDefault();
    void constructConfiguration();
    void constructDeviceNotFound();
    void constructMove();

    void quietLog();
    void ignoreUnrelatedOptions();

    void extensionsString();
    void isExtensionSupported();
    void isExtensionUnsupported();
    void isExtensionDisabled();
};

ContextALTest::ContextALTest():
    TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}
        .setSkippedArgumentPrefixes({"magnum"})}
{
    addTests({&ContextALTest::constructDefault,
              &ContextALTest::constructConfiguration,
              &ContextALTest::constructDeviceNotFound,
              &ContextALTest::constructMove});

    addInstancedTests({&ContextALTest::quietLog}, 2);

    addTests({&ContextALTest::ignoreUnrelatedOptions,
              &ContextALTest::extensionsString,
              &ContextALTest::isExtensionSupported,
              &ContextALTest::isExtensionUnsupported,
              &ContextALTest::isExtensionDisabled});
}

void ContextALTest::constructDefault() {
    CORRADE_VERIFY(!Context::hasCurrent());

    {
        Context context{arguments().first(), arguments().second()};
        CORRADE_VERIFY(Context::hasCurrent());
        CORRADE_COMPARE(&Context::current(), &context);

        /* Verify the queries make sense */
        #ifdef CORRADE_TARGET_APPLE
        if(context.vendorString() == "Apple Computer Inc.")
            CORRADE_WARN("Apple's OpenAL implementation returns bogus values. Sometimes. With a random value every call. Skipping the context properties comparison.");
        else
        #endif
        {
            CORRADE_COMPARE_AS(context.frequency(), 10000,
                TestSuite::Compare::Greater);
            CORRADE_COMPARE_AS(context.monoSourceCount(), 2,
                TestSuite::Compare::GreaterOrEqual);
            CORRADE_COMPARE_AS(context.stereoSourceCount(), 1,
                TestSuite::Compare::GreaterOrEqual);
            CORRADE_COMPARE_AS(context.refreshRate(), 10,
                TestSuite::Compare::Greater);
        }
    }

    CORRADE_VERIFY(!Context::hasCurrent());
}

void ContextALTest::constructConfiguration() {
    CORRADE_VERIFY(!Context::hasCurrent());

    {
        Context context{
            Context::Configuration{}
                .setFrequency(22050)
                .setHrtf(Context::Configuration::Hrtf::Enabled)
                .setMonoSourceCount(5)
                .setStereoSourceCount(4)
                .setRefreshRate(25),
            arguments().first(), arguments().second()
        };
        CORRADE_VERIFY(Context::hasCurrent());
        CORRADE_COMPARE(&Context::current(), &context);

        /* Verify the queries make sense. All of these are just hints so the
           actual value used could be higher. */
        #ifdef CORRADE_TARGET_APPLE
        if(context.vendorString() == "Apple Computer Inc.")
            CORRADE_WARN("Apple's OpenAL implementation returns bogus values. Sometimes. With a random value every call. Skipping the context properties comparison.");
        else
        #endif
        {
            CORRADE_COMPARE_AS(context.frequency(), 22050,
                TestSuite::Compare::GreaterOrEqual);
            CORRADE_COMPARE_AS(context.monoSourceCount(), 5,
                TestSuite::Compare::GreaterOrEqual);
            CORRADE_COMPARE(context.stereoSourceCount(), 4);
            CORRADE_COMPARE_AS(context.refreshRate(), 25,
                TestSuite::Compare::GreaterOrEqual);

            /* HRTF gets enabled only if the extension is supported */
            if(context.isExtensionSupported<Extensions::ALC::SOFT::HRTF>()) {
                CORRADE_COMPARE(context.hrtfStatus(), Context::HrtfStatus::Enabled);
                CORRADE_VERIFY(!context.hrtfSpecifierString().empty());
            } else if(context.isExtensionSupported<Extensions::ALC::SOFTX::HRTF>()) {
                CORRADE_COMPARE(context.hrtfStatus(), Context::HrtfStatus::Enabled);
                CORRADE_VERIFY(context.hrtfSpecifierString().empty());
            } else {
                CORRADE_COMPARE(context.hrtfStatus(), Context::HrtfStatus::Disabled);
            }
        }
    }

    CORRADE_VERIFY(!Context::hasCurrent());
}

void ContextALTest::constructDeviceNotFound() {
    CORRADE_VERIFY(!Context::hasCurrent());

    {
        Context context{NoCreate, arguments().first(), arguments().second()};
        {
            #ifdef CORRADE_TARGET_APPLE
            CORRADE_EXPECT_FAIL_IF(context.vendorString() == "Apple Computer Inc.",
                "Apple's OpenAL implementation doesn't fail if invalid device specifier is set.");
            #endif
            CORRADE_VERIFY(!context.tryCreate(Context::Configuration{}
                .setDeviceSpecifier("hello this definitely doesn't exist")));
            CORRADE_VERIFY(!Context::hasCurrent());
        }
    }

    CORRADE_VERIFY(!Context::hasCurrent());
}

void ContextALTest::constructMove() {
    Context context;
    CORRADE_COMPARE(&Context::current(), &context);
    Int frequency = context.frequency();

    {
        Context second{Utility::move(context)};
        CORRADE_COMPARE(&Context::current(), &second);
        #ifdef CORRADE_TARGET_APPLE
        if(context.vendorString() == "Apple Computer Inc.")
            CORRADE_WARN("Apple's OpenAL implementation returns bogus values. Sometimes. With a random value every call. Skipping the context properties comparison.");
        else
        #endif
        {
            CORRADE_COMPARE(second.frequency(), frequency);
        }
    }

    CORRADE_VERIFY(!Context::hasCurrent());
}

void ContextALTest::quietLog() {
    setTestCaseDescription(testCaseInstanceId() ? "true" : "false");

    const char* argv[] = { "", "--magnum-log", testCaseInstanceId() ? "quiet" : "default" };

    Containers::String out;
    Debug redirectOutput{&out};
    /* MSVC 2015 and 2017 needs the int cast otherwise C2398 */
    Context context{int(Containers::arraySize(argv)), argv};
    CORRADE_COMPARE(!out, bool(testCaseInstanceId()));
}

void ContextALTest::ignoreUnrelatedOptions() {
    const char* argv[] = { "", "--magnum-gpu-validation", "on" };

    /* MSVC 2015 and 2017 needs the int cast otherwise C2398 */
    Context context{int(Containers::arraySize(argv)), argv};
    CORRADE_VERIFY(Context::hasCurrent());
}

void ContextALTest::extensionsString() {
    Context context;

    std::vector<std::string> extensions = context.extensionStrings();

    CORRADE_VERIFY(!extensions.empty());
}

void ContextALTest::isExtensionSupported() {
    Context context;
    CORRADE_VERIFY(context.isExtensionSupported<Extensions::ALC::EXT::ENUMERATION>());
    CORRADE_VERIFY(!context.isExtensionDisabled<Extensions::ALC::EXT::ENUMERATION>());

    Extension e{Extensions::ALC::EXT::ENUMERATION::Index,
                Extensions::ALC::EXT::ENUMERATION::string()};
    CORRADE_VERIFY(context.isExtensionSupported(e));
    CORRADE_VERIFY(!context.isExtensionDisabled(e));
}

void ContextALTest::isExtensionUnsupported() {
    Context context;

    if(context.isExtensionSupported<Extensions::ALC::SOFTX::HRTF>())
        CORRADE_SKIP("Extension" << Extensions::ALC::SOFTX::HRTF::string() << "is supported, can't test.");

    CORRADE_VERIFY(!context.isExtensionSupported<Extensions::ALC::SOFTX::HRTF>());
    CORRADE_VERIFY(!context.isExtensionDisabled<Extensions::ALC::SOFTX::HRTF>());

    Extension e{Extensions::ALC::SOFTX::HRTF::Index,
                Extensions::ALC::SOFTX::HRTF::string()};
    CORRADE_VERIFY(!context.isExtensionSupported(e));
    CORRADE_VERIFY(!context.isExtensionDisabled(e));
}

void ContextALTest::isExtensionDisabled() {
    /* Yes, FFS. this is a weird-ass name. Also adding an unknown extension
       name that should be ignored. */
    const char* argv[] = { "", "--magnum-disable-extensions", "ALC_ENUMERATION_EXT AL_THIS_IS_NO_EXTENSION" };
    /* MSVC 2015 and 2017 needs the int cast otherwise C2398 */
    Context context{int(Containers::arraySize(argv)), argv};
    CORRADE_VERIFY(!context.isExtensionSupported<Extensions::ALC::EXT::ENUMERATION>());
    CORRADE_VERIFY(context.isExtensionDisabled<Extensions::ALC::EXT::ENUMERATION>());

    Extension e{Extensions::ALC::EXT::ENUMERATION::Index,
                Extensions::ALC::EXT::ENUMERATION::string()};
    CORRADE_VERIFY(!context.isExtensionSupported(e));
    CORRADE_VERIFY(context.isExtensionDisabled(e));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::ContextALTest)
