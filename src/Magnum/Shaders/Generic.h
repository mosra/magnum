#ifndef Magnum_Shaders_GenericShader_h
#define Magnum_Shaders_GenericShader_h
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

/** @file
 * @brief Struct @ref Magnum::Shaders::Generic, typedef @ref Magnum::Shaders::Generic2D, @ref Magnum::Shaders::Generic3D
 */

#include "Magnum/GL/Attribute.h"

namespace Magnum { namespace Shaders {

/**
@brief Generic shader definition

Definitions common for majority of shaders in the @ref Shaders namespace,
allowing mesh or a framebuffer configured for a generic shader to be used with
any of them. See @ref shaders-generic for more information.

@section Shaders-Generic-allocation Attribute allocation

The attribute locations are allocated like shown below, with various tradeoffs
as GPUs commonly support only 16 attribtes at most, while the mandated minimum
on OpenGL ES2 and WebGL 1 being only 8. Some locations are only reserved for
future use, with no attribute definition implemented yet.

@m_class{m-row m-container-inflate}

@parblock

@m_class{m-fullwidth}

<table>
<tr>
<th>\#</th>
<th>Attribute</th>
<th>Alternative</th>
<th>Alternative 2</th>
</tr>
<tr>
<td>0</td>
<td colspan="3">
@ref Position
</td>
</tr>
<tr>
<td>1</td>
<td colspan="3">
@ref TextureCoordinates

* *Reserved* --- third component for a layer
</td>
</tr>
<tr>
<td>2</td>
<td colspan="3">
@ref Color3 / @ref Color4 (per-vertex or instanced)
</td>
</tr>
<tr>
<td>3</td>
<td>
@ref Tangent / @ref Tangent4
</td>
<td>
@ref Tangent
</td>
<td>
* *Reserved* --- TBN as a @ref Magnum::Quaternion "Quaternion"
</td>
</tr>
<tr>
<td>4</td>
<td>
@ref ObjectId (instanced)

* *Reserved* --- additional components could \n
represent material ID and other indices, which \n
could then be used to fetch additional \n
per-instance properties that wouldn't fit into \n
vertex attributes.
</td>
<td colspan="2">
@ref Bitangent

Provided only as a convenience for models that \n
don't encode bitangent orientation in the last \n
component of @ref Tangent4. If a model needs \n
both bitangents and object ID for instancing, \n
@ref Tangent4 has to be used.
</td>
</tr>
<tr>
<td>5</td>
<td colspan="3">
@ref Normal
</td>
</tr>
<tr>
<td>6</td>
<td colspan="3">
* *Reserved* --- vertex weights
</td>
</tr>
<tr>
<td>7</td>
<td colspan="3">
* *Reserved* --- bone indices
</td>
</tr>
<tr>
<td>8</td>
<td rowspan="4">
@ref TransformationMatrix (instanced)
</td>
<td rowspan="2" colspan="2">
* *Reserved* --- instanced @ref Magnum::DualQuaternion "DualQuaternion" \n transformation for positions and normals
</td>
</tr>
<tr>
<td>9</td>
</tr>
<tr>
<td>10</td>
<td colspan="2">
* *Reserved* --- 2nd vertex weights
</td>
</tr>
<tr>
<td>11</td>
<td colspan="2">
* *Reserved* --- 2nd bone indices
</td>
</tr>
<tr>
<td>12</td>
<td rowspan="3">
@ref NormalMatrix (instanced)
</td>
<td colspan="2">
* *Reserved* --- instanced scale for positions
</td>
</tr>
<tr>
<td>13</td>
<td rowspan="2">
* *Reserved* --- instanced texture \n
rotation and scale
</td>
<td>
* *Reserved* --- 2nd vertex colors
</td>
</tr>
<tr>
<td>14</td>
<td>
* *Reserved* --- 3rd texture coords
</td>
</tr>
<tr>
<td>15</td>
<td>
@ref TextureOffset (instanced)

* *Reserved* --- third component for a layer
</td>
<td>
* *Reserved* --- a single component \n
representing instanced texture layer \n
index, UVs being the same always
</td>
<td>
* *Reserved* --- 2nd texture coords
</td>
</tr>
</table>

@endparblock

The three alternative allocations can be mixed freely as long as the locations
don't conflict --- so it's possible to have for example a mesh with two sets of
texture coordinates, weights and colors, as each of those occupies a different
attribute range; but instancing then has to be done using smaller types as full
matrices would occupy the locations used by the secondary sets. Additional
guarantees of the above:

-   @ref Tangent, @ref Bitangent and @ref Normal is in consecutive locations to
    allow those being passed as a single TBN @ref Magnum::Matrix3x3 "Matrix3x3"
    attribute as well.
-   @ref Normal and TBN represented as a quaternion use different locations in
    order to allow a mesh to contain both
-   Similarly, texture rotation/scale and offset is in consecutive locations to
    allow passing a single @ref Matrix3 attribute there.
-   Tnstanced texture transformation is available if and only if there's
    exactly one set of texture coordinates (as the additional sets would need
    additional transformations as well).

@section Shaders-Generic-custom Generic attributes and custom shaders

Note that while custom shaders don't *have to* follow the above, it's
recommended to so. If the custom shader diverges from predefined locations of
common attributes, meshes configured for the builtin shaders (for example with
@ref MeshTools::compile()) won't work with it and the mesh attribute
configuration has to be done manually. It also becomes impossible to render a
mesh configured for a custom shader with for example @ref MeshVisualizer.

If you're using @ref GL::AbstractShaderProgram::bindAttributeLocation(), it's
rather easy, as you can simply use the @ref GL::Attribute::Location of given
attribute:

@snippet MagnumShaders.cpp Generic-custom-bind

For attribute location defined directly in shader code (which is the
recommended way unless you need compatibility with WebGL 1.0 and OpenGL ES
2.0), the attribute locations can be propagated using a preprocessor define.
For example:

@snippet MagnumShaders.cpp Generic-custom-preprocessor

Then, the attribute definition in a shader will look like this:

@code{.glsl}
layout(location = POSITION_ATTRIBUTE_LOCATION) in vec3 position;
layout(location = NORMAL_ATTRIBUTE_LOCATION) in vec3 normal;
@endcode

@see @ref shaders, @ref Generic2D, @ref Generic3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt> struct Generic;
#else
template<UnsignedInt dimensions> struct Generic {
    /* Keep consistent with generic.glsl and the real definitions below */

