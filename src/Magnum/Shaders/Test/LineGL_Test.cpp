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

#include "Magnum/Shaders/LineGL.h"
/* Yes, really */
#include "Magnum/Shaders/generic.glsl"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct LineGL_Test: TestSuite::Tester {
    explicit LineGL_Test();

    void debugFlag();
    void debugFlags();
    void debugFlagsSupersets();

    void glslAttributeMatch();

    template<UnsignedInt dimensions> void configurationConstruct();
    template<UnsignedInt dimensions> void configurationConstructCopy();
    template<UnsignedInt dimensions> void configurationConstructMove();
    template<UnsignedInt dimensions> void configurationSetters();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();
};

using namespace Containers::Literals;

LineGL_Test::LineGL_Test() {
    addTests({&LineGL_Test::debugFlag,
              &LineGL_Test::debugFlags,
              &LineGL_Test::debugFlagsSupersets,

              &LineGL_Test::glslAttributeMatch,

              &LineGL_Test::configurationConstruct<2>,
              &LineGL_Test::configurationConstruct<3>,
              &LineGL_Test::configurationConstructCopy<2>,
              &LineGL_Test::configurationConstructCopy<3>,
              &LineGL_Test::configurationConstructMove<2>,
              &LineGL_Test::configurationConstructMove<3>,
              &LineGL_Test::configurationSetters<2>,
              &LineGL_Test::configurationSetters<3>,

              &LineGL_Test::constructNoCreate<2>,
              &LineGL_Test::constructNoCreate<3>,
              &LineGL_Test::constructCopy<2>,
              &LineGL_Test::constructCopy<3>});
}

void LineGL_Test::debugFlag() {
    Containers::String out;
    Debug{&out} << LineGL3D::Flag::VertexColor << LineGL3D::Flag(0xf00d);
    CORRADE_COMPARE(out, "Shaders::LineGL::Flag::VertexColor Shaders::LineGL::Flag(0xf00d)\n");
}

void LineGL_Test::debugFlags() {
    Containers::String out;
    Debug{&out} << (LineGL3D::Flag::VertexColor|LineGL3D::Flag::InstancedTransformation) << LineGL3D::Flags{};
    CORRADE_COMPARE(out, "Shaders::LineGL::Flag::VertexColor|Shaders::LineGL::Flag::InstancedTransformation Shaders::LineGL::Flags{}\n");
}

void LineGL_Test::debugFlagsSupersets() {
    /* InstancedObjectId is a superset of ObjectId so only one should be
       printed */
    {
        Containers::String out;
        Debug{&out} << (LineGL3D::Flag::ObjectId|LineGL3D::Flag::InstancedObjectId);
        CORRADE_COMPARE(out, "Shaders::LineGL::Flag::InstancedObjectId\n");
    }

    /* MultiDraw and ShaderStorageBuffers are a superset of UniformBuffers so
       only one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (LineGL3D::Flag::MultiDraw|LineGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::LineGL::Flag::MultiDraw\n");
    }
    #ifndef MAGNUM_TARGET_WEBGL
    {
        Containers::String out;
        Debug{&out} << (LineGL2D::Flag::ShaderStorageBuffers|LineGL2D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::LineGL::Flag::ShaderStorageBuffers\n");
    } {
        Containers::String out;
        Debug{&out} << (LineGL2D::Flag::MultiDraw|LineGL2D::Flag::ShaderStorageBuffers|LineGL2D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::LineGL::Flag::MultiDraw|Shaders::LineGL::Flag::ShaderStorageBuffers\n");
    }
    #endif
}

void LineGL_Test::glslAttributeMatch() {
    /* Position, Color and ObjectId tested in GenericGL_Test */

    CORRADE_COMPARE(LINE_PREVIOUS_POSITION_ATTRIBUTE_LOCATION, LineGL2D::PreviousPosition::Location);
    CORRADE_COMPARE(LINE_PREVIOUS_POSITION_ATTRIBUTE_LOCATION, LineGL3D::PreviousPosition::Location);

    CORRADE_COMPARE(LINE_NEXT_POSITION_ATTRIBUTE_LOCATION, LineGL2D::NextPosition::Location);
    CORRADE_COMPARE(LINE_NEXT_POSITION_ATTRIBUTE_LOCATION, LineGL3D::NextPosition::Location);

    CORRADE_COMPARE(LINE_ANNOTATION_ATTRIBUTE_LOCATION, LineGL2D::Annotation::Location);
    CORRADE_COMPARE(LINE_ANNOTATION_ATTRIBUTE_LOCATION, LineGL3D::Annotation::Location);
}

