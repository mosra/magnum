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

#if !defined(GL_ES) && !defined(NEW_GLSL)
#extension GL_EXT_gpu_shader4: require
#endif

#if defined(UNIFORM_BUFFERS) && defined(TEXTURE_ARRAYS) && !defined(GL_ES)
#extension GL_ARB_shader_bit_encoding: require
#endif

/* Use the noperspective keyword to avoid artifacts in screen-space
   interpolation if perspective projection is used in 3D. If not available,
   it's worked around by dividing gl_Position with gl_Position.w (which is
   extra instructions, so the noperspective keyword is preferred). */
#ifndef GL_ES
#define CAN_USE_NOPERSPECTIVE
#elif defined(GL_ES) && defined(GL_NV_shader_noperspective_interpolation)
#extension GL_NV_shader_noperspective_interpolation: require
#define CAN_USE_NOPERSPECTIVE
#endif

#ifdef MULTI_DRAW
#ifndef GL_ES
#extension GL_ARB_shader_draw_parameters: require
#else /* covers WebGL as well */
#extension GL_ANGLE_multi_draw: require
#endif
#endif

#ifndef NEW_GLSL
#define in attribute
#define out varying
#endif

#ifndef RUNTIME_CONST
#define const
#endif

/* Uniforms */

/* This one is for both classic and UBOs, as it's usually set globally instead
   of changing per-draw */
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0)
#endif
uniform lowp vec2 viewportSize; /* defaults to zero */

#ifndef UNIFORM_BUFFERS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
#ifdef TWO_DIMENSIONS
uniform highp mat3 transformationProjectionMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;
#elif defined(THREE_DIMENSIONS)
uniform highp mat4 transformationProjectionMatrix
    #ifndef GL_ES
    = mat4(1.0)
    #endif
    ;
#else
#error
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
#endif
uniform mediump float width
    #ifndef GL_ES
    = 1.0
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 5)
#endif
uniform mediump float smoothness
    #ifndef GL_ES
    = 0.0
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 6)
#endif
uniform mediump float miterLimit
    #ifndef GL_ES
    /* cos(2*asin(1.0/4.0)), with 4 being the documented length limit */
    = 0.875
    #endif
    ;

/* Uniform buffers */

#else
#if DRAW_COUNT > 1
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform highp uint drawOffset
    #ifndef GL_ES
    = 0u
    #endif
    ;
#else
#define drawOffset 0u
#endif

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 1
    #endif
) uniform TransformationProjection {
    highp
        #ifdef TWO_DIMENSIONS
        /* Can't be a mat3 because of ANGLE, see DrawUniform in Phong.vert for
           details */
        mat3x4
        #elif defined(THREE_DIMENSIONS)
        mat4
        #else
        #error
        #endif
    transformationProjectionMatrices[DRAW_COUNT];
};

struct DrawUniform {
    highp uvec4 materialIdReservedObjectIdReservedReserved;
    #define draw_materialIdReserved materialIdReservedObjectIdReservedReserved.x
    #define draw_objectId materialIdReservedObjectIdReservedReserved.y
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 2
    #endif
) uniform Draw {
    DrawUniform draws[DRAW_COUNT];
};

struct MaterialUniform {
    lowp vec4 backgroundColor;
    lowp vec4 color;
    highp vec4 widthSmoothnessMiterLimitReserved;
    #define material_width widthSmoothnessMiterLimitReserved.x
    #define material_smoothness widthSmoothnessMiterLimitReserved.y
    #define material_miterLimit widthSmoothnessMiterLimitReserved.z
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 3
    #endif
) uniform Material {
    MaterialUniform materials[MATERIAL_COUNT];
};
#endif

/* Inputs */

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = POSITION_ATTRIBUTE_LOCATION)
#endif
#ifdef TWO_DIMENSIONS
in highp vec2 position;
#elif defined(THREE_DIMENSIONS)
/* Last component is reserved for line distance */
in highp vec3 position;
#else
#error
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = LINE_PREVIOUS_POSITION_ATTRIBUTE_LOCATION)
#endif
#ifdef TWO_DIMENSIONS
in highp vec2 previousPosition;
#elif defined(THREE_DIMENSIONS)
in highp vec3 previousPosition;
#else
#error
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = LINE_NEXT_POSITION_ATTRIBUTE_LOCATION)
#endif
#ifdef TWO_DIMENSIONS
in highp vec2 nextPosition;
#elif defined(THREE_DIMENSIONS)
in highp vec3 nextPosition;
#else
#error
#endif

