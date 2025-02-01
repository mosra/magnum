#ifndef Magnum_Shaders_Line_h
#define Magnum_Shaders_Line_h
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

/** @file
 * @brief Struct @ref Magnum::Shaders::LineDrawUniform, @ref Magnum::Shaders::LineMaterialUniform, enum @ref Magnum::Shaders::LineCapStyle, @ref Magnum::Shaders::LineJoinStyle, @ref Magnum::Shaders::LineVertexAnnotation, enum set @ref Magnum::Shaders::LineVertexAnnotations
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Line cap style
@m_since_latest

@see @ref LineGL::capStyle(), @ref LineGL::Configuration::setCapStyle()
*/
enum class LineCapStyle: UnsignedByte {
    /* Keep these in sync with Ui::LineCapStyle (except for the related links,
       of course). The images are used directly from there. */

    /**
     * [Butt cap](https://en.wikipedia.org/wiki/Butt_joint). The line is cut
     * off right at the endpoint. Lines of zero length will be invisible.
     *
     * @htmlinclude line-cap-butt.svg
     */
    Butt,

    /**
     * Square cap. The line is extended by half of its width past the endpoint.
     * Lines of zero length will be shown as squares.
     *
     * @htmlinclude line-cap-square.svg
     */
    Square,

    /**
     * Round cap. The line is extended by half of its width past the endpoint.
     * It's still rendered as a quad but pixels outside of the half-circle have
     * the background color. Lines of zero length will be shown as circles.
     *
     * @htmlinclude line-cap-round.svg
     *
     * @see @ref LineMaterialUniform::backgroundColor,
     *      @ref LineGL::setBackgroundColor()
     */
    Round,

    /**
     * Triangle cap. The line is extended by half of its width past the
     * endpoint. It's still rendered as a quad but pixels outside of the
     * triangle have the background color. Lines of zero length will be shown
     * as squares rotated by 45°.
     *
     * @htmlinclude line-cap-triangle.svg
     *
     * @see @ref LineMaterialUniform::backgroundColor,
     *      @ref LineGL::setBackgroundColor()
     */
    Triangle
};

/**
@brief Line join style
@m_since_latest

@see @ref LineGL::joinStyle(), @ref LineGL::Configuration::setJoinStyle()
*/
enum class LineJoinStyle: UnsignedByte {
    /* Keep these in sync with Ui::LineJoinStyle (except for the related links,
       of course). The images are used directly from there. */

    /**
     * [Miter join](https://en.wikipedia.org/wiki/Miter_joint). The outer edges
     * of both line segments extend until they intersect.
     *
     * @htmlinclude line-join-miter.svg
     *
     * In this style, the points `A`, `B` and `C` collapse to a zero-area
     * triangle. If the miter length `l` would be larger than the limit set via
     * @ref LineGL::setMiterLengthLimit() /
     * @ref LineMaterialUniform::setMiterLengthLimit() or the angle between the
     * two segments `α` would be less than the limit set via
     * @ref LineGL::setMiterAngleLimit() /
     * @ref LineMaterialUniform::setMiterAngleLimit(), it switches to
     * @ref LineJoinStyle::Bevel instead.
     */
    Miter,

    /**
     * [Bevel join](https://en.wikipedia.org/wiki/Bevel). Outer edges of both
     * line segments are cut off at a right angle at their endpoints.
     *
     * @htmlinclude line-join-bevel.svg
     *
     * The area between points `A`, `B` and `C` is filled with an extra
     * triangle.
     */
    Bevel
};

