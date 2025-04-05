#ifndef Magnum_Shaders_FlatGL_h
#define Magnum_Shaders_FlatGL_h
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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Class @ref Magnum::Shaders::FlatGL, typedef @ref Magnum::Shaders::FlatGL2D, @ref Magnum::Shaders::FlatGL3D
 * @m_since_latest
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Utility/Move.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/glShaderWrapper.h"
#include "Magnum/Shaders/visibility.h"

#ifndef MAGNUM_TARGET_GLES2
#include <initializer_list>
#endif

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class FlatGLFlag: UnsignedShort {
        Textured = 1 << 0,
        AlphaMask = 1 << 1,
        VertexColor = 1 << 2,
        TextureTransformation = 1 << 3,
        #ifndef MAGNUM_TARGET_GLES2
        ObjectId = 1 << 4,
        InstancedObjectId = (1 << 5)|ObjectId,
        ObjectIdTexture = (1 << 11)|ObjectId,
        #endif
        InstancedTransformation = 1 << 6,
        InstancedTextureOffset = (1 << 7)|TextureTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        UniformBuffers = 1 << 8,
        #ifndef MAGNUM_TARGET_WEBGL
        ShaderStorageBuffers = UniformBuffers|(1 << 13),
        #endif
        MultiDraw = UniformBuffers|(1 << 9),
        TextureArrays = 1 << 10,
        DynamicPerVertexJointCount = 1 << 12
        #endif
    };
    typedef Containers::EnumSet<FlatGLFlag> FlatGLFlags;
    CORRADE_ENUMSET_OPERATORS(FlatGLFlags)
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

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section Shaders-FlatGL-colored Colored rendering

Common mesh setup:

@snippet Shaders-gl.cpp FlatGL-usage-colored1

Common rendering setup:

@snippet Shaders-gl.cpp FlatGL-usage-colored2

@section Shaders-FlatGL-textured Textured rendering

If you want to use a texture, you need to provide also the
@ref TextureCoordinates attribute. Pass @ref Flag::Textured to
@ref Configuration::setFlags() and then at render time don't forget to bind
also the texture via @ref bindTexture(). The texture is multiplied by the
color, which is by default set to @cpp 0xffffffff_rgbaf @ce. Common mesh setup:

@snippet Shaders-gl.cpp FlatGL-usage-textured1

Common rendering setup:

@snippet Shaders-gl.cpp FlatGL-usage-textured2

If @ref Flag::TextureArrays is enabled, pass a @ref GL::Texture2DArray instance
instead of @ref GL::Texture2D. By default layer @cpp 0 @ce is used, call
@ref setTextureLayer() to pick a different texture array layer.

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

@snippet Shaders-gl.cpp FlatGL-usage-object-id

If you have a batch of meshes with different object IDs, enable
@ref Flag::InstancedObjectId and supply per-vertex IDs to the @ref ObjectId
attribute. The object ID can be also supplied from an integer texture bound via
@ref bindObjectIdTexture() if @ref Flag::ObjectIdTexture is enabled. The output
will contain a sum of the per-vertex ID, texture ID and ID coming from
@ref setObjectId().

@requires_gl30 Extension @gl_extension{EXT,texture_integer}
@requires_gles30 Object ID output requires integer support in shaders, which
    is not available in OpenGL ES 2.0.
@requires_webgl20 Object ID output requires integer support in shaders, which
    is not available in WebGL 1.0.

@section Shaders-FlatGL-skinning Skinning

To render skinned meshes, bind up to two sets of up to four-component joint ID
and weight attributes to @ref JointIds / @ref SecondaryJointIds and
@ref Weights / @ref SecondaryWeights, set an appropriate joint count and
per-vertex primary and secondary joint count in
@ref Configuration::setJointCount() and upload appropriate joint matrices with
@ref setJointMatrices(). The usage is similar for all shaders, see
@ref shaders-usage-skinning for an example.

To avoid having to compile multiple shader variants for different joint matrix
counts, set the maximum used joint count in @ref Configuration::setJointCount()
and then upload just a prefix via @ref setJointMatrices(). Similarly, to avoid
multiple variants for different per-vertex joint counts, enable
@ref Flag::DynamicPerVertexJointCount, set the maximum per-vertex joint count
in @ref Configuration::setJointCount() and then adjust the actual per-draw
joint count with @ref setPerVertexJointCount().

@requires_gl30 Extension @gl_extension{EXT,texture_integer}
@requires_gles30 Skinning requires integer support in shaders, which is not
    available in OpenGL ES 2.0.
@requires_webgl20 Skinning requires integer support in shaders, which is not
    available in WebGL 1.0.

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