/* Point annotation, matching the LineVertexAnnotation enum bits */
#define ANNOTATION_UP_MASK 1u
#define ANNOTATION_JOIN_MASK 2u
#define ANNOTATION_BEGIN_MASK 4u
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = LINE_ANNOTATION_ATTRIBUTE_LOCATION)
#endif
in lowp uint annotation;

#ifdef VERTEX_COLOR
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = COLOR_ATTRIBUTE_LOCATION)
#endif
in lowp vec4 vertexColor;
#endif

#ifdef INSTANCED_OBJECT_ID
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = OBJECT_ID_ATTRIBUTE_LOCATION)
#endif
in highp uint instanceObjectId;
#endif

#ifdef INSTANCED_TRANSFORMATION
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TRANSFORMATION_MATRIX_ATTRIBUTE_LOCATION)
#endif
#ifdef TWO_DIMENSIONS
in highp mat3 instancedTransformationMatrix;
#elif defined(THREE_DIMENSIONS)
in highp mat4 instancedTransformationMatrix;
#else
#error
#endif
#endif

/* Outputs */

#ifdef CAN_USE_NOPERSPECTIVE
noperspective
#endif
out highp vec2 centerDistanceSigned;
out highp float halfSegmentLength;
#ifdef CAN_USE_NOPERSPECTIVE
noperspective
#endif
out highp float hasCap;

#ifdef VERTEX_COLOR
out lowp vec4 interpolatedVertexColor;
#endif

#ifdef INSTANCED_OBJECT_ID
flat out highp uint interpolatedInstanceObjectId;
#endif

#ifdef MULTI_DRAW
flat out highp uint drawId;
#endif

/* Same as Math::Vector2::perpendicular() */
vec2 perpendicular(vec2 a) {
    return vec2(-a.y, a.x);
}

