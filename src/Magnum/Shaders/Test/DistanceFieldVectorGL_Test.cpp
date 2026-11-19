/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Shaders/DistanceFieldVectorGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct DistanceFieldVectorGL_Test: TestSuite::Tester {
    explicit DistanceFieldVectorGL_Test();

    void debugFlag();
    void debugFlags();
    #ifndef MAGNUM_TARGET_GLES2
    void debugFlagsSupersets();
    #endif

    template<UnsignedInt dimensions> void configurationConstruct();
    template<UnsignedInt dimensions> void configurationConstructCopy();
    template<UnsignedInt dimensions> void configurationConstructMove();
    template<UnsignedInt dimensions> void configurationSetters();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();
};

using namespace Containers::Literals;

DistanceFieldVectorGL_Test::DistanceFieldVectorGL_Test() {
    addTests({&DistanceFieldVectorGL_Test::debugFlag,
              &DistanceFieldVectorGL_Test::debugFlags,
              #ifndef MAGNUM_TARGET_GLES2
              &DistanceFieldVectorGL_Test::debugFlagsSupersets,
              #endif

              &DistanceFieldVectorGL_Test::configurationConstruct<2>,
              &DistanceFieldVectorGL_Test::configurationConstruct<3>,
              &DistanceFieldVectorGL_Test::configurationConstructCopy<2>,
              &DistanceFieldVectorGL_Test::configurationConstructCopy<3>,
              &DistanceFieldVectorGL_Test::configurationConstructMove<2>,
              &DistanceFieldVectorGL_Test::configurationConstructMove<3>,
              &DistanceFieldVectorGL_Test::configurationSetters<2>,
              &DistanceFieldVectorGL_Test::configurationSetters<3>,

              &DistanceFieldVectorGL_Test::constructNoCreate<2>,
              &DistanceFieldVectorGL_Test::constructNoCreate<3>,

              &DistanceFieldVectorGL_Test::constructCopy<2>,
              &DistanceFieldVectorGL_Test::constructCopy<3>});
}

void DistanceFieldVectorGL_Test::debugFlag() {
    Containers::String out;
    Debug{&out} << DistanceFieldVectorGL2D::Flag::TextureTransformation << DistanceFieldVectorGL2D::Flag(0xf0);
    CORRADE_COMPARE(out, "Shaders::DistanceFieldVectorGL::Flag::TextureTransformation Shaders::DistanceFieldVectorGL::Flag(0xf0)\n");
}