    enum: UnsignedInt {
        /**
         * Color shader output. Present always, expects three- or
         * four-component floating-point or normalized buffer attachment.
        */
        ColorOutput = 0,

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * Object ID shader output. Expects a single-component unsigned
         * integral attachment.
         * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0 or WebGL 1.0.
         */
        ObjectIdOutput = 1
        #endif
    };

    /**
     * @brief Vertex position
     *
     * @ref Magnum::Vector2 "Vector2" in 2D and @ref Magnum::Vector3 "Vector3"
     * in 3D. Corresponds to @ref Trade::MeshAttribute::Position.
     */
    typedef GL::Attribute<0, T> Position;

    /**
     * @brief 2D texture coordinates
     *
     * @ref Magnum::Vector2 "Vector2". Corresponds to
     * @ref Trade::MeshAttribute::TextureCoordinates.
     */
    typedef GL::Attribute<1, Vector2> TextureCoordinates;

    /**
     * @brief Three-component vertex color
     *
     * @ref Magnum::Color3. Use either this or the @ref Color4 attribute.
     * Corresponds to @ref Trade::MeshAttribute::Color.
     */
    typedef GL::Attribute<2, Magnum::Color3> Color3;

    /**
     * @brief Four-component vertex color
     *
     * @ref Magnum::Color4. Use either this or the @ref Color3 attribute.
     * Corresponds to @ref Trade::MeshAttribute::Color.
     */
    typedef GL::Attribute<2, Magnum::Color4> Color4;

    /**
     * @brief Vertex tangent
     * @m_since{2019,10}
     *
     * @ref Magnum::Vector3 "Vector3", defined only in 3D. Use either this or
     * the @ref Tangent4 attribute. Corresponds to
     * @ref Trade::MeshAttribute::Tangent.
     */
    typedef GL::Attribute<3, Vector3> Tangent;

    /**
     * @brief Vertex tangent with a bitangent sign
     * @m_since{2020,06}
     *
     * @ref Magnum::Vector4 "Vector4", defined only in 3D. The last component
     * is a sign value (@cpp -1.0f @ce or @cpp +1.0f @ce) defining handedness
     * of the tangent basis. Reconstructing the @ref Bitangent attribute can be
     * then done like this:
     *
     * @snippet MagnumTrade.cpp MeshAttribute-bitangent-from-tangent
     *
     * Use either this or the @ref Tangent attribute. Corresponds to
     * @ref Trade::MeshAttribute::Tangent.
     */
    typedef GL::Attribute<3, Vector4> Tangent4;

