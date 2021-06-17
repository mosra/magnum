#ifndef Magnum_Shaders_PhongGL_h
#define Magnum_Shaders_PhongGL_h
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
 * @brief Class @ref Magnum::Shaders::PhongGL
 * @m_since_latest
 */

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Phong OpenGL shader
@m_since_latest

Uses ambient, diffuse and specular color or texture. For a colored mesh you
need to provide the @ref Position and @ref Normal attributes in your triangle
mesh. By default, the shader renders the mesh with a white color in an identity
transformation. Use @ref setTransformationMatrix(), @ref setNormalMatrix(),
@ref setProjectionMatrix(), @ref setLightPosition() and others to configure
the shader.

@image html shaders-phong.png width=256px

@section Shaders-PhongGL-colored Colored rendering

Common mesh setup:

@snippet MagnumShaders-gl.cpp PhongGL-usage-colored1

Common rendering setup:

@snippet MagnumShaders-gl.cpp PhongGL-usage-colored2

@section Shaders-PhongGL-texture Textured rendering

If you want to use textures, you need to provide also the
@ref TextureCoordinates attribute. Pass appropriate @ref Flag combination to
the constructor and then at render time don't forget to also call appropriate
subset of @ref bindAmbientTexture(), @ref bindDiffuseTexture() and
@ref bindSpecularTexture() (or the combined @ref bindTextures()). The texture
is multiplied by the color, which is by default set to fully opaque white for
enabled textures. Mesh setup with a diffuse and a specular texture:

@snippet MagnumShaders-gl.cpp PhongGL-usage-texture1

Common rendering setup:

@snippet MagnumShaders-gl.cpp PhongGL-usage-texture2

@section Shaders-PhongGL-lights Light specification

By default, the shader provides a single directional "fill" light, coming from
the center of the camera. Using the @p lightCount parameter in constructor, you
can specify how many lights you want, and then control light parameters using
@ref setLightPositions(), @ref setLightColors(), @ref setLightSpecularColors()
and @ref setLightRanges(). Light positions are specified as four-component
vectors, the last component distinguishing between directional and point
lights.

<ul><li>
Point lights are specified with camera-relative position and the last component
set to @cpp 1.0f @ce together with @ref setLightRanges() describing the
attenuation. The range corresponds to the @ref Trade::LightData::range() and
the attenuation is calculated as the following --- see
@ref Trade-LightData-attenuation for more information: @f[
    F_{att} = \frac{\operatorname{clamp}(1 - (\frac{d}{\color{m-info} R})^4, 0, 1)^2}{1 + d^2}
@f]

If you use @ref Constants::inf() as a range (which is also the default), the
equation reduces down to a simple inverse square: @f[
    F_{att} = \lim_{{\color{m-info} R} \to \infty} \frac{{\color{m-dim} \operatorname{clamp}(} 1 \mathbin{\color{m-dim} -} {\color{m-dim} (\frac{d}{R})^4, 0, 1)^2}}{1 + d^2} = \frac{1}{1 + d^2}
@f]
</li><li>
Directional lights are specified with a camera-relative direction *to* the
light with the last component set to @cpp 0.0f @ce --- which effectively makes
@f$ d = 0 @f$ --- and are not affected by values from @ref setLightRanges() in
any way: @f[
    F_{att} = \lim_{d \to 0} \frac{{\color{m-dim} \operatorname{clamp}(} 1 \mathbin{\color{m-dim} -} {\color{m-dim} (\frac{d}{R})^4, 0, 1)^2}}{1 \mathbin{\color{m-dim} +} {\color{m-dim} d^2}} = 1
@f]
</li></ul>

Light color and intensity, corresponding to @ref Trade::LightData::color() and
@ref Trade::LightData::intensity(), is meant to be multiplied together and
passed to @ref setLightColors() and @ref setLightSpecularColors().

The following example shows a three-light setup with one dim directional light
shining from the top and two stronger but range-limited point lights:

@snippet MagnumShaders-gl.cpp PhongGL-usage-lights

@subsection Shaders-PhongGL-lights-ambient Ambient lights

In order to avoid redundant uniform inputs, there's no dedicated way to specify
ambient lights. Instead, they are handled by the ambient color input, as the
math for ambient color and lights is equivalent. Add the ambient colors
together and reuse the diffuse texture in the @ref bindAmbientTexture() slot to
have it affected by the ambient as well:

@snippet MagnumShaders-gl.cpp PhongGL-usage-lights-ambient

@subsection Shaders-PhongGL-lights-zero Zero lights

As a special case, creating this shader with zero lights makes its output
equivalent to the @ref FlatGL3D shader --- only @ref setAmbientColor() and
@ref bindAmbientTexture() (if @ref Flag::AmbientTexture is enabled) are taken
into account, which corresponds to @ref FlatGL::setColor() and
@ref FlatGL::bindTexture(). This is useful to reduce complexity in apps that
render models with pre-baked lights. For instanced workflows using zero lights
means the @ref NormalMatrix instance attribute doesn't need to be supplied
either. In addition, enabling @ref Flag::VertexColor and using a default
ambient color with no texturing makes this shader equivalent to
@ref VertexColorGL.

@see @ref Trade::MaterialType::Flat

<b></b>

@m_class{m-note m-dim}

@par
    Attenuation based on constant/linear/quadratic factors (the
    @ref Trade::LightData::attenuation() property) and spot lights
    (@ref Trade::LightData::innerConeAngle(),
    @ref Trade::LightData::outerConeAngle() "outerConeAngle()") are not
    implemented at the moment.

@section Shaders-PhongGL-alpha Alpha blending and masking

Alpha / transparency is supported by the shader implicitly, but to have it
working on the framebuffer, you need to enable
@ref GL::Renderer::Feature::Blending and set up the blending function. See
@ref GL::Renderer::setBlendFunction() for details.

To avoid specular highlights on transparent areas, specular alpha should be
always set to @cpp 0.0f @ce. On the other hand, non-zero specular alpha can be
for example used to render transparent materials which are still expected to
have specular highlights such as glass or soap bubbles.