@snippet Shaders-gl.cpp FlatGL-usage-instancing

For instanced skinning the joint buffer is assumed to contain joint
transformations for all instances. By default all instances use the same joint
transformations, seting @ref setPerInstanceJointCount() will cause the shader
to offset the per-vertex joint IDs with
@glsl gl_InstanceID*perInstanceJointCount @ce.

@requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
@requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
    @gl_extension{EXT,instanced_arrays} or @gl_extension{NV,instanced_arrays}
    in OpenGL ES 2.0.
@requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays} in WebGL
    1.0.

@section Shaders-FlatGL-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, because the shader doesn't need a separate
projection and transformation matrix, a combined one is supplied via a
@ref TransformationProjectionUniform2D / @ref TransformationProjectionUniform3D
buffer bound with @ref bindTransformationProjectionBuffer(). To maximize use of
the limited uniform buffer memory, materials are supplied separately in a
@ref FlatMaterialUniform buffer bound with @ref bindMaterialBuffer() and then
referenced via @relativeref{FlatDrawUniform,materialId} from a
@ref FlatDrawUniform bound with @ref bindDrawBuffer(); for optional texture
transformation a per-draw @ref TextureTransformationUniform buffer bound with
@ref bindTextureTransformationBuffer() can be supplied as well. A uniform
buffer setup equivalent to
the @ref Shaders-FlatGL-colored "colored case at the top" would look like this:

@snippet Shaders-gl.cpp FlatGL-ubo

When uniform buffers with @ref Flag::TextureArrays are used,
@ref Flag::TextureTransformation has to be enabled as well in order to supply
the texture layer using @ref TextureTransformationUniform::layer.

For a multidraw workflow enable @ref Flag::MultiDraw (and possibly
@ref Flag::TextureArrays), supply desired material and draw count via
@ref Configuration::setMaterialCount() and
@relativeref{Configuration,setDrawCount()} and specify material references and
texture offsets/layers for every draw. The usage is similar for all shaders,
see @ref shaders-usage-multidraw for an example.

For skinning, joint matrices are supplied via a @ref TransformationUniform2D /
@ref TransformationUniform3D buffer bound with @ref bindJointBuffer(). In an
instanced scenario the per-instance joint count is supplied via
@ref FlatDrawUniform::perInstanceJointCount, a per-draw joint offset for the
multidraw scenario is supplied via @ref FlatDrawUniform::jointOffset.
Altogether for a particular draw, each per-vertex joint ID is offset with
@glsl gl_InstanceID*perInstanceJointCount + jointOffset @ce. The
@ref setPerVertexJointCount() stays as an immediate uniform in the UBO and
multidraw scenario as well, as it is tied to a particular mesh layout and thus
doesn't need to vary per draw.

@requires_gl30 Extension @gl_extension{EXT,texture_array} for texture arrays.
@requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object} for uniform
    buffers.
@requires_gl46 Extension @gl_extension{ARB,shader_draw_parameters} for
    multidraw.
@requires_gles30 Neither texture arrays nor uniform buffers are available in
    OpenGL ES 2.0.
@requires_webgl20 Neither texture arrays nor uniform buffers are available in
    WebGL 1.0.
@requires_es_extension Extension @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
    (unlisted) for multidraw.
@requires_webgl_extension Extension @webgl_extension{ANGLE,multi_draw} for
    multidraw.

