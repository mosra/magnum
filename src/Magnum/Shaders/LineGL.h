#ifndef Magnum_Shaders_LineGL_h
#define Magnum_Shaders_LineGL_h
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

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2)
/** @file
 * @brief Class @ref Magnum::Shaders::LineGL, typedef @ref Magnum::Shaders::LineGL2D, @ref Magnum::Shaders::LineGL3D
 * @m_since_latest
 */
#endif

#include "Magnum/configure.h"

#if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_GLES2)
#include <Corrade/Utility/Move.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/glShaderWrapper.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class LineGLFlag: UnsignedShort {
        VertexColor = 1 << 0,
        ObjectId = 1 << 1,
        InstancedObjectId = (1 << 2)|ObjectId,
        InstancedTransformation = 1 << 3,
        UniformBuffers = 1 << 4,
        #ifndef MAGNUM_TARGET_WEBGL
        ShaderStorageBuffers = UniformBuffers|(1 << 6),
        #endif
        MultiDraw = UniformBuffers|(1 << 5)
    };
    typedef Containers::EnumSet<LineGLFlag> LineGLFlags;
    CORRADE_ENUMSET_OPERATORS(LineGLFlags)
}

/**
@brief Line GL shader
@m_since_latest

Renders lines expanded to quads in screen space. Compared to builtin GPU line
rendering, the lines can be of arbitrary width, with configurable join and cap
styles, and antialiased independently of MSAA being used or not.

@image html shaders-line.png width=256px

@experimental

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
@requires_gles30 Requires integer support in shaders which is not available in
    OpenGL ES 2.0.
@requires_webgl20 Requires integer support in shaders which is not available in
    WebGL 1.0.

@section Shaders-LineGL-usage Usage

The shader doesn't work with @ref MeshPrimitive::Lines,
@ref MeshPrimitive::LineStrip or @ref MeshPrimitive::LineLoop directly, as that
would only be implementable with a relatively expensive geometry shader.
Instead, it requires the input data to be organized in quads, with
@ref Position, @ref PreviousPosition and @ref NextPosition attributes
describing point wíth their surroundings, and @ref Annotation with
point-specific annotation such as whether given point is a line cap or a join
with neighboring segment. The data layout is described in detail in
@ref Shaders-LineGL-mesh-representation below, however in practice it's easiest
to convert an existing line @ref Trade::MeshData to a form accepted by this
shader with @ref MeshTools::generateLines() and then compile it to a
@ref GL::Mesh with @ref MeshTools::compileLines():

@snippet Shaders-gl.cpp LineGL-usage

For rendering use @ref setTransformationProjectionMatrix(),
@ref setColor(), @ref setWidth() and others. It's important to pass viewport
size in @ref setViewportSize() as the line width is interpreted relative to it.

@snippet Shaders-gl.cpp LineGL-usage2

@subsection Shaders-LineGL-usage-triangulation Line triangulation

Each line segment is rendered as a quad consisting of two triangles. Standalone
segments have cap style configurable via @ref Configuration::setCapStyle() with
the following styles, segments of zero length can be also used to render
points. The @m_span{m-label m-default} white @m_endspan lines show generated
triangles, the @m_span{m-label m-info} blue @m_endspan line is the actually
visible edge between foreground and background:

@m_class{m-row}

@parblock

@m_div{m-col-l-6  m-text-center}
@htmlinclude line-cap-butt.svg
@ref LineCapStyle::Butt
@m_enddiv

@m_div{m-col-l-6 m-text-center}
@htmlinclude line-cap-square.svg
@ref LineCapStyle::Square
@m_enddiv

@m_div{m-col-l-6 m-text-center}
@htmlinclude line-cap-round.svg
@ref LineCapStyle::Round
@m_enddiv

@m_div{m-col-l-6 m-text-center}
@htmlinclude line-cap-triangle.svg
@ref LineCapStyle::Triangle
@m_enddiv

@endparblock

Joins between consecutive segments in contiguous line strips are expanded to
form a gap-less mesh without overlaps. Depending on join style picked in
@ref Configuration::setJoinStyle() and angle between the segments the area
between points `A`, `B` and `C` may be filled with another triangle:

@m_class{m-row}

@parblock

@m_div{m-col-l-6  m-text-center}
@htmlinclude line-join-miter.svg
@ref LineJoinStyle::Miter
@m_enddiv

@m_div{m-col-l-6 m-text-center}
@htmlinclude line-join-bevel.svg
@ref LineJoinStyle::Bevel
@m_enddiv

@endparblock

@subsection Shaders-LineGL-usage-antialiasing Antialiasing

The lines aren't smoothed out by default, use @ref setSmoothness() to pick a
tradeoff between the line being aliased and blurry. This is implemented by
interpolating between the foreground color and the background, which assumes
blending is set up for pre-multiplied alpha. If you're drawing lines on a
single-color background, you can @ref setBackgroundColor() to a color matching
the background and keep blending disabled, but note that you may get artifacts
if the lines are self-overlapping.

@snippet Shaders-gl.cpp LineGL-usage-antialiasing

@subsection Shaders-LineGL-usage-3d Lines in 3D

The 3D variant of this shader renders the geometry with depth values derived
from the original line endpoints, however without any perspective shortening
applied --- the line width is the same viewport-relative value independently of
the depth the point is at.

@section Shaders-LineGL-object-id Object ID output

The shader supports writing object ID to the framebuffer for object picking or
other annotation purposes. Enable it using @ref Flag::ObjectId and set up an
integer buffer attached to the @ref ObjectIdOutput attachment. If you have a
batch of meshes with different object IDs, enable @ref Flag::InstancedObjectId
and supply per-vertex IDs to the @ref ObjectId attribute. The output will
contain a sum of the per-vertex ID and ID coming from @ref setObjectId().

The functionality is practically the same as in the @ref FlatGL shader, see
@ref Shaders-FlatGL-object-id "its documentation" for more information and usage
example.

Note that the object ID is emitted for the whole triangle area, including
transparent areas of caps when using @ref LineCapStyle::Round or
@ref LineCapStyle::Triangle as well as semi-transparent edges with smoothness
values larger than zero. In particular, the object ID output will be aliased
even if the color output isn't.

@section Shaders-LineGL-instancing Instanced rendering

Enabling @ref Flag::InstancedTransformation will turn the shader into an
instanced one. It'll take per-instance transformation from the
@ref TransformationMatrix attribute, applying it before the matrix set by
@ref setTransformationProjectionMatrix(). Besides that, @ref Flag::VertexColor
(and the @ref Color3 / @ref Color4) attributes can work as both per-vertex and
per-instance. The functionality is practically the same as in the @ref FlatGL
shader, see @ref Shaders-FlatGL-instancing "its documentation" for more
information and usage example.

@requires_gl33 Extension @gl_extension{ARB,instanced_arrays}

@section Shaders-LineGL-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, because the shader doesn't need a separate
projection and transformation matrix, a combined one is supplied via a
@ref TransformationProjectionUniform2D / @ref TransformationProjectionUniform3D
buffer bound with @ref bindTransformationProjectionBuffer(). To maximize use of the limited uniform buffer memory, materials are supplied separately in a
@ref LineMaterialUniform buffer bound with @ref bindMaterialBuffer() and then
referenced via @relativeref{LineDrawUniform,materialId} from a
@ref LineDrawUniform bound with @ref bindDrawBuffer(). A uniform buffer setup
equivalent to the @ref Shaders-LineGL-usage "snippet at the top" would look
like this --- note that @ref setViewportSize() is an immediate uniform
here as well, as it's assumed to be set globally and rarely changed:

@snippet Shaders-gl.cpp LineGL-ubo

For a multidraw workflow enable @ref Flag::MultiDraw and supply desired
material and draw count via @ref Configuration::setMaterialCount() and
@relativeref{Configuration,setDrawCount()}. For every draw then specify
material references. Besides that, the usage is similar for all shaders, see
@ref shaders-usage-multidraw for an example.

@requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object} for uniform
    buffers.
@requires_gl46 Extension @gl_extension{ARB,shader_draw_parameters} for
    multidraw.
@requires_es_extension Extension @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
    (unlisted) for multidraw.
@requires_webgl_extension Extension @webgl_extension{ANGLE,multi_draw} for
    multidraw.

@section Shaders-LineGL-mesh-representation Line mesh representation

In order to avoid performing expensive CPU-side expansion of the quads every
time the transformation, line width and other parameters change, the shader
gets just the original line segment endpoints as an input, transforms them in
2D or 3D as usual, and then expands them in screen space for a desired line
width.

@htmlinclude line-quad-expansion.svg

Ignoring all complexity related to line caps and joins for now, an example
expansion of three line segments into quads is shown above --- the first two
segments form a join at the @m_span{m-label m-info} blue @m_endspan point, the
third segment is standalone. In order to form a quad, each of the points has
to be present twice in the vertex stream, with first copy expanding up and
second copy expanding down. The @ref Position data needed to render quads would
then look like below, color-coded to match the above, and in order following
the segment direction. An index buffer would then form two triangles out of
every four points --- @cpp {0, 1, 2, 2, 1, 3, …} @ce.

@htmlinclude line-quad-data.svg

To figure out the direction in which to expand, for given endpoint position the
shader needs also *screen-space direction* to the other endpoint. But since a
2D / 3D transformation has to be applied for both endpoints before calculating
their screen-space position, it makes more sense to supply directly its
position instead, and calculate the direction only after transforming both
points. The input data would then look like this, with "previous" positions
shown above and "next" positions shown below:

@htmlinclude line-quad-data-other.svg

With line joins and caps present, the quad expansion changes in the following
way. In the general case, to avoid overlapping geometry and gaps, points `B`
and `D` collapse to a single position and the area in between is filled with an
extra triangle. Depending on the transformation, it can however also happen
that `A` and `C` collapse into a single point instead, for example if the
@m_span{m-label m-primary} azure @m_endspan point would appear above the
@m_span{m-label m-success} green @m_endspan one instead of below. Thus the
index buffer needs to handle both cases --- @cpp {…, 2, 3, 4, 4, 3, 5, …} @ce
--- and one of them always denegerates to a zero-area triangle.

@htmlinclude line-quad-expansion-joins-caps.svg

To handle the join, the shader needs to know whether there's a neighboring line
segment to join with, and what is the position of its other endpoint. Thus,
every vertex gets *two* neighboring positions, a @ref PreviousPosition and a
@ref NextPosition. Both of them are filled only in case the point forms a line
join; if the point is a line cap, one of them is left unspecified.

@htmlinclude line-quad-data-neighbor.svg

What's left is giving the shader an ability to distinguish the direction in
which to expand the point (@ref LineVertexAnnotation::Up or downwards), whether
it's a @relativeref{LineVertexAnnotation,Join} or a cap and whether the point
is a @relativeref{LineVertexAnnotation,Begin} or an end of the segment in order
to know what the neigboring positions represent. This info is stored in the
@ref Annotation attribute and shown with `U`, `J` and `B` letters above. In
this particular case the info could also be inferred from the vertex index and
for example NaNs in the neigbor positions, but a dedicated attribute makes it
more flexible for optimized data layouts explained below.

@subsection Shaders-LineGL-data-representation-overlap Overlapping layouts with less data redundancy

Assuming a 3D line mesh with floating-point position attributes, the
@ref Annotation attribute packed into a single byte and
@ref MeshIndexType::UnsignedShort indices, a a single contiguous line strip
consisting of @f$ n @f$ line segments would need
@f$ (4(36 + 1) + 24)n = 172n @f$ bytes of data. In comparison,
CPU-side-generated indexed quads would need just
@f$ (24 + 18)n + 24 = 42n + 24 @f$ bytes, and a (non-indexed)
@ref MeshPrimitive::LineStrip only @f$ 12n + 12 @f$ bytes, which is ~14x less.
Fortunately, the position data can be organized in a way that makes it possible
to reuse them for previous and next positions as well, by binding the same data
again under an offset.

There's the following possibilites, each with different tradeoffs depending on
the use case. Such data layout variants require no special-casing in the
shader, only a different mesh setup, making it possible to pick the best option
for each line mesh without having to pay for expensive shader switching.

@subsubsection Shaders-LineGL-data-representation-overlap-nojoin Standalone line segments without joins

If the mesh consists just of loose line segments and no joints need to be
drawn, the @ref Position attribute can be bound with an offset of @cpp -2 @ce
elements to the @ref PreviousPosition and @cpp +2 @ce elements to the
@ref NextPosition. To avoid out-of-bound reads, the position buffer needs to be
padded with two elements at the front and at the end. Together with no indices
needed for joint triangles the memory requirement would be reduced to
@f$ (4(12 + 1) + 12)n + 12 = 64 n + 12 @f$ bytes, which is roughly the same
amount of data as for loose CPU-side-generated indexed quads, and ~2.7x as much
as @f$ 24n @f$ bytes a sufficiently large (non-indexed)
@ref MeshPrimitive::Lines would need.

@m_div{m-row m-container-inflate}
@m_div{m-col-l-12 m-nopady m-nopadx}
@htmlinclude line-quad-data-overlap-nojoin.svg
@m_enddiv
@m_enddiv

@subsubsection Shaders-LineGL-data-representation-overlap-generic Generic lines

For arbitrary lines that consist of both joined strips and standalone segments
and the joins can be of any style in any direction, the @ref Position attribute
has to be additionally padded with two elements at begin and end of every
contiguous line strip together with skipping the elements in the index buffer
appropriately, and then bound with an offset of @cpp -4 @ce elements to the
@ref PreviousPosition and @cpp +4 @ce elements to the @ref NextPosition.

This needs only one triangle in the index buffer for each join instead of two
and has a memory requirement of
@f$ (4(12 + 1) + 24)n + (4(12 + 1) - 12)l + 12 @f$ bytes, with @f$ l @f$
being the line strip count. With a mesh consisting of just a single strip this
is @f$ 76n + 52 @f$ bytes, which is ~1.8x as much as CPU-side-generated indexed
quads and ~6.3x as much as a @ref MeshPrimitive::LineStrip would need.

@m_div{m-row m-container-inflate}
@m_div{m-col-l-12 m-nopady m-nopadx}
@htmlinclude line-quad-data-overlap-generic.svg
@m_enddiv
@m_enddiv

@subsubsection Shaders-LineGL-data-representation-overlap-fixedjoin Lines with fixed join directions

If the joint direction is known to be fixed, i.e. the B and D points always
collapse to the same position independently of the transform used, the two
points can be replaced with just one. This is commonly the case in 2D if
negative transformation scaling isn't involved and with planar line art in 3D
if it additionally also isn't viewed from the back side. This allows padding
of the @ref Position attribute at the begin and end of every contiguous line
strip to be reduced to just one element, binding it with an offset of
@cpp -3 @ce elements to the @ref PreviousPosition and @cpp +3 @ce elements to
the @ref NextPosition.

This has a memory requirement of
@f$ (3(12 + 1) + 18)n + (3(12 + 1) - 6)l + 12 @f$ bytes. With a mesh consisting
of just a single strip this is @f$ 57n + 45 @f$ bytes, which is ~1.4x as much
as CPU-side-generated indexed quads and ~4.75x as much as a
@ref MeshPrimitive::LineStrip would need.

@m_div{m-row m-container-inflate}
@m_div{m-col-l-12 m-nopady m-nopadx}
@htmlinclude line-quad-data-overlap-fixedjoin.svg
@m_enddiv
@m_enddiv

@subsubsection Shaders-LineGL-data-representation-overlap-miterjoin Lines with miter joins only

The final and most data-efficient case is for line meshes where the contiguous
segments consist of miter joints only (i.e., with the assumption that the angle
between two segments is never too sharp to fall back to
@ref LineJoinStyle::Bevel), resulting in the join collapsing to just two
vertices, with no triangle in between:

@htmlinclude line-quad-expansion-joins-miter-caps.svg

This is the usual case for finely subdivided curves. Generic line art can be
patched in a preprocessing step, subdividing sharp corners to a sequence of
joins with larger angles. This layout doesn't require any padding of the
@ref Position attribute between contiguous line strips, and it's bound with an
offset of @cpp -2 @ce elements to the @ref PreviousPosition and @cpp +2 @ce
elements to the @ref NextPosition.

The memory requirement is @f$ (2(12 + 1) + 12)n + 2(12 + 1)l + 12 @f$ bytes.
With a mesh consisting of a single strip it's @f$ 38n + 38 @f$ bytes. This is
roughly the same memory use as @f$ 36n + 24 @f$ bytes for CPU-side-generated
quads with miter joins only, and ~3.2x as much as a
@ref MeshPrimitive::LineStrip would need.

@m_div{m-row m-container-inflate}
@m_div{m-col-l-12 m-nopady m-nopadx}
@htmlinclude line-quad-data-overlap-miterjoin.svg
@m_enddiv
@m_enddiv
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT LineGL: public GL::AbstractShaderProgram {
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
         * @brief Previous position
         *
         * @relativeref{Magnum,Vector2} in 2D, @relativeref{Magnum,Vector3} in
         * 3D. Uses the same location as @ref GenericGL::Tangent with the
         * assumption that lines don't need tangent space information.
         *
         * If @ref LineVertexAnnotation::Begin is set in @ref Annotation,
         * contains the other point of the neighboring line segment if
         * @ref LineVertexAnnotation::Join is also set, and is ignored
         * otherwise. If @ref LineVertexAnnotation::Begin is not set in
         * @ref Annotation, contains the other point of the line segment.
         */
        typedef GL::Attribute<3, VectorTypeFor<dimensions, Float>> PreviousPosition;

        /**
         * @brief Next position
         *
         * @relativeref{Magnum,Vector2} in 2D, @relativeref{Magnum,Vector3} in
         * 3D. Uses the same location as @ref GenericGL::Normal with the
         * assumption that lines don't need tangent space information.
         *
         * If @ref LineVertexAnnotation::Begin is set in @ref Annotation,
         * contains the other point of the line segment. If
         * @ref LineVertexAnnotation::Begin is not set in @ref Annotation,
         * contains the other point of the neighboring line segment if
         * @ref LineVertexAnnotation::Join is set, and is ignored otherwise.
         */
        typedef GL::Attribute<5, VectorTypeFor<dimensions, Float>> NextPosition;

        /**
         * @brief Vertex annotation
         *
         * Uses the same location as @ref GenericGL::TextureCoordinates with
         * the assumption that lines don't need a two-dimensional texture
         * space information.
         *
         * Contains a set of @ref LineVertexAnnotation bits, see their
         * documentation for more information. The values are guaranteed to fit
         * into 8 bits.
         */
        typedef GL::Attribute<1, UnsignedInt> Annotation;

        /**
         * @brief Three-component vertex color
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color3. Use
         * either this or the @ref Color4 attribute.
         */
        typedef typename GenericGL<dimensions>::Color3 Color3;

        /**
         * @brief Four-component vertex color
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color4. Use
         * either this or the @ref Color3 attribute.
         */
        typedef typename GenericGL<dimensions>::Color4 Color4;

        /**
         * @brief (Instanced) object ID
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,UnsignedInt}. Used only if
         * @ref Flag::InstancedObjectId is set.
         */
        typedef typename GenericGL<dimensions>::ObjectId ObjectId;

        /**
         * @brief (Instanced) transformation matrix
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Matrix3} in 2D, @relativeref{Magnum,Matrix4} in
         * 3D. Used only if @ref Flag::InstancedTransformation is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         */
        typedef typename GenericGL<dimensions>::TransformationMatrix TransformationMatrix;

        enum: UnsignedInt {
            /**
             * Color shader output. Present always, expects three- or
             * four-component floating-point or normalized buffer attachment.
             */
            ColorOutput = GenericGL<dimensions>::ColorOutput,

            /**
             * Object ID shader output. @ref shaders-generic "Generic output",
             * present only if @ref Flag::ObjectId is set. Expects a
             * single-component unsigned integral attachment. Writes the value
             * set in @ref setObjectId() and possibly also a per-vertex ID and
             * an ID fetched from a texture, see @ref Shaders-LineGL-object-id
             * for more information.
             * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
             */
            ObjectIdOutput = GenericGL<dimensions>::ObjectIdOutput
        };

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags(), @ref Configuration::setFlags()
         */
        enum class Flag: UnsignedShort {
            /**
             * Multiply the color with a vertex color. Requires either the
             * @ref Color3 or @ref Color4 attribute to be present.
             */
            VertexColor = 1 << 0,

            /**
             * Enable object ID output. See @ref Shaders-LineGL-object-id for
             * more information.
             */
            ObjectId = 1 << 1,

            /**
             * Instanced object ID. Retrieves a per-instance / per-vertex
             * object ID from the @ref ObjectId attribute, outputting a sum of
             * the per-vertex ID and ID coming from @ref setObjectId() or
             * @ref LineDrawUniform::objectId. Implicitly enables
             * @ref Flag::ObjectId. See @ref Shaders-LineGL-object-id for more
             * information.
             */
            InstancedObjectId = (1 << 2)|ObjectId,

            /**
             * Instanced transformation. Retrieves a per-instance
             * transformation matrix from the @ref TransformationMatrix
             * attribute and uses it together with the matrix coming from
             * @ref setTransformationProjectionMatrix() or
             * @ref TransformationProjectionUniform2D::transformationProjectionMatrix
             * / @ref TransformationProjectionUniform3D::transformationProjectionMatrix
             * (first the per-instance, then the uniform matrix). See
             * @ref Shaders-LineGL-instancing for more information.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             */
            InstancedTransformation = 1 << 3,

            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindTransformationProjectionBuffer(),
             * @ref bindDrawBuffer() and @ref bindMaterialBuffer() instead of
             * direct uniform setters.
             * @see @ref Flag::ShaderStorageBuffers
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             */
            UniformBuffers = 1 << 4,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Use shader storage buffers. Superset of functionality provided
             * by @ref Flag::UniformBuffers, compared to it doesn't have any
             * size limits on @ref Configuration::setMaterialCount() and
             * @relativeref{Configuration,setDrawCount()} in exchange for
             * potentially more costly access and narrower platform support.
             * @requires_gl43 Extension @gl_extension{ARB,shader_storage_buffer_object}
             * @requires_gles31 Shader storage buffers are not available in
             *      OpenGL ES 3.0 and older.
             * @requires_gles Shader storage buffers are not available in
             *      WebGL.
             * @m_since_latest
             */
            ShaderStorageBuffers = UniformBuffers|(1 << 6),
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
             */
            MultiDraw = UniformBuffers|(1 << 5)
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
        typedef Implementation::LineGLFlag Flag;
        typedef Implementation::LineGLFlags Flags;
        #endif

        /**
         * @brief Compile asynchronously
         *
         * Compared to @ref LineGL(const Configuration&) can perform an
         * asynchronous compilation and linking. See @ref shaders-async for
         * more information.
         * @see @ref LineGL(CompileState&&)
         */
        /* Compared to the non-templated shaders like PhongGL or
           MeshVisualizerGL2D using a forward declaration is fine here. Huh. */
        static CompileState compile(const Configuration& configuration = Configuration{});

        /** @brief Constructor */
        /* Compared to the non-templated shaders like PhongGL or
           MeshVisualizerGL2D using a forward declaration is fine here. Huh. */
        explicit LineGL(const Configuration& configuration = Configuration{});

        /**
         * @brief Finalize an asynchronous compilation
         *
         * Takes an asynchronous compilation state returned by @ref compile()
         * and forms a ready-to-use shader object. See @ref shaders-async for
         * more information.
         */
        explicit LineGL(CompileState&& state);

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
        explicit LineGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        LineGL(const LineGL<dimensions>&) = delete;

        /** @brief Move constructor */
        LineGL(LineGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        LineGL<dimensions>& operator=(const LineGL<dimensions>&) = delete;

        /** @brief Move assignment */
        LineGL<dimensions>& operator=(LineGL<dimensions>&&) noexcept = default;

        /**
         * @brief Flags
         *
         * @see @ref Configuration::setFlags()
         */
        Flags flags() const { return _flags; }

        /**
         * @brief Cap style
         *
         * @see @ref Configuration::setCapStyle()
         */
        LineCapStyle capStyle() const { return _capStyle; }

        /**
         * @brief Join style
         *
         * @see @ref Configuration::setJoinStyle()
         */
        LineJoinStyle joinStyle() const { return _joinStyle; }

        /**
         * @brief Material count
         *
         * Statically defined size of the @ref LineMaterialUniform uniform
         * buffer bound with @ref bindMaterialBuffer(). Has use only if
         * @ref Flag::UniformBuffers is set and @ref Flag::ShaderStorageBuffers
         * is not set.
         * @see @ref Configuration::setMaterialCount()
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Draw count
         *
         * Statically defined size of each of the
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D and @ref LineDrawUniform
         * uniform buffers bound with @ref bindTransformationProjectionBuffer()
         * and @ref bindDrawBuffer(). Has use only if @ref Flag::UniformBuffers
         * is set and @ref Flag::ShaderStorageBuffers is not set.
         * @see @ref Configuration::setDrawCount()
         */
        UnsignedInt drawCount() const { return _drawCount; }

        /**
         * @brief Set viewport size
         * @return Reference to self (for method chaining)
         *
         * Line width and smoothness set in either @ref setWidth() /
         * @ref setSmoothness() or @ref LineMaterialUniform::width /
         * @ref LineMaterialUniform::smoothness depends on this value --- i.e.,
         * a value of @cpp 1.0f @ce is one pixel only if @ref setViewportSize()
         * is called with the actual pixel size of the viewport. Initial value
         * is a zero vector.
         */
        LineGL<dimensions>& setViewportSize(const Vector2& size);

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
        LineGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set background color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x00000000_rgbaf @ce. Used for edge smoothing
         * if smoothness is non-zero, and for background areas if
         * @ref LineCapStyle::Round or @ref LineCapStyle::Triangle is used. If
         * smoothness is zero and @ref LineCapStyle::Butt or
         * @ref LineCapStyle::Square is used, only the foreground color is
         * used.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref LineMaterialUniform::backgroundColor and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref setColor(), @ref setSmoothness(),
         *      @ref Configuration::setCapStyle()
         */
        LineGL<dimensions>& setBackgroundColor(const Magnum::Color4& color);

        /**
         * @brief Set color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref LineMaterialUniform::color and call @ref bindMaterialBuffer()
         * instead.
         * @see @ref setBackgroundColor()
         */
        LineGL<dimensions>& setColor(const Magnum::Color4& color);

        /**
         * @brief Set line width
         * @return Reference to self (for method chaining)
         *
         * Screen-space, interpreted depending on the viewport size --- i.e.,
         * a value of @cpp 1.0f @ce is one pixel only if @ref setViewportSize()
         * is called with the actual pixel size of the viewport. Initial value
         * is @cpp 1.0f @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref LineMaterialUniform::width and call @ref bindMaterialBuffer()
         * instead.
         */
        LineGL<dimensions>& setWidth(Float width);

        /**
         * @brief Set line smoothness
         * @return Reference to self (for method chaining)
         *
         * Larger values will make edges look less aliased (but blurry),
         * smaller values will make them more crisp (but possibly aliased).
         * Screen-space, interpreted depending on the viewport size --- i.e.,
         * a value of @cpp 1.0f @ce is one pixel only if @ref setViewportSize()
         * is called with the actual pixel size of the viewport. Initial value
         * is @cpp 0.0f @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref LineMaterialUniform::smoothness and call @ref bindMaterialBuffer()
         * instead.
         */
        LineGL<dimensions>& setSmoothness(Float smoothness);

        /**
         * @brief Set miter length limit
         * @return Reference to self (for method chaining)
         *
         * Maximum length (relative to line width) over which a
         * @ref LineJoinStyle::Miter join is converted to a
         * @ref LineJoinStyle::Bevel in order to avoid sharp corners extending
         * too much. Default value is @cpp 4.0f @ce, which corresponds to
         * approximately 29 degrees. Alternatively you can set the limit as an
         * angle using @ref setMiterAngleLimit(). Miter length is calculated
         * using the following formula, where @f$ w @f$ is line half-width,
         * @f$ l @f$ is miter length and @f$ \theta @f$ is angle between two
         * line segments: @f[
         *      \frac{w}{l} = \sin(\frac{\theta}{2})
         * @f]
         *
         * Expects that @ref joinStyle() is @ref LineJoinStyle::Miter and
         * @p limit is greater than or equal to @cpp 1.0f @ce and finite.
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref LineMaterialUniform::miterLimit using
         * @ref LineMaterialUniform::setMiterLengthLimit() and call
         * @ref bindMaterialBuffer() instead.
         */
        LineGL<dimensions>& setMiterLengthLimit(Float limit);

        /**
         * @brief Set miter angle limit
         * @return Reference to self (for method chaining)
         *
         * Like @ref setMiterLengthLimit(), but specified as a minimum angle
         * between two line segments below which a @ref LineJoinStyle::Miter
         * join is converted to a @ref LineJoinStyle::Bevel in order to avoid
         * sharp corners extending too much. Default value is approximately
         * @cpp 28.955_degf @ce, see @ref setMiterLengthLimit() above for more
         * information.
         *
         * Expects that @ref joinStyle() is @ref LineJoinStyle::Miter and
         * @p limit is greater than @cpp 0.0_radf @ce. Expects that
         * @ref Flag::UniformBuffers is not set, in that case fill
         * @ref LineMaterialUniform::miterLimit using
         * @ref LineMaterialUniform::setMiterAngleLimit() and call
         * @ref bindMaterialBuffer() instead.
         */
        LineGL<dimensions>& setMiterAngleLimit(Rad limit);

        /**
         * @brief Set object ID
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::ObjectId
         * enabled. Value set here is written to the @ref ObjectIdOutput, see
         * @ref Shaders-LineGL-object-id for more information. Initial value
         * is @cpp 0 @ce. If @ref Flag::InstancedObjectId is enabled as well,
         * this value is added to the ID coming from the @ref ObjectId
         * attribute.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref LineDrawUniform::objectId and call @ref bindDrawBuffer()
         * instead.
         */
        LineGL<dimensions>& setObjectId(UnsignedInt id);

        /**
         * @}
         */

        /** @{
         * @name Uniform / shader storage buffer binding and related uniform setters
         *
         * Used if @ref Flag::UniformBuffers is set.
         */

        /**
         * @brief Bind a draw offset
         * @return Reference to self (for method chaining)
         *
         * Specifies which item in the @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D and @ref LineDrawUniform
         * buffers bound with @ref bindTransformationProjectionBuffer() and
         * @ref bindDrawBuffer() should be used for current draw. Expects that
         * @ref Flag::UniformBuffers is set and @p offset is less than
         * @ref drawCount(). Initial value is @cpp 0 @ce, if @ref drawCount()
         * is @cpp 1 @ce, the function is a no-op as the shader assumes draw
         * offset to be always zero.
         *
         * If @ref Flag::MultiDraw is set, @glsl gl_DrawID @ce is added to this
         * value, which makes each draw submitted via
         * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        LineGL<dimensions>& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Bind a transformation and projection uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D. At the very least you need
         * to call also @ref bindDrawBuffer() and @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         */
        LineGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer);
        LineGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size); /**< @overload */

        /**
         * @brief Bind a draw uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref LineDrawUniform. At the very least you need to call also
         * @ref bindTransformationProjectionBuffer() and
         * @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         */
        LineGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer);
        LineGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size); /**< @overload */

        /**
         * @brief Bind a material uniform / shader storage buffer
         * @return Reference to self (for method chaining)
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref materialCount() instances of
         * @ref LineMaterialUniform. At the very least you need to call also
         * @ref bindTransformationProjectionBuffer() and @ref bindDrawBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         */
        LineGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer);
        LineGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size); /**< @overload */

        /**
         * @}
         */

        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(LineGL<dimensions>)

    private:
        /* Creates the GL shader program object but does nothing else.
           Internal, used by compile(). */
        explicit LineGL(NoInitT);

        Flags _flags;
        LineCapStyle _capStyle;
        LineJoinStyle _joinStyle;
        UnsignedInt _materialCount{},
            _drawCount{};
        Int _viewportSizeUniform{0},
            _transformationProjectionMatrixUniform{1},
            _backgroundColorUniform{2},
            _colorUniform{3},
            _widthUniform{4},
            _smoothnessUniform{5},
            _miterLimitUniform{6},
            _objectIdUniform{7},
            /* Used instead of all other uniforms except viewportSize when
               Flag::UniformBuffers is set, so it can alias them */
            _drawOffsetUniform{1};
};

