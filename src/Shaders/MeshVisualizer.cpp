/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Utility/Resource.h>

#include "Context.h"
#include "Extensions.h"
#include "Shader.h"

namespace Magnum { namespace Shaders {

MeshVisualizer::MeshVisualizer(const Flags flags): flags(flags), transformationProjectionMatrixUniform(0), viewportSizeUniform(1), colorUniform(2), wireframeColorUniform(3), wireframeWidthUniform(4), smoothnessUniform(5) {
    #ifndef MAGNUM_TARGET_GLES
    if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader))
        MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::geometry_shader4);
    #endif

    Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    const Version v = Context::current()->supportedVersion({Version::GL320, Version::GL310, Version::GL210});
    #else
    const Version v = Context::current()->supportedVersion({Version::GLES300, Version::GLES200});
    #endif

    Shader vert(v, Shader::Type::Vertex);
    vert.addSource(flags & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        .addSource(flags & Flag::NoGeometryShader ? "#define NO_GEOMETRY_SHADER\n" : "")
        .addSource(rs.get("compatibility.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    vert.compile();
    attachShader(vert);

    #ifndef MAGNUM_TARGET_GLES
    if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader)) {
        Shader geom(v, Shader::Type::Geometry);
        geom.addSource(rs.get("compatibility.glsl"))
            .addSource(rs.get("MeshVisualizer.geom"));
        CORRADE_INTERNAL_ASSERT_OUTPUT(geom.compile());
        geom.compile();
        attachShader(geom);
    }
    #endif

    Shader frag(v, Shader::Type::Fragment);
    frag.addSource(flags & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        .addSource(flags & Flag::NoGeometryShader ? "#define NO_GEOMETRY_SHADER\n" : "")
        .addSource(rs.get("compatibility.glsl"))
        .addSource(rs.get("MeshVisualizer.frag"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    frag.compile();
    attachShader(frag);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>() ||
        Context::current()->version() == Version::GL210)
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");

        #ifndef MAGNUM_TARGET_GLES
        if(!Context::current()->isVersionSupported(Version::GL310))
        #endif
        {
            bindAttributeLocation(VertexIndex::Location, "vertexIndex");
        }
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
    link();

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>())
    #endif
    {
        transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
        colorUniform = uniformLocation("color");
        if(flags & Flag::Wireframe) {
            wireframeColorUniform = uniformLocation("wireframeColor");
            wireframeWidthUniform = uniformLocation("wireframeWidth");
            smoothnessUniform = uniformLocation("smoothness");
            if(!(flags & Flag::NoGeometryShader))
                viewportSizeUniform = uniformLocation("viewportSize");
        }
    }
}

}}
