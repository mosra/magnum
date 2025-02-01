/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "LineGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/Line.h"
#include "Magnum/Shaders/Implementation/lineMiterLimit.h"

#ifdef MAGNUM_BUILD_STATIC
static void importShaderResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumShaders_RESOURCES_GL)
}
#endif

namespace Magnum { namespace Shaders {

using namespace Containers::Literals;
using namespace Math::Literals;

namespace {
    enum: Int {
        /* 0/1/2/3 taken by Phong (A/D/S/N), 4 by MeshVisualizer colormap, 5 by
           object ID textures, 6 by Vector */
        TextureUnit = 7
    };

    enum: Int {
        /* Not using the zero binding to avoid conflicts with
           ProjectionBufferBinding from other shaders which can likely stay
           bound to the same buffer for the whole time */
        TransformationProjectionBufferBinding = 1,
        DrawBufferBinding = 2,
        MaterialBufferBinding = 3
    };
}

template<UnsignedInt dimensions> typename LineGL<dimensions>::CompileState LineGL<dimensions>::compile(const Configuration& configuration) {
    #ifndef CORRADE_NO_ASSERT
    #ifndef MAGNUM_TARGET_WEBGL
    if(!(configuration.flags() >= Flag::ShaderStorageBuffers))
    #endif
    {
        CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.materialCount(),
            "Shaders::LineGL: material count can't be zero", CompileState{NoCreate});
        CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
            "Shaders::LineGL: draw count can't be zero", CompileState{NoCreate});
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::gpu_shader4);
    if(configuration.flags() >= Flag::UniformBuffers)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::uniform_buffer_object);
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    if(configuration.flags() >= Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::shader_storage_buffer_object);
        #else
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GLES310);
        #endif
    }
    #endif
    if(configuration.flags() >= Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::shader_draw_parameters);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ANGLE::multi_draw);
        #else
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::WEBGL::multi_draw);
        #endif
    }

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShadersGL"_s))
        importShaderResources();
    #endif
    Utility::Resource rs{"MagnumShadersGL"_s};

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #elif !defined(MAGNUM_TARGET_WEBGL)
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES310, GL::Version::GLES300});
    #else
    constexpr GL::Version version = GL::Version::GLES300;
    #endif

    /* Cap and join style is needed by both the vertex and fragment shader,
       prepare their defines just once for both. This snippet is shared between
       Shaders::LineGL and Ui::LineLayerGL, keep in sync. */
    Containers::StringView capStyleDefine, joinStyleDefine;
    switch(configuration.capStyle()) {
        case LineCapStyle::Butt:
            capStyleDefine = "#define CAP_STYLE_BUTT\n"_s;
            break;
        case LineCapStyle::Square:
            capStyleDefine = "#define CAP_STYLE_SQUARE\n"_s;
            break;
        case LineCapStyle::Round:
            capStyleDefine = "#define CAP_STYLE_ROUND\n"_s;
            break;
        case LineCapStyle::Triangle:
            capStyleDefine = "#define CAP_STYLE_TRIANGLE\n"_s;
            break;
    }
    switch(configuration.joinStyle()) {
        case LineJoinStyle::Miter:
            joinStyleDefine = "#define JOIN_STYLE_MITER\n"_s;
            break;
        case LineJoinStyle::Bevel:
            joinStyleDefine = "#define JOIN_STYLE_BEVEL\n"_s;
            break;
    }
    CORRADE_INTERNAL_ASSERT(capStyleDefine);
    CORRADE_INTERNAL_ASSERT(joinStyleDefine);

    GL::Shader vert{version, GL::Shader::Type::Vertex};
    vert.addSource(rs.getString("compatibility.glsl"_s))
        .addSource(capStyleDefine)
        .addSource(joinStyleDefine)
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n"_s : ""_s)
        .addSource(dimensions == 2 ? "#define TWO_DIMENSIONS\n"_s : "#define THREE_DIMENSIONS\n"_s)
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n"_s : ""_s);
    if(configuration.flags() >= Flag::UniformBuffers) {
        #ifndef MAGNUM_TARGET_WEBGL
        /* SSBOs have unbounded per-draw arrays so just a plain string can be
           passed */
        if(configuration.flags() >= Flag::ShaderStorageBuffers) {
            vert.addSource(
                "#define UNIFORM_BUFFERS\n"
                "#define SHADER_STORAGE_BUFFERS\n"_s);
        } else
        #endif
        {
            vert.addSource(Utility::format(
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {}\n"
                "#define MATERIAL_COUNT {}\n",
                configuration.drawCount(),
                configuration.materialCount()));
        }
        vert.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n"_s : ""_s);
    }
    vert.addSource(rs.getString("generic.glsl"_s))
        .addSource(rs.getString("Line.vert"_s))
        .addSource(rs.getString("Line.in.vert"_s))
        .submitCompile();

    GL::Shader frag{version, GL::Shader::Type::Fragment};
    frag.addSource(rs.getString("compatibility.glsl"_s))
        .addSource(capStyleDefine)
        .addSource(joinStyleDefine)
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::ObjectId ? "#define OBJECT_ID\n"_s : ""_s)
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n"_s : ""_s);
    if(configuration.flags() >= Flag::UniformBuffers) {
        #ifndef MAGNUM_TARGET_WEBGL
        /* SSBOs have unbounded per-draw arrays so just a plain string can be
           passed */
        if(configuration.flags() >= Flag::ShaderStorageBuffers) {
            frag.addSource(
                "#define UNIFORM_BUFFERS\n"
                "#define SHADER_STORAGE_BUFFERS\n"_s);
        } else
        #endif
        {
            frag.addSource(Utility::format(
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {}\n"
                "#define MATERIAL_COUNT {}\n",
                configuration.drawCount(),
                configuration.materialCount()));
        }
        frag.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n"_s : ""_s);
    }
    frag.addSource(rs.getString("generic.glsl"_s))
        .addSource(rs.getString("Line.frag"_s))
        .addSource(rs.getString("Line.in.frag"_s))
        .submitCompile();

    LineGL<dimensions> out{NoInit};
    out._flags = configuration.flags();
    out._capStyle = configuration.capStyle();
    out._joinStyle = configuration.joinStyle();
    out._materialCount = configuration.materialCount();
    out._drawCount = configuration.drawCount();

    out.attachShaders({vert, frag});

    /* ES3 has this done in the shader directly and doesn't even provide
       bindFragmentDataLocation() */
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version)) {
        out.bindAttributeLocation(Position::Location, "position"_s);
        out.bindAttributeLocation(PreviousPosition::Location, "previousPosition"_s);
        out.bindAttributeLocation(NextPosition::Location, "nextPosition"_s);
        out.bindAttributeLocation(Annotation::Location, "annotation"_s);
        if(configuration.flags() & Flag::VertexColor)
            out.bindAttributeLocation(Color3::Location, "vertexColor"_s); /* Color4 is the same */
        if(configuration.flags() & Flag::ObjectId) {
            out.bindFragmentDataLocation(ColorOutput, "color"_s);
            out.bindFragmentDataLocation(ObjectIdOutput, "objectId"_s);
        }
        if(configuration.flags() >= Flag::InstancedObjectId)
            out.bindAttributeLocation(ObjectId::Location, "instanceObjectId"_s);
        if(configuration.flags() & Flag::InstancedTransformation)
            out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix"_s);
    }
    #endif

    out.submitLink();

    return CompileState{Utility::move(out), Utility::move(vert), Utility::move(frag)
        #if !defined(MAGNUM_TARGET_GLES) || !defined(MAGNUM_TARGET_WEBGL)
        , version
        #endif
    };
}