An alternative is to enable @ref Flag::AlphaMask and tune @ref setAlphaMask()
for simple binary alpha-masked drawing that doesn't require depth sorting or
blending enabled. Note that this feature is implemented using the GLSL
@glsl discard @ce operation which is known to have considerable performance
impact on some platforms. With proper depth sorting and blending you'll usually
get much better performance and output quality.

For general alpha-masked drawing you need to provide an ambient texture with
alpha channel and set alpha channel of the diffuse/specular color to @cpp 0.0f @ce
so only ambient alpha will be taken into account. If you have a diffuse texture
combined with the alpha mask, you can use that texture for both ambient and
diffuse part and then separate the alpha like this:

@snippet MagnumShaders-gl.cpp PhongGL-usage-alpha

@section Shaders-PhongGL-normal-mapping Normal mapping

If you want to use normal textures, enable @ref Flag::NormalTexture and call
@ref bindNormalTexture(). In addition you need to supply per-vertex tangent and
bitangent direction:

-   either using a four-component @ref Tangent4 attribute, where the sign of
    the fourth component defines handedness of tangent basis, as described in
    @ref Trade::MeshAttribute::Tangent;
-   or a using pair of three-component @ref Tangent and @ref Bitangent
    attributes together with enabling @ref Flag::Bitangent

If you supply just a three-component @ref Tangent attribute and no bitangents,
the shader will implicitly assume the fourth component to be @cpp 1.0f @ce,
forming a right-handed tangent space. This is a valid optimization when you
have full control over the bitangent orientation, but won't work with general
meshes.

@m_class{m-note m-success}

@par
    You can also use the @ref MeshVisualizerGL3D shader to visualize and debug
    per-vertex normal, tangent and binormal direction, among other things.

The strength of the effect can be controlled by
@ref setNormalTextureScale(). See
@ref Trade::MaterialAttribute::NormalTextureScale for a description of the
factor is used.

@section Shaders-PhongGL-object-id Object ID output

The shader supports writing object ID to the framebuffer for object picking or
other annotation purposes. Enable it using @ref Flag::ObjectId and set up an
integer buffer attached to the @ref ObjectIdOutput attachment. If you have a
batch of meshes with different object IDs, enable @ref Flag::InstancedObjectId
and supply per-vertex IDs to the @ref ObjectId attribute. The output will
contain a sum of the per-vertex ID and ID coming from @ref setObjectId().

The functionality is practically the same as in the @ref FlatGL shader, see
@ref Shaders-FlatGL-object-id "its documentation" for more information and usage
example.

@requires_gles30 Object ID output requires integer buffer attachments, which
    are not available in OpenGL ES 2.0 or WebGL 1.0.

@section Shaders-PhongGL-instancing Instanced rendering

Enabling @ref Flag::InstancedTransformation will turn the shader into an
instanced one. It'll take per-instance transformation and normal matrix from
the @ref TransformationMatrix and @ref NormalMatrix attributes, applying those
before the matrix set by @ref setTransformationMatrix() and
@ref setNormalMatrix(). Besides that, @ref Flag::VertexColor (and the
@ref Color3 / @ref Color4) attributes can work as both per-vertex and
per-instance, and for texturing it's possible to have per-instance texture
offset taken from @ref TextureOffset when @ref Flag::InstancedTextureOffset is
enabled (similarly to transformation, applied before @ref setTextureMatrix()).
The snippet below shows adding a buffer with per-instance transformation to a
mesh --- note how a normal matrix attribute has to be populated and supplied as
well to ensure lighting works:

@snippet MagnumShaders-gl.cpp PhongGL-usage-instancing

@requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
@requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
    @gl_extension{EXT,instanced_arrays} or @gl_extension{NV,instanced_arrays}
    in OpenGL ES 2.0.
@requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays} in WebGL
    1.0.

@section Shaders-PhongGL-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, the shader needs a separate
@ref ProjectionUniform3D and @ref TransformationUniform3D buffer, lights are
supplied via a @ref PhongLightUniform. To maximize use of the limited uniform
buffer memory, materials are supplied separately in a @ref PhongMaterialUniform
buffer and then referenced via @relativeref{PhongDrawUniform,materialId} from a
@ref PhongDrawUniform; for optional texture transformation a per-draw
@ref TextureTransformationUniform can be supplied as well. A uniform buffer
setup equivalent to the @ref Shaders-PhongGL-colored "colored case at the top",
with one default light, would look like this:

@snippet MagnumShaders-gl.cpp PhongGL-ubo

For a multidraw workflow enable @ref Flag::MultiDraw (and possibly
@ref Flag::TextureArrays) and supply desired light, material and draw count in
the @ref PhongGL(Flags, UnsignedInt, UnsignedInt, UnsignedInt) constructor. For
every draw then specify material references and texture offsets/layers. With
@ref Flag::LightCulling it's also possible to perform per-draw light culling by
supplying a subrange into the @ref PhongLightUniform array using
@ref PhongDrawUniform::lightOffset and @relativeref{PhongDrawUniform,lightCount}.
Besides that, the usage is similar for all shaders, see
@ref shaders-usage-multidraw for an example.

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

