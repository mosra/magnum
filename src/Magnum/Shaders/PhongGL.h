#ifndef Magnum_Shaders_PhongGL_h
#define Magnum_Shaders_PhongGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Jonathan Hale <squareys@googlemail.com>
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
 * @brief Class @ref Magnum::Shaders::PhongGL
 * @m_since_latest
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <initializer_list>
#include <Corrade/Utility/Move.h>

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/glShaderWrapper.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Phong OpenGL shader
@m_since_latest

Uses ambient, diffuse and specular color or texture. For a colored mesh you
need to provide the @ref Position and @ref Normal attributes in your triangle
mesh. By default, the shader renders the mesh with a white color in an identity
transformation. Use @ref setProjectionMatrix(). @ref setTransformationMatrix(),
@ref setNormalMatrix(), @ref setLightPosition() and others to configure the
shader.

@image html shaders-phong.png width=256px

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section Shaders-PhongGL-colored Colored rendering

Common mesh setup:

@snippet Shaders-gl.cpp PhongGL-usage-colored1

Common rendering setup:

@snippet Shaders-gl.cpp PhongGL-usage-colored2

@section Shaders-PhongGL-texture Textured rendering

If you want to use textures, you need to provide also the
@ref TextureCoordinates attribute. Pass appropriate @ref Flag combination to
@ref Configuration::setFlags() and then at render time don't forget to also
call appropriate subset of @ref bindAmbientTexture(), @ref bindDiffuseTexture()
and @ref bindSpecularTexture() (or the combined @ref bindTextures()). The
texture is multiplied by the color, which is by default set to fully opaque
white for enabled textures. Mesh setup with a diffuse and a specular texture:

@snippet Shaders-gl.cpp PhongGL-usage-texture1

Common rendering setup:

@snippet Shaders-gl.cpp PhongGL-usage-texture2

If @ref Flag::TextureArrays is enabled, pass @ref GL::Texture2DArray instances
instead of @ref GL::Texture2D. By default layer @cpp 0 @ce is used, call
@ref setTextureLayer() to pick a different texture array layer.

@section Shaders-PhongGL-lights Light specification

By default, the shader provides a single directional "fill" light, coming from
the center of the camera. With @ref Configuration::setLightCount() you can
specify how many lights you want, and then control light parameters using
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

@snippet Shaders-gl.cpp PhongGL-usage-lights

@subsection Shaders-PhongGL-lights-ambient Ambient lights

In order to avoid redundant uniform inputs, there's no dedicated way to specify
ambient lights. Instead, they are handled by the ambient color input, as the
math for ambient color and lights is equivalent. Add the ambient colors
together and reuse the diffuse texture in the @ref bindAmbientTexture() slot to
have it affected by the ambient as well:

@snippet Shaders-gl.cpp PhongGL-usage-lights-ambient

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

@snippet Shaders-gl.cpp PhongGL-usage-alpha

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
and supply per-vertex IDs to the @ref ObjectId attribute. The object ID can be
also supplied from an integer texture bound via @ref bindObjectIdTexture() if
@ref Flag::ObjectIdTexture is enabled. The output will contain a sum of the
per-vertex ID, texture ID and ID coming from @ref setObjectId().

The functionality is practically the same as in the @ref FlatGL shader, see
@ref Shaders-FlatGL-object-id "its documentation" for more information and usage
example.

@requires_gl30 Extension @gl_extension{EXT,texture_integer}
@requires_gles30 Object ID output requires integer support in shaders, which
    is not available in OpenGL ES 2.0.
@requires_webgl20 Object ID output requires integer support in shaders, which
    is not available in WebGL 1.0.

@section Shaders-PhongGL-skinning Skinning

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
@requires_gles30 Object ID output requires integer support in shaders, which
    is not available in OpenGL ES 2.0.
@requires_webgl20 Object ID output requires integer support in shaders, which
    is not available in WebGL 1.0.

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

@snippet Shaders-gl.cpp PhongGL-usage-instancing

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