/**
@brief Line vertex annotation
@m_since_latest

A line segment drawn by the @ref LineGL shader consists of four vertices, first
two having the @ref LineGL::Position attribute set to the first point of the
segment and second two having it set to the second point of the segment. In
order to distinguish the direction in which the point should be expanded to
form a quad and whether the expansion should be for a line join or line cap,
each vertex contains @ref LineVertexAnnotations in the @ref LineGL::Annotation
attribute.

@htmlinclude line-annotation.svg

In the above diagram, there's a line strip consisting of three line segments
and six pairs of points, with @m_span{m-label m-success} green @m_endspan and
@m_span{m-label m-default} white @m_endspan forming (square) line caps, while
@m_span{m-label m-primary} azure @m_endspan,
@m_span{m-label m-danger} red @m_endspan form a miter line join, and
@m_span{m-label m-info} blue @m_endspan and
@m_span{m-label m-warning} yellow @m_endspan form a bevel join. The
twelve corresponding annotations, forming three quads (and one extra triangle
for the bevel), are shown with `U`, `J` and `B` letters, color-coded to show
which original line point they correspond to. Line cap style and join style
isn't a part of the annotation, it's set with @ref LineCapStyle and
@ref LineJoinStyle at shader compilation time instead.

The type is 32-bit in order to match the default type of the
@ref LineGL::Annotation attribute, but the values are guaranteed to fit into 8
bits.
*/
enum class LineVertexAnnotation: UnsignedInt {
    /* Keep these in sync with the internal Ui::LineLayer LineVertexAnnotation*
       constants and ANNOTATION_* defines in Line.in.vert. */

    /**
     * The point extends upwards assuming a left-to-right direction of the line
     * segment. If not set, it extends downwards. Visualized as `U` in the
     * above diagram.
     */
    Up = 1 << 0,

    /**
     * The point is forming a join with a neighboring line segment defined by
     * either @ref LineGL::PreviousPosition or @ref LineGL::NextPosition based
     * on whether @ref LineVertexAnnotation::Begin is set. If not set, the
     * point is forming a line cap, extending in the opposite of the line
     * segment direction if @ref LineVertexAnnotation::Begin is set, and in the
     * direction if not set. Visualized as `J` in the above diagram.
     */
    Join = 1 << 1,

    /**
     * The point is forming the beginning of the line segment, i.e.
     * @ref LineGL::NextPosition contains the other point of the line segment.
     * If not set, @ref LineGL::PreviousPosition contains the other point of
     * the line segment instead.
     *
     * If @ref LineVertexAnnotation::Join is set as well, the point is a common
     * point of two neighboring line segments and @ref LineGL::PreviousPosition
     * contains the other point of the neighboring line segment. If
     * @ref LineVertexAnnotation::Join is set and this bit is not set,
     * @ref LineGL::NextPosition contaons the other point of the neighboring
     * line segment instead. Visualized as `B` in the above diagram.
     */
    Begin = 1 << 2,
};

/**
@brief Line vertex annotations
@m_since_latest

Contents of the @ref LineGL::Annotation attribute. See
@ref LineVertexAnnotation for more information.
*/
typedef Containers::EnumSet<LineVertexAnnotation> LineVertexAnnotations;

CORRADE_ENUMSET_OPERATORS(LineVertexAnnotations)

/**
 * @debugoperatorenum{LineCapStyle}
 * @m_since_latest
 */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, LineCapStyle value);

/**
 * @debugoperatorenum{LineJoinStyle}
 * @m_since_latest
 */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, LineJoinStyle value);

/**
 * @debugoperatorenum{LineVertexAnnotation}
 * @m_since_latest
 */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, LineVertexAnnotation value);

/**
 * @debugoperatorenum{LineVertexAnnotations}
 * @m_since_latest
 */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, LineVertexAnnotations value);

/**
@brief Per-draw uniform for line shaders
@m_since_latest

Together with the generic @ref TransformationProjectionUniform2D /
@ref TransformationProjectionUniform3D contains parameters that are specific to
each draw call. Material-related properties are expected to be shared among multiple draw calls and thus are provided in a separate
@ref LineMaterialUniform structure, referenced by @ref materialId.
@see @ref LineGL::bindDrawBuffer()
*/
struct LineDrawUniform {
    /** @brief Construct with default parameters */
    constexpr explicit LineDrawUniform(DefaultInitT = DefaultInit) noexcept: materialId{0}, objectId{0} {}