template<UnsignedInt dimensions> void LineGL_Test::configurationConstruct() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename LineGL<dimensions>::Configuration configuration;
    CORRADE_COMPARE(configuration.flags(), typename LineGL<dimensions>::Flags{});
    CORRADE_COMPARE(configuration.materialCount(), 1);
    CORRADE_COMPARE(configuration.drawCount(), 1);
    CORRADE_COMPARE(configuration.extraSource(), "");
    CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
}

template<UnsignedInt dimensions> void LineGL_Test::configurationConstructCopy() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* The copy is implicitly generated so just verify it propagates to the
       ConfigurationBaseGL constructor and does what's expected for other
       members */

    typename LineGL<dimensions>::Configuration a = typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::VertexColor)
        .setExtraSource("// hello\n"_s);

    typename LineGL<dimensions>::Configuration b = a;
    CORRADE_COMPARE(b.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_COMPARE(b.extraSource(), "// hello\n");
    /* Copying invokes a copy constructor on the string, so the Global flag
       gets lost */
    CORRADE_COMPARE(b.extraSource().flags(), Containers::StringViewFlag::NullTerminated);

    typename LineGL<dimensions>::Configuration c = typename LineGL<dimensions>::Configuration{}
        .setExtraSource("// hi\n"_s);
    c = a;
    CORRADE_COMPARE(c.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_COMPARE(c.extraSource(), "// hello\n");
    /* Same here */
    CORRADE_COMPARE(c.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
}

template<UnsignedInt dimensions> void LineGL_Test::configurationConstructMove() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* The copy is implicitly generated so just verify it propagates to the
       ConfigurationBaseGL constructor and does what's expected for other
       members */

    /* Not using method chaining because it returns & instead of && and so a
       copy would happen when forming `a`, losing the Global flag. Due to SSO
       the copy isn't such a big deal that it'd need && overloads for all
       setters right now. */
    typename LineGL<dimensions>::Configuration a;
    a.setFlags(LineGL<dimensions>::Flag::VertexColor);
    a.setExtraSource("// hello\n"_s);
    CORRADE_COMPARE(a.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_COMPARE(a.extraSource(), "// hello\n");
    CORRADE_COMPARE(a.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    typename LineGL<dimensions>::Configuration b = Utility::move(a);
    CORRADE_COMPARE(b.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_COMPARE(b.extraSource(), "// hello\n");
    CORRADE_COMPARE(b.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    typename LineGL<dimensions>::Configuration c;
    c.setExtraSource("// hi\n"_s);
    c = Utility::move(b);
    CORRADE_COMPARE(c.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_COMPARE(c.extraSource(), "// hello\n");
    CORRADE_COMPARE(c.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
}

template<UnsignedInt dimensions> void LineGL_Test::configurationSetters() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* Chaining the setters to verify they all return a meaningful reference.
       The chaining returns & instead of && and so a copy happens, losing any
       potential Global flag on the string. That's tested below instead. Due to
       SSO the copy isn't such a big deal that it'd need && overloads for all
       setters right now. */
    typename LineGL<dimensions>::Configuration configuration = typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::VertexColor)
        .setMaterialCount(17)
        .setDrawCount(266)
        .setExtraSource("// raw C string\n");
    CORRADE_COMPARE(configuration.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_COMPARE(configuration.materialCount(), 17);
    CORRADE_COMPARE(configuration.drawCount(), 266);
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

template<UnsignedInt dimensions> void LineGL_Test::constructNoCreate() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    {
        LineGL<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), typename LineGL<dimensions>::Flags{});
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void LineGL_Test::constructCopy() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_VERIFY(!std::is_copy_constructible<LineGL<dimensions>>{});
    CORRADE_VERIFY(!std::is_copy_assignable<LineGL<dimensions>>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::LineGL_Test)