template<UnsignedInt dimensions> LineGL<dimensions>::LineGL(CompileState&& state): LineGL{static_cast<LineGL&&>(Utility::move(state))} {
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* When graceful assertions fire from within compile(), we get a NoCreate'd
       CompileState. Exiting makes it possible to test the assert. */
    if(!id()) return;
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(state._version))
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(state._version < GL::Version::GLES310)
    #endif
    {
        _viewportSizeUniform = uniformLocation("viewportSize"_s);
        if(_flags >= Flag::UniformBuffers) {
            if(_drawCount > 1
                #ifndef MAGNUM_TARGET_WEBGL
                || flags() >= Flag::ShaderStorageBuffers
                #endif
            ) _drawOffsetUniform = uniformLocation("drawOffset"_s);
        } else {
            _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix"_s);
            _widthUniform = uniformLocation("width"_s);
            _smoothnessUniform = uniformLocation("smoothness"_s);
            if(_joinStyle == LineJoinStyle::Miter)
                _miterLimitUniform = uniformLocation("miterLimit"_s);
            _backgroundColorUniform = uniformLocation("backgroundColor"_s);
            _colorUniform = uniformLocation("color"_s);
            if(_flags & Flag::ObjectId)
                _objectIdUniform = uniformLocation("objectId"_s);
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(state._version))
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(state._version < GL::Version::GLES310)
    #endif
    {
        /* SSBOs have bindings defined in the source always */
        if(_flags >= Flag::UniformBuffers
            #ifndef MAGNUM_TARGET_WEBGL
            && !(_flags >= Flag::ShaderStorageBuffers)
            #endif
        ) {
            setUniformBlockBinding(uniformBlockIndex("TransformationProjection"_s), TransformationProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"_s), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"_s), MaterialBufferBinding);
        }
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    if(_flags >= Flag::UniformBuffers) {
        /* Draw offset is zero by default */
    } else {
        setTransformationProjectionMatrix(MatrixTypeFor<dimensions, Float>{Math::IdentityInit});
        setWidth(1.0f);
        /* Smoothness is zero by default */
        if(_joinStyle == LineJoinStyle::Miter)
            setMiterLengthLimit(4.0f);
        setColor(Magnum::Color4{1.0f});
        /* Object ID is zero by default */
    }
    #endif
}