    /**
     * @brief Vertex bitangent
     * @m_since{2020,06}
     *
     * @ref Magnum::Vector3 "Vector3", defined only in 3D. For better storage
     * efficiency, the bitangent can be also reconstructed from the normal and
     * tangent, see @ref Tangent4 for more information. Corresponds to
     * @ref Trade::MeshAttribute::Bitangent.
     *
     * This attribute conflicts with @ref ObjectId, if you want to use both
     * instanced object ID and bitangents, you need to reconstruct them from
     * @ref Tangent4 instead.
     */
    typedef GL::Attribute<4, Vector3> Bitangent;

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * @brief (Instanced) object ID
     * @m_since{2020,06}
     *
     * @ref Magnum::UnsignedInt "UnsignedInt". Corresponds to
     * @ref Trade::MeshAttribute::ObjectId.
     *
     * This attribute conflicts with @ref Bitangent, if you want to use both
     * instanced object ID and bitangents, you need to reconstruct them from
     * @ref Tangent4 instead.
     * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
     * @requires_gles30 Object ID output requires integer support in shaders,
     *      which is not available in OpenGL ES 2.0 or WebGL 1.0.
     */
    typedef GL::Attribute<4, UnsignedInt> ObjectId;
    #endif

    /* Index 4 also used by MeshVisualizer::VertexIndex (reusing ObjectId). Not
       making it generic yet, as its use case is limited to a single shader,
       and even there it's just a fallback for platforms w/o gl_VertexID. */

    /**
     * @brief Vertex normal
     *
     * @ref Magnum::Vector3 "Vector3", defined only in 3D. Corresponds to
     * @ref Trade::MeshAttribute::Normal.
     */
    typedef GL::Attribute<5, Vector3> Normal;

    /* 6, 7 reserved for vertex weights / bone IDs */

    /**
     * @brief (Instanced) transformation matrix
     * @m_since{2020,06}
     *
     * @ref Magnum::Matrix3 "Matrix3" in 2D and @ref Magnum::Matrix4 "Matrix4"
     * in 3D. Currently doesn't have a corresponding @ref Trade::MeshAttribute.
     * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
     * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
     *      @gl_extension{EXT,instanced_arrays} or
     *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
     *      in WebGL 1.0.
     */
    typedef GL::Attribute<8, T> TransformationMatrix;

    /* 9, 10, 11 occupied by TransformationMatrix */

    /**
     * @brief (Instanced) normal matrix
     * @m_since{2020,06}
     *
     * @ref Magnum::Matrix3 "Matrix3x3", defined only in 3D. Currently doesn't
     * have a corresponding @ref Trade::MeshAttribute.
     * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
     * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
     *      @gl_extension{EXT,instanced_arrays} or
     *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
     *      in WebGL 1.0.
     */
    typedef GL::Attribute<12, Matrix3x3> NormalMatrix;

    /* 13, 14 occupied by NormalMatrix */

    /**
     * @brief (Instanced) texture offset
     * @m_since{2020,06}
     *
     * @ref Magnum::Vector2 "Vector2". Currently doesn't have a corresponding
     * @ref Trade::MeshAttribute.
     * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
     * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
     *      @gl_extension{EXT,instanced_arrays} or
     *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
     *      in WebGL 1.0.
     */
    typedef GL::Attribute<15, Vector2> TextureOffset;
};
#endif

/** @brief Generic 2D shader definition */
typedef Generic<2> Generic2D;

/** @brief Generic 3D shader definition */
typedef Generic<3> Generic3D;

#ifndef DOXYGEN_GENERATING_OUTPUT
struct BaseGeneric {
    enum: UnsignedInt {
        ColorOutput = 0,
        #ifndef MAGNUM_TARGET_GLES2
        ObjectIdOutput = 1
        #endif
    };

    typedef GL::Attribute<1, Vector2> TextureCoordinates;
    typedef GL::Attribute<2, Magnum::Color3> Color3;
    typedef GL::Attribute<2, Magnum::Color4> Color4;
    #ifndef MAGNUM_TARGET_GLES2
    typedef GL::Attribute<4, UnsignedInt> ObjectId;
    #endif

    typedef GL::Attribute<15, Vector2> TextureOffset;
};

template<> struct Generic<2>: BaseGeneric {
    typedef GL::Attribute<0, Vector2> Position;
    /* 1, 2 used by TextureCoordinates and Color */

    typedef GL::Attribute<8, Matrix3> TransformationMatrix;
    /* 9, 10 occupied by TransformationMatrix */
    /* 15 used by TextureOffset */
};

template<> struct Generic<3>: BaseGeneric {
    typedef GL::Attribute<0, Vector3> Position;
    /* 1, 2 used by TextureCoordinates and Color */
    typedef GL::Attribute<3, Vector3> Tangent;
    typedef GL::Attribute<3, Vector4> Tangent4;
    typedef GL::Attribute<4, Vector3> Bitangent; /* also ObjectId */
    typedef GL::Attribute<5, Vector3> Normal;
    /* 6, 7 reserved for vertex weights / bone IDs */

    typedef GL::Attribute<8, Matrix4> TransformationMatrix;
    /* 9, 10, 11 occupied by TransformationMatrix */
    typedef GL::Attribute<12, Matrix3x3> NormalMatrix;
    /* 13, 14 occupied by NormalMatrix */
    /* 15 used by TextureOffset */
};
#endif

}}

#endif
