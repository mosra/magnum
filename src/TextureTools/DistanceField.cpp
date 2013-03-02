/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
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

    Corrade::Utility::Resource rs("MagnumTextureTools");
    attachShader(Shader::fromData(Version::GL330, Shader::Type::Vertex, rs.get("DistanceFieldShader.vert")));

    Shader fragmentShader(Version::GL330, Shader::Type::Fragment);
    fragmentShader.addSource(rs.get("compatibility.glsl"));
    fragmentShader.addSource(rs.get("DistanceFieldShader.frag"));
    attachShader(fragmentShader);

    link();
}

}

void distanceField(Texture2D* input, Texture2D* output, const Rectanglei& rectangle, const Int radius) {
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::EXT::framebuffer_object);

    /** @todo Disable depth test and then enable it back (if was previously) */

    Framebuffer framebuffer(rectangle);
    framebuffer.attachTexture2D(Framebuffer::ColorAttachment(0), output, 0);
    framebuffer.bind(Framebuffer::Target::Draw);

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
