/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "VertexColor.h"

#include <Corrade/Utility/Resource.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Shader.h"

#include "Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace {
    template<UnsignedInt> constexpr const char* vertexShaderName();
    template<> constexpr const char* vertexShaderName<2>() { return "VertexColor2D.vert"; }
    template<> constexpr const char* vertexShaderName<3>() { return "VertexColor3D.vert"; }
}

template<UnsignedInt dimensions> VertexColor<dimensions>::VertexColor() {
    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShaders"))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    const Version version = Context::current().supportedVersion({Version::GL320, Version::GL310, Version::GL300, Version::GL210});
    #else
    const Version version = Context::current().supportedVersion({Version::GLES300, Version::GLES200});
    #endif

    Shader vert = Implementation::createCompatibilityShader(rs, version, Shader::Type::Vertex);
    Shader frag = Implementation::createCompatibilityShader(rs, version, Shader::Type::Fragment);

    vert.addSource(rs.get("generic.glsl"))
        .addSource(rs.get(vertexShaderName<dimensions>()));
    frag.addSource(rs.get("VertexColor.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>(version))
    #else
    if(!Context::current().isVersionSupported(Version::GLES300))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(Color::Location, "color");
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>(version))
    #endif
    {
        _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setTransformationProjectionMatrix(MatrixTypeFor<dimensions, Float>{});
    #endif
}

template class VertexColor<2>;
template class VertexColor<3>;

}}
