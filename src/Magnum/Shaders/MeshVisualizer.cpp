/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "MeshVisualizer.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace Implementation {

MeshVisualizerBase::MeshVisualizerBase(FlagsBase flags): _flags{flags} {
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

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShaders"))
        importShaderResources();
    #endif
}

GL::Version MeshVisualizerBase::setupShaders(GL::Shader& vert, GL::Shader& frag, const Utility::Resource& rs) const {
    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    CORRADE_INTERNAL_ASSERT(!_flags || _flags & FlagBase::NoGeometryShader || version >= GL::Version::GL320);
    #elif !defined(MAGNUM_TARGET_WEBGL)
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES310, GL::Version::GLES300, GL::Version::GLES200});
    CORRADE_INTERNAL_ASSERT(!_flags || _flags & FlagBase::NoGeometryShader || version >= GL::Version::GLES310);
    #else
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource(_flags & FlagBase::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        .addSource(_flags & FlagBase::NoGeometryShader ? "#define NO_GEOMETRY_SHADER\n" : "")
        #ifdef MAGNUM_TARGET_WEBGL
        .addSource("#define SUBSCRIPTING_WORKAROUND\n")
        #elif defined(MAGNUM_TARGET_GLES2)
        .addSource(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Angle ?
            "#define SUBSCRIPTING_WORKAROUND\n" : "")
        #endif
        ;
    frag.addSource(_flags & FlagBase::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        .addSource(_flags & FlagBase::NoGeometryShader ? "#define NO_GEOMETRY_SHADER\n" : "");

    return version;
}

MeshVisualizerBase& MeshVisualizerBase::setViewportSize(const Vector2& size) {
    /* Not asserting here, since the relation to wireframe is a bit vague.
       Also it's an ugly hack that should be removed, ideally. */
    if(_flags & FlagBase::Wireframe && !(_flags & FlagBase::NoGeometryShader))
        setUniform(_viewportSizeUniform, size);
    return *this;
}

MeshVisualizerBase& MeshVisualizerBase::setColor(const Color4& color) {
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

MeshVisualizerBase& MeshVisualizerBase::setSmoothness(const Float smoothness) {
    /* This is a bit vaguely related too, but less vague than setViewportSize()
       so asserting. */
    CORRADE_ASSERT(_flags & FlagBase::Wireframe,
        "Shaders::MeshVisualizer::setSmoothness(): the shader was not created with wireframe enabled", *this);
    setUniform(_smoothnessUniform, smoothness);
    return *this;
}

}

MeshVisualizer2D::MeshVisualizer2D(const Flags flags): Implementation::MeshVisualizerBase{Implementation::MeshVisualizerBase::FlagBase(UnsignedByte(flags))} {
    Utility::Resource rs{"MagnumShaders"};
    GL::Shader vert{NoCreate};
    GL::Shader frag{NoCreate};
    const GL::Version version = setupShaders(vert, frag, rs);

    vert.addSource("#define TWO_DIMENSIONS\n")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    frag.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.frag"));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    Containers::Optional<GL::Shader> geom;
    if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader)) {
        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        geom->addSource(rs.get("MeshVisualizer.geom"));
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
        _colorUniform = uniformLocation("color");
        if(flags & Flag::Wireframe) {
            _wireframeColorUniform = uniformLocation("wireframeColor");
            _wireframeWidthUniform = uniformLocation("wireframeWidth");
            _smoothnessUniform = uniformLocation("smoothness");
            if(!(flags & Flag::NoGeometryShader))
                _viewportSizeUniform = uniformLocation("viewportSize");
        }
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setTransformationProjectionMatrix({});
    setColor(Color3(1.0f));
    if(flags & Flag::Wireframe) {
        /* Viewport size is zero by default */
        setWireframeColor(Color3{0.0f});
        setWireframeWidth(1.0f);
        setSmoothness(2.0f);
    }
    #endif
}

MeshVisualizer2D& MeshVisualizer2D::setTransformationProjectionMatrix(const Matrix3& matrix) {
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

MeshVisualizer3D::MeshVisualizer3D(const Flags flags): Implementation::MeshVisualizerBase{Implementation::MeshVisualizerBase::FlagBase(UnsignedByte(flags))} {
    Utility::Resource rs{"MagnumShaders"};
    GL::Shader vert{NoCreate};
    GL::Shader frag{NoCreate};
    const GL::Version version = setupShaders(vert, frag, rs);

    vert.addSource("#define THREE_DIMENSIONS\n")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    frag.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.frag"));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    Containers::Optional<GL::Shader> geom;
    if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader)) {
        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        geom->addSource(rs.get("MeshVisualizer.geom"));
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
        _colorUniform = uniformLocation("color");
        if(flags & Flag::Wireframe) {
            _wireframeColorUniform = uniformLocation("wireframeColor");
            _wireframeWidthUniform = uniformLocation("wireframeWidth");
            _smoothnessUniform = uniformLocation("smoothness");
            if(!(flags & Flag::NoGeometryShader))
                _viewportSizeUniform = uniformLocation("viewportSize");
        }
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setTransformationProjectionMatrix({});
    setColor(Color3(1.0f));
    if(flags & Flag::Wireframe) {
        /* Viewport size is zero by default */
        setWireframeColor(Color3{0.0f});
        setWireframeWidth(1.0f);
        setSmoothness(2.0f);
    }
    #endif
}

MeshVisualizer3D& MeshVisualizer3D::setTransformationProjectionMatrix(const Matrix4& matrix) {
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

Debug& operator<<(Debug& debug, const MeshVisualizer2D::Flag value) {
    debug << "Shaders::MeshVisualizer2D::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case MeshVisualizer2D::Flag::v: return debug << "::" #v;
        _c(NoGeometryShader)
        _c(Wireframe)
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
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshVisualizer2D::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::MeshVisualizer2D::Flags{}", {
        MeshVisualizer2D::Flag::Wireframe,
        /* Wireframe contains this on ES2 so it's not reported there */
        MeshVisualizer2D::Flag::NoGeometryShader
    });
}

Debug& operator<<(Debug& debug, const MeshVisualizer3D::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::MeshVisualizer3D::Flags{}", {
        MeshVisualizer3D::Flag::Wireframe,
        /* Wireframe contains this on ES2 so it's not reported there */
        MeshVisualizer3D::Flag::NoGeometryShader
    });
}

}}
