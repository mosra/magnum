/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "MeshVisualizer.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace {
    enum: Int {
        /* First four taken by Phong (A/D/S/N) */
        ColorMapTextureUnit = 4
    };
}

namespace Implementation {

MeshVisualizerBase::MeshVisualizerBase(FlagsBase flags): _flags{flags} {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef CORRADE_NO_ASSERT
    Int countMutuallyExclusive = 0;
    if(flags & FlagBase::InstancedObjectId) ++countMutuallyExclusive;
    if(flags & FlagBase::VertexId) ++countMutuallyExclusive;
    if(flags & FlagBase::PrimitiveIdFromVertexId) ++countMutuallyExclusive;
    #endif
    CORRADE_ASSERT(countMutuallyExclusive <= 1,
        "Shaders::MeshVisualizer: Flag::InstancedObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive", );
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(_flags & FlagBase::Wireframe && !(_flags & FlagBase::NoGeometryShader)) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL320);
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::geometry_shader4);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::geometry_shader);
        #endif
    }
    #else
    if(_flags & FlagBase::Wireframe)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::OES::standard_derivatives);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(_flags & FlagBase::PrimitiveId && !(_flags >= FlagBase::PrimitiveIdFromVertexId)) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL320);
        #else
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GLES320);
        #endif
    }
    #endif

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShaders"))
        importShaderResources();
    #endif
}

