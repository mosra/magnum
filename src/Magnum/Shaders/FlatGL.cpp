/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>

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

#include "FlatGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

#ifndef MAGNUM_TARGET_GLES2
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/TextureArray.h"
#endif

namespace Magnum { namespace Shaders {

namespace {
    enum: Int {
        TextureUnit = 0,
        /* 1/2/3 taken by Phong (D/S/N), 4 by MeshVisualizer colormap */
        ObjectIdTextureUnit = 5 /* shared with Phong and MeshVisualizer */
    };

    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        /* Not using the zero binding to avoid conflicts with
           ProjectionBufferBinding from other shaders which can likely stay
           bound to the same buffer for the whole time */
        TransformationProjectionBufferBinding = 1,
        DrawBufferBinding = 2,
        TextureTransformationBufferBinding = 3,
        MaterialBufferBinding = 4
    };
    #endif
}

template<UnsignedInt dimensions> typename FlatGL<dimensions>::CompileState FlatGL<dimensions>::compile(const Configuration& configuration) {
    #ifndef CORRADE_NO_ASSERT
    {
        const bool textureTransformationNotEnabledOrTextured = !(configuration.flags() & Flag::TextureTransformation) || configuration.flags() & Flag::Textured
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
            ;
        CORRADE_ASSERT(textureTransformationNotEnabledOrTextured,
            "Shaders::FlatGL: texture transformation enabled but the shader is not textured", CompileState{NoCreate});
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.materialCount(),
        "Shaders::FlatGL: material count can't be zero", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
        "Shaders::FlatGL: draw count can't be zero", CompileState{NoCreate});
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() & Flag::TextureArrays) || configuration.flags() & Flag::Textured || configuration.flags() >= Flag::ObjectIdTexture,
        "Shaders::FlatGL: texture arrays enabled but the shader is not textured", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::UniformBuffers) || !(configuration.flags() & Flag::TextureArrays) || configuration.flags() >= (Flag::TextureArrays|Flag::TextureTransformation),
        "Shaders::FlatGL: texture arrays require texture transformation enabled as well if uniform buffers are used", CompileState{NoCreate});
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(configuration.flags() >= Flag::UniformBuffers)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::uniform_buffer_object);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::shader_draw_parameters);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ANGLE::multi_draw);
        #else
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::WEBGL::multi_draw);
        #endif
    }
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(configuration.flags() >= Flag::TextureArrays)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::texture_array);
    #endif

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShadersGL"))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShadersGL");

    const GL::Context& context = GL::Context::current();

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version version = context.supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    GL::Shader frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource((configuration.flags() & Flag::Textured
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
            ) ? "#define TEXTURED\n" : "")
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        .addSource(configuration.flags() & Flag::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
        #endif
        .addSource(dimensions == 2 ? "#define TWO_DIMENSIONS\n" : "#define THREE_DIMENSIONS\n")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        .addSource(configuration.flags() & Flag::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n" : "")
        .addSource(configuration.flags() >= Flag::InstancedTextureOffset ? "#define INSTANCED_TEXTURE_OFFSET\n" : "");
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        vert.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n",
            configuration.drawCount()));
        vert.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    vert.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("Flat.vert"));
    frag.addSource(configuration.flags() & Flag::Textured ? "#define TEXTURED\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
        #endif
        .addSource(configuration.flags() & Flag::AlphaMask ? "#define ALPHA_MASK\n" : "")
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::ObjectId ? "#define OBJECT_ID\n" : "")
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        .addSource(configuration.flags() >= Flag::ObjectIdTexture ? "#define OBJECT_ID_TEXTURE\n" : "")
        #endif
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        frag.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n"
            "#define MATERIAL_COUNT {}\n",
            configuration.drawCount(),
            configuration.materialCount()));
        frag.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    frag.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("Flat.frag"));

    vert.submitCompile();
    frag.submitCompile();

    FlatGL<dimensions> out{NoInit};
    out._flags = configuration.flags();
    #ifndef MAGNUM_TARGET_GLES2
    out._materialCount = configuration.materialCount();
    out._drawCount = configuration.drawCount();
    #endif

    out.attachShaders({vert, frag});

    /* ES3 has this done in the shader directly and doesn't even provide
       bindFragmentDataLocation() */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        out.bindAttributeLocation(Position::Location, "position");
        if(configuration.flags() & Flag::Textured
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
        )
            out.bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
        if(configuration.flags() & Flag::VertexColor)
            out.bindAttributeLocation(Color3::Location, "vertexColor"); /* Color4 is the same */
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() & Flag::ObjectId) {
            out.bindFragmentDataLocation(ColorOutput, "color");
            out.bindFragmentDataLocation(ObjectIdOutput, "objectId");
        }
        if(configuration.flags() >= Flag::InstancedObjectId)
            out.bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        if(configuration.flags() & Flag::InstancedTransformation)
            out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix");
        if(configuration.flags() >= Flag::InstancedTextureOffset)
            out.bindAttributeLocation(TextureOffset::Location, "instancedTextureOffset");
    }
    #endif

    out.submitLink();

    return CompileState{std::move(out), std::move(vert), std::move(frag), version};
}