void DistanceFieldVectorGL_Test::debugFlags() {
    Containers::String out;
    Debug{&out} << DistanceFieldVectorGL3D::Flags{DistanceFieldVectorGL3D::Flag::TextureTransformation|DistanceFieldVectorGL3D::Flag(0x80)} << DistanceFieldVectorGL3D::Flags{};
    CORRADE_COMPARE(out, "Shaders::DistanceFieldVectorGL::Flag::TextureTransformation|Shaders::DistanceFieldVectorGL::Flag(0x80) Shaders::DistanceFieldVectorGL::Flags{}\n");
}

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldVectorGL_Test::debugFlagsSupersets() {
    /* MultiDraw and ShaderStorageBuffers are a superset of UniformBuffers so
       only one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (DistanceFieldVectorGL3D::Flag::MultiDraw|DistanceFieldVectorGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::DistanceFieldVectorGL::Flag::MultiDraw\n");
    }
    #ifndef MAGNUM_TARGET_WEBGL
    {
        Containers::String out;
        Debug{&out} << (DistanceFieldVectorGL2D::Flag::ShaderStorageBuffers|DistanceFieldVectorGL2D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::DistanceFieldVectorGL::Flag::ShaderStorageBuffers\n");
    } {
        Containers::String out;
        Debug{&out} << (DistanceFieldVectorGL3D::Flag::MultiDraw|DistanceFieldVectorGL3D::Flag::ShaderStorageBuffers|DistanceFieldVectorGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::DistanceFieldVectorGL::Flag::MultiDraw|Shaders::DistanceFieldVectorGL::Flag::ShaderStorageBuffers\n");
    }
    #endif
}
#endif

template<UnsignedInt dimensions> void DistanceFieldVectorGL_Test::configurationConstruct() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename DistanceFieldVectorGL<dimensions>::Configuration configuration;
    CORRADE_COMPARE(configuration.flags(), typename DistanceFieldVectorGL<dimensions>::Flags{});
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(configuration.materialCount(), 1);
    CORRADE_COMPARE(configuration.drawCount(), 1);
    #endif
    CORRADE_COMPARE(configuration.extraSource(), "");
    CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
}

template<UnsignedInt dimensions> void DistanceFieldVectorGL_Test::configurationConstructCopy() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* The copy is implicitly generated so just verify it propagates to the
       ConfigurationBaseGL constructor and does what's expected for other
       members */

    typename DistanceFieldVectorGL<dimensions>::Configuration a = typename DistanceFieldVectorGL<dimensions>::Configuration{}
        .setFlags(DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation)
        .setExtraSource("// hello\n"_s);

    typename DistanceFieldVectorGL<dimensions>::Configuration b = a;
    CORRADE_COMPARE(b.flags(), DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(b.extraSource(), "// hello\n");
    /* Copying invokes a copy constructor on the string, so the Global flag
       gets lost */
    CORRADE_COMPARE(b.extraSource().flags(), Containers::StringViewFlag::NullTerminated);

    typename DistanceFieldVectorGL<dimensions>::Configuration c = typename DistanceFieldVectorGL<dimensions>::Configuration{}
        .setExtraSource("// hi\n"_s);
    c = a;
    CORRADE_COMPARE(c.flags(), DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(c.extraSource(), "// hello\n");
    /* Same here */
    CORRADE_COMPARE(c.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
}

template<UnsignedInt dimensions> void DistanceFieldVectorGL_Test::configurationConstructMove() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* The copy is implicitly generated so just verify it propagates to the
       ConfigurationBaseGL constructor and does what's expected for other
       members */

    /* Not using method chaining because it returns & instead of && and so a
       copy would happen when forming `a`, losing the Global flag. Due to SSO
       the copy isn't such a big deal that it'd need && overloads for all
       setters right now. */
    typename DistanceFieldVectorGL<dimensions>::Configuration a;
    a.setFlags(DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation);
    a.setExtraSource("// hello\n"_s);
    CORRADE_COMPARE(a.flags(), DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(a.extraSource(), "// hello\n");
    CORRADE_COMPARE(a.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    typename DistanceFieldVectorGL<dimensions>::Configuration b = Utility::move(a);
    CORRADE_COMPARE(b.flags(), DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(b.extraSource(), "// hello\n");
    CORRADE_COMPARE(b.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    typename DistanceFieldVectorGL<dimensions>::Configuration c;
    c.setExtraSource("// hi\n"_s);
    c = Utility::move(b);
    CORRADE_COMPARE(c.flags(), DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(c.extraSource(), "// hello\n");
    CORRADE_COMPARE(c.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
}

template<UnsignedInt dimensions> void DistanceFieldVectorGL_Test::configurationSetters() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* Chaining the setters to verify they all return a meaningful reference.
       The chaining returns & instead of && and so a copy happens, losing any
       potential Global flag on the string. That's tested below instead. Due to
       SSO the copy isn't such a big deal that it'd need && overloads for all
       setters right now. */
    typename DistanceFieldVectorGL<dimensions>::Configuration configuration = typename DistanceFieldVectorGL<dimensions>::Configuration{}
        .setFlags(DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        .setMaterialCount(17)
        .setDrawCount(266)
        #endif
        .setExtraSource("// raw C string\n");
    CORRADE_COMPARE(configuration.flags(), DistanceFieldVectorGL<dimensions>::Flag::TextureTransformation);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(configuration.materialCount(), 17);
    CORRADE_COMPARE(configuration.drawCount(), 266);
    #endif
    CORRADE_COMPARE(configuration.extraSource(), "// raw C string\n");
    CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);

    /* Verify also that the other setExtraSource() overloads delegate
       properly */
    configuration.setExtraSource("// hello\n"_s);
    CORRADE_COMPARE(configuration.extraSource(), "// hello\n");
    CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    configuration.setExtraSource(Containers::String::nullTerminatedGlobalView("// r-value global string\n"_s));
    CORRADE_COMPARE(configuration.extraSource(), "// r-value global string\n");
    CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
}

template<UnsignedInt dimensions> void DistanceFieldVectorGL_Test::constructNoCreate() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    {
        DistanceFieldVectorGL<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), typename DistanceFieldVectorGL<dimensions>::Flags{});
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void DistanceFieldVectorGL_Test::constructCopy() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_VERIFY(!std::is_copy_constructible<DistanceFieldVectorGL<dimensions>>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DistanceFieldVectorGL<dimensions>>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::DistanceFieldVectorGL_Test)