GL::Version MeshVisualizerBase::setupShaders(GL::Shader& vert, GL::Shader& frag, const Utility::Resource& rs) const {
    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    /* Extended in MeshVisualizer3D for TBN visualization */
    CORRADE_INTERNAL_ASSERT(!(_flags & FlagBase::Wireframe) || _flags & FlagBase::NoGeometryShader || version >= GL::Version::GL320);
    #elif !defined(MAGNUM_TARGET_WEBGL)
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES310, GL::Version::GLES300, GL::Version::GLES200});
    /* Extended in MeshVisualizer3D for TBN visualization */
    CORRADE_INTERNAL_ASSERT(!(_flags & FlagBase::Wireframe) || _flags & FlagBase::NoGeometryShader || version >= GL::Version::GLES310);
    #else
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource(_flags & FlagBase::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(_flags & FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        .addSource(_flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
        .addSource(_flags >= FlagBase::PrimitiveIdFromVertexId ? "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" : "")
        #endif
        #ifdef MAGNUM_TARGET_WEBGL
        .addSource("#define SUBSCRIPTING_WORKAROUND\n")
        #elif defined(MAGNUM_TARGET_GLES2)
        .addSource(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Angle ?
            "#define SUBSCRIPTING_WORKAROUND\n" : "")
        #endif
        ;
    frag.addSource(_flags & FlagBase::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(_flags & FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        .addSource(_flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
        .addSource(_flags & FlagBase::PrimitiveId ?
            (_flags >= FlagBase::PrimitiveIdFromVertexId ?
                "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                "#define PRIMITIVE_ID\n") : "")
        #endif
        ;

    return version;
}

MeshVisualizerBase& MeshVisualizerBase::setColor(const Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(_flags & (FlagBase::Wireframe|FlagBase::InstancedObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
        "Shaders::MeshVisualizer::setColor(): the shader was not created with wireframe or object/vertex/primitive ID enabled", *this);
    #else
    CORRADE_ASSERT(_flags & FlagBase::Wireframe,
        "Shaders::MeshVisualizer::setColor(): the shader was not created with wireframe enabled", *this);
    #endif
    setUniform(_colorUniform, color);
    return *this;
}

MeshVisualizerBase& MeshVisualizerBase::setWireframeColor(const Color4& color) {
    CORRADE_ASSERT(_flags & FlagBase::Wireframe,
        "Shaders::MeshVisualizer::setWireframeColor(): the shader was not created with wireframe enabled", *this);
    setUniform(_wireframeColorUniform, color);
    return *this;
}

MeshVisualizerBase& MeshVisualizerBase::setWireframeWidth(const Float width) {
    CORRADE_ASSERT(_flags & FlagBase::Wireframe,
        "Shaders::MeshVisualizer::setWireframeWidth(): the shader was not created with wireframe enabled", *this);
    setUniform(_wireframeWidthUniform, width);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerBase& MeshVisualizerBase::setColorMapTransformation(const Float offset, const Float scale) {
    CORRADE_ASSERT(_flags & (FlagBase::InstancedObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
        "Shaders::MeshVisualizer::setColorMapTransformation(): the shader was not created with object/vertex/primitive ID enabled", *this);
    setUniform(_colorMapOffsetScaleUniform, Vector2{offset, scale});
    return *this;
}

MeshVisualizerBase& MeshVisualizerBase::bindColorMapTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & (FlagBase::InstancedObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
        "Shaders::MeshVisualizer::bindColorMapTexture(): the shader was not created with object/vertex/primitive ID enabled", *this);
    texture.bind(ColorMapTextureUnit);
    return *this;
}
#endif

}

MeshVisualizer2D::MeshVisualizer2D(const Flags flags): Implementation::MeshVisualizerBase{Implementation::MeshVisualizerBase::FlagBase(UnsignedShort(flags))} {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(flags & ((Flag::Wireframe|Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizer2D: at least one visualization feature has to be enabled", );
    #else
    CORRADE_ASSERT(flags & (Flag::Wireframe & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizer2D: at least Flag::Wireframe has to be enabled", );
    #endif

    Utility::Resource rs{"MagnumShaders"};
    GL::Shader vert{NoCreate};
    GL::Shader frag{NoCreate};
    const GL::Version version = setupShaders(vert, frag, rs);

    vert.addSource("#define TWO_DIMENSIONS\n")
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the shader code */
        .addSource((flags & Flag::NoGeometryShader) || !(flags & Flag::Wireframe) ?
            "#define NO_GEOMETRY_SHADER\n" : "")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    frag
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the shader code */
        .addSource((flags & Flag::NoGeometryShader) || !(flags & Flag::Wireframe) ?
            "#define NO_GEOMETRY_SHADER\n" : "")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.frag"));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    Containers::Optional<GL::Shader> geom;
    if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader)) {
        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        (*geom)
            .addSource("#define WIREFRAME_RENDERING\n#define MAX_VERTICES 3\n")
            .addSource(_flags & FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
            .addSource(_flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
            .addSource(_flags & FlagBase::PrimitiveId ?
                (_flags >= FlagBase::PrimitiveIdFromVertexId ?
                    "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                    "#define PRIMITIVE_ID\n") : "")
            .addSource(rs.get("MeshVisualizer.geom"));
    }
    #else
    static_cast<void>(version);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(geom) CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, *geom, frag}));
    else
    #endif
        CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(geom) attachShader(*geom);
    #endif

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::InstancedObjectId)
            bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isVersionSupported(GL::Version::GL310))
        #endif
        {
            bindAttributeLocation(VertexIndex::Location, "vertexIndex");
        }
        #endif
    }
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
        if(flags & (Flag::Wireframe
            #ifndef MAGNUM_TARGET_GLES2
            |Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
            #endif
        ))
            _colorUniform = uniformLocation("color");
        if(flags & Flag::Wireframe) {
            _wireframeColorUniform = uniformLocation("wireframeColor");
            _wireframeWidthUniform = uniformLocation("wireframeWidth");
            _smoothnessUniform = uniformLocation("smoothness");
            if(!(flags & Flag::NoGeometryShader))
                _viewportSizeUniform = uniformLocation("viewportSize");
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
            _colorMapOffsetScaleUniform = uniformLocation("colorMapOffsetScale");
            setUniform(uniformLocation("colorMapTexture"), ColorMapTextureUnit);
        }
        #endif
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setTransformationProjectionMatrix(Matrix3{Math::IdentityInit});
    if(flags & (Flag::Wireframe
        #ifndef MAGNUM_TARGET_GLES2
        |Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
        #endif
    ))
        setColor(Color3(1.0f));
    if(flags & Flag::Wireframe) {
        /* Viewport size is zero by default */
        setWireframeColor(Color3{0.0f});
        setWireframeWidth(1.0f);
        setSmoothness(2.0f);
    }
    #ifndef MAGNUM_TARGET_GLES2
    if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId))
        setColorMapTransformation(1.0f/512.0f, 1.0f/256.0f);
    #endif
    #endif
}

MeshVisualizer2D& MeshVisualizer2D::setViewportSize(const Vector2& size) {
    /* Not asserting here, since the relation to wireframe is a bit vague.
       Also it's an ugly hack that should be removed, ideally. */
    if(flags() & Flag::Wireframe && !(flags() & Flag::NoGeometryShader))
        setUniform(_viewportSizeUniform, size);
    return *this;
}

MeshVisualizer2D& MeshVisualizer2D::setTransformationProjectionMatrix(const Matrix3& matrix) {
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

MeshVisualizer2D& MeshVisualizer2D::setSmoothness(const Float smoothness) {
    /* This is a bit vaguely related but less vague than setViewportSize() so
       asserting in this case. */
    CORRADE_ASSERT(flags() & Flag::Wireframe,
        "Shaders::MeshVisualizer2D::setSmoothness(): the shader was not created with wireframe enabled", *this);
    setUniform(_smoothnessUniform, smoothness);
    return *this;
}

MeshVisualizer3D::MeshVisualizer3D(const Flags flags): Implementation::MeshVisualizerBase{Implementation::MeshVisualizerBase::FlagBase(UnsignedShort(flags))} {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_ASSERT(flags & ((Flag::Wireframe|Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection|Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizer3D: at least one visualization feature has to be enabled", );
    CORRADE_ASSERT(!(flags & Flag::NoGeometryShader && flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)),
        "Shaders::MeshVisualizer3D: geometry shader has to be enabled when rendering TBN direction", );
    CORRADE_ASSERT(!(flags & Flag::BitangentDirection && flags & Flag::BitangentFromTangentDirection),
        "Shaders::MeshVisualizer3D: Flag::BitangentDirection and Flag::BitangentFromTangentDirection are mutually exclusive", );
    #elif !defined(MAGNUM_TARGET_GLES2)
    CORRADE_ASSERT(flags & ((Flag::Wireframe|Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizer3D: at least one visualization feature has to be enabled", );
    #else
    CORRADE_ASSERT(flags & (Flag::Wireframe & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizer3D: at least Flag::Wireframe has to be enabled", );
    #endif

    Utility::Resource rs{"MagnumShaders"};
    GL::Shader vert{NoCreate};
    GL::Shader frag{NoCreate};
    const GL::Version version = setupShaders(vert, frag, rs);

    /* Expands the check done for wireframe in MeshVisualizerBase with TBN */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_INTERNAL_ASSERT(!(flags & (Flag::NormalDirection|Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection)) || version >= GL::Version::GL320);
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_INTERNAL_ASSERT(!(flags & (Flag::NormalDirection|Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection)) || version >= GL::Version::GLES310);
    #endif

    vert.addSource("#define THREE_DIMENSIONS\n")
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the vertex shader code */
        .addSource((flags & Flag::NoGeometryShader) || !(flags & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection|Flag::NormalDirection
            #endif
            )) ? "#define NO_GEOMETRY_SHADER\n" : "")
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        .addSource(flags & Flag::TangentDirection ? "#define TANGENT_DIRECTION\n" : "")
        .addSource(flags & Flag::BitangentFromTangentDirection ? "#define BITANGENT_FROM_TANGENT_DIRECTION\n" : "")
        .addSource(flags & Flag::BitangentDirection ? "#define BITANGENT_DIRECTION\n" : "")
        .addSource(flags & Flag::NormalDirection ? "#define NORMAL_DIRECTION\n" : "")
        #endif
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    frag
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the vertex shader code */
        .addSource((flags & Flag::NoGeometryShader) || !(flags & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection|Flag::NormalDirection
            #endif
            )) ? "#define NO_GEOMETRY_SHADER\n" : "")
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        .addSource(flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection) ? "#define TBN_DIRECTION\n" : "")
        #endif
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.frag"));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    Containers::Optional<GL::Shader> geom;
    if(flags & (Flag::Wireframe|Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection) && !(flags & Flag::NoGeometryShader)) {
        Int maxVertices = 0;
        if(flags & Flag::Wireframe) maxVertices += 3;
        if(flags & Flag::TangentDirection) maxVertices += 3*6;
        if(flags & (Flag::BitangentDirection|Flag::BitangentFromTangentDirection))
            maxVertices += 3*6;
        if(flags & Flag::NormalDirection) maxVertices += 3*6;

        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        (*geom)
            .addSource(Utility::formatString("#define MAX_VERTICES {}\n", maxVertices))
            .addSource(flags & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
            .addSource(_flags & FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
            .addSource(_flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
            .addSource(_flags & FlagBase::PrimitiveId ?
                (_flags >= FlagBase::PrimitiveIdFromVertexId ?
                    "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                    "#define PRIMITIVE_ID\n") : "")
            .addSource(flags & Flag::TangentDirection ? "#define TANGENT_DIRECTION\n" : "")
            .addSource(flags & (Flag::BitangentDirection|Flag::BitangentFromTangentDirection) ? "#define BITANGENT_DIRECTION\n" : "")
            .addSource(flags & Flag::NormalDirection ? "#define NORMAL_DIRECTION\n" : "")
            .addSource(rs.get("MeshVisualizer.geom"));
    }
    #else
    static_cast<void>(version);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(geom) CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, *geom, frag}));
    else
    #endif
        CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(geom) attachShader(*geom);
    #endif

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::InstancedObjectId)
            bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(flags & Flag::TangentDirection ||
           flags & Flag::BitangentFromTangentDirection)
            bindAttributeLocation(Tangent4::Location, "tangent");
        if(flags & Flag::BitangentDirection)
            bindAttributeLocation(Bitangent::Location, "bitangent");
        if(flags & Flag::NormalDirection ||
           flags & Flag::BitangentFromTangentDirection)
            bindAttributeLocation(Normal::Location, "normal");
        #endif

        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isVersionSupported(GL::Version::GL310))
        #endif
        {
            bindAttributeLocation(VertexIndex::Location, "vertexIndex");
        }
        #endif
    }
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        _transformationMatrixUniform = uniformLocation("transformationMatrix");
        _projectionMatrixUniform = uniformLocation("projectionMatrix");
        if(flags & (Flag::Wireframe
            #ifndef MAGNUM_TARGET_GLES2
            |Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
            #endif
        ))
            _colorUniform = uniformLocation("color");
        if(flags & Flag::Wireframe) {
            _wireframeColorUniform = uniformLocation("wireframeColor");
            _wireframeWidthUniform = uniformLocation("wireframeWidth");
        }
        if(flags & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection
            #endif
        )) {
            _smoothnessUniform = uniformLocation("smoothness");
            if(!(flags & Flag::NoGeometryShader))
                _viewportSizeUniform = uniformLocation("viewportSize");
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
            _colorMapOffsetScaleUniform = uniformLocation("colorMapOffsetScale");
            setUniform(uniformLocation("colorMapTexture"), ColorMapTextureUnit);
        }
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)) {
            _normalMatrixUniform = uniformLocation("normalMatrix");
            _lineWidthUniform = uniformLocation("lineWidth");
            _lineLengthUniform = uniformLocation("lineLength");
        }
        #endif
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setTransformationMatrix(Matrix4{Math::IdentityInit});
    setProjectionMatrix(Matrix4{Math::IdentityInit});
    if(flags & (Flag::Wireframe
        #ifndef MAGNUM_TARGET_GLES2
        |Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
        #endif
    ))
        setColor(Color3(1.0f));
    if(flags & Flag::Wireframe) {
        /* Viewport size is zero by default */
        setWireframeColor(Color3{0.0f});
        setWireframeWidth(1.0f);
    }
    if(flags & (Flag::Wireframe
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        |Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection
        #endif
    )) {
        setSmoothness(2.0f);
    }
    #ifndef MAGNUM_TARGET_GLES2
    if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId))
        setColorMapTransformation(1.0f/512.0f, 1.0f/256.0f);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)) {
        setNormalMatrix(Matrix3x3{Math::IdentityInit});
        setLineWidth(1.0f);
        setLineLength(1.0f);
    }
    #endif
    #endif
}

MeshVisualizer3D& MeshVisualizer3D::setTransformationMatrix(const Matrix4& matrix) {
    setUniform(_transformationMatrixUniform, matrix);
    return *this;
}

MeshVisualizer3D& MeshVisualizer3D::setProjectionMatrix(const Matrix4& matrix) {
    setUniform(_projectionMatrixUniform, matrix);
    return *this;
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
MeshVisualizer3D& MeshVisualizer3D::setNormalMatrix(const Matrix3x3& matrix) {
    CORRADE_ASSERT(flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection),
        "Shaders::MeshVisualizer3D::setNormalMatrix(): the shader was not created with TBN direction enabled", *this);
    setUniform(_normalMatrixUniform, matrix);
    return *this;
}
#endif

MeshVisualizer3D& MeshVisualizer3D::setViewportSize(const Vector2& size) {
    /* Not asserting here, since the relation to wireframe is a bit vague.
       Also it's an ugly hack that should be removed, ideally. */
    if((flags() & Flag::Wireframe && !(flags() & Flag::NoGeometryShader))
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        || flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)
        #endif
    )
        setUniform(_viewportSizeUniform, size);
    return *this;
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
MeshVisualizer3D& MeshVisualizer3D::setLineWidth(const Float width) {
    CORRADE_ASSERT(flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection),
        "Shaders::MeshVisualizer3D::setLineWidth(): the shader was not created with TBN direction enabled", *this);
    setUniform(_lineWidthUniform, width);
    return *this;
}

MeshVisualizer3D& MeshVisualizer3D::setLineLength(const Float length) {
    CORRADE_ASSERT(flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection),
        "Shaders::MeshVisualizer3D::setLineLength(): the shader was not created with TBN direction enabled", *this);
    setUniform(_lineLengthUniform, length);
    return *this;
}
#endif

MeshVisualizer3D& MeshVisualizer3D::setSmoothness(const Float smoothness) {
    #ifndef CORRADE_NO_ASSERT
    /* This is a bit vaguely related but less vague than setViewportSize() so
       asserting in this case. */
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    constexpr Flags allowed = Flag::Wireframe|Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection;
    #else
    constexpr Flags allowed = Flag::Wireframe;
    #endif
    CORRADE_ASSERT(flags() & allowed,
        "Shaders::MeshVisualizer3D::setSmoothness(): the shader was not created with wireframe or TBN direction enabled", *this);
    #endif
    setUniform(_smoothnessUniform, smoothness);
    return *this;
}

Debug& operator<<(Debug& debug, const MeshVisualizer2D::Flag value) {
    debug << "Shaders::MeshVisualizer2D::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case MeshVisualizer2D::Flag::v: return debug << "::" #v;
        _c(NoGeometryShader)
        _c(Wireframe)
        #ifndef MAGNUM_TARGET_GLES2
        _c(InstancedObjectId)
        _c(VertexId)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(PrimitiveId)
        #endif
        _c(PrimitiveIdFromVertexId)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshVisualizer3D::Flag value) {
    debug << "Shaders::MeshVisualizer3D::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case MeshVisualizer3D::Flag::v: return debug << "::" #v;
        _c(NoGeometryShader)
        _c(Wireframe)
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        _c(TangentDirection)
        _c(BitangentFromTangentDirection)
        _c(BitangentDirection)
        _c(NormalDirection)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(InstancedObjectId)
        _c(VertexId)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(PrimitiveId)
        #endif
        _c(PrimitiveIdFromVertexId)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshVisualizer2D::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::MeshVisualizer2D::Flags{}", {
        MeshVisualizer2D::Flag::Wireframe,
        /* Wireframe contains this on ES2 and WebGL 1 so it's not reported
           there */
        MeshVisualizer2D::Flag::NoGeometryShader,
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizer2D::Flag::InstancedObjectId,
        MeshVisualizer2D::Flag::VertexId,
        MeshVisualizer2D::Flag::PrimitiveIdFromVertexId, /* Superset of PrimitiveId */
        #ifndef MAGNUM_TARGET_WEBGL
        MeshVisualizer2D::Flag::PrimitiveId
        #endif
        #endif
    });
}

Debug& operator<<(Debug& debug, const MeshVisualizer3D::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::MeshVisualizer3D::Flags{}", {
        MeshVisualizer3D::Flag::Wireframe,
        /* Wireframe contains this on ES2 and WebGL 1 so it's not reported
           there */
        MeshVisualizer3D::Flag::NoGeometryShader,
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        MeshVisualizer3D::Flag::TangentDirection,
        MeshVisualizer3D::Flag::BitangentFromTangentDirection,
        MeshVisualizer3D::Flag::BitangentDirection,
        MeshVisualizer3D::Flag::NormalDirection,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizer3D::Flag::InstancedObjectId,
        MeshVisualizer3D::Flag::VertexId,
        MeshVisualizer3D::Flag::PrimitiveIdFromVertexId, /* Superset of PrimitiveId */
        #ifndef MAGNUM_TARGET_WEBGL
        MeshVisualizer3D::Flag::PrimitiveId
        #endif
        #endif
    });
}

}}