template<UnsignedInt dimensions> typename FlatGL<dimensions>::CompileState FlatGL<dimensions>::compile() {
    return compile(Configuration{});
}

#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> typename FlatGL<dimensions>::CompileState FlatGL<dimensions>::compile(const Flags flags) {
    return compile(Configuration{}
        .setFlags(flags));
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> typename FlatGL<dimensions>::CompileState FlatGL<dimensions>::compile(const Flags flags, const UnsignedInt materialCount, const UnsignedInt drawCount) {
    return compile(Configuration{}
        .setFlags(flags)
        .setMaterialCount(materialCount)
        .setDrawCount(drawCount));
}
#endif
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(CompileState&& state): FlatGL{static_cast<FlatGL&&>(std::move(state))} {
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* When graceful assertions fire from within compile(), we get a NoCreate'd
       CompileState. Exiting makes it possible to test the assert. */
    if(!id()) return;
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

    const GL::Context& context = GL::Context::current();
    const GL::Version version = state._version;

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::UniformBuffers) {
            if(_drawCount > 1) _drawOffsetUniform = uniformLocation("drawOffset");
        } else
        #endif
        {
            _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
            if(_flags & Flag::TextureTransformation)
                _textureMatrixUniform = uniformLocation("textureMatrix");
            #ifndef MAGNUM_TARGET_GLES2
            if(_flags & Flag::TextureArrays)
                _textureLayerUniform = uniformLocation("textureLayer");
            #endif
            _colorUniform = uniformLocation("color");
            if(_flags & Flag::AlphaMask) _alphaMaskUniform = uniformLocation("alphaMask");
            #ifndef MAGNUM_TARGET_GLES2
            if(_flags & Flag::ObjectId) _objectIdUniform = uniformLocation("objectId");
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        if(_flags & Flag::Textured) setUniform(uniformLocation("textureData"), TextureUnit);
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::ObjectIdTexture) setUniform(uniformLocation("objectIdTextureData"), ObjectIdTextureUnit);
        if(_flags >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("TransformationProjection"), TransformationProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            if(_flags & Flag::TextureTransformation)
                setUniformBlockBinding(uniformBlockIndex("TextureTransformation"), TextureTransformationBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
        }
        #endif
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(_flags >= Flag::UniformBuffers) {
        /* Draw offset is zero by default */
    } else
    #endif
    {
        setTransformationProjectionMatrix(MatrixTypeFor<dimensions, Float>{Math::IdentityInit});
        if(_flags & Flag::TextureTransformation)
            setTextureMatrix(Matrix3{Math::IdentityInit});
        /* Texture layer is zero by default */
        setColor(Magnum::Color4{1.0f});
        if(_flags & Flag::AlphaMask) setAlphaMask(0.5f);
        /* Object ID is zero by default */
    }
    #endif

    static_cast<void>(version);
    static_cast<void>(context);
}

template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(const Configuration& configuration): FlatGL{compile(configuration)} {}

template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(): FlatGL{Configuration{}} {}

#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(const Flags flags): FlatGL{compile(Configuration{}
    .setFlags(flags))} {}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(const Flags flags, const UnsignedInt materialCount, const UnsignedInt drawCount): FlatGL{compile(Configuration{}
    .setFlags(flags)
    .setMaterialCount(materialCount)
    .setDrawCount(drawCount))} {}
