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

#include <Corrade/Containers/ArrayView.h> /* arraySize() */
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Shaders/PhongGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct PhongGL_Test: TestSuite::Tester {
    explicit PhongGL_Test();

    void configurationSetLightCountInvalid();
    #ifndef MAGNUM_TARGET_GLES2
    void configurationSetJointCountInvalid();
    #endif

    void constructNoCreate();
    void constructCopy();

    void debugFlag();
    void debugFlags();
    void debugFlagsSupersets();
};

const struct {
    const char* name;
    UnsignedInt count, perDrawCount;
    const char* message;
} ConfigurationSetLightCountInvalidData[] {
    {"count but no per-draw count",
        10, 0,
        "count has to be zero if per-draw count is zero"},
    /* The rest depends on flags being set and is thus verified in constructor,
       tested in PhongGLTest::constructInvalid() and
       constructUniformBuffersInvalid() */
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    UnsignedInt jointCount, perVertexJointCount, secondaryPerVertexJointCount;
    const char* message;
} ConfigurationSetJointCountInvalidData[] {
    {"per-vertex joint count too large",
        10, 5, 0,
        "expected at most 4 per-vertex joints, got 5"},
    {"secondary per-vertex joint count too large",
        10, 0, 5,
        "expected at most 4 secondary per-vertex joints, got 5"},
    {"joint count but no per-vertex joint count",
        10, 0, 0,
        "count has to be zero if per-vertex joint count is zero"},
    /* The rest depends on flags being set and is thus verified in constructor,
       tested in PhongGLTest::constructInvalid() and
       constructUniformBuffersInvalid() */
};
#endif

PhongGL_Test::PhongGL_Test() {
    addInstancedTests({&PhongGL_Test::configurationSetLightCountInvalid},
        Containers::arraySize(ConfigurationSetLightCountInvalidData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&PhongGL_Test::configurationSetJointCountInvalid},
        Containers::arraySize(ConfigurationSetJointCountInvalidData));
    #endif

    addTests({&PhongGL_Test::constructNoCreate,
              &PhongGL_Test::constructCopy,

              &PhongGL_Test::debugFlag,
              &PhongGL_Test::debugFlags,
              &PhongGL_Test::debugFlagsSupersets});
}

void PhongGL_Test::configurationSetLightCountInvalid() {
    auto&& data = ConfigurationSetLightCountInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL::Configuration configuration;

    Containers::String out;
    Error redirectError{&out};
    configuration.setLightCount(data.count, data.perDrawCount);
    CORRADE_COMPARE(out, Utility::format("Shaders::PhongGL::Configuration::setLightCount(): {}\n", data.message));
}

#ifndef MAGNUM_TARGET_GLES2
void PhongGL_Test::configurationSetJointCountInvalid() {
    auto&& data = ConfigurationSetJointCountInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    PhongGL::Configuration configuration;

    Containers::String out;
    Error redirectError{&out};
    configuration.setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount);
    CORRADE_COMPARE(out, Utility::format("Shaders::PhongGL::Configuration::setJointCount(): {}\n", data.message));
}
#endif

void PhongGL_Test::constructNoCreate() {
    {
        PhongGL shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), PhongGL::Flags{});
        CORRADE_COMPARE(shader.lightCount(), 0);
    }

    CORRADE_VERIFY(true);
}

void PhongGL_Test::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<PhongGL>{});
    CORRADE_VERIFY(!std::is_copy_assignable<PhongGL>{});
}

void PhongGL_Test::debugFlag() {
    Containers::String out;

    Debug{&out} << PhongGL::Flag::AmbientTexture << PhongGL::Flag(0xcafedead);
    CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::AmbientTexture Shaders::PhongGL::Flag(0xcafedead)\n");
}

void PhongGL_Test::debugFlags() {
    Containers::String out;

    Debug{&out} << (PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture) << PhongGL::Flags{};
    CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::DiffuseTexture|Shaders::PhongGL::Flag::SpecularTexture Shaders::PhongGL::Flags{}\n");
}

void PhongGL_Test::debugFlagsSupersets() {
    #ifndef MAGNUM_TARGET_GLES2
    /* InstancedObjectId and ObjectIdTexture are a superset of ObjectId so only
       one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (PhongGL::Flag::ObjectId|PhongGL::Flag::InstancedObjectId);
        CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::InstancedObjectId\n");
    } {
        Containers::String out;
        Debug{&out} << (PhongGL::Flag::ObjectId|PhongGL::Flag::ObjectIdTexture);
        CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::ObjectIdTexture\n");
    } {
        Containers::String out;
        Debug{&out} << (PhongGL::Flag::ObjectId|PhongGL::Flag::InstancedObjectId|PhongGL::Flag::ObjectIdTexture);
        CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::InstancedObjectId|Shaders::PhongGL::Flag::ObjectIdTexture\n");
    }
    #endif

    /* InstancedTextureOffset is a superset of TextureTransformation so only
       one should be printed */
    {
        Containers::String out;
        Debug{&out} << (PhongGL::Flag::InstancedTextureOffset|PhongGL::Flag::TextureTransformation);
        CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::InstancedTextureOffset\n");
    }

    #ifndef MAGNUM_TARGET_GLES2
    /* MultiDraw and ShaderStorageBuffers are a superset of UniformBuffers so
       only one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (PhongGL::Flag::MultiDraw|PhongGL::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::MultiDraw\n");
    }
    #ifndef MAGNUM_TARGET_WEBGL
    {
        Containers::String out;
        Debug{&out} << (PhongGL::Flag::ShaderStorageBuffers|PhongGL::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::ShaderStorageBuffers\n");
    } {
        Containers::String out;
        Debug{&out} << (PhongGL::Flag::MultiDraw|PhongGL::Flag::ShaderStorageBuffers|PhongGL::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::PhongGL::Flag::MultiDraw|Shaders::PhongGL::Flag::ShaderStorageBuffers\n");
    }
    #endif
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongGL_Test)
