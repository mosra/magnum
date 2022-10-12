#ifndef Magnum_Shaders_LineGL_h
#define Magnum_Shaders_LineGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/glShaderWrapper.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class LineGLFlag: UnsignedShort {
        VertexColor = 1 << 0,
        #ifndef MAGNUM_TARGET_GLES2
        ObjectId = 1 << 1,
        InstancedObjectId = (1 << 2)|ObjectId,
        #endif
        InstancedTransformation = 1 << 3,
        #ifndef MAGNUM_TARGET_GLES2
        UniformBuffers = 1 << 4,
        MultiDraw = UniformBuffers|(1 << 5)
        #endif
    };
    typedef Containers::EnumSet<LineGLFlag> LineGLFlags;
}

template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT LineGL: public GL::AbstractShaderProgram {
    public:
        class Configuration;
        class CompileState;

        typedef typename GenericGL<dimensions>::Position Position;

        // TODO move to Generic
        typedef GL::Attribute<3, VectorTypeFor<dimensions, Float>> LineDirection;
        typedef GL::Attribute<5, VectorTypeFor<dimensions, Float>> LineNeighborDirection;

        typedef typename GenericGL<dimensions>::Color3 Color3;

        typedef typename GenericGL<dimensions>::Color4 Color4;

        #ifndef MAGNUM_TARGET_GLES2
        typedef typename GenericGL<dimensions>::ObjectId ObjectId;
        #endif

        typedef typename GenericGL<dimensions>::TransformationMatrix TransformationMatrix;

        enum: UnsignedInt {
            ColorOutput = GenericGL<dimensions>::ColorOutput,

            #ifndef MAGNUM_TARGET_GLES2
            ObjectIdOutput = GenericGL<dimensions>::ObjectIdOutput
            #endif
        };

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedShort {
            VertexColor = 1 << 0,
            #ifndef MAGNUM_TARGET_GLES2
            ObjectId = 1 << 1,
            InstancedObjectId = (1 << 2)|ObjectId,
            #endif
            InstancedTransformation = 1 << 3,
            #ifndef MAGNUM_TARGET_GLES2
            UniformBuffers = 1 << 4,
            MultiDraw = UniformBuffers|(1 << 5)
            #endif
        };

        /**
         * @brief Flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;
        #else
        /* Done this way to be prepared for possible future diversion of 2D
           and 3D flags (e.g. introducing 3D-specific features) */
        typedef Implementation::LineGLFlag Flag;
        typedef Implementation::LineGLFlags Flags;
        #endif

        static CompileState compile(const Configuration& configuration = Configuration{});

        explicit LineGL(const Configuration& configuration = Configuration{});

        explicit LineGL(CompileState&& state);

        explicit LineGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        LineGL(const LineGL<dimensions>&) = delete;

        /** @brief Move constructor */
        LineGL(LineGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        LineGL<dimensions>& operator=(const LineGL<dimensions>&) = delete;

        /** @brief Move assignment */
        LineGL<dimensions>& operator=(LineGL<dimensions>&&) noexcept = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt materialCount() const { return _materialCount; }

        UnsignedInt drawCount() const { return _drawCount; }
        #endif

        // TODO split projection for 3D?
        LineGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        LineGL<dimensions>& setViewportSize(const Vector2& size);

        LineGL<dimensions>& setWidth(Float width);

        LineGL<dimensions>& setSmoothness(Float smoothness);

        LineGL<dimensions>& setBackgroundColor(const Magnum::Color4& color);

        LineGL<dimensions>& setColor(const Magnum::Color4& color);

        #ifndef MAGNUM_TARGET_GLES2
        LineGL<dimensions>& setObjectId(UnsignedInt id);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        LineGL<dimensions>& setDrawOffset(UnsignedInt offset);

        LineGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer);
        LineGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        LineGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer);
        LineGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        LineGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer);
        LineGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);
        #endif

        // TODO draw() overloads, as a macro in AbstractShaderProgram

    private:
        /* Creates the GL shader program object but does nothing else.
           Internal, used by compile(). */
        explicit LineGL(NoInitT);

        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _materialCount{}, _drawCount{};
        #endif
        Int _viewportSizeUniform{0},
            _transformationProjectionMatrixUniform{1},
            _widthUniform{2},
            _smoothnessUniform{3},
            _backgroundColorUniform{4},
            _colorUniform{5};
        #ifndef MAGNUM_TARGET_GLES2
        Int _objectIdUniform{6};
        /* Used instead of all other uniforms except viewportSize when
           Flag::UniformBuffers is set, so it can alias them */
        Int _drawOffsetUniform{1};
        #endif
};

/**
@brief Configuration

*/
template<UnsignedInt dimensions> class LineGL<dimensions>::Configuration {
    public:
        explicit Configuration() = default;

        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        Flags flags() const { return _flags; }

        #ifndef MAGNUM_TARGET_GLES2
        Configuration& setMaterialCount(UnsignedInt count) {
            _materialCount = count;
            return *this;
        }

        UnsignedInt materialCount() const { return _materialCount; }

        Configuration& setDrawCount(UnsignedInt count) {
            _drawCount = count;
            return *this;
        }

        UnsignedInt drawCount() const { return _drawCount; }
        #endif

    private:
        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _materialCount{1};
        UnsignedInt _drawCount{1};
        #endif
};

/**
@brief Asynchronous compilation state

Returned by @ref compile(). See @ref shaders-async for more information.
*/
template<UnsignedInt dimensions> class LineGL<dimensions>::CompileState: public LineGL<dimensions> {
    /* Everything deliberately private except for the inheritance */
    friend class LineGL;

    explicit CompileState(NoCreateT): LineGL{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

    explicit CompileState(LineGL<dimensions>&& shader, GL::Shader&& vert, GL::Shader&& frag, GL::Version version): LineGL<dimensions>{std::move(shader)}, _vert{std::move(vert)}, _frag{std::move(frag)}, _version{version} {}

    Implementation::GLShaderWrapper _vert, _frag;
    GL::Version _version;
};

/**
@brief 2D line OpenGL shader
@m_since_latest
*/
typedef LineGL<2> LineGL2D;

/**
@brief 3D LineGL OpenGL shader
@m_since_latest
*/
typedef LineGL<3> LineGL3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{LineGL,LineGL::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, LineGL<dimensions>::Flag value);

/** @debugoperatorclassenum{LineGL,LineGL::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, LineGL<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, LineGLFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, LineGLFlags value);
    CORRADE_ENUMSET_OPERATORS(LineGLFlags)
}
#endif

}}

#endif