#endif
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(NoInitT) {}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setTextureMatrix(const Matrix3& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setTextureMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::FlatGL::setTextureMatrix(): the shader was not created with texture transformation enabled", *this);
    setUniform(_textureMatrixUniform, matrix);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setTextureLayer(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setTextureLayer(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::FlatGL::setTextureLayer(): the shader was not created with texture arrays enabled", *this);
    setUniform(_textureLayerUniform, id);
    return *this;
}
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setColor(const Magnum::Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_colorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setAlphaMask(Float mask) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setAlphaMask(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::AlphaMask,
        "Shaders::FlatGL::setAlphaMask(): the shader was not created with alpha mask enabled", *this);
    setUniform(_alphaMaskUniform, mask);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setObjectId(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setObjectId(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::ObjectId,
        "Shaders::FlatGL::setObjectId(): the shader was not created with object ID enabled", *this);
    setUniform(_objectIdUniform, id);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setDrawOffset(const UnsignedInt offset) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::FlatGL::setDrawOffset(): draw offset" << offset << "is out of bounds for" << _drawCount << "draws", *this);
    if(_drawCount > 1) setUniform(_drawOffsetUniform, offset);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTextureTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTextureTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
    return *this;
}
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::Textured,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texturing enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::FlatGL::bindTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    texture.bind(TextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::Textured,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texturing enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    texture.bind(TextureUnit);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindObjectIdTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags >= Flag::ObjectIdTexture,
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    texture.bind(ObjectIdTextureUnit);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindObjectIdTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags >= Flag::ObjectIdTexture,
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    texture.bind(ObjectIdTextureUnit);
    return *this;
}
#endif

template class MAGNUM_SHADERS_EXPORT FlatGL<2>;
template class MAGNUM_SHADERS_EXPORT FlatGL<3>;

namespace Implementation {

Debug& operator<<(Debug& debug, const FlatGLFlag value) {
    #ifndef MAGNUM_TARGET_GLES2
    /* Special case coming from the FlatGLFlags printer. As both flags are a
       superset of ObjectId, printing just one would result in
       `Flag::InstancedObjectId|Flag(0x800)` in the output. */
    if(value == FlatGLFlag(UnsignedShort(FlatGLFlag::InstancedObjectId|FlatGLFlag::ObjectIdTexture)))
        return debug << FlatGLFlag::InstancedObjectId << Debug::nospace << "|" << Debug::nospace << FlatGLFlag::ObjectIdTexture;
    #endif

    debug << "Shaders::FlatGL::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case FlatGLFlag::v: return debug << "::" #v;
        _c(Textured)
        _c(AlphaMask)
        _c(VertexColor)
        _c(TextureTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        _c(ObjectId)
        _c(InstancedObjectId)
        _c(ObjectIdTexture)
        #endif
        _c(InstancedTransformation)
        _c(InstancedTextureOffset)
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        _c(MultiDraw)
        _c(TextureArrays)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const FlatGLFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::FlatGL::Flags{}", {
        FlatGLFlag::Textured,
        FlatGLFlag::AlphaMask,
        FlatGLFlag::VertexColor,
        FlatGLFlag::InstancedTextureOffset, /* Superset of TextureTransformation */
        FlatGLFlag::TextureTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        /* Both are a superset of ObjectId, meaning printing just one would
           result in `Flag::InstancedObjectId|Flag(0x800)` in the output. So we
           pass both and let the FlatGLFlag printer deal with that. */
        FlatGLFlag(UnsignedShort(FlatGLFlag::InstancedObjectId|FlatGLFlag::ObjectIdTexture)),
        FlatGLFlag::InstancedObjectId, /* Superset of ObjectId */
        FlatGLFlag::ObjectIdTexture, /* Superset of ObjectId */
        FlatGLFlag::ObjectId,
        #endif
        FlatGLFlag::InstancedTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        FlatGLFlag::MultiDraw, /* Superset of UniformBuffers */
        FlatGLFlag::UniformBuffers,
        FlatGLFlag::TextureArrays
        #endif
    });
}

}

}}
