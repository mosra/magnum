#ifndef Magnum_Shaders_FlatGL_h
#define Magnum_Shaders_FlatGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

/** @file
 * @brief Class @ref Magnum::Shaders::FlatGL, typedef @ref Magnum::Shaders::FlatGL2D, @ref Magnum::Shaders::FlatGL3D
 * @m_since_latest
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class FlatGLFlag: UnsignedByte {
        Textured = 1 << 0,
        AlphaMask = 1 << 1,
        VertexColor = 1 << 2,
        TextureTransformation = 1 << 3,
        #ifndef MAGNUM_TARGET_GLES2
        ObjectId = 1 << 4,
        InstancedObjectId = (1 << 5)|ObjectId,
        #endif
        InstancedTransformation = 1 << 6,
        InstancedTextureOffset = (1 << 7)|TextureTransformation
    };
    typedef Containers::EnumSet<FlatGLFlag> FlatGLFlags;
}

/**
@brief Flat OpenGL shader
@m_since_latest

Draws the whole mesh with given color or texture. For a colored mesh you need
to provide the @ref Position attribute in your triangle mesh. By default, the
shader renders the mesh with a white color in an identity transformation.
Use @ref setTransformationProjectionMatrix(), @ref setColor() and others to
configure the shader.

@image html shaders-flat.png width=256px

@section Shaders-FlatGL-colored Colored rendering

Common mesh setup:

@snippet MagnumShaders-gl.cpp FlatGL-usage-colored1

Common rendering setup:

@snippet MagnumShaders-gl.cpp FlatGL-usage-colored2

@section Shaders-FlatGL-textured Textured rendering

If you want to use a texture, you need to provide also the
@ref TextureCoordinates attribute. Pass @ref Flag::Textured to the constructor
and then at render time don't forget to bind also the texture via
@ref bindTexture(). The texture is multipled by the color, which is by default
set to @cpp 0xffffffff_rgbaf @ce. Common mesh setup:

@snippet MagnumShaders-gl.cpp FlatGL-usage-textured1

Common rendering setup:

@snippet MagnumShaders-gl.cpp FlatGL-usage-textured2

For coloring the texture based on intensity you can use the @ref VectorGL
shader. The 3D version of this shader is equivalent to @ref PhongGL with zero
lights, however this implementation is much simpler and thus likely also
faster. See @ref Shaders-PhongGL-lights-zero "its documentation" for more
information. Conversely, enabling @ref Flag::VertexColor and using a default
color with no texturing makes this shader equivalent to @ref VertexColorGL.

@section Shaders-FlatGL-alpha Alpha blending and masking

Alpha / transparency is supported by the shader implicitly, but to have it
working on the framebuffer, you need to enable
@ref GL::Renderer::Feature::Blending and set up the blending function. See
@ref GL::Renderer::setBlendFunction() for details.

An alternative is to enable @ref Flag::AlphaMask and tune @ref setAlphaMask()
for simple binary alpha-masked drawing that doesn't require depth sorting or
blending enabled. Note that this feature is implemented using the GLSL
@glsl discard @ce operation which is known to have considerable performance
impact on some platforms. With proper depth sorting and blending you'll usually
get much better performance and output quality.

@section Shaders-FlatGL-object-id Object ID output

The shader supports writing object ID to the framebuffer for object picking or
other annotation purposes. Enable it using @ref Flag::ObjectId and set up an
integer buffer attached to the @ref ObjectIdOutput attachment. Note that for
portability you should use @ref GL::Framebuffer::clearColor() instead of
@ref GL::Framebuffer::clear() as the former usually emits GL errors when called
on framebuffers with integer attachments.

@snippet MagnumShaders-gl.cpp FlatGL-usage-object-id

If you have a batch of meshes with different object IDs, enable
@ref Flag::InstancedObjectId and supply per-vertex IDs to the @ref ObjectId
attribute. The output will contain a sum of the per-vertex ID and ID coming
from @ref setObjectId().

@requires_gles30 Object ID output requires integer buffer attachments, which
    are not available in OpenGL ES 2.0 or WebGL 1.0.

@section Shaders-FlatGL-instancing Instanced rendering

Enabling @ref Flag::InstancedTransformation will turn the shader into an
instanced one. It'll take per-instance transformation from the
@ref TransformationMatrix attribute, applying it before the matrix set by
@ref setTransformationProjectionMatrix(). Besides that, @ref Flag::VertexColor
(and the @ref Color3 / @ref Color4) attributes can work as both per-vertex and
per-instance, and for texturing it's possible to have per-instance texture
offset taken from @ref TextureOffset when @ref Flag::InstancedTextureOffset is
enabled (similarly to transformation, applied before @ref setTextureMatrix()).
The snippet below shows adding a buffer with per-instance transformation and
color to a mesh:

@snippet MagnumShaders-gl.cpp FlatGL-usage-instancing

@requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
@requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
    @gl_extension{EXT,instanced_arrays} or @gl_extension{NV,instanced_arrays}
    in OpenGL ES 2.0.
@requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays} in WebGL
    1.0.

@see @ref shaders, @ref FlatGL2D, @ref FlatGL3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT FlatGL: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2" in 2D, @ref Magnum::Vector3 "Vector3"
         * in 3D.
         */
        typedef typename GenericGL<dimensions>::Position Position;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2". Used only if @ref Flag::Textured is
         * set.
         */
        typedef typename GenericGL<dimensions>::TextureCoordinates TextureCoordinates;

        /**
         * @brief Three-component vertex color
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color3. Use
         * either this or the @ref Color4 attribute. Used only if
         * @ref Flag::VertexColor is set.
         */
        typedef typename GenericGL<dimensions>::Color3 Color3;

        /**
         * @brief Four-component vertex color
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color4. Use
         * either this or the @ref Color3 attribute. Used only if
         * @ref Flag::VertexColor is set.
         */
        typedef typename GenericGL<dimensions>::Color4 Color4;

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) object ID
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::UnsignedInt.
         * Used only if @ref Flag::InstancedObjectId is set.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0 or WebGL 1.0.
         */
        typedef typename GenericGL<dimensions>::ObjectId ObjectId;
        #endif

        /**
         * @brief (Instanced) transformation matrix
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Matrix3 in
         * 2D, @ref Magnum::Matrix4 in 3D. Used only if
         * @ref Flag::InstancedTransformation is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef typename GenericGL<dimensions>::TransformationMatrix TransformationMatrix;

        /**
         * @brief (Instanced) texture offset
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Vector2. Used
         * only if @ref Flag::InstancedTextureOffset is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef typename GenericGL<dimensions>::TextureOffset TextureOffset;

        enum: UnsignedInt {
            /**
             * Color shader output. Present always, expects three- or
             * four-component floating-point or normalized buffer attachment.
             * @m_since{2019,10}
             */
            ColorOutput = GenericGL<dimensions>::ColorOutput,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Object ID shader output. @ref shaders-generic "Generic output",
             * present only if @ref Flag::ObjectId is set. Expects a
             * single-component unsigned integral attachment. Writes the value
             * set in @ref setObjectId() there, see
             * @ref Shaders-FlatGL-object-id for more information.
             * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2019,10}
             */
            ObjectIdOutput = GenericGL<dimensions>::ObjectIdOutput
            #endif
        };

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            /**
             * Multiply color with a texture.
             * @see @ref setColor(), @ref bindTexture()
             */
            Textured = 1 << 0,

            /**
             * Enable alpha masking. If the combined fragment color has an
             * alpha less than the value specified with @ref setAlphaMask(),
             * given fragment is discarded.
             *
             * This uses the @glsl discard @ce operation which is known to have
             * considerable performance impact on some platforms. While useful
             * for cheap alpha masking that doesn't require depth sorting,
             * with proper depth sorting and blending you'll usually get much
             * better performance and output quality.
             */
            AlphaMask = 1 << 1,

            /**
             * Multiply diffuse color with a vertex color. Requires either
             * the @ref Color3 or @ref Color4 attribute to be present.
             * @m_since{2019,10}
             */
            VertexColor = 1 << 2,

            /**
             * Enable texture coordinate transformation. If this flag is set,
             * the shader expects that @ref Flag::Textured is enabled as well.
             * @see @ref setTextureMatrix()
             * @m_since{2020,06}
             */
            TextureTransformation = 1 << 3,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Enable object ID output. See @ref Shaders-FlatGL-object-id for
             * more information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2019,10}
             */
            ObjectId = 1 << 4,

            /**
             * Instanced object ID. Retrieves a per-instance / per-vertex
             * object ID from the @ref ObjectId attribute, outputting a sum of
             * the per-vertex ID and ID coming from @ref setObjectId().
             * Implicitly enables @ref Flag::ObjectId. See
             * @ref Shaders-FlatGL-object-id for more information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2020,06}
             */
            InstancedObjectId = (1 << 5)|ObjectId,
            #endif

            /**
             * Instanced transformation. Retrieves a per-instance
             * transformation matrix from the @ref TransformationMatrix
             * attribute and uses it together with the matrix coming from
             * @ref setTransformationProjectionMatrix() (first the
             * per-instance, then the uniform matrix). See
             * @ref Shaders-FlatGL-instancing for more information.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
             *      @gl_extension{EXT,instanced_arrays} or
             *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
             *      in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedTransformation = 1 << 6,

            /**
             * Instanced texture offset. Retrieves a per-instance offset vector
             * from the @ref TextureOffset attribute and uses it together with
             * the matrix coming from @ref setTextureMatrix() (first the
             * per-instance vector, then the uniform matrix). Instanced texture
             * scaling and rotation is not supported at the moment, you can
             * specify that only via the uniform @ref setTextureMatrix().
             * Implicitly enables @ref Flag::TextureTransformation. See
             * @ref Shaders-FlatGL-instancing for more information.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
             *      @gl_extension{EXT,instanced_arrays} or
             *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
             *      in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedTextureOffset = (1 << 7)|TextureTransformation
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
        typedef Implementation::FlatGLFlag Flag;
        typedef Implementation::FlatGLFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit FlatGL(Flags flags = {});

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to a moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit FlatGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        FlatGL(const FlatGL<dimensions>&) = delete;

        /** @brief Move constructor */
        FlatGL(FlatGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        FlatGL<dimensions>& operator=(const FlatGL<dimensions>&) = delete;

        /** @brief Move assignment */
        FlatGL<dimensions>& operator=(FlatGL<dimensions>&&) noexcept = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /** @{
         * @name Uniform setters
         */

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix.
         */
        FlatGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix.
         */
        FlatGL<dimensions>& setTextureMatrix(const Matrix3& matrix);

        /**
         * @brief Set color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. If @ref Flag::Textured
         * is set, the color will be multiplied with the texture.
         * @see @ref bindTexture()
         */
        FlatGL<dimensions>& setColor(const Magnum::Color4& color);

        /**
         * @brief Set alpha mask value
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::AlphaMask
         * enabled. Fragments with alpha values smaller than the mask value
         * will be discarded. Initial value is @cpp 0.5f @ce. See the flag
         * documentation for further information.
         *
         * This corresponds to @m_class{m-doc-external} [glAlphaFunc()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glAlphaFunc.xml)
         * in classic OpenGL.
         * @m_keywords{glAlphaFunc()}
         */
        FlatGL<dimensions>& setAlphaMask(Float mask);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set object ID
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::ObjectId
         * enabled. Value set here is written to the @ref ObjectIdOutput, see
         * @ref Shaders-FlatGL-object-id for more information. Default is
         * @cpp 0 @ce. If @ref Flag::InstancedObjectId is enabled as well, this
         * value is combined with ID coming from the @ref ObjectId attribute.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0 or WebGL 1.0.
         */
        FlatGL<dimensions>& setObjectId(UnsignedInt id);
        #endif

        /**
         * @}
         */

        /** @{
         * @name Texture binding
         */

        /**
         * @brief Bind a color texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::Textured
         * enabled.
         * @see @ref setColor(), @ref Flag::TextureTransformation,
         *      @ref setTextureMatrix()
         */
        FlatGL<dimensions>& bindTexture(GL::Texture2D& texture);

        /**
         * @}
         */

    private:
        /* Prevent accidentally calling irrelevant functions */
        #ifndef MAGNUM_TARGET_GLES
        using GL::AbstractShaderProgram::drawTransformFeedback;
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        using GL::AbstractShaderProgram::dispatchCompute;
        #endif

        Flags _flags;
        Int _transformationProjectionMatrixUniform{0},
            _textureMatrixUniform{1},
            _colorUniform{2},
            _alphaMaskUniform{3};
        #ifndef MAGNUM_TARGET_GLES2
        Int _objectIdUniform{4};
        #endif
};

/**
@brief 2D flat OpenGL shader
@m_since_latest
*/
typedef FlatGL<2> FlatGL2D;

/**
@brief 3D flat OpenGL shader
@m_since_latest
*/
typedef FlatGL<3> FlatGL3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{FlatGL,FlatGL::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, FlatGL<dimensions>::Flag value);

/** @debugoperatorclassenum{FlatGL,FlatGL::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, FlatGL<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, FlatGLFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, FlatGLFlags value);
    CORRADE_ENUMSET_OPERATORS(FlatGLFlags)
}
#endif

}}

#endif