template<UnsignedInt dimensions> LineGL<dimensions>::LineGL(const Configuration& configuration): LineGL{compile(configuration)} {}

template<UnsignedInt dimensions> LineGL<dimensions>::LineGL(NoInitT) {}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setViewportSize(const Vector2& size) {
    setUniform(_viewportSizeUniform, size);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setBackgroundColor(const Magnum::Color4& color) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setBackgroundColor(): the shader was created with uniform buffers enabled", *this);
    setUniform(_backgroundColorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setColor(const Magnum::Color4& color) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setColor(): the shader was created with uniform buffers enabled", *this);
    setUniform(_colorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setWidth(const Float width) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setWidth(): the shader was created with uniform buffers enabled", *this);
    setUniform(_widthUniform, width);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setSmoothness(const Float smoothness) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setSmoothness(): the shader was created with uniform buffers enabled", *this);
    setUniform(_smoothnessUniform, smoothness);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setMiterLengthLimit(const Float limit) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setMiterLengthLimit(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_joinStyle == LineJoinStyle::Miter,
        "Shaders::LineGL::setMiterLengthLimit(): the shader was created with" << _joinStyle, *this);
    setUniform(_miterLimitUniform, Implementation::lineMiterLengthLimit("Shaders::LineGL::setMiterLengthLimit():", limit));
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setMiterAngleLimit(const Rad limit) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setMiterAngleLimit(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_joinStyle == LineJoinStyle::Miter,
        "Shaders::LineGL::setMiterAngleLimit(): the shader was created with" << _joinStyle, *this);
    setUniform(_miterLimitUniform, Implementation::lineMiterAngleLimit("Shaders::LineGL::setMiterAngleLimit():", limit));
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setObjectId(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setObjectId(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::ObjectId,
        "Shaders::LineGL::setObjectId(): the shader was not created with object ID enabled", *this);
    setUniform(_objectIdUniform, id);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setDrawOffset(const UnsignedInt offset) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(_flags >= Flag::ShaderStorageBuffers || offset < _drawCount,
        "Shaders::LineGL::setDrawOffset(): draw offset" << offset << "is out of range for" << _drawCount << "draws", *this);
    #else
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::LineGL::setDrawOffset(): draw offset" << offset << "is out of range for" << _drawCount << "draws", *this);
    #endif
    if(_drawCount > 1
        #ifndef MAGNUM_TARGET_WEBGL
        || _flags >= Flag::ShaderStorageBuffers
        #endif
    ) setUniform(_drawOffsetUniform, offset);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, MaterialBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>::Configuration::Configuration(): _capStyle{LineCapStyle::Square}, _joinStyle{LineJoinStyle::Miter} {}

template class MAGNUM_SHADERS_EXPORT LineGL<2>;
template class MAGNUM_SHADERS_EXPORT LineGL<3>;

namespace Implementation {

Debug& operator<<(Debug& debug, const LineGLFlag value) {
    #ifndef MAGNUM_TARGET_WEBGL
    /* Special case coming from the Flags printer. As both flags are a superset
       of UniformBuffers, printing just one would result in
       `Flag::MultiDraw|Flag(0x40)` in the output. */
    if(value == LineGLFlag(UnsignedShort(LineGLFlag::MultiDraw|LineGLFlag::ShaderStorageBuffers)))
        return debug << LineGLFlag::MultiDraw << Debug::nospace << "|" << Debug::nospace << LineGLFlag::ShaderStorageBuffers;
    #endif

    debug << "Shaders::LineGL::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case LineGLFlag::v: return debug << "::" #v;
        _c(VertexColor)
        _c(ObjectId)
        _c(InstancedObjectId)
        _c(InstancedTransformation)
        _c(UniformBuffers)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(ShaderStorageBuffers)
        #endif
        _c(MultiDraw)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedShort(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const LineGLFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::LineGL::Flags{}", {
        LineGLFlag::VertexColor,
        LineGLFlag::InstancedObjectId, /* Superset of ObjectId */
        LineGLFlag::ObjectId,
        LineGLFlag::InstancedTransformation,
        #ifndef MAGNUM_TARGET_WEBGL
        /* Both are a superset of UniformBuffers, meaning printing just one
           would result in `Flag::MultiDraw|Flag(0x40)` in the output. So we
           pass both and let the Flag printer deal with that. */
        LineGLFlag(UnsignedShort(LineGLFlag::MultiDraw|LineGLFlag::ShaderStorageBuffers)),
        #endif
        LineGLFlag::MultiDraw, /* Superset of UniformBuffers */
        #ifndef MAGNUM_TARGET_WEBGL
        LineGLFlag::ShaderStorageBuffers, /* Superset of UniformBuffers */
        #endif
        LineGLFlag::UniformBuffers
    });
}

}

}}
