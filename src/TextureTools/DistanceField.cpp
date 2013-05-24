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

#include "TextureTools/DistanceField.h"

#include <Utility/Resource.h>
#include "Math/Geometry/Rectangle.h"
#include "AbstractShaderProgram.h"
#include "Extensions.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

namespace Magnum { namespace TextureTools {

namespace {

class DistanceFieldShader: public AbstractShaderProgram {
    public:
        enum: Int {
            TextureLayer = 8
        };

        explicit DistanceFieldShader();

        inline DistanceFieldShader* setRadius(Int radius) {
            setUniform(radiusUniform, radius);
            return this;
        }

        inline DistanceFieldShader* setScaling(Vector2 scaling) {
            setUniform(scalingUniform, scaling);
            return this;
        }

    private:
        static const Int radiusUniform = 0,
            scalingUniform = 1;
};

DistanceFieldShader::DistanceFieldShader() {
    MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::explicit_attrib_location);
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::explicit_uniform_location);
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::shading_language_420pack);

    /** @todo compatibility! */

    Utility::Resource rs("MagnumTextureTools");

    Shader vert(Version::GL330, Shader::Type::Vertex);
    vert.addSource(rs.get("DistanceFieldShader.vert"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    attachShader(vert);

    Shader frag(Version::GL330, Shader::Type::Fragment);
    frag.addSource(rs.get("compatibility.glsl"))
        .addSource(rs.get("DistanceFieldShader.frag"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    attachShader(frag);

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

}

void distanceField(Texture2D* input, Texture2D* output, const Rectanglei& rectangle, const Int radius) {
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::EXT::framebuffer_object);

    /** @todo Disable depth test, blending and then enable it back (if was previously) */

    Framebuffer framebuffer(rectangle);
    framebuffer.attachTexture2D(Framebuffer::ColorAttachment(0), output, 0);
    framebuffer.bind(FramebufferTarget::Draw);

    DistanceFieldShader shader;
    shader.setRadius(radius)
        ->setScaling(Vector2(input->imageSize(0))/rectangle.size())
        ->use();

    input->bind(DistanceFieldShader::TextureLayer);

    Mesh mesh;
    mesh.setPrimitive(Mesh::Primitive::Triangles)
        ->setVertexCount(3)
        ->draw();
}

}}
