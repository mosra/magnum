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

#include "Magnum/Shaders/MeshVisualizerGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct MeshVisualizerGL_Test: TestSuite::Tester {
    explicit MeshVisualizerGL_Test();

    #ifndef MAGNUM_TARGET_GLES2
    void configurationSetJointCountInvalid2D();
    void configurationSetJointCountInvalid3D();
    #endif

    void constructNoCreate2D();
    void constructNoCreate3D();

    void constructCopy2D();
    void constructCopy3D();

    void vertexIndexSameAsObjectId();

    void debugFlag2D();
    void debugFlag3D();
    void debugFlags2D();
    void debugFlags3D();
    #ifndef MAGNUM_TARGET_GLES2
    void debugFlagsSupersets2D();
    void debugFlagsSupersets3D();
    #endif
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
       tested in MeshVisualizerGLTest::constructInvalid() and
       constructUniformBuffersInvalid() */
};
#endif

MeshVisualizerGL_Test::MeshVisualizerGL_Test() {
    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({
        &MeshVisualizerGL_Test::configurationSetJointCountInvalid2D,
        &MeshVisualizerGL_Test::configurationSetJointCountInvalid3D},
        Containers::arraySize(ConfigurationSetJointCountInvalidData));
    #endif

    addTests({
        &MeshVisualizerGL_Test::constructNoCreate2D,
        &MeshVisualizerGL_Test::constructNoCreate3D,

        &MeshVisualizerGL_Test::constructCopy2D,
        &MeshVisualizerGL_Test::constructCopy3D,

        &MeshVisualizerGL_Test::vertexIndexSameAsObjectId,

        &MeshVisualizerGL_Test::debugFlag2D,
        &MeshVisualizerGL_Test::debugFlag3D,
        &MeshVisualizerGL_Test::debugFlags2D,
        &MeshVisualizerGL_Test::debugFlags3D,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshVisualizerGL_Test::debugFlagsSupersets2D,
        &MeshVisualizerGL_Test::debugFlagsSupersets3D,
        #endif
    });
}

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGL_Test::configurationSetJointCountInvalid2D() {
    auto&& data = ConfigurationSetJointCountInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    MeshVisualizerGL2D::Configuration configuration;

    Containers::String out;
    Error redirectError{&out};
    configuration.setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount);
    CORRADE_COMPARE(out, Utility::format("Shaders::MeshVisualizerGL2D::Configuration::setJointCount(): {}\n", data.message));
}

void MeshVisualizerGL_Test::configurationSetJointCountInvalid3D() {
    auto&& data = ConfigurationSetJointCountInvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    MeshVisualizerGL3D::Configuration configuration;

    Containers::String out;
    Error redirectError{&out};
    configuration.setJointCount(data.jointCount, data.perVertexJointCount, data.secondaryPerVertexJointCount);
    CORRADE_COMPARE(out, Utility::format("Shaders::MeshVisualizerGL3D::Configuration::setJointCount(): {}\n", data.message));
}
#endif

void MeshVisualizerGL_Test::constructNoCreate2D() {
    {
        MeshVisualizerGL2D shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), MeshVisualizerGL2D::Flags{});
    }

    CORRADE_VERIFY(true);
}

void MeshVisualizerGL_Test::constructNoCreate3D() {
    {
        MeshVisualizerGL3D shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), MeshVisualizerGL3D::Flags{});
    }

    CORRADE_VERIFY(true);
}

void MeshVisualizerGL_Test::constructCopy2D() {
    CORRADE_VERIFY(!std::is_copy_constructible<MeshVisualizerGL2D>{});
    CORRADE_VERIFY(!std::is_copy_assignable<MeshVisualizerGL2D>{});
}

void MeshVisualizerGL_Test::constructCopy3D() {
    CORRADE_VERIFY(!std::is_copy_constructible<MeshVisualizerGL3D>{});
    CORRADE_VERIFY(!std::is_copy_assignable<MeshVisualizerGL3D>{});
}

void MeshVisualizerGL_Test::vertexIndexSameAsObjectId() {
    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("Object ID is not available on ES2.");
    #else
    CORRADE_COMPARE(MeshVisualizerGL2D::VertexIndex::Location, GenericGL2D::ObjectId::Location);
    CORRADE_COMPARE(MeshVisualizerGL3D::VertexIndex::Location, GenericGL3D::ObjectId::Location);
    #endif
}

void MeshVisualizerGL_Test::debugFlag2D() {
    Containers::String out;

    Debug{&out} << MeshVisualizerGL2D::Flag::Wireframe << MeshVisualizerGL2D::Flag(0xbad00000);
    CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::Wireframe Shaders::MeshVisualizerGL2D::Flag(0xbad00000)\n");
}