    /** @brief Construct without initializing the contents */
    explicit LineDrawUniform(NoInitT) noexcept {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref materialId field
     * @return Reference to self (for method chaining)
     */
    LineDrawUniform& setMaterialId(UnsignedInt id) {
        materialId = id;
        return *this;
    }

    /**
     * @brief Set the @ref objectId field
     * @return Reference to self (for method chaining)
     */
    LineDrawUniform& setObjectId(UnsignedInt id) {
        objectId = id;
        return *this;
    }

    /**
     * @}
     */

    /** @var materialId
     * @brief Material ID
     *
     * References a particular material from a @ref LineMaterialUniform array.
     * Useful when an UBO with more than one material is supplied or in a
     * multi-draw scenario. Should be less than the material count passed to
     * @ref LineGL::Configuration::setMaterialCount(), if material count is
     * @cpp 1 @ce, this field is assumed to be @cpp 0 @ce and isn't even read
     * by the shader. Default value is @cpp 0 @ce, meaning the first material
     * gets used.
     */

    /* This field is an UnsignedInt in the shader and materialId is extracted
       as (value & 0xffff), so the order has to be different on BE */
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    alignas(4) UnsignedShort materialId;
    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    UnsignedShort:16; /* reserved */
    #endif
    #else
    alignas(4) UnsignedShort:16; /* reserved */
    UnsignedShort materialId;
    #endif

    /**
     * @brief Object ID
     *
     * Used only for the object ID framebuffer output, not to access any other
     * uniform data. Default value is @cpp 0 @ce.
     *
     * Used only if @ref LineGL::Flag::ObjectId is enabled, ignored otherwise.
     * If @ref LineGL::Flag::InstancedObjectId is enabled as well, this value
     * is added to the ID coming from the @ref LineGL::ObjectId attribute.
     * @see @ref LineGL::setObjectId()
     */
    UnsignedInt objectId;

    /* warning: Member __pad1__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int:32;
    Int:32;
    #endif
};

/**
@brief Material uniform for line shaders
@m_since_latest

Describes material properties referenced from
@ref LineDrawUniform::materialId.
@see @ref LineGL::bindMaterialBuffer()
*/
struct MAGNUM_SHADERS_EXPORT LineMaterialUniform {
    /** @brief Construct with default parameters */
    constexpr explicit LineMaterialUniform(DefaultInitT = DefaultInit) noexcept: backgroundColor{0.0f, 0.0f, 0.0f, 0.0f}, color{1.0f, 1.0f, 1.0f, 1.0f}, width{1.0f}, smoothness{0.0f}, miterLimit{0.875f} {}

    /** @brief Construct without initializing the contents */
    explicit LineMaterialUniform(NoInitT) noexcept: color{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref color field
     * @return Reference to self (for method chaining)
     */
    LineMaterialUniform& setColor(const Color4& color) {
        this->color = color;
        return *this;
    }

    /**
     * @brief Set the @ref backgroundColor field
     * @return Reference to self (for method chaining)
     */
    LineMaterialUniform& setBackgroundColor(const Color4& color) {
        backgroundColor = color;
        return *this;
    }

    /**
     * @brief Set the @ref width field
     * @return Reference to self (for method chaining)
     */
    LineMaterialUniform& setWidth(Float width) {
        this->width = width;
        return *this;
    }

    /**
     * @brief Set the @ref smoothness field
     * @return Reference to self (for method chaining)
     */
    LineMaterialUniform& setSmoothness(Float smoothness) {
        this->smoothness = smoothness;
        return *this;
    }

    /**
     * @brief Set the @ref miterLimit field
     * @return Reference to self (for method chaining)
     *
     * For convenience it's recommended to use the @ref setMiterLengthLimit()
     * and @ref setMiterAngleLimit() helpers instead of setting this value
     * directly.
     */
    LineMaterialUniform& setMiterLimit(Float limit) {
        miterLimit = limit;
        return *this;
    }

    /**
     * @brief Set the @ref miterLimit field to a length value
     * @return Reference to self (for method chaining)
     *
     * Expects that @p limit is greater than or equal to @cpp 1.0f @ce and
     * finite.
     */
    LineMaterialUniform& setMiterLengthLimit(Float limit);

    /**
     * @brief Set the @ref miterLimit field to an angle value
     * @return Reference to self (for method chaining)
     *
     * Expects that @p limit is greater than @cpp 0.0_radf @ce.
     */
    LineMaterialUniform& setMiterAngleLimit(Rad limit);

    /**
     * @}
     */

    /**
     * @brief Background color
     *
     * Default value is @cpp 0x00000000_rgbaf @ce. Used for edge smoothing if
     * smoothness is non-zero, and for background areas if
     * @ref LineCapStyle::Round or @ref LineCapStyle::Triangle is used. If
     * smoothness is zero and @ref LineCapStyle::Butt or
     * @ref LineCapStyle::Square is used, only the foreground color is used.
     * @see @ref LineGL::setBackgroundColor(), @ref LineGL::setSmoothness(),
     *      @ref LineGL::Configuration::setCapStyle()
     */
    Color4 backgroundColor;

    /**
     * @brief Color
     *
     * Default value is @cpp 0xffffffff_rgbaf @ce.
     *
     * If @ref LineGL::Flag::VertexColor is enabled, the color is multiplied
     * with a color coming from the @ref LineGL::Color3 / @ref LineGL::Color4
     * attribute.
     * @see @ref LineGL::setColor()
     */
    Color4 color;

    /**
     * @brief Line width
     *
     * Screen-space, interpreted depending on the viewport size --- i.e., a
     * value of @cpp 1.0f @ce is one pixel only if @ref LineGL::setViewportSize()
     * is called with the actual pixel size of the viewport. Default value is
     * @cpp 1.0f @ce.
     * @see @ref LineGL::setWidth()
     */
    Float width;

    /**
     * @brief Line smoothness
     *
     * Larger values will make edges look less aliased (but blurry), smaller
     * values will make them more crisp (but possibly aliased). Screen-space,
     * interpreted depending on the viewport size --- i.e., a value of
     * @cpp 1.0f @ce is one pixel only if @ref LineGL::setViewportSize() is
     * called with the actual pixel size of the viewport. Initial value is
     * @cpp 0.0f @ce.
     * @see @ref LineGL::setSmoothness()
     */
    Float smoothness;

    /**
     * @brief Miter limit
     *
     * Limit at which a @ref LineJoinStyle::Miter join is converted to a
     * @ref LineJoinStyle::Bevel in order to avoid sharp corners extending too
     * much. If joint style is not @ref LineJoinStyle::Miter, this value is
     * unused.
     *
     * Represented as a cosine of the angle between two neighboring line
     * segments, with @ref LineJoinStyle::Bevel used for angles below the limit
     * (thus their cosine larger than this value). For length-based limits,
     * the relation between angle @f$ \theta @f$, miter length @f$ l @f$ and
     * line half-width @f$ w @f$ is as follows: @f[
     *      \frac{w}{l} = \sin(\frac{\theta}{2})
     * @f]
     *
     * For convenience it's recommended to use the @ref setMiterLengthLimit()
     * and @ref setMiterAngleLimit() helpers instead of setting this value
     * directly. Default value is @cpp 0.875f @ce, which corresponds to a
     * length of @cpp 4.0f @ce and angle of approximately @cpp 28.955_degf @ce.
     * @see @ref LineGL::setMiterLengthLimit(),
     *      @ref LineGL::setMiterAngleLimit()
     */
    Float miterLimit;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int:32; /* reserved for dynamic cap/join style */
    #endif
};

}}

#endif
