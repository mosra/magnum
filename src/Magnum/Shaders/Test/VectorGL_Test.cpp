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

#include "Magnum/Shaders/VectorGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct VectorGL_Test: TestSuite::Tester {
    explicit VectorGL_Test();

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

VectorGL_Test::VectorGL_Test() {
    addTests({&VectorGL_Test::debugFlag,
              &VectorGL_Test::debugFlags,
              #ifndef MAGNUM_TARGET_GLES2
              &VectorGL_Test::debugFlagsSupersets,
              #endif

              &VectorGL_Test::configurationConstruct<2>,
              &VectorGL_Test::configurationConstruct<3>,
              &VectorGL_Test::configurationConstructCopy<2>,
              &VectorGL_Test::configurationConstructCopy<3>,
              &VectorGL_Test::configurationConstructMove<2>,
              &VectorGL_Test::configurationConstructMove<3>,
              &VectorGL_Test::configurationSetters<2>,
              &VectorGL_Test::configurationSetters<3>,

              &VectorGL_Test::constructNoCreate<2>,
              &VectorGL_Test::constructNoCreate<3>,

              &VectorGL_Test::constructCopy<2>,
              &VectorGL_Test::constructCopy<3>});
}

void VectorGL_Test::debugFlag() {
    Containers::String out;
    Debug{&out} << VectorGL2D::Flag::TextureTransformation << VectorGL2D::Flag(0xf0);
    CORRADE_COMPARE(out, "Shaders::VectorGL::Flag::TextureTransformation Shaders::VectorGL::Flag(0xf0)\n");
}

void VectorGL_Test::debugFlags() {
    Containers::String out;
    Debug{&out} << VectorGL3D::Flags{VectorGL3D::Flag::TextureTransformation|VectorGL3D::Flag(0x80)} << VectorGL3D::Flags{};
    CORRADE_COMPARE(out, "Shaders::VectorGL::Flag::TextureTransformation|Shaders::VectorGL::Flag(0x80) Shaders::VectorGL::Flags{}\n");
}

#ifndef MAGNUM_TARGET_GLES2
void VectorGL_Test::debugFlagsSupersets() {
    /* MultiDraw and ShaderStorageBuffers are a superset of UniformBuffers so
       only one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (VectorGL3D::Flag::MultiDraw|VectorGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::VectorGL::Flag::MultiDraw\n");
    }
    #ifndef MAGNUM_TARGET_WEBGL
    {
        Containers::String out;
        Debug{&out} << (VectorGL2D::Flag::ShaderStorageBuffers|VectorGL2D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::VectorGL::Flag::ShaderStorageBuffers\n");
    } {
        Containers::String out;
        Debug{&out} << (VectorGL3D::Flag::MultiDraw|VectorGL3D::Flag::ShaderStorageBuffers|VectorGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::VectorGL::Flag::MultiDraw|Shaders::VectorGL::Flag::ShaderStorageBuffers\n");
    }
    #endif
}
#endif

template<UnsignedInt dimensions> void VectorGL_Test::configurationConstruct() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename VectorGL<dimensions>::Configuration configuration;
    CORRADE_COMPARE(configuration.flags(), typename VectorGL<dimensions>::Flags{});
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(configuration.materialCount(), 1);
    CORRADE_COMPARE(configuration.drawCount(), 1);
    #endif
    CORRADE_COMPARE(configuration.extraSource(), "");
    CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
}

template<UnsignedInt dimensions> void VectorGL_Test::configurationConstructCopy() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* The copy is implicitly generated so just verify it propagates to the
       ConfigurationBaseGL constructor and does what's expected for other
       members */

    typename VectorGL<dimensions>::Configuration a = typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL<dimensions>::Flag::TextureTransformation)
        .setExtraSource("// hello\n"_s);

    typename VectorGL<dimensions>::Configuration b = a;
    CORRADE_COMPARE(b.flags(), VectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(b.extraSource(), "// hello\n");
    /* Copying invokes a copy constructor on the string, so the Global flag
       gets lost */
    CORRADE_COMPARE(b.extraSource().flags(), Containers::StringViewFlag::NullTerminated);

    typename VectorGL<dimensions>::Configuration c = typename VectorGL<dimensions>::Configuration{}
        .setExtraSource("// hi\n"_s);
    c = a;
    CORRADE_COMPARE(c.flags(), VectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(c.extraSource(), "// hello\n");
    /* Same here */
    CORRADE_COMPARE(c.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
}

template<UnsignedInt dimensions> void VectorGL_Test::configurationConstructMove() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* The copy is implicitly generated so just verify it propagates to the
       ConfigurationBaseGL constructor and does what's expected for other
       members */

    /* Not using method chaining because it returns & instead of && and so a
       copy would happen when forming `a`, losing the Global flag. Due to SSO
       the copy isn't such a big deal that it'd need && overloads for all
       setters right now. */
    typename VectorGL<dimensions>::Configuration a;
    a.setFlags(VectorGL<dimensions>::Flag::TextureTransformation);
    a.setExtraSource("// hello\n"_s);
    CORRADE_COMPARE(a.flags(), VectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(a.extraSource(), "// hello\n");
    CORRADE_COMPARE(a.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    typename VectorGL<dimensions>::Configuration b = Utility::move(a);
    CORRADE_COMPARE(b.flags(), VectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(b.extraSource(), "// hello\n");
    CORRADE_COMPARE(b.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    typename VectorGL<dimensions>::Configuration c;
    c.setExtraSource("// hi\n"_s);
    c = Utility::move(b);
    CORRADE_COMPARE(c.flags(), VectorGL<dimensions>::Flag::TextureTransformation);
    CORRADE_COMPARE(c.extraSource(), "// hello\n");
    CORRADE_COMPARE(c.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
}

template<UnsignedInt dimensions> void VectorGL_Test::configurationSetters() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* Chaining the setters to verify they all return a meaningful reference.
       The chaining returns & instead of && and so a copy happens, losing any
       potential Global flag on the string. That's tested below instead. Due to
       SSO the copy isn't such a big deal that it'd need && overloads for all
       setters right now. */
    typename VectorGL<dimensions>::Configuration configuration = typename VectorGL<dimensions>::Configuration{}
        .setFlags(VectorGL<dimensions>::Flag::TextureTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        .setMaterialCount(17)
        .setDrawCount(266)
        #endif
        .setExtraSource("// raw C string\n");
    CORRADE_COMPARE(configuration.flags(), VectorGL<dimensions>::Flag::TextureTransformation);
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

template<UnsignedInt dimensions> void VectorGL_Test::constructNoCreate() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    {
        VectorGL<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), typename VectorGL<dimensions>::Flags{});
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void VectorGL_Test::constructCopy() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_VERIFY(!std::is_copy_constructible<VectorGL<dimensions>>{});
    CORRADE_VERIFY(!std::is_copy_assignable<VectorGL<dimensions>>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VectorGL_Test)