@see @ref shaders
*/
class MAGNUM_SHADERS_EXPORT PhongGL: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3".
         */
        typedef GenericGL3D::Position Position;

        /**
         * @brief Normal direction
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3".
         */
        typedef GenericGL3D::Normal Normal;

        /**
         * @brief Tangent direction
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3". Use either this or the @ref Tangent4
         * attribute. If only a three-component attribute is used and
         * @ref Flag::Bitangent is not enabled, it's the same as if
         * @ref Tangent4 was specified with the fourth component always being
         * @cpp 1.0f @ce. Used only if @ref Flag::NormalTexture is set.
         * @see @ref Shaders-PhongGL-normal-mapping
         */
        typedef GenericGL3D::Tangent Tangent;

        /**
         * @brief Tangent direction with a bitangent sign
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector4 "Vector4". Use either this or the @ref Tangent
         * attribute. If @ref Flag::Bitangent is set, the fourth component is
         * ignored and bitangents are taken from the @ref Bitangent attribute
         * instead. Used only if @ref Flag::NormalTexture is set.
         * @see @ref Shaders-PhongGL-normal-mapping
         */
        typedef GenericGL3D::Tangent4 Tangent4;

        /**
         * @brief Bitangent direction
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3". Use either this or the @ref Tangent4
         * attribute. Used only if both @ref Flag::NormalTexture and
         * @ref Flag::Bitangent are set.
         * @see @ref Shaders-PhongGL-normal-mapping
         */
        typedef GenericGL3D::Bitangent Bitangent;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2", used only if at least one of
         * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture and
         * @ref Flag::SpecularTexture is set.
         */
        typedef GenericGL3D::TextureCoordinates TextureCoordinates;

        /**
         * @brief Three-component vertex color
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color3. Use
         * either this or the @ref Color4 attribute. Used only if
         * @ref Flag::VertexColor is set.
         */
        typedef GenericGL3D::Color3 Color3;

        /**
         * @brief Four-component vertex color
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color4. Use
         * either this or the @ref Color3 attribute. Used only if
         * @ref Flag::VertexColor is set.
         */
        typedef GenericGL3D::Color4 Color4;

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
        typedef GenericGL3D::ObjectId ObjectId;
        #endif

        /**
         * @brief (Instanced) transformation matrix
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Matrix4.
         * Used only if @ref Flag::InstancedTransformation is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef GenericGL3D::TransformationMatrix TransformationMatrix;

        /**
         * @brief (Instanced) normal matrix
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Matrix3x3.
         * Used only if @ref Flag::InstancedTransformation is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef GenericGL3D::NormalMatrix NormalMatrix;

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
        typedef typename GenericGL3D::TextureOffset TextureOffset;

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) texture offset and layer
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Vector3, with
         * the last component interpreted as an integer. Use either this or the
         * @ref TextureOffset attribute. First two components used only if
         * @ref Flag::InstancedTextureOffset is set, third component only if
         * @ref Flag::TextureArrays is set.
         * @requires_gl33 Extension @gl_extension{EXT,texture_array} and
         *      @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
         */
        typedef typename GenericGL3D::TextureOffsetLayer TextureOffsetLayer;
        #endif

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             * @m_since{2019,10}
             */
            ColorOutput = GenericGL3D::ColorOutput,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Object ID shader output. @ref shaders-generic "Generic output",
             * present only if @ref Flag::ObjectId is set. Expects a
             * single-component unsigned integral attachment. Writes the value
             * set in @ref setObjectId() there, see
             * @ref Shaders-PhongGL-object-id for more information.
             * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2019,10}
             */
            ObjectIdOutput = GenericGL3D::ObjectIdOutput
            #endif
        };

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedInt {
            /**
             * Multiply ambient color with a texture.
             * @see @ref setAmbientColor(), @ref bindAmbientTexture()
             */
            AmbientTexture = 1 << 0,

            /**
             * Multiply diffuse color with a texture.
             * @see @ref setDiffuseColor(), @ref bindDiffuseTexture()
             */
            DiffuseTexture = 1 << 1,

            /**
             * Multiply specular color with a texture.
             * @see @ref setSpecularColor(), @ref bindSpecularTexture()
             */
            SpecularTexture = 1 << 2,

            /**
             * Modify normals according to a texture. Requires the
             * @ref Tangent attribute to be present.
             * @m_since{2019,10}
             */
            NormalTexture = 1 << 4,

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
            AlphaMask = 1 << 3,

            /**
             * Multiply the diffuse and ambient color with a vertex color.
             * Requires either the @ref Color3 or @ref Color4 attribute to be
             * present.
             * @m_since{2019,10}
             */
            VertexColor = 1 << 5,

            /**
             * Use the separate @ref Bitangent attribute for retrieving vertex
             * bitangents. If this flag is not present, the last component of
             * @ref Tangent4 is used to calculate bitangent direction. See
             * @ref Shaders-PhongGL-normal-mapping for more information.
             * @m_since_latest
             */
            Bitangent = 1 << 11,

            /**
             * Enable texture coordinate transformation. If this flag is set,
             * the shader expects that at least one of
             * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture,
             * @ref Flag::SpecularTexture or @ref Flag::NormalTexture is
             * enabled as well.
             * @see @ref setTextureMatrix()
             * @m_since{2020,06}
             */
            TextureTransformation = 1 << 6,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Enable object ID output. See @ref Shaders-PhongGL-object-id
             * for more information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2019,10}
             */
            ObjectId = 1 << 7,

            /**
             * Instanced object ID. Retrieves a per-instance / per-vertex
             * object ID from the @ref ObjectId attribute, outputting a sum of
             * the per-vertex ID and ID coming from @ref setObjectId() or
             * @ref PhongDrawUniform::objectId. Implicitly enables
             * @ref Flag::ObjectId. See @ref Shaders-PhongGL-object-id for more
             * information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2020,06}
             */
            InstancedObjectId = (1 << 8)|ObjectId,
            #endif

            /**
             * Instanced transformation. Retrieves a per-instance
             * transformation and normal matrix from the
             * @ref TransformationMatrix / @ref NormalMatrix attributes and
             * uses them together with matrices coming from
             * @ref setTransformationMatrix() and @ref setNormalMatrix() or
             * @ref TransformationUniform3D::transformationMatrix and
             * @ref PhongDrawUniform::normalMatrix (first the per-instance,
             * then the uniform matrix). See @ref Shaders-PhongGL-instancing
             * for more information.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
             *      @gl_extension{EXT,instanced_arrays} or
             *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
             *      in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedTransformation = 1 << 9,

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
             * @ref Shaders-PhongGL-instancing for more information.
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
            InstancedTextureOffset = (1 << 10)|TextureTransformation,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindProjectionBuffer(), @ref bindTransformationBuffer(),
             * @ref bindDrawBuffer(), @ref bindTextureTransformationBuffer(),
             * @ref bindMaterialBuffer() and @ref bindLightBuffer() instead of
             * direct uniform setters.
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 12,

            /**
             * Enable multidraw functionality. Implies @ref Flag::UniformBuffers
             * and adds the value from @ref setDrawOffset() with the
             * @glsl gl_DrawID @ce builtin, which makes draws submitted via
             * @ref GL::AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
             * pick up per-draw parameters directly, without having to rebind
             * the uniform buffers or specify @ref setDrawOffset() before each
             * draw. In a non-multidraw scenario, @glsl gl_DrawID @ce is
             * @cpp 0 @ce, which means a shader with this flag enabled can be
             * used for regular draws as well.
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
            MultiDraw = UniformBuffers|(1 << 13),

            /**
             * Use 2D texture arrays. Expects that the texture is supplied via
             * @ref bindAmbientTexture(GL::Texture2DArray&) /
             * @ref bindDiffuseTexture(GL::Texture2DArray&) /
             * @ref bindSpecularTexture(GL::Texture2DArray&) /
             * @ref bindNormalTexture(GL::Texture2DArray&) instead of
             * @ref bindAmbientTexture(GL::Texture2D&) /
             * @ref bindDiffuseTexture(GL::Texture2D&) /
             * @ref bindSpecularTexture(GL::Texture2D&) /
             * @ref bindNormalTexture(GL::Texture2D&) and the layer shared by
             * all textures is set via @ref setTextureLayer() or
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
            TextureArrays = 1 << 14,

            /**
             * Enable light culling in uniform buffer workflows using the
             * @ref PhongDrawUniform::lightOffset and
             * @ref PhongDrawUniform::lightCount fields. If not enabled, all
             * @ref lightCount() lights are used for every draw. Expects that
             * @ref Flag::UniformBuffers is enabled as well.
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            LightCulling = 1 << 15,
            #endif

            /**
             * Disable specular contribution in light calculation. Can result
             * in a significant performance improvement compared to calling
             * @ref setSpecularColor() with @cpp 0x00000000_rgbaf @ce when
             * specular highlights are not desired.
             * @m_since_latest
             */
            NoSpecular = 1 << 16
        };

        /**
         * @brief Flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags         Flags
         * @param lightCount    Count of light sources
         *
         * While this function is meant mainly for the classic uniform
         * scenario (without @ref Flag::UniformBuffers set), it's equivalent to
         * @ref PhongGL(Flags, UnsignedInt, UnsignedInt, UnsignedInt) with
         * @p materialCount and @p drawCount set to @cpp 1 @ce.
         */
        explicit PhongGL(Flags flags = {}, UnsignedInt lightCount = 1);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @param flags         Flags
         * @param lightCount    Size of a @ref PhongLightUniform buffer bound
         *      with @ref bindLightBuffer()
         * @param materialCount Size of a @ref PhongMaterialUniform buffer
         *      bound with @ref bindMaterialBuffer()
         * @param drawCount     Size of a @ref ProjectionUniform3D /
         *      @ref TransformationUniform3D / @ref PhongDrawUniform /
         *      @ref TextureTransformationUniform buffer bound with
         *      @ref bindProjectionBuffer(), @ref bindTransformationBuffer(),
         *      @ref bindDrawBuffer() and @ref bindTextureTransformationBuffer()
         *
         * If @p flags contains @ref Flag::UniformBuffers, @p lightCount,
         * @p materialCount and @p drawCount describe the uniform buffer sizes
         * as these are required to have a statically defined size. The draw
         * offset is then set via @ref setDrawOffset() and the per-draw
         * materials and lights are specified via
         * @ref PhongDrawUniform::materialId,
         * @ref PhongDrawUniform::lightOffset and
         * @ref PhongDrawUniform::lightCount.
         *
         * If @p flags don't contain @ref Flag::UniformBuffers,
         * @p materialCount and @p drawCount is ignored and the constructor
         * behaves the same as @ref PhongGL(Flags, UnsignedInt).
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        /** @todo this constructor will eventually need to have also joint
            count, per-vertex weight count, view count for multiview and clip
            plane count ... and putting them in arbitrary order next to each
            other is too error-prone, so it needs some other solution
            (accepting pairs of parameter type and value like in GL context
            creation, e.g., which will probably need a new enum as reusing Flag
            for this might be too confusing) */
        explicit PhongGL(Flags flags, UnsignedInt lightCount, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif

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
        explicit PhongGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        PhongGL(const PhongGL&) = delete;

        /** @brief Move constructor */
        PhongGL(PhongGL&&) noexcept = default;

        /** @brief Copying is not allowed */
        PhongGL& operator=(const PhongGL&) = delete;

        /** @brief Move assignment */
        PhongGL& operator=(PhongGL&&) noexcept = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Light count
         *
         * If @ref Flag::UniformBuffers is set, this is the statically defined
         * size of the @ref PhongLightUniform uniform buffer.
         * @see @ref bindLightBuffer()
         */
        UnsignedInt lightCount() const { return _lightCount; }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Material count
         * @m_since_latest
         *
         * Statically defined size of the @ref PhongMaterialUniform uniform
         * buffer. Has use only if @ref Flag::UniformBuffers is set.
         * @see @ref bindMaterialBuffer()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Draw count
         * @m_since_latest
         *
         * Statically defined size of each of the @ref ProjectionUniform3D,
         * @ref TransformationUniform3D, @ref PhongDrawUniform and
         * @ref TextureTransformationUniform uniform buffers. Has use only if
         * @ref Flag::UniformBuffers is set.
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }
        #endif

        /** @{
         * @name Uniform setters
         *
         * Used only if @ref Flag::UniformBuffers is not set.
         */

        /**
         * @brief Set ambient color
         * @return Reference to self (for method chaining)
         *
         * If @ref Flag::AmbientTexture is set, default value is
         * @cpp 0xffffffff_rgbaf @ce and the color will be multiplied with
         * ambient texture, otherwise default value is @cpp 0x00000000_rgbaf @ce.
         * If @ref Flag::VertexColor is set, the color is multiplied with a
         * color coming from the @ref Color3 / @ref Color4 attribute.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongMaterialUniform::ambientColor and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref bindAmbientTexture(), @ref Shaders-PhongGL-lights-ambient
         */
        PhongGL& setAmbientColor(const Magnum::Color4& color);

        /**
         * @brief Set diffuse color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. If
         * @ref Flag::DiffuseTexture is set, the color will be multiplied with
         * the texture. If @ref lightCount() is zero, this function is a no-op,
         * as diffuse color doesn't contribute to the output in that case.
         * If @ref Flag::VertexColor is set, the color is multiplied with a
         * color coming from the @ref Color3 / @ref Color4 attribute.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongMaterialUniform::diffuseColor and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref bindDiffuseTexture()
         */
        PhongGL& setDiffuseColor(const Magnum::Color4& color);

        /**
         * @brief Set normal texture scale
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Affects strength of the normal mapping. Initial value is
         * @cpp 1.0f @ce, meaning the normal texture is not changed in any way;
         * a value of @cpp 0.0f @ce disables the normal texture effect
         * altogether.
         *
         * Expects that the shader was created with @ref Flag::NormalTexture
         * enabled. If @ref lightCount() is zero, this function is a no-op, as
         * normals don't contribute to the output in that case.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongMaterialUniform::normalTextureScale and call
         * @ref bindDrawBuffer() instead.
         * @see @ref Shaders-PhongGL-normal-mapping, @ref bindNormalTexture(),
         *      @ref Trade::MaterialAttribute::NormalTextureScale
         */
        PhongGL& setNormalTextureScale(Float scale);

        /**
         * @brief Set specular color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffff00_rgbaf @ce. Expects that the shader
         * was not created with @ref Flag::NoSpecular. If
         * @ref Flag::SpecularTexture is set, the color will be multiplied with
         * the texture. If you want to have a fully diffuse material, it's
         * recommended to disable the specular contribution altogether with
         * @ref Flag::NoSpecular. If having a dedicated shader variant is not
         * possible, set the specular color to @cpp 0x00000000_rgbaf @ce. If
         * @ref lightCount() is zero, this function is a no-op, as specular
         * color doesn't contribute to the output in that case.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongMaterialUniform::specularColor and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref bindSpecularTexture()
         */
        PhongGL& setSpecularColor(const Magnum::Color4& color);

        /**
         * @brief Set shininess
         * @return Reference to self (for method chaining)
         *
         * The larger value, the harder surface (smaller specular highlight).
         * Initial value is @cpp 80.0f @ce. If @ref lightCount() is zero, this
         * function is a no-op, as specular color doesn't contribute to the
         * output in that case.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongMaterialUniform::shininess and call
         * @ref bindMaterialBuffer() instead.
         */
        PhongGL& setShininess(Float shininess);

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
         * @ref PhongMaterialUniform::alphaMask and call
         * @ref bindMaterialBuffer() instead.
         * @m_keywords{glAlphaFunc()}
         */
        PhongGL& setAlphaMask(Float mask);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set object ID
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::ObjectId
         * enabled. Value set here is written to the @ref ObjectIdOutput, see
         * @ref Shaders-PhongGL-object-id for more information. Default is
         * @cpp 0 @ce. If @ref Flag::InstancedObjectId is enabled as well, this
         * value is added to the ID coming from the @ref ObjectId attribute.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongDrawUniform::objectId and call @ref bindDrawBuffer()
         * instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0 or WebGL 1.0.
         */
        PhongGL& setObjectId(UnsignedInt id);
        #endif

        /**
         * @brief Set transformation matrix
         * @return Reference to self (for method chaining)
         *
         * You need to set also @ref setNormalMatrix() with a corresponding
         * value. Initial value is an identity matrix. If
         * @ref Flag::InstancedTransformation is set, the per-instance
         * transformation coming from the @ref TransformationMatrix attribute
         * is applied first, before this one.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform3D::transformationMatrix and call
         * @ref bindTransformationBuffer() instead.
         */
        PhongGL& setTransformationMatrix(const Matrix4& matrix);

        /**
         * @brief Set normal matrix
         * @return Reference to self (for method chaining)
         *
         * The matrix doesn't need to be normalized, as renormalization is done
         * per-fragment anyway. You need to set also
         * @ref setTransformationMatrix() with a corresponding value. Initial
         * value is an identity matrix. If @ref lightCount() is zero, this
         * function is a no-op, as normals don't contribute to the output in
         * that case. If @ref Flag::InstancedTransformation is set, the
         * per-instance normal matrix coming from the @ref NormalMatrix
         * attribute is applied first, before this one.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongDrawUniform::normalMatrix and call
         * @ref bindDrawBuffer() instead.
         * @see @ref Math::Matrix4::normalMatrix()
         */
        PhongGL& setNormalMatrix(const Matrix3x3& matrix);

        /**
         * @brief Set projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix (i.e., an orthographic
         * projection of the default @f$ [ -\boldsymbol{1} ; \boldsymbol{1} ] @f$
         * cube).
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref ProjectionUniform3D::projectionMatrix and call
         * @ref bindProjectionBuffer() instead.
         */
        PhongGL& setProjectionMatrix(const Matrix4& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix. If @ref Flag::InstancedTextureOffset is set, the
         * per-instance offset coming from the @ref TextureOffset atttribute is
         * applied first, before this matrix.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TextureTransformationUniform::rotationScaling and
         * @ref TextureTransformationUniform::offset and call
         * @ref bindTextureTransformationBuffer() instead.
         */
        PhongGL& setTextureMatrix(const Matrix3& matrix);

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
        PhongGL& setTextureLayer(UnsignedInt layer);
        #endif

        /**
         * @brief Set light positions
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Depending on the fourth component, the value is treated as either a
         * camera-relative position of a point light, if the fourth component
         * is @cpp 1.0f @ce; or a direction *to* a directional light, if the
         * fourth component is @cpp 0.0f @ce. Expects that the size of the
         * @p positions array is the same as @ref lightCount(). Initial values
         * are @cpp {0.0f, 0.0f, 1.0f, 0.0f} @ce --- a directional "fill" light
         * coming from the camera.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongLightUniform::position and call
         * @ref bindLightBuffer() instead
         * @see @ref Shaders-PhongGL-lights, @ref setLightPosition()
         */
        PhongGL& setLightPositions(Containers::ArrayView<const Vector4> positions);

        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& setLightPositions(std::initializer_list<Vector4> positions);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief setLightPositions(Containers::ArrayView<const Vector4>)
         * @m_deprecated_since_latest Use @ref setLightPositions(Containers::ArrayView<const Vector4>)
         *      instead. This function sets the fourth component to
         *      @cpp 0.0f @ce to preserve the original behavior as close as
         *      possible.
         */
        CORRADE_DEPRECATED("use setLightPositions(Containers::ArrayView<const Vector4>) instead") PhongGL& setLightPositions(Containers::ArrayView<const Vector3> positions);

        /**
         * @brief @copybrief setLightPositions(std::initializer_list<Vector4>)
         * @m_deprecated_since_latest Use @ref setLightPositions(std::initializer_list<Vector4>)
         *      instead. This function sets the fourth component to
         *      @cpp 0.0f @ce to preserve the original behavior as close as
         *      possible.
         */
        CORRADE_DEPRECATED("use setLightPositions(std::initializer_list<Vector4>) instead") PhongGL& setLightPositions(std::initializer_list<Vector3> positions);
        #endif

        /**
         * @brief Set position for given light
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Unlike @ref setLightPositions() updates just a single light
         * position. If updating more than one light, prefer the batch function
         * instead to reduce the count of GL API calls. Expects that @p id is
         * less than @ref lightCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongLightUniform::position and call @ref bindLightBuffer()
         * instead.
         */
        PhongGL& setLightPosition(UnsignedInt id, const Vector4& position);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief setLightPosition(UnsignedInt, const Vector4&)
         * @m_deprecated_since_latest Use @ref setLightPosition(UnsignedInt, const Vector4&)
         *      instead. This function sets the fourth component to
         *      @cpp 0.0f @ce to preserve the original behavior as close as
         *      possible.
         */
        CORRADE_DEPRECATED("use setLightPosition(UnsignedInt, const Vector4&) instead") PhongGL& setLightPosition(UnsignedInt id, const Vector3& position);

        /**
         * @brief Set light position
         * @m_deprecated_since_latest Use @ref setLightPositions(std::initializer_list<Vector4>)
         *      with a single item instead --- it's short enough to not warrant
         *      the existence of a dedicated overload. This function sets the
         *      fourth component to @cpp 0.0f @ce to preserve the original
         *      behavior as close as possible.
         */
        CORRADE_DEPRECATED("use setLightPositions(std::initializer_list<Vector4>) instead") PhongGL& setLightPosition(const Vector3& position);
        #endif

        /**
         * @brief Set light colors
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Initial values are @cpp 0xffffff_rgbf @ce. Expects that the size
         * of the @p colors array is the same as @ref lightCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongLightUniform::color and call @ref bindLightBuffer()
         * instead.
         * @see @ref Shaders-PhongGL-lights, @ref setLightColor()
         */
        PhongGL& setLightColors(Containers::ArrayView<const Magnum::Color3> colors);

        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& setLightColors(std::initializer_list<Magnum::Color3> colors);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief setLightColors(Containers::ArrayView<const Magnum::Color3>)
         * @m_deprecated_since_latest Use @ref setLightColors(Containers::ArrayView<const Magnum::Color3>)
         *      instead. The alpha channel isn't used in any way.
         */
        CORRADE_DEPRECATED("use setLightColors(Containers::ArrayView<const Magnum::Color3>) instead") PhongGL& setLightColors(Containers::ArrayView<const Magnum::Color4> colors);

        /**
         * @brief @copybrief setLightColors(std::initializer_list<Magnum::Color3>)
         * @m_deprecated_since_latest Use @ref setLightColors(std::initializer_list<Magnum::Color3>)
         *      instead. The alpha channel isn't used in any way.
         */
        CORRADE_DEPRECATED("use setLightColors(std::initializer_list<Magnum::Color3>) instead") PhongGL& setLightColors(std::initializer_list<Magnum::Color4> colors);
        #endif

        /**
         * @brief Set color for given light
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Unlike @ref setLightColors() updates just a single light color. If
         * updating more than one light, prefer the batch function instead to
         * reduce the count of GL API calls. Expects that @p id is less than
         * @ref lightCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongLightUniform::color and call @ref bindLightBuffer()
         * instead.
         */
        PhongGL& setLightColor(UnsignedInt id, const Magnum::Color3& color);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief setLightColor(UnsignedInt, const Magnum::Color3&)
         * @m_deprecated_since_latest Use @ref setLightColor(UnsignedInt, const Magnum::Color3&)
         *      instead. The alpha channel isn't used in any way.
         */
        CORRADE_DEPRECATED("use setLightColor(UnsignedInt, const Magnum::Color3&) instead") PhongGL& setLightColor(UnsignedInt id, const Magnum::Color4& color);

        /**
         * @brief Set light color
         * @m_deprecated_since_latest Use @ref setLightColors(std::initializer_list<Magnum::Color3>)
         *      with a single item instead --- it's short enough to not warrant
         *      the existence of a dedicated overload. The alpha channel isn't
         *      used in any way.
         */
        CORRADE_DEPRECATED("use setLightColor(std::initializer_list<Color3>) instead") PhongGL& setLightColor(const Magnum::Color4& color);
        #endif

        /**
         * @brief Set light specular colors
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Usually you'd set this value to the same as @ref setLightColors(),
         * but it allows for greater flexibility such as disabling specular
         * highlights on certain lights. Initial values are
         * @cpp 0xffffff_rgbf @ce. Expects that the size of the @p colors array
         * is the same as @ref lightCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongLightUniform::specularColor and call @ref bindLightBuffer()
         * instead.
         * @see @ref Shaders-PhongGL-lights, @ref setLightColor()
         */
        PhongGL& setLightSpecularColors(Containers::ArrayView<const Magnum::Color3> colors);

        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& setLightSpecularColors(std::initializer_list<Magnum::Color3> colors);

        /**
         * @brief Set specular color for given light
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Unlike @ref setLightSpecularColors() updates just a single light
         * color. If updating more than one light, prefer the batch function
         * instead to reduce the count of GL API calls. Expects that @p id is
         * less than @ref lightCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongLightUniform::specularColor and call
         * @ref bindLightBuffer() instead.
         */
        PhongGL& setLightSpecularColor(UnsignedInt id, const Magnum::Color3& color);

        /**
         * @brief Set light attenuation ranges
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Initial values are @ref Constants::inf(). Expects that the size of
         * the @p ranges array is the same as @ref lightCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongLightUniform::range and call @ref bindLightBuffer()
         * instead.
         * @see @ref Shaders-PhongGL-lights, @ref setLightRange()
         */
        PhongGL& setLightRanges(Containers::ArrayView<const Float> ranges);

        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& setLightRanges(std::initializer_list<Float> ranges);

        /**
         * @brief Set attenuation range for given light
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Unlike @ref setLightRanges() updates just a single light range. If
         * updating more than one light, prefer the batch function instead to
         * reduce the count of GL API calls. Expects that @p id is less than
         * @ref lightCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongLightUniform::range and call @ref bindLightBuffer()
         * instead.
         */
        PhongGL& setLightRange(UnsignedInt id, Float range);

        /**
         * @}
         */

        #ifndef MAGNUM_TARGET_GLES2
        /** @{
         * @name Uniform buffer binding and related uniform setters
         *
         * Used if @ref Flag::UniformBuffers is set.
         */

        /**
         * @brief Set a draw offset
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Specifies which item in the @ref TransformationUniform3D,
         * @ref PhongDrawUniform and @ref TextureTransformationUniform buffers
         * bound with @ref bindTransformationBuffer(), @ref bindDrawBuffer()
         * and @ref bindTextureTransformationBuffer() should be used for
         * current draw. Expects that @ref Flag::UniformBuffers is set and
         * @p offset is less than @ref drawCount(). Initial value is
         * @cpp 0 @ce, if @ref drawCount() is @cpp 1 @ce, the function is a
         * no-op as the shader assumes draw offset to be always zero.
         *
         * If @ref Flag::MultiDraw is set, @glsl gl_DrawID @ce is added to this
         * value, which makes each draw submitted via
         * @ref GL::AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        PhongGL& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Set a projection uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain at least one instance of
         * @ref ProjectionUniform3D. At the very least you need to call also
         * @ref bindTransformationBuffer(), @ref bindDrawBuffer() and
         * @ref bindMaterialBuffer(), usually @ref bindLightBuffer() as well.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        PhongGL& bindProjectionBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& bindProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a transformation uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref TransformationUniform3D. At the very least you need to call
         * also @ref bindProjectionBuffer(), @ref bindDrawBuffer() and
         * @ref bindMaterialBuffer(), usually @ref bindLightBuffer() as well.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        PhongGL& bindTransformationBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& bindTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a draw uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref PhongDrawUniform. At the very least you need to call also
         * @ref bindProjectionBuffer(), @ref bindTransformationBuffer() and
         * @ref bindMaterialBuffer(), usually @ref bindLightBuffer() as well.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        PhongGL& bindDrawBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a texture transformation uniform buffer
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
        PhongGL& bindTextureTransformationBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a material uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref materialCount() instances of
         * @ref PhongMaterialUniform. At the very least you need to call also
         * @ref bindProjectionBuffer(), @ref bindTransformationBuffer() and
         * @ref bindDrawBuffer(), usually @ref bindLightBuffer() as well.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        PhongGL& bindMaterialBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a light uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref lightCount() instances of
         * @ref PhongLightUniform.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        PhongGL& bindLightBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& bindLightBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @}
         */
        #endif

        /** @{
         * @name Texture binding
         */

        /**
         * @brief Bind an ambient texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::AmbientTexture
         * enabled. If @ref Flag::TextureArrays is enabled as well, use
         * @ref bindAmbientTexture(GL::Texture2DArray&) instead.
         * @see @ref bindTextures(), @ref setAmbientColor(),
         *      @ref Shaders-PhongGL-lights-ambient
         */
        PhongGL& bindAmbientTexture(GL::Texture2D& texture);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Bind an ambient array texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with both
         * @ref Flag::AmbientTexture and @ref Flag::TextureArrays enabled. If
         * @ref Flag::UniformBuffers is not enabled, the layer is set via
         * @ref setTextureLayer(); if @ref Flag::UniformBuffers is enabled,
         * @ref Flag::TextureTransformation has to be enabled as well and the
         * layer is set via @ref TextureTransformationUniform::layer.
         * @see @ref setAmbientColor(), @ref Shaders-PhongGL-lights-ambient
         * @requires_gl30 Extension @gl_extension{EXT,texture_array}
         * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
         */
        PhongGL& bindAmbientTexture(GL::Texture2DArray& texture);
        #endif

        /**
         * @brief Bind a diffuse texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::DiffuseTexture
         * enabled. If @ref Flag::TextureArrays is enabled as well, use
         * @ref bindDiffuseTexture(GL::Texture2DArray&) instead. If
         * @ref lightCount() is zero, this function is a no-op, as diffuse
         * color doesn't contribute to the output in that case.
         * @see @ref bindTextures(), @ref setDiffuseColor()
         */
        PhongGL& bindDiffuseTexture(GL::Texture2D& texture);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Bind a diffuse array texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with both
         * @ref Flag::DiffuseTexture and @ref Flag::TextureArrays
         * enabled. If @ref Flag::UniformBuffers is not enabled, the layer is
         * set via @ref setTextureLayer(); if @ref Flag::UniformBuffers is
         * enabled, @ref Flag::TextureTransformation has to be enabled as well
         * and the layer is set via @ref TextureTransformationUniform::layer.
         * If @ref lightCount() is zero, this function is a no-op, as diffuse
         * color doesn't contribute to the output in that case.
         * @see @ref setDiffuseColor()
         * @requires_gl30 Extension @gl_extension{EXT,texture_array}
         * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
         */
        PhongGL& bindDiffuseTexture(GL::Texture2DArray& texture);
        #endif

        /**
         * @brief Bind a specular texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::SpecularTexture
         * enabled and that @ref Flag::NoSpecular is not set. If
         * @ref Flag::TextureArrays is enabled as well, use
         * @ref bindSpecularTexture(GL::Texture2DArray&) instead. If
         * @ref lightCount() is zero, this function is a no-op, as specular
         * color doesn't contribute to the output in that case.
         * @see @ref bindTextures(), @ref setSpecularColor()
         */
        PhongGL& bindSpecularTexture(GL::Texture2D& texture);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Bind a specular array texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with both
         * @ref Flag::SpecularTexture and @ref Flag::TextureArrays enabled and
         * that @ref Flag::NoSpecular is not set. If @ref Flag::UniformBuffers
         * is not enabled, the layer is set via @ref setTextureLayer(); if
         * @ref Flag::UniformBuffers is enabled,
         * @ref Flag::TextureTransformation has to be enabled as well and the
         * layer is set via @ref TextureTransformationUniform::layer. If
         * @ref lightCount() is zero, this function is a no-op, as specular
         * color doesn't contribute to the output in that case.
         * @see @ref setSpecularColor()
         * @requires_gl30 Extension @gl_extension{EXT,texture_array}
         * @requires_gles30 Texture arrays are not available in OpenGL ES 2.0.
         * @requires_webgl20 Texture arrays are not available in WebGL 1.0.
         */
        PhongGL& bindSpecularTexture(GL::Texture2DArray& texture);
        #endif

        /**
         * @brief Bind a normal texture
         * @return Reference to self (for method chaining)
         * @m_since{2019,10}
         *
         * Expects that the shader was created with @ref Flag::NormalTexture
         * enabled and the @ref Tangent attribute was supplied. If
         * @ref Flag::TextureArrays is enabled as well, use
         * @ref bindNormalTexture(GL::Texture2DArray&) instead. If
         * @ref lightCount() is zero, this function is a no-op, as normals
         * don't contribute to the output in that case.
         * @see @ref Shaders-PhongGL-normal-mapping,
         *      @ref bindTextures(), @ref setNormalTextureScale()
         */
        PhongGL& bindNormalTexture(GL::Texture2D& texture);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Bind a normal array texture
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that the shader was created with both
         * @ref Flag::NormalTexture and @ref Flag::TextureArrays enabled and
         * the @ref Tangent attribute was supplied. If @ref lightCount() is
         * zero, this function is a no-op, as normals don't contribute to the
         * output in that case.
         * @see @ref Shaders-PhongGL-normal-mapping,
         *      @ref setNormalTextureScale()
         */
        PhongGL& bindNormalTexture(GL::Texture2DArray& texture);
        #endif

        /**
         * @brief Bind textures
         * @return Reference to self (for method chaining)
         *
         * A particular texture has effect only if particular texture flag from
         * @ref PhongGL::Flag "Flag" is set, you can use @cpp nullptr @ce for
         * the rest. Expects that the shader was created with at least one of
         * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture,
         * @ref Flag::SpecularTexture or @ref Flag::NormalTexture enabled and
         * @ref Flag::TextureArrays is not set. More efficient than setting
         * each texture separately.
         * @see @ref bindAmbientTexture(), @ref bindDiffuseTexture(),
         *      @ref bindSpecularTexture(), @ref bindNormalTexture()
         */
        PhongGL& bindTextures(GL::Texture2D* ambient, GL::Texture2D* diffuse, GL::Texture2D* specular, GL::Texture2D* normal
            #ifdef MAGNUM_BUILD_DEPRECATED
            = nullptr
            #endif
        );

        /**
         * @}
         */

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        PhongGL& draw(GL::Mesh& mesh) {
            return static_cast<PhongGL&>(GL::AbstractShaderProgram::draw(mesh));
        }
        PhongGL& draw(GL::Mesh&& mesh) {
            return static_cast<PhongGL&>(GL::AbstractShaderProgram::draw(mesh));
        }
        PhongGL& draw(GL::MeshView& mesh) {
            return static_cast<PhongGL&>(GL::AbstractShaderProgram::draw(mesh));
        }
        PhongGL& draw(GL::MeshView&& mesh) {
            return static_cast<PhongGL&>(GL::AbstractShaderProgram::draw(mesh));
        }
        PhongGL& draw(Containers::ArrayView<const Containers::Reference<GL::MeshView>> meshes) {
            return static_cast<PhongGL&>(GL::AbstractShaderProgram::draw(meshes));
        }
        PhongGL& draw(std::initializer_list<Containers::Reference<GL::MeshView>> meshes) {
            return static_cast<PhongGL&>(GL::AbstractShaderProgram::draw(meshes));
        }
        #endif

    private:
        /* Prevent accidentally calling irrelevant functions */
        #ifndef MAGNUM_TARGET_GLES
        using GL::AbstractShaderProgram::drawTransformFeedback;
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        using GL::AbstractShaderProgram::dispatchCompute;
        #endif

        Flags _flags;
        UnsignedInt _lightCount{};
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _materialCount{}, _drawCount{};
        #endif
        Int _transformationMatrixUniform{0},
            _projectionMatrixUniform{1},
            _normalMatrixUniform{2},
            _textureMatrixUniform{3},
            #ifndef MAGNUM_TARGET_GLES2
            _textureLayerUniform{4},
            #endif
            _ambientColorUniform{5},
            _diffuseColorUniform{6},
            _specularColorUniform{7},
            _shininessUniform{8},
            _normalTextureScaleUniform{9},
            _alphaMaskUniform{10};
        #ifndef MAGNUM_TARGET_GLES2
        Int _objectIdUniform{11};
        #endif
        Int _lightPositionsUniform{12},
            _lightColorsUniform, /* 12 + lightCount, set in the constructor */
            _lightSpecularColorsUniform, /* 12 + 2*lightCount */
            _lightRangesUniform; /* 12 + 3*lightCount */
        #ifndef MAGNUM_TARGET_GLES2
        /* Used instead of all other uniforms when Flag::UniformBuffers is set,
           so it can alias them */
        Int _drawOffsetUniform{0};
        #endif
};

/** @debugoperatorclassenum{PhongGL,PhongGL::Flag} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, PhongGL::Flag value);

/** @debugoperatorclassenum{PhongGL,PhongGL::Flags} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, PhongGL::Flags value);

CORRADE_ENUMSET_OPERATORS(PhongGL::Flags)

}}

#endif
