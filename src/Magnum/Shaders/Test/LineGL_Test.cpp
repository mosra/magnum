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

    void glslAttributeMatch();

    template<UnsignedInt dimensions> void constructConfigurationDefault();
    template<UnsignedInt dimensions> void constructConfigurationSetters();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();

    void debugFlag();
    void debugFlags();
    void debugFlagsSupersets();
};

LineGL_Test::LineGL_Test() {
    addTests({&LineGL_Test::glslAttributeMatch,

              &LineGL_Test::constructConfigurationDefault<2>,
              &LineGL_Test::constructConfigurationDefault<3>,
              &LineGL_Test::constructConfigurationSetters<2>,
              &LineGL_Test::constructConfigurationSetters<3>,

              &LineGL_Test::constructNoCreate<2>,
              &LineGL_Test::constructNoCreate<3>,
              &LineGL_Test::constructCopy<2>,
              &LineGL_Test::constructCopy<3>,

              &LineGL_Test::debugFlag,
              &LineGL_Test::debugFlags,
              &LineGL_Test::debugFlagsSupersets});
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

template<UnsignedInt dimensions> void LineGL_Test::constructConfigurationDefault() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename LineGL<dimensions>::Configuration configuration;
    CORRADE_COMPARE(configuration.flags(), typename LineGL<dimensions>::Flags{});
    CORRADE_COMPARE(configuration.materialCount(), 1);
    CORRADE_COMPARE(configuration.drawCount(), 1);
}

template<UnsignedInt dimensions> void LineGL_Test::constructConfigurationSetters() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename LineGL<dimensions>::Configuration configuration = typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::VertexColor)
        .setMaterialCount(17)
        .setDrawCount(266);
    CORRADE_COMPARE(configuration.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_COMPARE(configuration.materialCount(), 17);
    CORRADE_COMPARE(configuration.drawCount(), 266);
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

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::LineGL_Test)
