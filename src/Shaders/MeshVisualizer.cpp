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
    if(flags & Flag::Wireframe) MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::geometry_shader4);
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::explicit_attrib_location);
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::explicit_uniform_location);

    Utility::Resource rs("MagnumShaders");

    Shader vert(Version::GL330, Shader::Type::Vertex);
    vert.addSource(flags & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        .addSource(rs.get("compatibility.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    attachShader(vert);

    if(flags & Flag::Wireframe) {
        Shader geom(Version::GL330, Shader::Type::Geometry);
        geom.addSource(rs.get("compatibility.glsl"))
            .addSource(rs.get("MeshVisualizer.geom"));
        CORRADE_INTERNAL_ASSERT_OUTPUT(geom.compile());
        attachShader(geom);
    }

    Shader frag(Version::GL330, Shader::Type::Fragment);
    frag.addSource(flags & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        .addSource(rs.get("compatibility.glsl"))
        .addSource(rs.get("MeshVisualizer.frag"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    attachShader(frag);

    link();
}

}}