@see @ref shaders, @ref FlatGL2D, @ref FlatGL3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT FlatGL: public GL::AbstractShaderProgram {
    public:
        /* MSVC needs dllexport here as well */
        class MAGNUM_SHADERS_EXPORT Configuration;
        class CompileState;

        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2} in 2D, @relativeref{Magnum,Vector3} in
         * 3D.
         */
        typedef typename GenericGL<dimensions>::Position Position;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2}. Used only if @ref Flag::Textured is
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
         * @brief Joint ids
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4ui}. Used only if
         * @ref perVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::JointIds JointIds;

        /**
         * @brief Weights
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4}. Used only if
         * @ref perVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::Weights Weights;

        /**
         * @brief Secondary joint ids
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4ui}. Used only if
         * @ref secondaryPerVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::SecondaryJointIds SecondaryJointIds;

        /**
         * @brief Secondary weights
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4}. Used only if
         * @ref secondaryPerVertexJointCount() isn't @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        typedef GenericGL3D::SecondaryWeights SecondaryWeights;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) object ID
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,UnsignedInt}. Used only if
         * @ref Flag::InstancedObjectId is set.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID output requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        typedef typename GenericGL<dimensions>::ObjectId ObjectId;
        #endif

        /**
         * @brief (Instanced) transformation matrix
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Matrix3} in 2D, @relativeref{Magnum,Matrix4} in
         * 3D. Used only if @ref Flag::InstancedTransformation is set.
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
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2}. Use either this or the
         * @ref TextureOffsetLayer attribute. Used only if
         * @ref Flag::InstancedTextureOffset is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef typename GenericGL<dimensions>::TextureOffset TextureOffset;

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) texture offset and layer
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}, with the last component interpreted as
         * an integer. Use either this or the @ref TextureOffset attribute.
         * First two components used only if @ref Flag::InstancedTextureOffset
         * is set, third component only if @ref Flag::TextureArrays is set.
         * @requires_gl33 Extension @gl_extension{EXT,texture_array} and
         *      @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
         */
        typedef typename GenericGL<dimensions>::TextureOffsetLayer TextureOffsetLayer;
        #endif

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             * @m_since{2019,10}
             */
            ColorOutput = GenericGL<dimensions>::ColorOutput,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Object ID shader output. @ref shaders-generic "Generic output",
             * present only if @ref Flag::ObjectId is set. Expects a
             * single-component unsigned integral attachment. Writes the value
             * set in @ref setObjectId() and possibly also a per-vertex ID and
             * an ID fetched from a texture, see @ref Shaders-FlatGL-object-id
             * for more information.
             * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID output requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since{2019,10}
             */
            ObjectIdOutput = GenericGL<dimensions>::ObjectIdOutput
            #endif
        };

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags(), @ref Configuration::setFlags()
         */
        enum class Flag: UnsignedShort {
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
             * Multiply the color with a vertex color. Requires either the
             * @ref Color3 or @ref Color4 attribute to be present.
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
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID output requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since{2019,10}
             */
            ObjectId = 1 << 4,

            /**
             * Instanced object ID. Retrieves a per-instance / per-vertex
             * object ID from the @ref ObjectId attribute, outputting a sum of
             * the per-vertex ID and ID coming from @ref setObjectId() or
             * @ref FlatDrawUniform::objectId. Implicitly enables
             * @ref Flag::ObjectId. See @ref Shaders-FlatGL-object-id for more
             * information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID output requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedObjectId = (1 << 5)|ObjectId,

            /**
             * Object ID texture. Retrieves object IDs from a texture bound
             * with @ref bindObjectIdTexture(), outputting a sum of the object
             * ID texture, the ID coming from @ref setObjectId() or
             * @ref FlatDrawUniform::objectId and possibly also the per-vertex
             * ID, if @ref Flag::InstancedObjectId is enabled as well.
             * Implicitly enables @ref Flag::ObjectId. See
             * @ref Shaders-FlatGL-object-id for more information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID output requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             */
            ObjectIdTexture = (1 << 11)|ObjectId,
            #endif

            /**
             * Instanced transformation. Retrieves a per-instance
             * transformation matrix from the @ref TransformationMatrix
             * attribute and uses it together with the matrix coming from
             * @ref setTransformationProjectionMatrix() or
             * @ref TransformationProjectionUniform2D::transformationProjectionMatrix
             * / @ref TransformationProjectionUniform3D::transformationProjectionMatrix
             * (first the per-instance, then the uniform matrix). See
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
             * the matrix coming from @ref setTextureMatrix() or
             * @ref TextureTransformationUniform::rotationScaling and
             * @ref TextureTransformationUniform::offset (first the
             * per-instance vector, then the uniform matrix). Instanced texture
             * scaling and rotation is not supported at the moment, you can
             * specify that only via the uniform @ref setTextureMatrix().
             * Implicitly enables @ref Flag::TextureTransformation. See
             * @ref Shaders-FlatGL-instancing for more information.
             *
             * If @ref Flag::TextureArrays is set as well, a three-component
             * @ref TextureOffsetLayer attribute can be used instead of
             * @ref TextureOffset to specify per-instance texture layer, which
             * gets added to the uniform layer numbers set by
             * @ref setTextureLayer() or
             * @ref TextureTransformationUniform::layer.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
             *      @gl_extension{EXT,instanced_arrays} or
             *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
             *      in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedTextureOffset = (1 << 7)|TextureTransformation,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindTransformationProjectionBuffer(),
             * @ref bindDrawBuffer(), @ref bindTextureTransformationBuffer()
             * and @ref bindMaterialBuffer() instead of direct uniform setters.
             * @see @ref Flag::ShaderStorageBuffers
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 8,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Use shader storage buffers. Superset of functionality provided
             * by @ref Flag::UniformBuffers, compared to it doesn't have any
             * size limits on @ref Configuration::setJointCount(),
             * @relativeref{Configuration,setMaterialCount()} and
             * @relativeref{Configuration,setDrawCount()} in exchange for
             * potentially more costly access and narrower platform support.
             * @requires_gl43 Extension @gl_extension{ARB,shader_storage_buffer_object}
             * @requires_gles31 Shader storage buffers are not available in
             *      OpenGL ES 3.0 and older.
             * @requires_gles Shader storage buffers are not available in
             *      WebGL.
             * @m_since_latest
             */
            ShaderStorageBuffers = UniformBuffers|(1 << 13),
            #endif

            /**
             * Enable multidraw functionality. Implies @ref Flag::UniformBuffers
             * and adds the value from @ref setDrawOffset() with the
             * @glsl gl_DrawID @ce builtin, which makes draws submitted via
             * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
             * and related APIs pick up per-draw parameters directly, without
             * having to rebind the uniform buffers or specify
             * @ref setDrawOffset() before each draw. In a non-multidraw
             * scenario, @glsl gl_DrawID @ce is @cpp 0 @ce, which means a
             * shader with this flag enabled can be used for regular draws as
             * well.
             * @requires_gl46 Extension @gl_extension{ARB,uniform_buffer_object}
             *      and @gl_extension{ARB,shader_draw_parameters}
             * @requires_es_extension OpenGL ES 3.0 and extension
             *      @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
             *      (unlisted). While the extension alone needs only OpenGL ES
             *      2.0, the shader implementation relies on uniform buffers,
             *      which require OpenGL ES 3.0.
             * @requires_webgl_extension WebGL 2.0 and extension
             *      @webgl_extension{ANGLE,multi_draw}. While the extension
             *      alone needs only WebGL 1.0, the shader implementation
             *      relies on uniform buffers, which require WebGL 2.0.
             * @m_since_latest
             */
            MultiDraw = UniformBuffers|(1 << 9),

            /**
             * Use 2D texture arrays. Expects that the texture is supplied via
             * @ref bindTexture(GL::Texture2DArray&) instead of
             * @ref bindTexture(GL::Texture2D&) and the layer is set via
             * @ref setTextureLayer() or
             * @ref TextureTransformationUniform::layer. If
             * @ref Flag::InstancedTextureOffset is set as well and a
             * three-component @ref TextureOffsetLayer attribute is used
             * instead of @ref TextureOffset, the per-instance and uniform
             * layer numbers are added together.
             * @requires_gl30 Extension @gl_extension{EXT,texture_array}
             * @requires_gles30 Texture arrays are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
             * @m_since_latest
             */
            TextureArrays = 1 << 10,

            /**
             * Dynamic per-vertex joint count for skinning. Uses only the first
             * M / N primary / secondary components defined by
             * @ref setPerVertexJointCount() instead of
             * all primary / secondary components defined by
             * @ref Configuration::setJointCount() at shader compilation time.
             * Useful in order to avoid having a shader permutation defined for
             * every possible joint count. Unfortunately it's not possible to
             * make use of default values for unspecified input components as
             * the last component is always @cpp 1.0 @ce instead of
             * @cpp 0.0 @ce, on the other hand dynamically limiting the joint
             * count can reduce the time spent executing the vertex shader
             * compared to going through the full set of per-vertex joints
             * always.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Skinning requires integer support in shaders,
             *      which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Skinning requires integer support in shaders,
             *      which is not available in WebGL 1.0.
             * @m_since_latest
             */
            DynamicPerVertexJointCount = 1 << 12,
            #endif
        };

        /**
         * @brief Flags
         *
         * @see @ref flags(), @ref Configuration::setFlags()
         */
        typedef Containers::EnumSet<Flag> Flags;
        #else
        /* Done this way to be prepared for possible future diversion of 2D
           and 3D flags (e.g. introducing 3D-specific features) */
        typedef Implementation::FlatGLFlag Flag;
        typedef Implementation::FlatGLFlags Flags;
        #endif

        /**
         * @brief Compile asynchronously
         * @m_since_latest
         *
         * Compared to @ref FlatGL(const Configuration&) can perform an
         * asynchronous compilation and linking. See @ref shaders-async for
         * more information.
         * @see @ref FlatGL(CompileState&&)
         */
        /* Compared to the non-templated shaders like PhongGL or
           MeshVisualizerGL2D using a forward declaration is fine here. Huh. */
        static CompileState compile(const Configuration& configuration = Configuration{});

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Compile asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Compile for a multi-draw scenario asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Constructor
         * @m_since_latest
         */
        /* Compared to the non-templated shaders like PhongGL or
           MeshVisualizerGL2D using a forward declaration is fine here. Huh. */
        explicit FlatGL(const Configuration& configuration = Configuration{});

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since_latest Use @ref FlatGL(const Configuration&)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use FlatGL(const Configuration& instead") FlatGL(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @m_deprecated_since_latest Use @ref FlatGL(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        explicit CORRADE_DEPRECATED("use FlatGL(const Configuration& instead") FlatGL(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Finalize an asynchronous compilation
         * @m_since_latest
         *
         * Takes an asynchronous compilation state returned by @ref compile()
         * and forms a ready-to-use shader object. See @ref shaders-async for
         * more information.
         */
        explicit FlatGL(CompileState&& state);

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

        /**
         * @brief Flags
         *
         * @see @ref Configuration::setFlags()
         */
        Flags flags() const { return _flags; }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint count
         * @m_since_latest
         *
         * If @ref Flag::UniformBuffers is not set, this is the number of joint
         * matrices accepted by @ref setJointMatrices() / @ref setJointMatrix().
         * If @ref Flag::UniformBuffers is set, this is the statically defined
         * size of the @ref TransformationUniform2D /
         * @ref TransformationUniform3D uniform buffer bound with
         * @ref bindJointBuffer(). Has no use if @ref Flag::ShaderStorageBuffers
         * is set.
         * @see @ref Configuration::setJointCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt jointCount() const { return _jointCount; }

        /**
         * @brief Per-vertex joint count
         * @m_since_latest
         *
         * Returns the value set with @ref Configuration::setJointCount(). If
         * @ref Flag::DynamicPerVertexJointCount is set, the count can be
         * additionally modified per-draw using @ref setPerVertexJointCount().
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt perVertexJointCount() const { return _perVertexJointCount; }

        /**
         * @brief Secondary per-vertex joint count
         * @m_since_latest
         *
         * Returns the value set with @ref Configuration::setJointCount(). If
         * @ref Flag::DynamicPerVertexJointCount is set, the count can be
         * additionally modified per-draw using @ref setPerVertexJointCount().
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt secondaryPerVertexJointCount() const { return _secondaryPerVertexJointCount; }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Material count
         * @m_since_latest
         *
         * Statically defined size of the @ref FlatMaterialUniform uniform
         * buffer bound with @ref bindMaterialBuffer(). Has use only if
         * @ref Flag::UniformBuffers is set and @ref Flag::ShaderStorageBuffers
         * is not set.
         * @see @ref Configuration::setMaterialCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Draw count
         * @m_since_latest
         *
         * Statically defined size of each of the
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D, @ref FlatDrawUniform and
         * @ref TextureTransformationUniform uniform buffers bound with
         * @ref bindTransformationProjectionBuffer(), @ref bindDrawBuffer() and
         * @ref bindTextureTransformationBuffer(). Has use only if
         * @ref Flag::UniformBuffers is set and @ref Flag::ShaderStorageBuffers
         * is not set.
         * @see @ref Configuration::setDrawCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set dynamic per-vertex skinning joint count
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Allows reducing the count of iterated joints for a particular draw
         * call, making it possible to use a single shader with meshes that
         * contain different count of per-vertex joints. See
         * @ref Flag::DynamicPerVertexJointCount for more information. As the
         * joint count is tied to the mesh layout, this is a per-draw-call
         * setting even in case of @ref Flag::UniformBuffers instead of being
         * a value in @ref FlatDrawUniform. Initial value is the same as
         * @ref perVertexJointCount() and @ref secondaryPerVertexJointCount().
         *
         * Expects that @ref Flag::DynamicPerVertexJointCount is set,
         * @p count is not larger than @ref perVertexJointCount() and
         * @p secondaryCount not larger than @ref secondaryPerVertexJointCount().
         * @see @ref Configuration::setJointCount()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders, which
         *      is not available in WebGL 1.0.
         */
        FlatGL<dimensions>& setPerVertexJointCount(UnsignedInt count, UnsignedInt secondaryCount = 0);
        #endif

        /** @{
         * @name Uniform setters
         *
         * Used only if @ref Flag::UniformBuffers is not set.
         */

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix. If
         * @ref Flag::InstancedTransformation is set, the per-instance
         * transformation matrix coming from the @ref TransformationMatrix
         * attribute is applied first, before this one.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationProjectionUniform2D::transformationProjectionMatrix /
         * @ref TransformationProjectionUniform3D::transformationProjectionMatrix
         * and call @ref bindTransformationProjectionBuffer() instead.
         */
        FlatGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix. If @ref Flag::InstancedTextureOffset is set, the
         * per-instance offset coming from the @ref TextureOffset attribute is
         * applied first, before this matrix.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TextureTransformationUniform::rotationScaling and
         * @ref TextureTransformationUniform::offset and call
         * @ref bindTextureTransformationBuffer() instead.
         */
        FlatGL<dimensions>& setTextureMatrix(const Matrix3& matrix);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set texture array layer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with @ref Flag::TextureArrays
         * enabled. Initial value is @cpp 0 @ce. If
         * @ref Flag::InstancedTextureOffset is set and a three-component
         * @ref TextureOffsetLayer attribute is used instead of
         * @ref TextureOffset, this value is added to the layer coming from the
         * third component.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TextureTransformationUniform::layer and call
         * @ref bindTextureTransformationBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,texture_array}
         * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& setTextureLayer(UnsignedInt layer);
        #endif

        /**
         * @brief Set color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. If @ref Flag::Textured
         * is set, the color is multiplied with the texture. If
         * @ref Flag::VertexColor is set, the color is multiplied with a color
         * coming from the @ref Color3 / @ref Color4 attribute.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref FlatMaterialUniform::color and call @ref bindMaterialBuffer()
         * instead.
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
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref FlatMaterialUniform::alphaMask and call
         * @ref bindMaterialBuffer() instead.
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
         * @ref Shaders-FlatGL-object-id for more information. Initial value is
         * @cpp 0 @ce. If @ref Flag::InstancedObjectId and/or
         * @ref Flag::ObjectIdTexture is enabled as well, this value is added
         * to the ID coming from the @ref ObjectId attribute and/or the
         * texture.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref FlatDrawUniform::objectId and call @ref bindDrawBuffer()
         * instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID output requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        FlatGL<dimensions>& setObjectId(UnsignedInt id);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set joint matrices
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Initial values are identity transformations. Expects that the size
         * of the @p matrices array is not larger than @ref jointCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform2D::transformationMatrix /
         * @ref TransformationUniform3D::transformationMatrix and call
         * @ref bindJointBuffer() instead.
         * @see @ref setJointMatrix(UnsignedInt, const MatrixTypeFor<dimensions, Float>&)
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        FlatGL<dimensions>& setJointMatrices(Containers::ArrayView<const MatrixTypeFor<dimensions, Float>> matrices);

        /**
         * @overload
         * @m_since_latest
         */
        FlatGL<dimensions>& setJointMatrices(std::initializer_list<MatrixTypeFor<dimensions, Float>> matrices);

        /**
         * @brief Set joint matrix for given joint
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Unlike @ref setJointMatrices() updates just a single joint matrix.
         * Expects that @p id is less than @ref jointCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform2D::transformationMatrix /
         * @ref TransformationUniform3D::transformationMatrix and call
         * @ref bindJointBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        FlatGL<dimensions>& setJointMatrix(UnsignedInt id, const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set per-instance joint count
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Offset added to joint IDs in the @ref JointIds and
         * @ref SecondaryJointIds in instanced draws. Should be less than
         * @ref jointCount(). Initial value is @cpp 0 @ce, meaning every
         * instance will use the same joint matrices, setting it to a non-zero
         * value causes the joint IDs to be interpreted as
         * @glsl gl_InstanceID*count + jointId @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref FlatDrawUniform::perInstanceJointCount and call
         * @ref bindDrawBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        FlatGL<dimensions>& setPerInstanceJointCount(UnsignedInt count);
        #endif

        /**
         * @}
         */

        #ifndef MAGNUM_TARGET_GLES2
        /** @{
         * @name Uniform / shader storage buffer binding and related uniform setters
         *
         * Used if @ref Flag::UniformBuffers is set.
         */

        /**
         * @brief Set a draw offset
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Specifies which item in the @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D, @ref FlatDrawUniform and
         * @ref TextureTransformationUniform buffers bound with
         * @ref bindTransformationProjectionBuffer(), @ref bindDrawBuffer() and
         * @ref bindTextureTransformationBuffer() should be used for current
         * draw. Expects that @ref Flag::UniformBuffers is set and @p offset is
         * less than @ref drawCount(). Initial value is @cpp 0 @ce, if
         * @ref drawCount() is @cpp 1 @ce, the function is a no-op as the
         * shader assumes draw offset to be always zero.
         *
         * If @ref Flag::MultiDraw is set, @glsl gl_DrawID @ce is added to this
         * value, which makes each draw submitted via
         * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Bind a transformation and projection uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D. At the very least you need
         * to call also @ref bindDrawBuffer() and @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        FlatGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a draw uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref FlatDrawUniform. At the very least you need to call also
         * @ref bindTransformationProjectionBuffer() and
         * @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        FlatGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a texture transformation uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that both @ref Flag::UniformBuffers and
         * @ref Flag::TextureTransformation is set. The buffer is expected to
         * contain @ref drawCount() instances of
         * @ref TextureTransformationUniform.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& bindTextureTransformationBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        FlatGL<dimensions>& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a material uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref materialCount() instances of
         * @ref FlatMaterialUniform. At the very least you need to call also
         * @ref bindTransformationProjectionBuffer() and @ref bindDrawBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        FlatGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a joint matrix uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref jointCount() instances of
         * @ref TransformationUniform2D / @ref TransformationUniform3D.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& bindJointBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        FlatGL<dimensions>& bindJointBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @}
         */
        #endif

        /** @{
         * @name Texture binding
         */

        /**
         * @brief Bind a color texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::Textured
         * enabled. If @ref Flag::TextureArrays is enabled as well, use
         * @ref bindTexture(GL::Texture2DArray&) instead.
         * @see @ref setColor(), @ref Flag::TextureTransformation,
         *      @ref setTextureMatrix()
         */
        FlatGL<dimensions>& bindTexture(GL::Texture2D& texture);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Bind a color array texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with both @ref Flag::Textured
         * and @ref Flag::TextureArrays enabled. If @ref Flag::UniformBuffers
         * is not enabled, the layer is set via @ref setTextureLayer(); if
         * @ref Flag::UniformBuffers is enabled,
         * @ref Flag::TextureTransformation has to be enabled as well and the
         * layer is set via @ref TextureTransformationUniform::layer.
         * @see @ref setColor(), @ref Flag::TextureTransformation,
         *      @ref setTextureLayer()
         * @requires_gl30 Extension @gl_extension{EXT,texture_array}
         * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& bindTexture(GL::Texture2DArray& texture);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Bind an object ID texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with @ref Flag::ObjectIdTexture
         * enabled. If @ref Flag::TextureArrays is enabled as well, use
         * @ref bindObjectIdTexture(GL::Texture2DArray&) instead. The texture
         * needs to have an unsigned integer format.
         * @see @ref setObjectId(), @ref Flag::TextureTransformation,
         *      @ref setTextureMatrix()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID output requires integer support in
         *      shaders, which is not available in WebGL 1.0.
         */
        FlatGL<dimensions>& bindObjectIdTexture(GL::Texture2D& texture);

        /**
         * @brief Bind an object ID array texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with both
         * @ref Flag::ObjectIdTexture and @ref Flag::TextureArrays enabled. If
         * @ref Flag::UniformBuffers is not enabled, the layer is set via
         * @ref setTextureLayer(); if @ref Flag::UniformBuffers is enabled,
         * @ref Flag::TextureTransformation has to be enabled as well and the
         * layer is set via @ref TextureTransformationUniform::layer.
         * @see @ref setObjectId(), @ref Flag::TextureTransformation,
         *      @ref setTextureLayer()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4} and
         *      @gl_extension{EXT,texture_array}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0. Texture
         *      arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID output requires integer support in
         *      shaders, which is not available in WebGL 1.0. Texture arrays
         *      are not available in WebGL 1.0.
         */
        FlatGL<dimensions>& bindObjectIdTexture(GL::Texture2DArray& texture);
        #endif

        /**
         * @}
         */

        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(FlatGL<dimensions>)

    private:
        /* Creates the GL shader program object but does nothing else.
           Internal, used by compile(). */
        explicit FlatGL(NoInitT);

        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _jointCount{},
            _perVertexJointCount{},
            _secondaryPerVertexJointCount{},
            _materialCount{},
            _drawCount{};
        #endif
        Int _transformationProjectionMatrixUniform{0},
            _textureMatrixUniform{1},
            #ifndef MAGNUM_TARGET_GLES2
            _textureLayerUniform{2},
            #endif
            _colorUniform{3},
            _alphaMaskUniform{4};
        #ifndef MAGNUM_TARGET_GLES2
        Int _objectIdUniform{5},
            _jointMatricesUniform{6},
            _perInstanceJointCountUniform, /* 6 + jointCount */
            /* Used instead of all other uniforms when Flag::UniformBuffers is
               set, so it can alias them */
            _drawOffsetUniform{0},
            _perVertexJointCountUniform; /* 7 + jointCount, or 1 with UBOs */
        #endif
};