@section Shaders-PhongGL-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, the shader needs a separate
@ref ProjectionUniform3D and @ref TransformationUniform3D buffer bound with
@ref bindProjectionBuffer() and @ref bindTransformationBuffer(), respectively,
lights are supplied via a @ref PhongLightUniform buffer bound with
@ref bindLightBuffer(). To maximize use of the limited uniform buffer memory,
materials are supplied separately in a @ref PhongMaterialUniform buffer
bound with @ref bindMaterialBuffer() and then referenced via
@relativeref{PhongDrawUniform,materialId} from a @ref PhongDrawUniform bound
with @ref bindDrawBuffer(); for optional texture transformation a per-draw
@ref TextureTransformationUniform buffer bound with
@ref bindTextureTransformationBuffer() can be supplied as well. A uniform
buffer setup equivalent to the
@ref Shaders-PhongGL-colored "colored case at the top", with one default light,
would look like this:

@snippet Shaders-gl.cpp PhongGL-ubo

When uniform buffers with @ref Flag::TextureArrays are used,
@ref Flag::TextureTransformation has to be enabled as well in order to supply
the texture layer using @ref TextureTransformationUniform::layer.

For a multidraw workflow enable @ref Flag::MultiDraw (and possibly
@ref Flag::TextureArrays) and supply desired light, material and draw count via
@ref Configuration::setLightCount(),
@relativeref{Configuration,setMaterialCount()}
and @relativeref{Configuration,setDrawCount()}. For every draw then specify
material references and texture offsets/layers. With @ref Flag::LightCulling
it's also possible to perform per-draw light culling by supplying a subrange
into the @ref PhongLightUniform array using @ref PhongDrawUniform::lightOffset
and @relativeref{PhongDrawUniform,lightCount}. Besides that, the usage is
similar for all shaders, see @ref shaders-usage-multidraw for an example.

For skinning, joint matrices are supplied via a @ref TransformationUniform3D
buffer bound with @ref bindJointBuffer(). In an instanced scenario the
per-instance joint count is supplied via
@ref PhongDrawUniform::perInstanceJointCount, a per-draw joint offset for the
multidraw scenario is supplied via @ref PhongDrawUniform::jointOffset.
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

