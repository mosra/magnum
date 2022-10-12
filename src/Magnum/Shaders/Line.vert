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
layout(location = LINE_DIRECTION_ATTRIBUTE_LOCATION)
#endif
#ifdef TWO_DIMENSIONS
in highp vec2 direction_;
#elif defined(THREE_DIMENSIONS)
in highp vec3 direction_;
#else
#error
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = LINE_NEIGHBOR_DIRECTION_ATTRIBUTE_LOCATION)
#endif
#ifdef TWO_DIMENSIONS
in highp vec2 neighborDirection_;
#elif defined(THREE_DIMENSIONS)
in highp vec3 neighborDirection_;
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

out highp vec2 centerDistanceSigned;
out highp float halfSegmentLength;

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

    #ifdef TWO_DIMENSIONS
    const vec2 lineCenterPosition = (transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec3(position, 1.0)).xy;

    // TODO take prev/next positions, this makes no sense
    const vec2 direction = (transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec3(position + direction_, 1.0)).xy - lineCenterPosition;

    const vec2 neighborDirection = (transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        vec3(position + neighborDirection_, 1.0)).xy - lineCenterPosition;

    const float directionLength = length(direction);
    // TODO since this is used just for AA, set to a special value in case of
    //  a line joint that shouldn't be AAd?
    halfSegmentLength = length(direction*0.5*viewportSize/2.0);
    // TODO zero-sized lines better?
    const vec2 directionNormalized = directionLength == 0.0 ? vec2(1.0, 0.0) : direction/directionLength;

    /* Line width includes also twice the smoothness radius, some extra padding
       on top, and is rounded to whole pixels. So for the edge distance we need
       half of it. */
    // TODO ref the paper here; actually just drop all that, smoothstep FTW
    const float edgeDistance = ceil(width + 2.0*smoothness)*0.5;

    /* Copied from MeshTools::generateLines() internals for completenes. The
       position is always either `A` or `B` for all four quad corners, the `d`
       comes in the direction attribute, `pd`/`nd` in neighborDirection and
       the vertex order, which is (4n +) 0/1/2/3, in gl_VertexID.

            0-d->-------2-d->
            |          / \
            A---------B   nd
            |        / \   v
            1-d->---3-d->   \
                     \   \    .
                      nd   .   .
                        v   C    . */
    // TODO redo all this here
    /* The perpendicular direction is rotating 90° counterclockwise. Which
       means for points 1 and 3 (i.e., gl_VertexID not divisible by 2) we need
       to negate it to point the other way. */
    // TODO zero-sized lines do what?
    const float edgeSign = (gl_VertexID & 1) == 0 ? 1.0 : -1.0;
    const float capSign = (gl_VertexID & 2) == 0 ? -1.0 : 1.0;
    const float edgeDistanceSigned = edgeDistance*edgeSign;
//     vec2 edgeDirection = perpendicular(directionNormalized)*edgeDistanceSigned*2.0/viewportSize;

//     float centerDirection
    vec2 edgeDirection;

    /* Distance to center, passed to the fragment shader. It's chosen in a way
       that interpolates to a zero vector in the quad center, and the area
       where `abs(centerDirection) <= [d+w,w]` is inside the line. Inside the
       line strip (where there's no line caps, shown on the left) the X value
       is always 0, resulting in no antialiasing done on the beginning/end edge
       in order to join tightly with the neigboring segments.

          0------------------2              0-----------------------------2
        [0,+w]-------------[0,+w]        [-d-w,+w]------------------[+d+w,+w]
          |                  |              | |                         | |
        [0,0]     [0,0]    [0,0]         [-d-w,0]        [0,0]      [+d+w,0]
          |                  |              | |                         | |
        [0,-w]-------------[0,-w]        [-d-w,-w]------------------[+d+w,-w]
          1------------------3              1-----------------------------3 */
    // TODO handle caps
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
    if(all(isnan(neighborDirection))) {
        edgeDirection =
            (directionNormalized*capSign +
            perpendicular(directionNormalized)*edgeSign)*edgeDistance*2.0/viewportSize
            ;
        centerDistanceSigned.x = (edgeDistance + halfSegmentLength)*capSign;

    /* Otherwise we need to create a tight joint with the neighboring line
       segment, as shown with the points 2 and 3. Given normalized direction
       `d` and neighbor direction `nd`, `normalized(d + nd)` is the "average"
       direction of the two and `perpendicular(normalized(d + nd))` gives us
       the direction from B to 2:

          --------+----2
                  |  / alpha/2
                w | / j
                  |/
            --d->-B
         alpha/2 / \
                /   nd
               /     v
          ----3

       With `alpha` being the angle between `d` and `nd`, `alpha/2` appears in
       two right triangles and the following holds, `w` being the edge distance
       from above, and `j` being the length that's needed to scale  `perpendicular(normalized(d + nd))` to get point 2:

                       |d + nd|    w               2 w
        sin(alpha/2) = -------- = ---   -->  j = --------
                        2 |d|      j             |d + nd|

       Point 3 is then just in the opposite direction; for the other side it's
       done equivalently. */
    } else {
//         edgeDirection = perpendicular(directionNormalized)*edgeSign*edgeDistance*2.0/viewportSize;

        const vec2 averageDirection = capSign*directionNormalized + normalize(neighborDirection);
        const float averageDirectionLength = length(averageDirection);
        edgeDirection = perpendicular(averageDirection)*(capSign*edgeSign*edgeDistance*4.0/averageDirectionLength)/viewportSize;

//         const float ex = sqrt((4.0*edgeDistance*edgeDistance/(averageDirectionLength*averageDirectionLength)) - edgeDistance*edgeDistance);
        const float ex = sqrt(dot(edgeDirection, edgeDirection) - edgeDistance*edgeDistance);

//         edgeDirection = -perpendicular(averageDirection)*(1.0*edgeDistance*edgeSign*capSign/averageDirectionLength)
//             *2.0/viewportSize;

        // TODO it can't be 0!! it also has to include the skew in here but
        //  then somehow be marked to not smooth
        centerDistanceSigned.x = halfSegmentLength*capSign;// + ex*sign(dot(direction*capSign, edgeDirection));
    }

    // TODO cap ends / joints, depending on neighborDirection size

    gl_Position.xyzw = vec4(lineCenterPosition + edgeDirection, 0.0, 1.0);
    #elif defined(THREE_DIMENSIONS)
    // TODO 3D, how to handle perspective? multiply edgeDistance with w?
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
