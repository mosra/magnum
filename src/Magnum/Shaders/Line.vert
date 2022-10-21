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

#if defined(INSTANCED_OBJECT_ID) && !defined(GL_ES) && !defined(NEW_GLSL)
#extension GL_EXT_gpu_shader4: require
#endif

#if defined(UNIFORM_BUFFERS) && defined(TEXTURE_ARRAYS) && !defined(GL_ES)
#extension GL_ARB_shader_bit_encoding: require
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
layout(location = 2)
#endif
uniform mediump float width
    #ifndef GL_ES
    = 1.0
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 3)
#endif
uniform mediump float smoothness
    #ifndef GL_ES
    = 0.0
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
#endif

/* Inputs */

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = POSITION_ATTRIBUTE_LOCATION)
#endif
#ifdef TWO_DIMENSIONS
in highp vec2 position;
#elif defined(THREE_DIMENSIONS)
in highp vec4 position;
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

// TODO document, maybe join together?
out highp vec2 centerDistanceSigned;
out highp float halfSegmentLength;
out lowp float hasCap;

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
    #endif

    // TODO look at the precision qualifiers, same for *.frag
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

    highp const vec2 direction = (gl_VertexID & 2) == 0 ?
        transformedNextPosition - transformedPosition :
        transformedPosition - transformedPreviousPosition;
    highp const vec2 neighborDirection = (gl_VertexID & 2) == 0 ?
        transformedPosition - transformedPreviousPosition :
        transformedNextPosition - transformedPosition;
    highp const vec2 firstPoint = (gl_VertexID & 2) == 0 ?
        transformedPosition : transformedPreviousPosition;
    highp const vec2 neighborEndPoint = (gl_VertexID & 2) == 0 ?
        transformedPreviousPosition : transformedNextPosition;

    highp const float directionLength = length(direction);
    halfSegmentLength = length(direction*0.5*viewportSize/2.0);
    // TODO zero-sized lines better? average from prev/next?
    highp const vec2 directionNormalized = directionLength == 0.0 ? vec2(1.0, 0.0) : direction/directionLength;

    /* Line width includes also twice the smoothness radius, some extra padding
       on top, and is rounded to whole pixels. So for the edge distance we need
       half of it. */
    // TODO ref the paper here; actually just drop all that, smoothstep FTW
    highp const float edgeDistance = ceil(width + 2.0*smoothness)*0.5;

    /* Copied from MeshTools::generateLines() internals for completenes. The
       position is always either `A` or `B` for all four quad corners, the `d`
       comes in the direction attribute, `pd`/`nd` in neighborDirection and
       the vertex order, which is (4n +) 0/1/2/3, in gl_VertexID.
    // TODO redo all this here, the whole comment is outdated
            0-d->-------2-d->
            |          / \
            A---------B   nd
            |        / \   v
            1-d->---3-d->   \
                     \   \    .
                      nd   .   .
                        v   C    . */
    /* The perpendicular direction is rotating 90° counterclockwise. Which
       means for points 1 and 3 (i.e., gl_VertexID not divisible by 2) we need
       to negate it to point the other way. */
    // TODO zero-sized lines do what?
    highp const float edgeSign = (gl_VertexID & 1) == 0 ? 1.0 : -1.0;
    highp const float capSign = (gl_VertexID & 2) == 0 ? -1.0 : 1.0;
    highp const float edgeDistanceSigned = edgeDistance*edgeSign;
//     vec2 edgeDirection = perpendicular(directionNormalized)*edgeDistanceSigned*2.0/viewportSize;

