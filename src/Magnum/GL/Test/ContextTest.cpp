/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <sstream>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Version.h"

namespace Magnum { namespace GL { namespace Test { namespace {

using namespace Containers::Literals;

struct ContextTest: TestSuite::Tester {
    explicit ContextTest();

    void isExtension();

    void configurationConstruct();
    void configurationConstructUnknownWorkaround();
    void configurationConstructCopy();
    void configurationConstructMove();

    void constructNoCreate();
    void constructCopy();

    void makeCurrentNoOp();

    void extensions();

    void debugFlag();
    void debugFlags();

    void debugDetectedDriver();
    void debugDetectedDrivers();
};

ContextTest::ContextTest() {
    addTests({&ContextTest::isExtension,

              &ContextTest::configurationConstruct,
              &ContextTest::configurationConstructUnknownWorkaround,
              &ContextTest::configurationConstructCopy,
              &ContextTest::configurationConstructMove,

              &ContextTest::constructNoCreate,
              &ContextTest::constructCopy,

              &ContextTest::makeCurrentNoOp,

              &ContextTest::extensions,

              &ContextTest::debugFlag,
              &ContextTest::debugFlags,

              &ContextTest::debugDetectedDriver,
              &ContextTest::debugDetectedDrivers});
}

void ContextTest::isExtension() {
    CORRADE_VERIFY(Implementation::IsExtension<Extensions::EXT::texture_filter_anisotropic>::value);
    CORRADE_VERIFY(!Implementation::IsExtension<Extension>::value);
    CORRADE_VERIFY(!Implementation::IsExtension<int>::value);

    {
        /* Not really a problem right now, but once people hit this we might
           want to guard against this (especially because the Index might be
           out of bounds) */
        struct ALExtension {
            enum: std::size_t { Index };
        };
        CORRADE_EXPECT_FAIL("AL/Vk extensions are not rejected right now.");
        CORRADE_VERIFY(!Implementation::IsExtension<ALExtension>::value);
    }

    /* Variadic check (used in variadic Configuration::addDisabledExtensions()),
       check that it properly fails for each occurence of a non-extension */
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_VERIFY((Implementation::IsExtension<
        Extensions::KHR::debug,
        Extensions::EXT::texture_filter_anisotropic,
        Extensions::KHR::texture_compression_astc_hdr>::value));
    CORRADE_VERIFY(!(Implementation::IsExtension<
        Extension,
        Extensions::KHR::debug,
        Extensions::EXT::texture_filter_anisotropic>::value));
    CORRADE_VERIFY(!(Implementation::IsExtension<
        Extensions::KHR::debug,
        Extension,
        Extensions::EXT::texture_filter_anisotropic>::value));
    CORRADE_VERIFY(!(Implementation::IsExtension<
        Extensions::KHR::debug,
        Extensions::EXT::texture_filter_anisotropic,
        Extension>::value));
    #else
    CORRADE_VERIFY((Implementation::IsExtension<
        Extensions::OES::texture_float_linear,
        Extensions::EXT::texture_filter_anisotropic,
        Extensions::WEBGL::compressed_texture_s3tc>::value));
    CORRADE_VERIFY(!(Implementation::IsExtension<
        Extension,
        Extensions::OES::texture_float_linear,
        Extensions::EXT::texture_filter_anisotropic>::value));
    CORRADE_VERIFY(!(Implementation::IsExtension<
        Extensions::OES::texture_float_linear,
        Extension,
        Extensions::EXT::texture_filter_anisotropic>::value));
    CORRADE_VERIFY(!(Implementation::IsExtension<
        Extensions::OES::texture_float_linear,
        Extensions::EXT::texture_filter_anisotropic,
        Extension>::value));
    #endif

    /* Empty variadic list should return true */
    CORRADE_VERIFY(Implementation::IsExtension<>::value);
}

void ContextTest::configurationConstruct() {
    /* In order to verify the string literals get properly interned I could
       make them non-global (by converting from const char*) and then test that
       they are global and with a different pointer. However, compilers are
       clever and on static builds they could just deduplicate the literals,
       which would cause this test to fail. Instead I make them
       non-null-terminated which blocks the compiler from combining them
       together. */
    #ifndef MAGNUM_TARGET_GLES
    const Containers::StringView a = "no-layout-qualifiers-on-old-glsl!"_s.except(1);
    const Containers::StringView b = "nv-compressed-block-size-in-bits!"_s.except(1);
    const Containers::StringView c = "nv-cubemap-inconsistent-compressed-image-size!"_s.except(1);
    #elif !defined(MAGNUM_TARGET_WEBGL)
    const Containers::StringView a = "swiftshader-no-empty-egl-context-flags!"_s.except(1);
    const Containers::StringView b = "swiftshader-egl-context-needs-pbuffer!"_s.except(1);
    const Containers::StringView c = "angle-chatty-shader-compiler!"_s.except(1);
    #else
    /* No general WebGL workarounds to test */
    #endif

    Context::Configuration configuration;
    configuration
        .setFlags(Context::Configuration::Flag::GpuValidation|
                  Context::Configuration::Flag::VerboseLog)
        #ifndef MAGNUM_TARGET_WEBGL
        .addDisabledWorkarounds({a, b})
        .addDisabledWorkarounds({c})
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        .addDisabledExtensions({Extensions::EXT::texture_filter_anisotropic{},
                                Extensions::KHR::debug{}})
        .addDisabledExtensions<Extensions::KHR::robustness,
                               Extensions::KHR::texture_compression_astc_hdr>()
        #else
        .addDisabledExtensions({Extensions::EXT::texture_filter_anisotropic{},
                                Extensions::EXT::texture_compression_rgtc{}})
        .addDisabledExtensions<Extensions::EXT::float_blend,
                               Extensions::OES::texture_float_linear>()
        #endif
        ;

    CORRADE_COMPARE(UnsignedLong(configuration.flags()), UnsignedLong(
        Context::Configuration::Flag::GpuValidation|
        Context::Configuration::Flag::VerboseLog));

    /* The workaround strings should get interned */
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE_AS(configuration.disabledWorkarounds(),
        Containers::arrayView({a, b, c}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(configuration.disabledWorkarounds()[0].data() != a.data());
    CORRADE_VERIFY(configuration.disabledWorkarounds()[1].data() != b.data());
    CORRADE_VERIFY(configuration.disabledWorkarounds()[2].data() != c.data());
    CORRADE_COMPARE(configuration.disabledWorkarounds()[0].flags(),
        Containers::StringViewFlag::Global|Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(configuration.disabledWorkarounds()[1].flags(),
        Containers::StringViewFlag::Global|Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(configuration.disabledWorkarounds()[2].flags(),
        Containers::StringViewFlag::Global|Containers::StringViewFlag::NullTerminated);
    #endif

    CORRADE_COMPARE(configuration.disabledExtensions().size(), 4);
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(configuration.disabledExtensions()[0].index(), Extensions::EXT::texture_filter_anisotropic::Index);
    CORRADE_COMPARE(configuration.disabledExtensions()[1].index(), Extensions::KHR::debug::Index);
    CORRADE_COMPARE(configuration.disabledExtensions()[2].index(), Extensions::KHR::robustness::Index);
    CORRADE_COMPARE(configuration.disabledExtensions()[3].index(), Extensions::KHR::texture_compression_astc_hdr::Index);
    #else
    CORRADE_COMPARE(configuration.disabledExtensions()[0].index(), Extensions::EXT::texture_filter_anisotropic::Index);
    CORRADE_COMPARE(configuration.disabledExtensions()[1].index(), Extensions::EXT::texture_compression_rgtc::Index);
    CORRADE_COMPARE(configuration.disabledExtensions()[2].index(), Extensions::EXT::float_blend::Index);
    CORRADE_COMPARE(configuration.disabledExtensions()[3].index(), Extensions::OES::texture_float_linear::Index);
    #endif
}

void ContextTest::configurationConstructUnknownWorkaround() {
    Context::Configuration configuration;

    /* Unknown workarounds should get ignored -- we're storing views on
       internally known workaround strings to avoid allocations so there's no
       other way */
    std::ostringstream out;
    Warning redirectWarning{&out};
    configuration.addDisabledWorkarounds({"all-drivers-are-shit"});
    CORRADE_VERIFY(configuration.disabledWorkarounds().empty());
    CORRADE_COMPARE(out.str(), "GL::Context::Configuration::addDisabledWorkarounds(): unknown workaround all-drivers-are-shit\n");
}

void ContextTest::configurationConstructCopy() {
    #ifndef MAGNUM_TARGET_GLES
    Containers::StringView workaround = "no-layout-qualifiers-on-old-glsl";
    Containers::StringView another = "nv-compressed-block-size-in-bits";
    #elif !defined(MAGNUM_TARGET_WEBGL)
    Containers::StringView workaround = "swiftshader-no-empty-egl-context-flags";
    Containers::StringView another = "angle-chatty-shader-compiler";
    #else
    /* No general WebGL workarounds to test */
    #endif

    Context::Configuration a;
    a.setFlags(Context::Configuration::Flag::VerboseLog)
     #ifndef MAGNUM_TARGET_WEBGL
     .addDisabledWorkarounds({workaround})
     #endif
     .addDisabledExtensions<Extensions::EXT::texture_filter_anisotropic>();

    Context::Configuration b = a;
    CORRADE_COMPARE(UnsignedLong(b.flags()), UnsignedLong(Context::Configuration::Flag::VerboseLog));
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE_AS(b.disabledWorkarounds(),
        Containers::arrayView({workaround}),
        TestSuite::Compare::Container);
    #endif
    CORRADE_COMPARE(b.disabledExtensions().size(), 1);
    CORRADE_COMPARE(b.disabledExtensions()[0].index(), Extensions::EXT::texture_filter_anisotropic::Index);

    Context::Configuration c;
    c.setFlags(Context::Configuration::Flag::QuietLog)
     #ifndef MAGNUM_TARGET_WEBGL
     .addDisabledWorkarounds({another})
     .addDisabledExtensions<Extensions::KHR::debug>()
     #else
     .addDisabledExtensions<Extensions::OES::texture_float_linear>()
     #endif
     ;

    c = b;
    CORRADE_COMPARE(UnsignedLong(c.flags()), UnsignedLong(Context::Configuration::Flag::VerboseLog));
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE_AS(c.disabledWorkarounds(),
        Containers::arrayView({workaround}),
        TestSuite::Compare::Container);
    #endif
    CORRADE_COMPARE(c.disabledExtensions().size(), 1);
    CORRADE_COMPARE(c.disabledExtensions()[0].index(), Extensions::EXT::texture_filter_anisotropic::Index);
}

void ContextTest::configurationConstructMove() {
    #ifndef MAGNUM_TARGET_GLES
    Containers::StringView workaround = "no-layout-qualifiers-on-old-glsl";
    Containers::StringView another = "nv-compressed-block-size-in-bits";
    #elif !defined(MAGNUM_TARGET_WEBGL)
    Containers::StringView workaround = "swiftshader-no-empty-egl-context-flags";
    Containers::StringView another = "angle-chatty-shader-compiler";
    #else
    /* No general WebGL workarounds to test */
    #endif

    Context::Configuration a;
    a.setFlags(Context::Configuration::Flag::VerboseLog)
     #ifndef MAGNUM_TARGET_WEBGL
     .addDisabledWorkarounds({workaround})
     #endif
     .addDisabledExtensions<Extensions::EXT::texture_filter_anisotropic>();

    Context::Configuration b = std::move(a);
    CORRADE_COMPARE(UnsignedLong(b.flags()), UnsignedLong(Context::Configuration::Flag::VerboseLog));
    CORRADE_VERIFY(a.disabledWorkarounds().empty());
    CORRADE_VERIFY(a.disabledExtensions().empty());
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE_AS(b.disabledWorkarounds(),
        Containers::arrayView({workaround}),
        TestSuite::Compare::Container);
    #endif
    CORRADE_COMPARE(b.disabledExtensions().size(), 1);
    CORRADE_COMPARE(b.disabledExtensions()[0].index(), Extensions::EXT::texture_filter_anisotropic::Index);

    Context::Configuration c;
    c.setFlags(Context::Configuration::Flag::QuietLog)
     #ifndef MAGNUM_TARGET_WEBGL
     .addDisabledWorkarounds({another, another})
     .addDisabledExtensions<Extensions::KHR::debug,
                            Extensions::KHR::debug>()
     #else
     .addDisabledExtensions<Extensions::OES::texture_float_linear,
                            Extensions::OES::texture_float_linear>()
     #endif
     ;

    c = std::move(b);
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(b.disabledWorkarounds().size(), 2);
    #endif
    CORRADE_COMPARE(b.disabledExtensions().size(), 2);
    CORRADE_COMPARE(UnsignedLong(c.flags()), UnsignedLong(Context::Configuration::Flag::VerboseLog));
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE_AS(c.disabledWorkarounds(),
        Containers::arrayView({workaround}),
        TestSuite::Compare::Container);
    #endif
    CORRADE_COMPARE(c.disabledExtensions().size(), 1);
    CORRADE_COMPARE(c.disabledExtensions()[0].index(), Extensions::EXT::texture_filter_anisotropic::Index);
}

void ContextTest::constructNoCreate() {
    {
        /* Shouldn't crash during construction, shouldn't attempt to access GL,
           shouldn't crash when destructing */
        struct MyContext: Context {
            explicit MyContext(): Context{NoCreate, 0, nullptr, nullptr} {}
        } context;

        CORRADE_VERIFY(!Context::hasCurrent());
    }

    CORRADE_VERIFY(!Context::hasCurrent());

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoCreateT, Context>::value));
}

void ContextTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Context>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Context>{});
}

void ContextTest::makeCurrentNoOp() {
    CORRADE_VERIFY(!Context::hasCurrent());
    Context::makeCurrent(nullptr);
    CORRADE_VERIFY(!Context::hasCurrent());
}

void ContextTest::extensions() {
    const char* used[GL::Implementation::ExtensionCount]{};

    std::set<std::string> unique;

    /* Check that all extension indices are unique, are in correct lists, are
       not compiled on versions that shouldn't have them, are listed just once
       etc. */
    for(Version version: {
        #ifndef MAGNUM_TARGET_GLES
        Version::GL300,
        Version::GL310,
        Version::GL320,
        Version::GL330,
        Version::GL400,
        Version::GL410,
        Version::GL420,
        Version::GL430,
        Version::GL440,
        Version::GL450,
        Version::GL460,
        #else
        Version::GLES200,
        Version::GLES300,
        #ifndef MAGNUM_TARGET_WEBGL
        Version::GLES310,
        Version::GLES320,
        #endif
        #endif
        Version::None})
    {
        std::string previous;
        for(const Extension& e: Extension::extensions(version)) {
            CORRADE_ITERATION(version);
            CORRADE_ITERATION(e.string());

            /** @todo convert to CORRADE_ERROR() when that's done */

            if(!previous.empty() && previous >= e.string()) {
                Error{} << "Extension not sorted after" << previous;
                CORRADE_VERIFY(false);
            }

            if(e.index() >= GL::Implementation::ExtensionCount) {
                Error{} << "Index" << e.index() << "larger than" << GL::Implementation::ExtensionCount;
                CORRADE_VERIFY(false);
            }

            if(used[e.index()]) {
                Error{} << "Index" << e.index() << "already used by" << used[e.index()];
                CORRADE_VERIFY(false);
            }

            used[e.index()] = e.string();
            if(!unique.insert(e.string()).second) {
                Error{} << "Extension listed more than once";
                CORRADE_VERIFY(false);
            }

            CORRADE_VERIFY(Int(e.coreVersion()) >= Int(e.requiredVersion()));
            if(e.coreVersion() != version
                #if defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL)
                /* These two are replaced by EXT_color_buffer_float for 2.0,
                   but aren't core in WebGL 2 */
                && e.index() != Extensions::EXT::color_buffer_half_float::Index
                && e.index() != Extensions::WEBGL::color_buffer_float::Index
                #endif
            ) {
                Error{} << "Extension should have core version" << version << "but has" << e.coreVersion();
                CORRADE_VERIFY(false);
            }

            #ifdef MAGNUM_TARGET_GLES2
            if(e.requiredVersion() != Version::GLES200) {
                Error{} << "Extension should have required version" << Version::GLES200 << "but has" << e.requiredVersion();
                CORRADE_VERIFY(false);
            }
            #endif

            #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
            if(e.coreVersion() == Version::GLES300 && e.index() != Extensions::MAGNUM::shader_vertex_id::Index) {
                Error{} << "Extension has core version" << e.coreVersion() << "on a GLES3 build -- it shouldn't be present at all";
                CORRADE_VERIFY(false);
            }
            #endif

            previous = e.string();
        }
    }

    CORRADE_VERIFY(true);
}

void ContextTest::debugFlag() {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("No context flags on Emscripten yet.");
    #else
    std::ostringstream out;
    Debug(&out) << Context::Flag::Debug << Context::Flag(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Context::Flag::Debug GL::Context::Flag(0xdead)\n");
    #endif
}

void ContextTest::debugFlags() {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("No context flags on Emscripten yet.");
    #else
    std::ostringstream out;
    Debug(&out) << Context::Flags{} << (Context::Flag::Debug|Context::Flag::NoError) << (Context::Flag::Debug|Context::Flag(0xded0));
    CORRADE_COMPARE(out.str(), "GL::Context::Flags{} GL::Context::Flag::Debug|GL::Context::Flag::NoError GL::Context::Flag::Debug|GL::Context::Flag(0xded0)\n");
    #endif
}

void ContextTest::debugDetectedDriver() {
    std::ostringstream out;
    #ifndef MAGNUM_TARGET_WEBGL
    Debug{&out} << Context::DetectedDriver::Amd << Context::DetectedDriver(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Context::DetectedDriver::Amd GL::Context::DetectedDriver(0xdead)\n");
    #else
    Debug{&out} << Context::DetectedDriver::Angle << Context::DetectedDriver(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Context::DetectedDriver::Angle GL::Context::DetectedDriver(0xdead)\n");
    #endif
}

void ContextTest::debugDetectedDrivers() {
    std::ostringstream out;
    #ifndef MAGNUM_TARGET_WEBGL
    Debug{&out} << Context::DetectedDrivers{} << (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia) << (Context::DetectedDriver::Mesa|Context::DetectedDriver(0xde00));
    CORRADE_COMPARE(out.str(), "GL::Context::DetectedDrivers{} GL::Context::DetectedDriver::Amd|GL::Context::DetectedDriver::NVidia GL::Context::DetectedDriver::Mesa|GL::Context::DetectedDriver(0xde00)\n");
    #else
    Debug{&out} << Context::DetectedDrivers{} << (Context::DetectedDriver::Angle) << (Context::DetectedDriver::Angle|Context::DetectedDriver(0xde00));
    CORRADE_COMPARE(out.str(), "GL::Context::DetectedDrivers{} GL::Context::DetectedDriver::Angle GL::Context::DetectedDriver::Angle|GL::Context::DetectedDriver(0xde00)\n");
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::ContextTest)