/**
@brief Configuration

@see @ref LineGL(const Configuration&), @ref compile(const Configuration&)
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT LineGL<dimensions>::Configuration {
    public:
        explicit Configuration();

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set flags
         *
         * No flags are set by default.
         * @see @ref LineGL::flags()
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        /** @brief Cap style */
        LineCapStyle capStyle() const { return _capStyle; }

        /**
         * @brief Set cap style
         *
         * Unlike for example the SVG specification that uses
         * @ref LineCapStyle::Butt by default, the default value is
         * @ref LineCapStyle::Square, in order to make zero-length lines
         * visible.
         * @see @ref LineGL::capStyle()
         */
        Configuration& setCapStyle(LineCapStyle style) {
            _capStyle = style;
            return *this;
        }

        /** @brief Join style */
        LineJoinStyle joinStyle() const { return _joinStyle; }

        /**
         * @brief Set join style
         *
         * Default value is @ref LineJoinStyle::Miter, consistently with the
         * SVG specification.
         * @see @ref LineGL::joinStyle()
         */
        Configuration& setJoinStyle(LineJoinStyle style) {
            _joinStyle = style;
            return *this;
        }

        /** @brief Material count */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Set material count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref LineMaterialUniform buffer bound with
         * @ref bindMaterialBuffer(). Uniform buffers have a statically defined
         * size and @cpp count*sizeof(LineMaterialUniform) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffer is
         * unbounded and @p count is ignored. The per-draw materials are
         * specified via @ref LineDrawUniform::materialId. Default value is
         * @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setDrawCount(),
         *      @ref LineGL::materialCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         */
        Configuration& setMaterialCount(UnsignedInt count) {
            _materialCount = count;
            return *this;
        }

        /** @brief Draw count */
        UnsignedInt drawCount() const { return _drawCount; }

        /**
         * @brief Set draw count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D /
         * @ref LineDrawUniform buffer bound with
         * @ref bindTransformationProjectionBuffer() and @ref bindDrawBuffer().
         * Uniform buffers have a statically defined size and the maximum of
         * @cpp count*sizeof(TransformationProjectionUniform2D) @ce /
         * @cpp count*sizeof(TransformationProjectionUniform3D) @ce and
         * @cpp count*sizeof(LineDrawUniform) @ce has to be within
         * @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffers are
         * unbounded and @p count is ignored. The draw offset is set via
         * @ref setDrawOffset(). Default value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setMaterialCount(),
         *      @ref LineGL::drawCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         */
        Configuration& setDrawCount(UnsignedInt count) {
            _drawCount = count;
            return *this;
        }

    private:
        Flags _flags;
        LineCapStyle _capStyle;
        LineJoinStyle _joinStyle;
        UnsignedInt _materialCount{1};
        UnsignedInt _drawCount{1};
};

