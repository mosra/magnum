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

#if defined(OBJECT_ID) && !defined(GL_ES) && !defined(NEW_GLSL)
#extension GL_EXT_gpu_shader4: require
#endif

#if defined(SHADER_STORAGE_BUFFERS) && !defined(GL_ES)
#extension GL_ARB_shader_storage_buffer_object: require
#endif

/* See the corresponding block in Line.vert for more information */
#ifndef GL_ES
#define CAN_USE_NOPERSPECTIVE
#elif defined(GL_NV_shader_noperspective_interpolation)
#extension GL_NV_shader_noperspective_interpolation: require
#define CAN_USE_NOPERSPECTIVE
#endif

#ifndef NEW_GLSL
#define fragmentColor gl_FragColor
#define in varying
#endif

#ifndef RUNTIME_CONST
#define const
#endif

/* Uniforms */

#ifndef UNIFORM_BUFFERS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 2)
#endif
uniform lowp vec4 backgroundColor
    #ifndef GL_ES
    = vec4(0.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 3)
#endif
uniform lowp vec4 color
    #ifndef GL_ES
    = vec4(1.0)
    #endif
    ;

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

#ifdef OBJECT_ID
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 7)
#endif
/* mediump is just 2^10, which might not be enough, this is 2^16 */
uniform highp uint objectId; /* defaults to zero */
#endif

/* Uniform / shader storage buffers */

#else
/* For SSBOs, the per-draw and material arrays are unbounded */
#ifdef SHADER_STORAGE_BUFFERS
#define DRAW_COUNT
#define MATERIAL_COUNT
#define BUFFER_OR_UNIFORM buffer
#define BUFFER_READONLY readonly
#else
#define BUFFER_OR_UNIFORM uniform
#define BUFFER_READONLY
#endif

#ifndef MULTI_DRAW
/* With SSBOs DRAW_COUNT is defined to be empty, +0 makes the condition not
   cause a compile error */
#if defined(SHADER_STORAGE_BUFFERS) || DRAW_COUNT+0 > 1
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
#define drawId drawOffset
#endif

struct DrawUniform {
    highp uvec4 materialIdReservedObjectIdReservedReserved;
    #define draw_materialIdReserved materialIdReservedObjectIdReservedReserved.x
    #define draw_objectId materialIdReservedObjectIdReservedReserved.y
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 2
    #endif
) BUFFER_OR_UNIFORM Draw {
    BUFFER_READONLY DrawUniform draws[DRAW_COUNT];
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
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 3
    #endif
) BUFFER_OR_UNIFORM Material {
    BUFFER_READONLY MaterialUniform materials[MATERIAL_COUNT];
};
#endif

/* Inputs */

#ifdef CAN_USE_NOPERSPECTIVE
noperspective
#endif
in highp vec2 centerDistanceSigned;
in highp float halfSegmentLength;
#ifdef CAN_USE_NOPERSPECTIVE
noperspective
#endif
in highp float hasCap;

#ifdef VERTEX_COLOR
in lowp vec4 interpolatedVertexColor;
#endif

#ifdef INSTANCED_OBJECT_ID
flat in highp uint interpolatedInstanceObjectId;
#endif

#ifdef MULTI_DRAW
flat in highp uint drawId;
#endif

/* Outputs */

#ifdef NEW_GLSL
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = COLOR_OUTPUT_ATTRIBUTE_LOCATION)
#endif
out lowp vec4 fragmentColor;
#endif
#ifdef OBJECT_ID
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = OBJECT_ID_OUTPUT_ATTRIBUTE_LOCATION)
#endif
/* mediump is just 2^10, which might not be enough, this is 2^16 */
out highp uint fragmentObjectId;
#endif

void main() {
    #ifdef UNIFORM_BUFFERS
    #ifdef OBJECT_ID
    highp const uint objectId = draws[drawId].draw_objectId;
    #endif
    /* With SSBOs MATERIAL_COUNT is defined to be empty, +0 makes the condition
       not cause a compile error */
    #if defined(SHADER_STORAGE_BUFFERS) || MATERIAL_COUNT+0 > 1
    mediump const uint materialId = draws[drawId].draw_materialIdReserved & 0xffffu;
    #else
    #define materialId 0u
    #endif
    lowp const vec4 backgroundColor = materials[materialId].backgroundColor;
    lowp const vec4 color = materials[materialId].color;
    mediump const float width = materials[materialId].material_width;
    mediump const float smoothness = materials[materialId].material_smoothness;
    #endif

    /* Calculate a distance from the original line endpoint (B). Assuming a cap
       that's not a butt, actual quad vertices (2, 3 on the left diagram) would
       be at a distance `width/2` in both X and Y (in the space of the line
       segment, where X is in direction of the segment and Y is in direction to
       the line edges):

          ----------2                       --------2
                    |                               |
        [0,0]   B   |                     [0,0]     B
                    |                               |
          ----------3                       --------3

       For a butt cap, the endpoint B would be at the edge instead (right
       diagram) -- to have handling consistent for all cap styles, add
       `width/2` to the center distance in that case. For fragments on the left
       of B the X distance would be negative, make it 0 in that case
       instead. */
    highp const vec2 centerDistance = abs(centerDistanceSigned);
    highp vec2 endpointDistance = vec2(max(centerDistance.x
        #ifdef CAP_STYLE_BUTT
        + width*0.5
        #endif
        - halfSegmentLength, 0.0), centerDistance.y);

    /* If hasCap is negative, it means the nearest endpoint is a join, not a
       cap. Thus no smoothing happens in the direction of a cap, i.e. same as
       if we'd be at the center of the line. */
    if(hasCap < 0.0) endpointDistance.x = 0.0;

    /* Calculate a single distance factor out of the two-dimensional endpoint
       distance. This will form the cap shape. */
    #if defined(CAP_STYLE_BUTT) || defined(CAP_STYLE_SQUARE)
    highp const float distance1D = max(endpointDistance.x, endpointDistance.y);
    #elif defined(CAP_STYLE_ROUND)
    highp const float distance1D = length(endpointDistance);
    #elif defined(CAP_STYLE_TRIANGLE)
    highp const float distance1D = endpointDistance.x + endpointDistance.y;
    #else
    #error
    #endif

    mediump const float factor = smoothstep(width*0.5 - smoothness, width*0.5 + smoothness, distance1D);

    fragmentColor = mix(
        #ifdef VERTEX_COLOR
        interpolatedVertexColor*
        #endif
        color, backgroundColor, factor);

    #ifdef OBJECT_ID
    fragmentObjectId =
        #ifdef INSTANCED_OBJECT_ID
        interpolatedInstanceObjectId +
        #endif
        objectId;
    #endif
}