//     float centerDirection
    highp vec2 edgeDirection;

    /* Distance to center, passed to the fragment shader. It's chosen in a way
       that interpolates to a zero vector in the quad center, and the area
       where `abs(centerDirection) <= [d+w,w]` is inside the line.

       On the left is shown a line segment that has caps on both sides, thus
       has
       Inside the
       line strip (where there's no line caps, shown on the left) the X value
       is always 0, resulting in no antialiasing done on the beginning/end edge
       in order to join tightly with the neigboring segments.

        TODO what the hell, this is never 0, this is all wrong
          0------------------2              0-----------------------------2
        [0,+w]-------------[0,+w]        [-d-w,+w]------------------[+d+w,+w]
          |                  |              | |                         | |
        [0,0]     [0,0]    [0,0]         [-d-w,0]        [0,0]      [+d+w,0]
          |                  |              | |                         | |
        [0,-w]-------------[0,-w]        [-d-w,-w]------------------[+d+w,-w]
          1------------------3              1-----------------------------3 */
    centerDistanceSigned.y = edgeDistanceSigned;

    /* If the neighbor direction is a NaN, it means we're at the line cap --
       the quad gets extended beyond A or B in the direction of the segment,
       as shown with the points 0 and 1:

        0--+---
        |  |
        |  A--d->--
        |  |
        1--+---

       For points 0 and 1 it'll be in the negative direction `d`, for points 2
       and 3 in positive `d`. */
    if(all(isnan(neighborDirection)) ||
        /* Cap limit */
        // TODO make the 0.7 configurable; no actually drop it altogether
        dot(normalize(direction), normalize(neighborDirection)) < -0.99 ||
        /* Neighbor segment too short */
        // TODO why the 2*?? why the square??
        (abs(dot(perpendicular(normalize(direction))*viewportSize/2.0, (neighborEndPoint - firstPoint)*viewportSize/2.0)) < 2*edgeDistance*edgeDistance &&
            // TODO this is a wrong attempt to handle colinear, needs to
            //  calculate proper distance from a line segment instead or do
            //  something else entirely ffs
            dot(direction, neighborDirection) <= 0.0)
    ) {
        edgeDirection =
            (directionNormalized*capSign +
            perpendicular(directionNormalized)*edgeSign)*edgeDistance*2.0/viewportSize
            ;
        centerDistanceSigned.x = (edgeDistance + halfSegmentLength)*capSign;

    /* Otherwise we need to create a tight joint with the neighboring line
       segment, as shown with the points 2 and 3. Given normalized direction
       `d` and neighbor direction `nd`, `normalized(d + nd)` is the "average"
       direction of the two and `perpendicular(normalized(d + nd))` gives us
       the direction from B to 2 (or from 3 to B):

           --------+---2
                   | α/
                 w | / j
                   |/
        +_-----d->-B
           -_    α/α\
              -_ /   nd
         d + nd /-_   v
           ----3    -_ \
                       -+

       With `2α` being the angle between `d` and `nd`, `α` appears in two right
       triangles and the following holds, `w` being the edge distance from
       above, and `j` being the length that's needed to scale
       `perpendicular(normalized(d + nd))` to get point 2:

                 |d + nd|    w               2 w
        sin(α) = -------- = ---   -->  j = --------
                  2 |d|      j             |d + nd|

       Point 3 is then just in the opposite direction; for the other side it's
       done equivalently. */
    } else {
        const highp vec2 averageDirection = capSign*(directionNormalized + normalize(neighborDirection));
        const highp float averageDirectionLength = length(averageDirection);
        const highp float j = 2.0*edgeDistance/averageDirectionLength;
        edgeDirection = (normalize(perpendicular(averageDirection))*capSign*edgeSign*j)*2.0/viewportSize;

        const highp float ex = sqrt(j*j - edgeDistance*edgeDistance);

        centerDistanceSigned.x = halfSegmentLength*capSign + ex*sign(dot(direction*capSign, (perpendicular(averageDirection))*edgeSign));
    }

    /* Cap is there only if neighbors are NaN, otherwise a joint is rendered */
    // TODO uhhhhh document why the sign comparison
    if(all(isnan(neighborDirection)) || sign(centerDistanceSigned.x) != sign(halfSegmentLength*capSign))
        hasCap = abs(centerDistanceSigned.x);
    else
        hasCap = -abs(centerDistanceSigned.x);

    gl_Position.xyzw = vec4(transformedPosition + edgeDirection, 0.0, 1.0);
    #elif defined(THREE_DIMENSIONS)
    // TODO 3D, how to handle perspective? multiply edgeDistance with w?
    // TODO also how to handle depth?
    gl_Position = transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        position;
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