/**
@brief Asynchronous compilation state

Returned by @ref compile(). See @ref shaders-async for more information.
*/
template<UnsignedInt dimensions> class LineGL<dimensions>::CompileState: public LineGL<dimensions> {
    /* Everything deliberately private except for the inheritance */
    friend class LineGL;

    explicit CompileState(NoCreateT): LineGL{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

    explicit CompileState(LineGL<dimensions>&& shader, GL::Shader&& vert, GL::Shader&& frag
        #if !defined(MAGNUM_TARGET_GLES) || !defined(MAGNUM_TARGET_WEBGL)
        , GL::Version version
        #endif
    ): LineGL<dimensions>{Utility::move(shader)}, _vert{Utility::move(vert)}, _frag{Utility::move(frag)}
        #if !defined(MAGNUM_TARGET_GLES) || !defined(MAGNUM_TARGET_WEBGL)
        , _version{version}
        #endif
        {}

    Implementation::GLShaderWrapper _vert, _frag;
    #if !defined(MAGNUM_TARGET_GLES) || !defined(MAGNUM_TARGET_WEBGL)
    GL::Version _version;
    #endif
};

/**
@brief 2D line OpenGL shader
@m_since_latest

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
typedef LineGL<2> LineGL2D;

/**
@brief 3D LineGL OpenGL shader
@m_since_latest

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
typedef LineGL<3> LineGL3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/**
 * @debugoperatorclassenum{LineGL,LineGL::Flag}
 * @m_since_latest
 */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, LineGL<dimensions>::Flag value);

/**
 * @debugoperatorclassenum{LineGL,LineGL::Flags}
 * @m_since_latest
 */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, LineGL<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, LineGLFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, LineGLFlags value);
}
#endif

}}
#elif defined(MAGNUM_TARGET_GLES2)
#error this header is not available in the OpenGL ES 2.0 / WebGL 1.0 build
#else
#error this header is available only in the OpenGL build
#endif

#endif