void main() {
    #ifdef UNIFORM_BUFFERS
    #ifdef MULTI_DRAW
    drawId = drawOffset + uint(
        #ifndef GL_ES
        gl_DrawIDARB /* Using GL_ARB_shader_draw_parameters, not GLSL 4.6 */
        #else
        gl_DrawID
        #endif
        );
    #else
    #define drawId drawOffset
    #endif

    #ifdef TWO_DIMENSIONS
    highp const mat3 transformationProjectionMatrix = mat3(transformationProjectionMatrices[drawId]);
    #elif defined(THREE_DIMENSIONS)
    highp const mat4 transformationProjectionMatrix = transformationProjectionMatrices[drawId];
    #else
    #error
    #endif
    #if MATERIAL_COUNT > 1
    mediump const uint materialId = draws[drawId].draw_materialIdReserved & 0xffffu;
    #else
    #define materialId 0u
    #endif
    mediump const float width = materials[materialId].material_width;
    mediump const float smoothness = materials[materialId].material_smoothness;
    highp const float miterLimit = materials[materialId].material_miterLimit;
    #endif

    #ifdef TWO_DIMENSIONS
    highp const vec2 transformedPosition = (transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec3(position, 1.0)).xy;

    highp const vec2 transformedPreviousPosition = (transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec3(previousPosition, 1.0)).xy;

    highp const vec2 transformedNextPosition = (transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec3(nextPosition, 1.0)).xy;
    #elif defined(THREE_DIMENSIONS)
    highp const vec4 transformedPosition4 = transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec4(position, 1.0);
    highp const vec2 transformedPosition = transformedPosition4.xy/transformedPosition4.w;

    highp const vec4 transformedPreviousPosition4 = transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec4(previousPosition, 1.0);
    highp const vec2 transformedPreviousPosition = transformedPreviousPosition4.xy/transformedPreviousPosition4.w;

    highp const vec4 transformedNextPosition4 = transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec4(nextPosition, 1.0);
    highp const vec2 transformedNextPosition = transformedNextPosition4.xy/transformedNextPosition4.w;
    #else
    #error
    #endif

    /* Decide about the line direction vector `d` and edge direction vector `e`
       from the `pointMarkerComponent` input. Quad corners 0 and 1 come from
       segment endpoint A, are marked with the POINT_MARKER_BEGIN_MASK bit and
       so their line direction is taken from `nextPosition`, quad corners 2 and
       3 come from B and are *not* marked with POINT_MARKER_BEGIN_MASK and so
       their line direction is taken from `previousPosition`, with the
       direction being always from point A to point B. The edge direction is
       then perpendicular to the line direction, with points 0 and 2 marked
       with POINT_MARKER_UP_MASK using it directly, while points 1 and 3
       don't have POINT_MARKER_UP_MASK and have to negate it:

                 ^        ^
                 e        e
                 |        |
     [UP, BEGIN] 0-d-->   2-d--> [UP]

                 A        B

         [BEGIN] 1-d-->   3-d--> []
                 |        |
                 e        e
                 v        v

       The POINT_MARKER_CAP_MASK is then used below. */
    highp const vec2 lineDirection = bool(annotation & ANNOTATION_BEGIN_MASK) ?
        transformedNextPosition - transformedPosition :
        transformedPosition - transformedPreviousPosition;
    mediump const float edgeSign = bool(annotation & ANNOTATION_UP_MASK) ? 1.0 : -1.0;
    mediump const float neighborSign = bool(annotation & ANNOTATION_BEGIN_MASK) ? -1.0 : 1.0;

    /* Line direction and its length converted from the [-1, 1] unit square to
       the screen space so we properly take aspect ratio into account. In the
       end it undoes the transformation by multiplying by 2.0/viewportSize
       again. */
    highp const vec2 screenspaceLineDirection = lineDirection*viewportSize/2.0;
    highp const float screenspaceLineDirectionLength = length(screenspaceLineDirection);

    /* Normalized screenspace line and edge direction. In case of zero-sized
       lines (i.e., points) the X axis is picked as line direction instead, and
       thus Y axis for edge direction. */
    highp const vec2 screenspaceLineDirectionNormalized = screenspaceLineDirectionLength == 0.0 ? vec2(1.0, 0.0) : screenspaceLineDirection/screenspaceLineDirectionLength;
    highp const vec2 screenspaceEdgeDirectionNormalized = perpendicular(screenspaceLineDirectionNormalized);

    /* Line width includes also twice the smoothness (because it's a radius
       instead of a diameter, and is on both sides of the line), and is rounded
       to whole pixels. So for the edge distance we need half of it. */
    mediump const float edgeDistance = ceil(width + 2.0*smoothness)*0.5;
    #ifdef CAP_STYLE_BUTT
    mediump const float capDistance = ceil(2.0*smoothness)*0.5;
    #elif defined(CAP_STYLE_SQUARE) || defined(CAP_STYLE_ROUND) || defined(CAP_STYLE_TRIANGLE)
    mediump const float capDistance = edgeDistance;
    #else
    #error
    #endif

    /* Line segment half-length, passed to the fragment shader. Same for all
       four points. */
    halfSegmentLength = screenspaceLineDirectionLength*0.5;

    /* Calculate the actual endpoint parameters depending on whether we're at a
       line cap, line join bevel, line join miter etc.

        -   `screenspacePointDirection` contains screenspace direction from
            `transformedPosition` to the actual point. After undoing the
            screenspace projection the sum of the two is written to
            gl_Position.
        -   `centerDistanceSigned` contains signed distance from the edge to
            center, passed to the fragment shader. It's chosen in a way that
            interpolates to zero in the quad center, and the area where
            `all(abs(centerDistanceSigned) <= vec2(halfSegmentLength +
            capDistance, edgeDistance))` is inside the line.
        -   `hasCap` contains `abs(centerDistanceSigned.x)` with a sign
            positive if the point is a cap and negative if it isn't. Given
            segment endpoints A and B (and quad points 0/1 and 2/3
            corresponding to these), the following cases can happen:

            -   if both have a cap, it's a negative value in both, thus has a
                constant negative value in the fragment shader
            -   if neither have a cap, it's a positive value in both, thus has
                a constant positive value in the fragment shader
            -   if one has a cap and the other not, it's a negative value in
                one and positive in the other, interpolating to zero in the
                quad center

       In the fragment shader, `abs(centerDistanceSigned)` and `sign(hasCap)`
       is then used to perform cap rendering and antialiasing. For example,
       with a standalone line segment that has square caps on both ends, the
       value of `centerDistanceSigned` is like in the following diagram, with
       `d` being `halfSegmentLength`, `w` being `edgeDistance`, `c` being
       `capDistance`, and an extra margin for `smoothness` indicated by `s` and
       the double border:

    [-d-c-s,+w+s]                 [+d+c+s,+w+s]
           0-----------------------------2
        [-d-c,+w]------------------[+d+c,+w]
           | |                         | |      hasCap[0] = hasCap[1] = +d+c+s
        [-d-c,0]        [0,0]      [+d+c,0]
           | |                         | |      hasCap[2] = hasCap[3] = +d+c+s
        [-d-c,-w]------------------[+d+c,-w]
           1-----------------------------3
    [-d-c-s,-w-s]                 [+d+c+s,-w+s]

       With a cap only on the left side, `centerDistanceSigned` would be like
       this. Note the absence of a smoothness margin on the right side:

    [-d-c-s,+w+s]                   [+d,+w+s]
           0---------------------------2
        [-d-c,+w]-------------------[+d,+w]
           | |                         |        hasCap[0] = hasCap[1] = +d+c+s
        [-d-c,0]        [0,0]       [+d,0]
           | |                         |        hasCap[2] = hasCap[3] = -d
        [-d-c,-w]-------------------[+d,-w]
           1---------------------------3
    [-d-c-s,-w-s]                   [+d,-w-s]

    */
    centerDistanceSigned =
        /* The the Y coordinate is same for all cases, X coordinate gets
           further adjusted below */
        vec2(halfSegmentLength*neighborSign, edgeDistance*edgeSign);
    highp vec2 screenspacePointDirection;

    /* Line join */
    if(bool(annotation & ANNOTATION_JOIN_MASK)) {
        /* Neighbor direction `nd`, needed to distinguish whether this is the
           inner or outer join point. Calculated with basically an inverse of
           the logic used to calculate `lineDirection`, with the neighbor
           direction always pointing from the A/B endpoint to the other
           neighbor line endpoint:

            <--nd-0 [BEGIN]   [END] 2-nd-->

                  A                 B

            <--nd-1 [BEGIN]   [END] 3-nd-->  */
        highp const vec2 neighborDirection = bool(annotation & ANNOTATION_BEGIN_MASK) ?
            transformedPreviousPosition - transformedPosition :
            transformedNextPosition - transformedPosition;
        /* Screenspace neighbor direction and its length, calculated
           equivalently to screenspace line direction above */
        highp const vec2 screenspaceNeighborDirectionNormalized = normalize(neighborDirection*viewportSize/2.0);

        /* If the edge direction vector `e` and the neighbor direction vector
           `nd` point to the opposite direction (i.e., their dot product is
           negative), this is an outer point of the line and a candidate for
           a bevel.

                 ^
                 e
                 |
            -d->-2
                 |\
             B   | nd
                 |  \
            -----3   v

           If a miter join is used instead of a bevel, the point is beveled
           only if the line direction `d` and neighbor direction `nd` is
           sharper than a limit (i.e., their dot product, or a cosine of their
           angle, is between `[-1, -miterLimit]`). */
        const bool outerBeveledPoint =
            dot(screenspaceEdgeDirectionNormalized*edgeSign, screenspaceNeighborDirectionNormalized) < 0.0
            #if defined(JOIN_STYLE_MITER)
            && dot(screenspaceLineDirectionNormalized*neighborSign, screenspaceNeighborDirectionNormalized) < -miterLimit
            #elif !defined(JOIN_STYLE_BEVEL)
            #error
            #endif
            ;

        /* Outer point of a beveled join -- although
           https://www.w3.org/TR/svg-strokes/#LineJoin doesn't define *what
           exactly* is a bevel, it's defined as "Cuts the outside edge off
           where a circle the diameter of the stroke intersects the stroke." at
           e.g. https://apike.ca/prog_svg_line_cap_join.html.

            0---   ----2a
            |          |^\
            |         | e -_
            |         | |ρ  \
            A--  ----|--B-e->2b
            |       |   |  _-|
            |       |   _-   |
            |      | _- |    |
            1--  --3    |    |
                   |    |    |
                        C

           Which ultimately means the `2a` and `2b` quad endpoints are simply
           the edge direction vector `e` away from point B, in one case with
           the `e` calculated from the AB segment, and in the other from the BC
           segment. */
        if(outerBeveledPoint) {
            screenspacePointDirection = screenspaceEdgeDirectionNormalized*edgeDistance*edgeSign;
            /* centerDistanceSigned doesn't need any adjustment, hasCap is set
               below for both */

        /* Otherwise it's either an outer point of a miter join (basically
           points 2a and 2b from above evaluated to the same position), or the
           inner point, which is the same for bevel and mitter joins. Given
           normalized direction `d` and neighbor direction `nd`,
           `normalized(d + nd)` is the "average" direction of the two and `perpendicular(normalized(d + nd))` gives us the direction from B to
           2 (or from 3 to B):

            0---    --------+---2
            |               | α/ \
            |             w | / j \
            |               |/     \
            A--  +_-----d->-B       \
            |       -_    α/α\       \
            |          -_ /   nd      \
            |     d + nd /-_   v       \
            1----   ----3    -_ \
                         \      -+
                          \       \
                                   C

           With `2α` being the angle between `d` and `nd`, `α` appears in two
           right triangles and the following holds, `w` being the edge distance
           from above, and `j` having the length that's needed to scale
           `perpendicular(normalized(d + nd))` to get point 2:

                     |d + nd|    w                 2 w
            sin(α) = -------- = ---   -->  |j| = --------
                      2 |d|     |j|              |d + nd|

           Then, vector j is the following, meaning we avoid the normalization
           square root completely:

                    perp(d + nd)   (2 w)perp(d + nd)
            j = |j| ------------ = -----------------
                      |d + nd|        dot(d + nd)

           Point 3 is then just in the opposite direction; for the other side
           it's done equivalently. */
        } else {
            highp const vec2 averageDirection = neighborSign*screenspaceLineDirectionNormalized + screenspaceNeighborDirectionNormalized;
            screenspacePointDirection = (perpendicular(averageDirection)*(neighborSign*edgeSign*2.0*edgeDistance/dot(averageDirection, averageDirection)));

            /* By projecting the point direction onto the line direction we
               get a signed distance from the endpoint, adjust center distance
               with that */
            centerDistanceSigned.x += dot(screenspacePointDirection, screenspaceLineDirectionNormalized);
        }

        /* No cap here, store a negative value. TODO If
           sign(centerDistanceSigned.x) is different from neighborSign, then
           the sign here should be taken based on whether the other point is a
           join -- add more bits to the vertex annotation? */
        hasCap = -abs(centerDistanceSigned.x);

    /* Line cap otherwise -- the quad corner 0/1/2/3 a sum of the signed cap
       distance (`cdS`) and signed edge distance vectors (`eDS`), which are
       formed by the line direction vector `d` and its perpendicular vector.
       Neighbor direction (i.e., the other input from the one used to calculate
       `lineDirection`) isn't used at all in this case.

          cDS
        0<---+----------
        |    ^
        |    | eDS
        |    |
        |    A--d-->
        |
        |
        |
        1---

       The signed center distance a sum of half segment length and the cap
       distance, multiplied by the cap sign (thus negative for points derived
       from A and positive for B). */
    } else {
        screenspacePointDirection =
            screenspaceLineDirectionNormalized*capDistance*neighborSign +
            screenspaceEdgeDirectionNormalized*edgeDistance*edgeSign;

        /* Add signed cap distance to the center distance */
        centerDistanceSigned.x += capDistance*neighborSign;

        /* Cap is here, store a positive value */
        hasCap = abs(centerDistanceSigned.x);
    }

    /* Undo the screenspace projection */
    highp const vec2 pointDirection = screenspacePointDirection*2.0/viewportSize;

    #ifdef TWO_DIMENSIONS
    gl_Position = vec4(transformedPosition + pointDirection, 0.0, 1.0);
    #elif defined(THREE_DIMENSIONS)
    gl_Position = vec4(transformedPosition4.xy + pointDirection*transformedPosition4.w, transformedPosition4.zw);
    #ifndef CAN_USE_NOPERSPECTIVE
    /* See the CAN_USE_NOPERSPECTIVE macro definition at the top for details */
    gl_Position /= gl_Position.w;
    #endif
    #else
    #error
    #endif

    #ifdef VERTEX_COLOR
    /* Vertex colors, if enabled */
    interpolatedVertexColor = vertexColor;
    #endif

    #ifdef INSTANCED_OBJECT_ID
    /* Instanced object ID, if enabled */
    interpolatedInstanceObjectId = instanceObjectId;
    #endif
}