void MeshVisualizerGL_Test::debugFlag3D() {
    Containers::String out;

    Debug{&out} << MeshVisualizerGL3D::Flag::Wireframe << MeshVisualizerGL3D::Flag(0xbad00000);
    CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::Wireframe Shaders::MeshVisualizerGL3D::Flag(0xbad00000)\n");
}

void MeshVisualizerGL_Test::debugFlags2D() {
    Containers::String out;

    Debug{&out} << (MeshVisualizerGL2D::Flag::Wireframe|MeshVisualizerGL2D::Flag::NoGeometryShader) << MeshVisualizerGL2D::Flags{};
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::Wireframe|Shaders::MeshVisualizerGL2D::Flag::NoGeometryShader Shaders::MeshVisualizerGL2D::Flags{}\n");
    #else
    CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::Wireframe Shaders::MeshVisualizerGL2D::Flags{}\n");
    #endif
}

void MeshVisualizerGL_Test::debugFlags3D() {
    Containers::String out;

    Debug{&out} << (MeshVisualizerGL3D::Flag::Wireframe|MeshVisualizerGL3D::Flag::NoGeometryShader) << MeshVisualizerGL3D::Flags{};
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::Wireframe|Shaders::MeshVisualizerGL3D::Flag::NoGeometryShader Shaders::MeshVisualizerGL3D::Flags{}\n");
    #else
    CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::Wireframe Shaders::MeshVisualizerGL3D::Flags{}\n");
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void MeshVisualizerGL_Test::debugFlagsSupersets2D() {
    /* InstancedObjectId and ObjectIdTexture are a superset of ObjectId so only
       one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL2D::Flag::ObjectId|MeshVisualizerGL2D::Flag::InstancedObjectId);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::InstancedObjectId\n");
    } {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL2D::Flag::ObjectId|MeshVisualizerGL2D::Flag::ObjectIdTexture);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::ObjectIdTexture\n");
    } {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL2D::Flag::ObjectId|MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::ObjectIdTexture);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::InstancedObjectId|Shaders::MeshVisualizerGL2D::Flag::ObjectIdTexture\n");
    }

    /* InstancedTextureOffset is a superset of TextureTransformation so only
       one should be printed */
    {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL2D::Flag::InstancedTextureOffset|MeshVisualizerGL2D::Flag::TextureTransformation);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::InstancedTextureOffset\n");
    }

    /* MultiDraw and ShaderStorageBuffers are a superset of UniformBuffers so
       only one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::MultiDraw\n");
    }
    #ifndef MAGNUM_TARGET_WEBGL
    {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL2D::Flag::ShaderStorageBuffers|MeshVisualizerGL2D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::ShaderStorageBuffers\n");
    } {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL2D::Flag::MultiDraw|MeshVisualizerGL2D::Flag::ShaderStorageBuffers|MeshVisualizerGL2D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL2D::Flag::MultiDraw|Shaders::MeshVisualizerGL2D::Flag::ShaderStorageBuffers\n");
    }
    #endif
}

void MeshVisualizerGL_Test::debugFlagsSupersets3D() {
    /* InstancedObjectId and ObjectIdTexture are a superset of ObjectId so only
       one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL3D::Flag::ObjectId|MeshVisualizerGL3D::Flag::InstancedObjectId);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::InstancedObjectId\n");
    } {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL3D::Flag::ObjectId|MeshVisualizerGL3D::Flag::ObjectIdTexture);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::ObjectIdTexture\n");
    } {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL3D::Flag::ObjectId|MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::ObjectIdTexture);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::InstancedObjectId|Shaders::MeshVisualizerGL3D::Flag::ObjectIdTexture\n");
    }

    /* InstancedTextureOffset is a superset of TextureTransformation so only
       one should be printed */
    {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL3D::Flag::InstancedTextureOffset|MeshVisualizerGL3D::Flag::TextureTransformation);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::InstancedTextureOffset\n");
    }

    /* MultiDraw and ShaderStorageBuffers are a superset of UniformBuffers so
       only one should be printed, but if there are both then both should be */
    {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::MultiDraw\n");
    }
    #ifndef MAGNUM_TARGET_WEBGL
    {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL3D::Flag::ShaderStorageBuffers|MeshVisualizerGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::ShaderStorageBuffers\n");
    } {
        Containers::String out;
        Debug{&out} << (MeshVisualizerGL3D::Flag::MultiDraw|MeshVisualizerGL3D::Flag::ShaderStorageBuffers|MeshVisualizerGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out, "Shaders::MeshVisualizerGL3D::Flag::MultiDraw|Shaders::MeshVisualizerGL3D::Flag::ShaderStorageBuffers\n");
    }
    #endif
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerGL_Test)