@see @ref shaders
*/
class MAGNUM_SHADERS_EXPORT PhongGL: public GL::AbstractShaderProgram {
    public:
        /* MSVC needs dllexport here as well */
        class MAGNUM_SHADERS_EXPORT Configuration;
        class CompileState;

        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}.
         */
        typedef GenericGL3D::Position Position;

        /**
         * @brief Normal direction
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}. Used only if @ref perDrawLightCount()
         * isn't @cpp 0 @ce.
         */
        typedef GenericGL3D::Normal Normal;

        /**
         * @brief Tangent direction
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}. Use either this or the @ref Tangent4
         * attribute. If only a three-component attribute is used and
         * @ref Flag::Bitangent is not enabled, it's the same as if
         * @ref Tangent4 was specified with the fourth component always being
         * @cpp 1.0f @ce. Used only if @ref Flag::NormalTexture is set and
         * @ref perDrawLightCount() isn't @cpp 0 @ce.
         * @see @ref Shaders-PhongGL-normal-mapping
         */
        typedef GenericGL3D::Tangent Tangent;

        /**
         * @brief Tangent direction with a bitangent sign
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector4}. Use either this or the @ref Tangent
         * attribute. If @ref Flag::Bitangent is set, the fourth component is
         * ignored and bitangents are taken from the @ref Bitangent attribute
         * instead. Used only if @ref Flag::NormalTexture is set and
         * @ref perDrawLightCount() isn't @cpp 0 @ce.
         * @see @ref Shaders-PhongGL-normal-mapping
         */
        typedef GenericGL3D::Tangent4 Tangent4;

        /**
         * @brief Bitangent direction
         * @m_since_latest
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector3}. Use either this or the @ref Tangent4
         * attribute. Used only if both @ref Flag::NormalTexture and
         * @ref Flag::Bitangent are set and @ref perDrawLightCount() isn't
         * @cpp 0 @ce.
         * @see @ref Shaders-PhongGL-normal-mapping
         */
        typedef GenericGL3D::Bitangent Bitangent;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2}, used only if at least one of
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
        typedef GenericGL3D::ObjectId ObjectId;
        #endif

        /**
         * @brief (Instanced) transformation matrix
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Matrix4}. Used only if
         * @ref Flag::InstancedTransformation is set.
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
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Matrix3x3}. Used only if
         * @ref Flag::InstancedTransformation is set.
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
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2}. Used only if
         * @ref Flag::InstancedTextureOffset is set.
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
             * set in @ref setObjectId() and possibly also a per-vertex ID and
             * an ID fetched from a texture, see @ref Shaders-PhongGL-object-id
             * for more information.
             * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID output requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since{2019,10}
             */
            ObjectIdOutput = GenericGL3D::ObjectIdOutput
            #endif
        };

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags(), @ref Configuration::setFlags()
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
             * Double-sided rendering. By default, lighting is applied only to
             * front-facing triangles, with back-facing triangles receiving
             * just the ambient color or being culled away. If enabled, the
             * shader will evaluate the lighting also on back-facing triangles
             * with the normal flipped. Has no effect if no lights are used.
             *
             * Rendering back-facing triangles requires
             * @ref GL::Renderer::Feature::FaceCulling to be disabled.
             * @see @ref Trade::MaterialAttribute::DoubleSided
             * @m_since_latest
             */
            DoubleSided = 1 << 20,

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
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID output requires integer support in
             *      shaders, which is not available in WebGL 1.0.
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
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID output requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedObjectId = (1 << 8)|ObjectId,

            /**
             * Object ID texture. Retrieves object IDs from a texture bound
             * with @ref bindObjectIdTexture(), outputting a sum of the object
             * ID texture, the ID coming from @ref setObjectId() or
             * @ref PhongDrawUniform::objectId and possibly also the per-vertex
             * ID, if @ref Flag::InstancedObjectId is enabled as well.
             * Implicitly enables @ref Flag::ObjectId. See
             * @ref Shaders-PhongGL-object-id for more information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0.
             * @requires_webgl20 Object ID output requires integer support in
             *      shaders, which is not available in WebGL 1.0.
             * @m_since_latest
             */
            ObjectIdTexture = (1 << 17)|ObjectId,
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
             * @see @ref Flag::ShaderStorageBuffers
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 12,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Use shader storage buffers. Superset of functionality provided
             * by @ref Flag::UniformBuffers, compared to it doesn't have any
             * size limits on @ref Configuration::setLightCount(),
             * @relativeref{Configuration,setJointCount()},
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
            ShaderStorageBuffers = UniformBuffers|(1 << 19),
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
             * @ref perDrawLightCount() lights are used for every draw. Expects
             * that @ref Flag::UniformBuffers is enabled as well.
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
            NoSpecular = 1 << 16,

            #ifndef MAGNUM_TARGET_GLES2
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
            DynamicPerVertexJointCount = 1 << 18,
            #endif
        };

        /**
         * @brief Flags
         *
         * @see @ref flags(), @ref Configuration::setFlags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Compile asynchronously
         * @m_since_latest
         *
         * Compared to @ref PhongGL(const Configuration&) can perform an
         * asynchronous compilation and linking. See @ref shaders-async for
         * more information.
         * @see @ref PhongGL(CompileState&&)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        static CompileState compile(const Configuration& configuration = Configuration{});
        #else
        /* Configuration is forward-declared */
        static CompileState compile(const Configuration& configuration);
        static CompileState compile();
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Compile asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags, UnsignedInt lightCount = 1);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Compile for a multi-draw scenario asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags, UnsignedInt lightCount, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Constructor
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit PhongGL(const Configuration& configuration = Configuration{});
        #else
        /* Configuration is forward-declared */
        explicit PhongGL(const Configuration& configuration);
        explicit PhongGL();
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since_latest Use @ref PhongGL(const Configuration&)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use PhongGL(const Configuration& instead") PhongGL(Flags flags, UnsignedInt lightCount = 1);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @m_deprecated_since_latest Use @ref PhongGL(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        explicit CORRADE_DEPRECATED("use PhongGL(const Configuration& instead") PhongGL(Flags flags, UnsignedInt lightCount, UnsignedInt materialCount, UnsignedInt drawCount);
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
        explicit PhongGL(CompileState&& state);

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

        /**
         * @brief Flags
         *
         * @see @ref Configuration::setFlags()
         */
        Flags flags() const { return _flags; }

        /**
         * @brief Light count
         *
         * If @ref Flag::UniformBuffers is not set, this is the number of
         * light properties accepted by @ref setLightPositions() /
         * @ref setLightPosition(), @ref setLightColors() /
         * @ref setLightColor(), @ref setLightSpecularColors() /
         * @ref setSpecularColor() and @ref setLightRanges() /
         * @ref setLightRange().
         *
         * If @ref Flag::UniformBuffers is set, this is the statically defined
         * size of the @ref PhongLightUniform uniform buffer bound with
         * @ref bindLightBuffer(). Has no use if @ref Flag::ShaderStorageBuffers
         * is set.
         * @see @ref perDrawLightCount(), @ref Configuration::setLightCount()
         */
        UnsignedInt lightCount() const { return _lightCount; }

        /**
         * @brief Per-draw light count
         * @m_since_latest
         *
         * Number of lights out of @ref lightCount() applied per draw. If
         * @ref Flag::LightCulling is enabled, this is only an upper bound on
         * the light count applied per draw, with the actual count supplied via
         * @ref PhongDrawUniform::lightCount. If @cpp 0 @ce, no lighting
         * calculations are performed and only the ambient contribution to the
         * color is used.
         * @see @ref Configuration::setLightCount()
         */
        UnsignedInt perDrawLightCount() const { return _perDrawLightCount; }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Joint count
         * @m_since_latest
         *
         * If @ref Flag::UniformBuffers is not set, this is the number of joint
         * matrices accepted by @ref setJointMatrices() / @ref setJointMatrix().
         * If @ref Flag::UniformBuffers is set, this is the statically defined
         * size of the @ref TransformationUniform3D uniform buffer bound with
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
         * Statically defined size of the @ref PhongMaterialUniform uniform
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
         * Statically defined size of each of the @ref TransformationUniform3D,
         * @ref PhongDrawUniform and @ref TextureTransformationUniform uniform
         * buffers bound with @ref bindTransformationBuffer(),
         * @ref bindDrawBuffer() and @ref bindTextureTransformationBuffer().
         * Has use only if @ref Flag::UniformBuffers is set and
         * @ref Flag::ShaderStorageBuffers is not set.
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
         * a value in @ref PhongDrawUniform. Initial value is the same as
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
        PhongGL& setPerVertexJointCount(UnsignedInt count, UnsignedInt secondaryCount = 0);
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
         * the texture. If @ref perDrawLightCount() is zero, this function is a
         * no-op, as diffuse color doesn't contribute to the output in that
         * case. If @ref Flag::VertexColor is set, the color is multiplied with
         * a color coming from the @ref Color3 / @ref Color4 attribute.
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
         * enabled. If @ref perDrawLightCount() is zero, this function is a
         * no-op, as normals don't contribute to the output in that case.
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
         * @ref perDrawLightCount() is zero, this function is a no-op, as
         * specular color doesn't contribute to the output in that case.
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
         * Initial value is @cpp 80.0f @ce. If @ref perDrawLightCount() is
         * zero, this function is a no-op, as specular color doesn't contribute
         * to the output in that case.
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
         * @ref Shaders-PhongGL-object-id for more information. Initial value
         * is @cpp 0 @ce. If @ref Flag::InstancedObjectId and/or
         * @ref Flag::ObjectIdTexture is enabled as well, this value is added
         * to the ID coming from the @ref ObjectId attribute and/or the
         * texture.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref PhongDrawUniform::objectId and call @ref bindDrawBuffer()
         * instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0.
         * @requires_webgl20 Object ID output requires integer support in
         *      shaders, which is not available in WebGL 1.0.
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
         * value is an identity matrix. If @ref perDrawLightCount() is zero,
         * this function is a no-op, as normals don't contribute to the output
         * in that case. If @ref Flag::InstancedTransformation is set, the
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

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set joint matrices
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Initial values are identity transformations. Expects that the size
         * of the @p matrices array is the same as @ref jointCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform3D::transformationMatrix and call
         * @ref bindJointBuffer() instead.
         * @see @ref setJointMatrix(UnsignedInt, const Matrix4&)
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        PhongGL& setJointMatrices(Containers::ArrayView<const Matrix4> matrices);

        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& setJointMatrices(std::initializer_list<Matrix4> matrices);

        /**
         * @brief Set joint matrix for given joint
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Unlike @ref setJointMatrices() updates just a single joint matrix.
         * Expects that @p id is less than @ref jointCount().
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationUniform3D::transformationMatrix and call
         * @ref bindJointBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        PhongGL& setJointMatrix(UnsignedInt id, const Matrix4& matrix);

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
         * @ref PhongDrawUniform::perInstanceJointCount and call
         * @ref bindDrawBuffer() instead.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Skinning requires integer support in shaders, which
         *      is not available in OpenGL ES 2.0.
         * @requires_webgl20 Skinning requires integer support in shaders,
         *      which is not available in WebGL 1.0.
         */
        PhongGL& setPerInstanceJointCount(UnsignedInt count);
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
         * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        PhongGL& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Bind a projection uniform / shader storage buffer
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
         * @brief Bind a transformation uniform / shader storage buffer
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
         * @brief Bind a draw uniform / shader storage buffer
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
        PhongGL& bindTextureTransformationBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a material uniform / shader storage buffer
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
         * @brief Bind a light uniform / shader storage buffer
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
         * @brief Bind a joint matrix uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref jointCount() instances of
         * @ref TransformationUniform3D.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        PhongGL& bindJointBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        PhongGL& bindJointBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

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
         * @ref perDrawLightCount() is zero, this function is a no-op, as
         * diffuse color doesn't contribute to the output in that case.
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
         * If @ref perDrawLightCount() is zero, this function is a no-op, as
         * diffuse color doesn't contribute to the output in that case.
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
         * @ref perDrawLightCount() is zero, this function is a no-op, as
         * specular color doesn't contribute to the output in that case.
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
         * @ref perDrawLightCount() is zero, this function is a no-op, as
         * specular color doesn't contribute to the output in that case.
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
         * @ref perDrawLightCount() is zero, this function is a no-op, as
         * normals don't contribute to the output in that case.
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
         * the @ref Tangent attribute was supplied. If @ref perDrawLightCount()
         * is zero, this function is a no-op, as normals don't contribute to
         * the output in that case.
         * @see @ref Shaders-PhongGL-normal-mapping,
         *      @ref setNormalTextureScale()
         */
        PhongGL& bindNormalTexture(GL::Texture2DArray& texture);
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
        PhongGL& bindObjectIdTexture(GL::Texture2D& texture);

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
        PhongGL& bindObjectIdTexture(GL::Texture2DArray& texture);
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

        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(PhongGL)

    private:
        /* Creates the GL shader program object but does nothing else.
           Internal, used by compile(). */
        explicit PhongGL(NoInitT) {}

        Flags _flags;
        UnsignedInt _lightCount{},
            _perDrawLightCount{};
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _jointCount{},
            _perVertexJointCount{},
            _secondaryPerVertexJointCount{},
            _materialCount{},
            _drawCount{};
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
        Int _jointMatricesUniform, /* 12 + 4*lightCount */
            _perInstanceJointCountUniform, /* 12 + 4*lightCount + jointCount */
            /* Used instead of all other uniforms when Flag::UniformBuffers is
               set, so it can alias them */
            _drawOffsetUniform{0},
            /* 13 + 4*lightCount + jointCount, or 1 with UBOs */
            _perVertexJointCountUniform;
        #endif
};

/**
@brief Configuration
@m_since_latest

@see @ref PhongGL(const Configuration&), @ref compile(const Configuration&)
*/
class MAGNUM_SHADERS_EXPORT PhongGL::Configuration {
    public:
        explicit Configuration() = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set flags
         *
         * No flags are set by default.
         * @see @ref PhongGL::flags()
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        /** @brief Light count */
        UnsignedInt lightCount() const { return _lightCount; }

        /** @brief Per-draw light count */
        UnsignedInt perDrawLightCount() const { return _perDrawLightCount; }

        /**
         * @brief Set light count
         *
         * If @ref Flag::UniformBuffers isn't set, @p count corresponds to the
         * range / array size accepted by @ref setLightPosition() /
         * @ref setLightPositions(), @ref setLightColor() /
         * @ref setLightColors(), @ref setLightSpecularColor() /
         * @ref setLightSpecularColors() and @ref setLightRange() /
         * @ref setLightRanges().
         *
         * If @ref Flag::UniformBuffers is set, @p count describes size of a
         * @ref PhongLightUniform buffer bound with @ref bindLightBuffer().
         * Uniform buffers have a statically defined size and
         * @cpp count*sizeof(PhongLightUniform) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-draw lights are specified
         * via @ref PhongDrawUniform::lightOffset and
         * @ref PhongDrawUniform::lightCount.
         *
         * The @p perDrawCount parameter describes how many lights out of
         * @p count get applied to each draw. Useful mainly in combination with
         * @ref Flag::LightCulling, without it can be used for conveniently
         * reducing the light count without having to reduce sizes of the light
         * arrays as well. Unless @ref Flag::ShaderStorageBuffers is set,
         * @p perDrawCount is expected to not be larger than @p count. If set
         * to @cpp 0 @ce, no lighting calculations are performed and only the
         * ambient contribution to the color is used. If @p perDrawCount is
         * @cpp 0 @ce, @p count is expected to be zero as well.
         *
         * Default value is @cpp 1 @ce for both.
         * @see @ref setFlags(), @ref setMaterialCount(), @ref setDrawCount(),
         *      @ref PhongGL::lightCount(), @ref PhongGL::perDrawLightCount()
         */
        Configuration& setLightCount(UnsignedInt count, UnsignedInt perDrawCount);

        /**
         * @brief Set light count
         *
         * Same as calling @ref setLightCount(UnsignedInt, UnsignedInt) with
         * both parameters set to @p count.
         */
        Configuration& setLightCount(UnsignedInt count) {
            return setLightCount(count, count);
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
         * bound on how many joint matrices get supplied to each draw with
         * @ref setJointMatrices() / @ref setJointMatrix().
         *
         * If @ref Flag::UniformBuffers is set, @p count describes size of a
         * @ref TransformationUniform3D buffer bound with
         * @ref bindJointBuffer(). Uniform buffers have a statically defined
         * size and @cpp count*sizeof(TransformationUniform3D) @ce has to be
         * within @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-vertex joints index into
         * the array offset by @ref PhongDrawUniform::jointOffset.
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
         * @see @ref PhongGL::jointCount(), @ref PhongGL::perVertexJointCount(),
         *      @ref PhongGL::secondaryPerVertexJointCount(),
         *      @ref Flag::DynamicPerVertexJointCount,
         *      @ref PhongGL::setPerVertexJointCount()
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
         * @ref PhongMaterialUniform buffer bound with
         * @ref bindMaterialBuffer(). Uniform buffers have a statically defined
         * size and @cpp count*sizeof(PhongMaterialUniform) @ce has to be
         * within @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-draw materials are
         * specified via @ref PhongDrawUniform::materialId. Default value is
         * @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setLightCount(), @ref setDrawCount(),
         *      @ref PhongGL::materialCount()
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
         * @ref TransformationUniform3D / @ref PhongDrawUniform /
         * @ref TextureTransformationUniform buffer bound with
         * @ref bindTransformationBuffer(), @ref bindDrawBuffer() and
         * @ref bindTextureTransformationBuffer(). Uniform buffers have a
         * statically defined size and the maximum of
         * @cpp count*sizeof(TransformationUniform3D) @ce,
         * @cpp count*sizeof(PhongDrawUniform) @ce and
         * @cpp count*sizeof(TextureTransformationUniform) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffers are
         * unbounded and @p count is ignored. The draw offset is set via
         * @ref setDrawOffset(). Default value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setLightCount(), @ref setMaterialCount(),
         *      @ref PhongGL::drawCount()
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
        UnsignedInt _lightCount = 1,
            _perDrawLightCount = 1;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt
            _jointCount = 0,
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
class PhongGL::CompileState: public PhongGL {
    /* Everything deliberately private except for the inheritance */
    friend class PhongGL;

    explicit CompileState(NoCreateT): PhongGL{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

    explicit CompileState(PhongGL&& shader, GL::Shader&& vert, GL::Shader&& frag
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , GL::Version version
        #endif
    ): PhongGL{Utility::move(shader)}, _vert{Utility::move(vert)}, _frag{Utility::move(frag)}
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , _version{version}
        #endif
        {}

    Implementation::GLShaderWrapper _vert, _frag;
    #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    GL::Version _version;
    #endif
};

/** @debugoperatorclassenum{PhongGL,PhongGL::Flag} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, PhongGL::Flag value);

/** @debugoperatorclassenum{PhongGL,PhongGL::Flags} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, PhongGL::Flags value);

CORRADE_ENUMSET_OPERATORS(PhongGL::Flags)

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
