/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
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

#include "VertexColorGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#ifndef MAGNUM_TARGET_GLES2
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/GL/Buffer.h"
#endif

#ifdef MAGNUM_BUILD_STATIC
static void importShaderResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumShaders_RESOURCES_GL)
}
#endif

namespace Magnum { namespace Shaders {

using namespace Containers::Literals;

namespace {
    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        /* Not using the zero binding to avoid conflicts with
           ProjectionBufferBinding from other shaders which can likely stay
           bound to the same buffer for the whole time */
        TransformationProjectionBufferBinding = 1
    };
    #endif
}

template<UnsignedInt dimensions> typename VertexColorGL<dimensions>::CompileState VertexColorGL<dimensions>::compile(const Configuration& configuration) {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_NO_ASSERT)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!(configuration.flags() >= Flag::ShaderStorageBuffers))
    #endif
    {
        CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
            "Shaders::VertexColorGL: draw count can't be zero", CompileState{NoCreate});
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(configuration.flags() >= Flag::UniformBuffers)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::uniform_buffer_object);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(configuration.flags() >= Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::shader_storage_buffer_object);
        #else
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GLES310);
        #endif
    }
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

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShadersGL"_s))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShadersGL"_s);

    const GL::Context& context = GL::Context::current();

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version version = context.supportedVersion({
        #ifndef MAGNUM_TARGET_WEBGL
        GL::Version::GLES310,
        #endif
        GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert{version, GL::Shader::Type::Vertex};
    vert.addSource(rs.getString("compatibility.glsl"_s))
        .addSource(dimensions == 2 ? "#define TWO_DIMENSIONS\n"_s : "#define THREE_DIMENSIONS\n"_s);
    #ifndef MAGNUM_TARGET_GLES2
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
                "#define DRAW_COUNT {}\n",
                configuration.drawCount()));
        }
        vert.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n"_s : ""_s);
    }
    #endif
    vert.addSource(rs.getString("generic.glsl"_s))
        .addSource(rs.getString("VertexColor.vert"_s))
        .submitCompile();

    GL::Shader frag{version, GL::Shader::Type::Fragment};
    frag.addSource(rs.getString("compatibility.glsl"_s))
        .addSource(rs.getString("generic.glsl"_s))
        .addSource(rs.getString("VertexColor.frag"_s))
        .submitCompile();

    VertexColorGL<dimensions> out{NoInit};
    out._flags = configuration.flags();
    #ifndef MAGNUM_TARGET_GLES2
    out._drawCount = configuration.drawCount();
    #endif

    out.attachShaders({vert, frag});

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        out.bindAttributeLocation(Position::Location, "position"_s);
        out.bindAttributeLocation(Color3::Location, "color"_s); /* Color4 is the same */
    }
    #endif

    out.submitLink();

    return CompileState{Utility::move(out), Utility::move(vert), Utility::move(frag)
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , version
        #endif
    };
}

#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> typename VertexColorGL<dimensions>::CompileState VertexColorGL<dimensions>::compile(const Flags flags) {
    return compile(Configuration{}
        .setFlags(flags));
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> typename VertexColorGL<dimensions>::CompileState VertexColorGL<dimensions>::compile(const Flags flags, const UnsignedInt drawCount) {
    return compile(Configuration{}
        .setFlags(flags)
        .setDrawCount(drawCount));
}
#endif
#endif

template<UnsignedInt dimensions> VertexColorGL<dimensions>::VertexColorGL(CompileState&& state): VertexColorGL{static_cast<VertexColorGL&&>(Utility::move(state))} {
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* When graceful assertions fire from within compile(), we get a NoCreate'd
       CompileState. Exiting makes it possible to test the assert. */
    if(!id()) return;
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(state._version))
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(state._version < GL::Version::GLES310)
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::UniformBuffers) {
            if(_drawCount > 1
                #ifndef MAGNUM_TARGET_WEBGL
                || flags() >= Flag::ShaderStorageBuffers
                #endif
            ) _drawOffsetUniform = uniformLocation("drawOffset"_s);
        } else
        #endif
        {
            _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix"_s);
        }
    }

    #ifndef MAGNUM_TARGET_GLES2
    /* SSBOs have bindings defined in the source always */
    if(_flags >= Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        && !(_flags >= Flag::ShaderStorageBuffers)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(state._version)
        #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        && state._version < GL::Version::GLES310
        #endif
    ) {
        setUniformBlockBinding(uniformBlockIndex("TransformationProjection"_s), TransformationProjectionBufferBinding);
    }
    #endif

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(_flags >= Flag::UniformBuffers) {
        /* Draw offset is zero by default */
    } else
    #endif
    {
        setTransformationProjectionMatrix(MatrixTypeFor<dimensions, Float>{Math::IdentityInit});
    }
    #endif
}

