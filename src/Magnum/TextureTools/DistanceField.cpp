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

#include "DistanceField.h"

#include <Corrade/Utility/Resource.h>

#include "Magnum/Math/Range.h"
#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/Shader.h"
#include "Magnum/Texture.h"
#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

#ifdef MAGNUM_BUILD_STATIC
static void importTextureToolResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumTextureTools_RCS)
}
#endif

namespace Magnum { namespace TextureTools {

namespace {

class DistanceFieldShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector2> Position;

        explicit DistanceFieldShader();

        DistanceFieldShader& setRadius(Int radius) {
            setUniform(radiusUniform, radius);
            return *this;
        }

        DistanceFieldShader& setScaling(const Vector2& scaling) {
            setUniform(scalingUniform, scaling);
            return *this;
        }

        DistanceFieldShader& setImageSizeInverted(const Vector2& size) {
            setUniform(imageSizeInvertedUniform, size);
            return *this;
        }

        DistanceFieldShader& setTexture(Texture2D& texture) {
            texture.bind(TextureUnit);
            return *this;
        }

    private:
        enum: Int { TextureUnit = 8 };

        Int radiusUniform{0},
            scalingUniform{1},
            imageSizeInvertedUniform;
};

DistanceFieldShader::DistanceFieldShader() {
    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumTextureTools"))
        importTextureToolResources();
    #endif
    Utility::Resource rs("MagnumTextureTools");

    #ifndef MAGNUM_TARGET_GLES
    const Version v = Context::current().supportedVersion({Version::GL320, Version::GL300, Version::GL210});
    #else
    const Version v = Context::current().supportedVersion({Version::GLES300, Version::GLES200});
    #endif

    Shader vert = Shaders::Implementation::createCompatibilityShader(rs, v, Shader::Type::Vertex);
    Shader frag = Shaders::Implementation::createCompatibilityShader(rs, v, Shader::Type::Fragment);

    vert.addSource(rs.get("FullScreenTriangle.glsl"))
        .addSource(rs.get("DistanceFieldShader.vert"));
    frag.addSource(rs.get("DistanceFieldShader.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    /* Older GLSL doesn't have gl_VertexID, vertices must be supplied explicitly */
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL300))
    #else
    if(!Context::current().isVersionSupported(Version::GLES300))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>())
    #endif
    {
        radiusUniform = uniformLocation("radius");
        scalingUniform = uniformLocation("scaling");

        #ifndef MAGNUM_TARGET_GLES
        if(!Context::current().isVersionSupported(Version::GL320))
        #else
        if(!Context::current().isVersionSupported(Version::GLES300))
        #endif
        {
            imageSizeInvertedUniform = uniformLocation("imageSizeInverted");
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shading_language_420pack>())
    #endif
    {
        setUniform(uniformLocation("textureData"), TextureUnit);
    }
}

}
#ifndef MAGNUM_TARGET_GLES
void distanceField(Texture2D& input, Texture2D& output, const Range2Di& rectangle, const Int radius, const Vector2i&)
#else
void distanceField(Texture2D& input, Texture2D& output, const Range2Di& rectangle, const Int radius, const Vector2i& imageSize)
#endif
{
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::framebuffer_object);
    #endif

    /** @todo Disable depth test, blending and then enable it back (if was previously) */

    #ifndef MAGNUM_TARGET_GLES
    Vector2i imageSize = input.imageSize(0);
    #endif

    Framebuffer framebuffer(rectangle);
    framebuffer.attachTexture(Framebuffer::ColorAttachment(0), output, 0);
    framebuffer.bind();
    framebuffer.clear(FramebufferClear::Color);

    const Framebuffer::Status status = framebuffer.checkStatus(FramebufferTarget::Draw);
    if(status != Framebuffer::Status::Complete) {
        Error() << "TextureTools::distanceField(): cannot render to given output texture, unexpected framebuffer status"
                << status;
        return;
    }

    DistanceFieldShader shader;
    shader.setRadius(radius)
        .setScaling(Vector2(imageSize)/Vector2(rectangle.size()))
        .setTexture(input);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL320))
    #else
    if(!Context::current().isVersionSupported(Version::GLES300))
    #endif
    {
        shader.setImageSizeInverted(1.0f/Vector2(imageSize));
    }

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(3);

    /* Older GLSL doesn't have gl_VertexID, vertices must be supplied explicitly */
    Buffer buffer;
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL300))
    #else
    if(!Context::current().isVersionSupported(Version::GLES300))
    #endif
    {
        constexpr Vector2 triangle[] = {
            Vector2(-1.0,  1.0),
            Vector2(-1.0, -3.0),
            Vector2( 3.0,  1.0)
        };
        buffer.setData(triangle, BufferUsage::StaticDraw);
        mesh.addVertexBuffer(buffer, 0, DistanceFieldShader::Position());
    }

    /* Draw the mesh */
    mesh.draw(shader);
}

}}
