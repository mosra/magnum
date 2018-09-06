/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

MeshVisualizer::MeshVisualizer(const Flags flags): _flags{flags} {
    #ifndef MAGNUM_TARGET_GLES2
    if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader)) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL320);
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::geometry_shader4);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::geometry_shader);
        #endif
    }
    #else
    if(_flags & Flag::Wireframe)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::OES::standard_derivatives);
    #endif

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShaders"))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    CORRADE_INTERNAL_ASSERT(!flags || flags & Flag::NoGeometryShader || version >= GL::Version::GL320);
    #elif !defined(MAGNUM_TARGET_WEBGL)
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES310, GL::Version::GLES300, GL::Version::GLES200});
    CORRADE_INTERNAL_ASSERT(!flags || flags & Flag::NoGeometryShader || version >= GL::Version::GLES310);
    #else
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    GL::Shader frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource(flags & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        .addSource(flags & Flag::NoGeometryShader ? "#define NO_GEOMETRY_SHADER\n" : "")
        #ifdef MAGNUM_TARGET_WEBGL
        .addSource("#define SUBSCRIPTING_WORKAROUND\n")
        #elif defined(MAGNUM_TARGET_GLES2)
        .addSource(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::Angle ?
            "#define SUBSCRIPTING_WORKAROUND\n" : "")
        #endif
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    frag.addSource(flags & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        .addSource(flags & Flag::NoGeometryShader ? "#define NO_GEOMETRY_SHADER\n" : "")
        .addSource(rs.get("MeshVisualizer.frag"));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    Containers::Optional<GL::Shader> geom;
    if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader)) {
        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        geom->addSource(rs.get("MeshVisualizer.geom"));
    }
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

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #else
    if(!GL::Context::current().isVersionSupported(GL::Version::GLES300))
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
    if(_flags & Flag::Wireframe) {
        /* Viewport size is zero by default */
        setWireframeColor(Color3{0.0f});
        setWireframeWidth(1.0f);
        setSmoothness(2.0f);
    }
    #endif
}

Debug& operator<<(Debug& debug, const MeshVisualizer::Flag value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case MeshVisualizer::Flag::v: return debug << "Shaders::MeshVisualizer::Flag::" #v;
        _c(NoGeometryShader)
        _c(Wireframe)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Shaders::MeshVisualizer::Flag(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshVisualizer::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::MeshVisualizer::Flags{}", {
        MeshVisualizer::Flag::Wireframe,
        /* Wireframe contains this on ES2 so it's not reported there */
        MeshVisualizer::Flag::NoGeometryShader
    });
}

}}