/**
@brief Configuration
@m_since_latest

@see @ref FlatGL(const Configuration&), @ref compile(const Configuration&)
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT FlatGL<dimensions>::Configuration {
    public:
        explicit Configuration() = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set flags
         *
         * No flags are set by default.
         * @see @ref FlatGL::flags()
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt jointCount() const { return _jointCount; }

        /**
         * @brief Per-vertex joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt perVertexJointCount() const { return _perVertexJointCount; }

        /**
         *@brief Secondary per-vertex joint count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt secondaryPerVertexJointCount() const { return _secondaryPerVertexJointCount; }

        /**
         * @brief Set joint count
         *
         * If @ref Flag::UniformBuffers isn't set, @p count describes an upper
         * bound on how many joint matrices get supplied to each draw by
         * @ref setJointMatrices() / @ref setJointMatrix().
         *
         * If @ref Flag::UniformBuffers is set, @p count describes size of a
         * @ref TransformationUniform2D / @ref TransformationUniform3D buffer
         * bound with @ref bindJointBuffer(). Uniform buffers have a statically
         * defined size and @cpp count*sizeof(TransformationUniform2D) @ce /
         * @cpp count*sizeof(TransformationUniform3D) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-vertex joints index into
         * the array offset by @ref FlatDrawUniform::jointOffset.
         *
         * The @p perVertexCount and @p secondaryPerVertexCount parameters
         * describe how many components are taken from @ref JointIds /
         * @ref Weights and @ref SecondaryJointIds / @ref SecondaryWeights
         * attributes. Both values are expected to not be larger than
         * @cpp 4 @ce, setting either of these to @cpp 0 @ce means given
         * attribute is not used at all. If both @p perVertexCount and
         * @p secondaryPerVertexCount are set to @cpp 0 @ce, skinning is not
         * performed. Unless @ref Flag::ShaderStorageBuffers is set, if either
         * of them is non-zero, @p count is expected to be non-zero as well.
         *
         * Default value for all three is @cpp 0 @ce.
         * @see @ref FlatGL::jointCount(), @ref FlatGL::perVertexJointCount(),
         *      @ref FlatGL::secondaryPerVertexJointCount(),
         *      @ref Flag::DynamicPerVertexJointCount,
         *      @ref FlatGL::setPerVertexJointCount()
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        Configuration& setJointCount(UnsignedInt count, UnsignedInt perVertexCount, UnsignedInt secondaryPerVertexCount = 0);

        /**
         * @brief Material count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Set material count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref FlatMaterialUniform buffer bound with
         * @ref bindMaterialBuffer(). Uniform buffers have a statically defined
         * size and @cpp count*sizeof(FlatMaterialUniform) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-draw materials are
         * specified via @ref FlatDrawUniform::materialId. Default value is
         * @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setDrawCount(),
         *      @ref FlatGL::materialCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setMaterialCount(UnsignedInt count) {
            _materialCount = count;
            return *this;
        }

        /**
         * @brief Draw count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }

        /**
         * @brief Set draw count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D /
         * @ref FlatDrawUniform / @ref TextureTransformationUniform buffer
         * bound with @ref bindTransformationProjectionBuffer(),
         * @ref bindDrawBuffer() and @ref bindTextureTransformationBuffer().
         * Uniform buffers have a statically defined size and the maximum of
         * @cpp count*sizeof(TransformationProjectionUniform2D) @ce /
         * @cpp count*sizeof(TransformationProjectionUniform3D) @ce,
         * @cpp count*sizeof(FlatDrawUniform) @ce and
         * @cpp count*sizeof(TextureTransformationUniform) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffers are
         * unbounded and @p count is ignored. The draw offset is set via
         * @ref setDrawOffset(). Default value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setMaterialCount(),
         *      @ref FlatGL::drawCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setDrawCount(UnsignedInt count) {
            _drawCount = count;
            return *this;
        }
        #endif

    private:
        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _jointCount = 0,
            _perVertexJointCount = 0,
            _secondaryPerVertexJointCount = 0,
            _materialCount = 1,
            _drawCount = 1;
        #endif
};

/**
@brief Asynchronous compilation state
@m_since_latest

Returned by @ref compile(). See @ref shaders-async for more information.
*/
template<UnsignedInt dimensions> class FlatGL<dimensions>::CompileState: public FlatGL<dimensions> {
    /* Everything deliberately private except for the inheritance */
    friend class FlatGL;

    explicit CompileState(NoCreateT): FlatGL{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

    explicit CompileState(FlatGL<dimensions>&& shader, GL::Shader&& vert, GL::Shader&& frag
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , GL::Version version
        #endif
    ): FlatGL<dimensions>{Utility::move(shader)}, _vert{Utility::move(vert)}, _frag{Utility::move(frag)}
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , _version{version}
        #endif
        {}

    Implementation::GLShaderWrapper _vert, _frag;
    #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    GL::Version _version;
    #endif
};

/**
@brief 2D flat OpenGL shader
@m_since_latest

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
typedef FlatGL<2> FlatGL2D;

/**
@brief 3D flat OpenGL shader
@m_since_latest

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
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
}
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