template<UnsignedInt dimensions> VertexColorGL<dimensions>::VertexColorGL(const Configuration& configuration): VertexColorGL{compile(configuration)} {}

#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> VertexColorGL<dimensions>::VertexColorGL(const Flags flags): VertexColorGL{compile(Configuration{}
    .setFlags(flags))} {}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> VertexColorGL<dimensions>::VertexColorGL(const Flags flags, const UnsignedInt drawCount): VertexColorGL{compile(Configuration{}
    .setFlags(flags)
    .setDrawCount(drawCount))} {}
#endif
#endif

template<UnsignedInt dimensions> VertexColorGL<dimensions>::VertexColorGL(NoInitT) {}

template<UnsignedInt dimensions> VertexColorGL<dimensions>& VertexColorGL<dimensions>::setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::VertexColorGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> VertexColorGL<dimensions>& VertexColorGL<dimensions>::setDrawOffset(const UnsignedInt offset) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::VertexColorGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(_flags >= Flag::ShaderStorageBuffers || offset < _drawCount,
        "Shaders::VertexColorGL::setDrawOffset(): draw offset" << offset << "is out of range for" << _drawCount << "draws", *this);
    #else
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::VertexColorGL::setDrawOffset(): draw offset" << offset << "is out of range for" << _drawCount << "draws", *this);
    #endif
    if(_drawCount > 1) setUniform(_drawOffsetUniform, offset);
    return *this;
}

template<UnsignedInt dimensions> VertexColorGL<dimensions>& VertexColorGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::VertexColorGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> VertexColorGL<dimensions>& VertexColorGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::VertexColorGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding, offset, size);
    return *this;
}
#endif

template class MAGNUM_SHADERS_EXPORT VertexColorGL<2>;
template class MAGNUM_SHADERS_EXPORT VertexColorGL<3>;

namespace Implementation {

Debug& operator<<(Debug& debug, const VertexColorGLFlag value) {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Special case coming from the Flags printer. As both flags are a superset
       of UniformBuffers, printing just one would result in
       `Flag::MultiDraw|Flag(0x4)` in the output. */
    if(value == VertexColorGLFlag(UnsignedByte(VertexColorGLFlag::MultiDraw|VertexColorGLFlag::ShaderStorageBuffers)))
        return debug << VertexColorGLFlag::MultiDraw << Debug::nospace << "|" << Debug::nospace << VertexColorGLFlag::ShaderStorageBuffers;
    #endif

    debug << "Shaders::VertexColorGL::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case VertexColorGLFlag::v: return debug << "::" #v;
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(ShaderStorageBuffers)
        #endif
        _c(MultiDraw)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const VertexColorGLFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::VertexColorGL::Flags{}", {
        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        /* Both are a superset of UniformBuffers, meaning printing just one
           would result in `Flag::MultiDraw|Flag(0x4)` in the output. So we
           pass both and let the Flag printer deal with that. */
        VertexColorGLFlag(UnsignedByte(VertexColorGLFlag::MultiDraw|VertexColorGLFlag::ShaderStorageBuffers)),
        #endif
        VertexColorGLFlag::MultiDraw, /* Superset of UniformBuffers */
        #ifndef MAGNUM_TARGET_WEBGL
        VertexColorGLFlag::ShaderStorageBuffers, /* Superset of UniformBuffers */
        #endif
        VertexColorGLFlag::UniformBuffers
        #endif
    });
}

}

}}
