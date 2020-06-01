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

#include "DistanceField.h"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/Math/Range.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

#ifdef MAGNUM_BUILD_STATIC
static void importTextureToolResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumTextureTools_RCS)
}
#endif

namespace Magnum { namespace TextureTools {

namespace {

class DistanceFieldShader: public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector2> Position;

        explicit DistanceFieldShader(UnsignedInt radius);

        DistanceFieldShader& setScaling(const Vector2& scaling) {
            setUniform(scalingUniform, scaling);
            return *this;
        }

        DistanceFieldShader& setImageSizeInverted(const Vector2& size) {
            setUniform(imageSizeInvertedUniform, size);
            return *this;
        }

        DistanceFieldShader& bindTexture(GL::Texture2D& texture) {
            texture.bind(TextureUnit);
            return *this;
        }

    private:
        /* ES2 on iOS (apparently independent on the device) has only 8 texture
           units, so be careful to not step over that. ES3 on the same has 16.
           Not using the default (0) because this shader is quite specific.
           Unit 6 is used by Shaders::Vector and Shaders::DistanceFieldVector. */
        enum: Int { TextureUnit = 7 };

        Int scalingUniform{0},
            imageSizeInvertedUniform;
};

DistanceFieldShader::DistanceFieldShader(const UnsignedInt radius) {
    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumTextureTools"))
        importTextureToolResources();
    #endif
    Utility::Resource rs("MagnumTextureTools");

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version v = GL::Context::current().supportedVersion({GL::Version::GL320, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version v = GL::Context::current().supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert = Shaders::Implementation::createCompatibilityShader(rs, v, GL::Shader::Type::Vertex);
    GL::Shader frag = Shaders::Implementation::createCompatibilityShader(rs, v, GL::Shader::Type::Fragment);

    vert.addSource(rs.get("FullScreenTriangle.glsl"))
        .addSource(rs.get("DistanceFieldShader.vert"));
    frag.addSource(Utility::formatString("#define RADIUS {}\n", radius))
        .addSource(rs.get("DistanceFieldShader.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    #ifndef MAGNUM_TARGET_GLES2
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>())
    #endif
    {
        scalingUniform = uniformLocation("scaling");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isVersionSupported(GL::Version::GL320))
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES300))
        #endif
        {
            imageSizeInvertedUniform = uniformLocation("imageSizeInverted");
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>())
    #endif
    {
        setUniform(uniformLocation("textureData"), TextureUnit);
    }
}

}

struct DistanceField::State {
    explicit State(UnsignedInt radius): shader{radius}, radius{radius} {}

    DistanceFieldShader shader;
    UnsignedInt radius;
    GL::Mesh mesh;
};

DistanceField::DistanceField(const UnsignedInt radius): _state{new State{radius}} {
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::framebuffer_object);
    #endif

    _state->mesh.setPrimitive(GL::MeshPrimitive::Triangles)
        .setCount(3);

    #ifndef MAGNUM_TARGET_GLES2
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
    #endif
    {
        constexpr Vector2 triangle[] = {
            Vector2(-1.0,  1.0),
            Vector2(-1.0, -3.0),
            Vector2( 3.0,  1.0)
        };
        GL::Buffer buffer;
        buffer.setData(triangle, GL::BufferUsage::StaticDraw);
        _state->mesh.addVertexBuffer(std::move(buffer), 0, DistanceFieldShader::Position());
    }
}

DistanceField::~DistanceField() = default;

UnsignedInt DistanceField::radius() const { return _state->radius; }

void DistanceField::operator()(GL::Texture2D& input, GL::Texture2D& output, const Range2Di& rectangle, const Vector2i&
    #ifdef MAGNUM_TARGET_GLES
    imageSize
    #endif
) {
    /** @todo Disable depth test, blending and then enable it back (if was previously) */

    #ifndef MAGNUM_TARGET_GLES
    Vector2i imageSize = input.imageSize(0);
    #endif

    /* Framebuffer is instantiated here so it gets correctly unbound at the end
       (and bound framebuffer reset back to the default) */
    GL::Framebuffer framebuffer{rectangle};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment(0), output, 0)
        .clear(GL::FramebufferClear::Color)
        .bind();

    const GL::Framebuffer::Status status = framebuffer.checkStatus(GL::FramebufferTarget::Draw);
    if(status != GL::Framebuffer::Status::Complete) {
        Error() << "TextureTools::DistanceField: cannot render to given output texture, unexpected framebuffer status"
                << status;
        return;
    }

    _state->shader.setScaling(Vector2(imageSize)/Vector2(rectangle.size()))
        .bindTexture(input);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isVersionSupported(GL::Version::GL320))
    #else
    if(!GL::Context::current().isVersionSupported(GL::Version::GLES300))
    #endif
    {
        _state->shader.setImageSizeInverted(1.0f/Vector2(imageSize));
    }

    /* Draw the mesh */
    _state->shader.draw(_state->mesh);
}

}}
