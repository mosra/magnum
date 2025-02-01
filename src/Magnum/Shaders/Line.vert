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

#if !defined(GL_ES) && !defined(NEW_GLSL)
#extension GL_EXT_gpu_shader4: require
#endif

#if defined(SHADER_STORAGE_BUFFERS) && !defined(GL_ES)
#extension GL_ARB_shader_storage_buffer_object: require
#endif

/* Use the noperspective keyword to avoid artifacts in screen-space
   interpolation if perspective projection is used in 3D. If not available,
   it's worked around by dividing gl_Position with gl_Position.w (which is
   extra instructions, so the noperspective keyword is preferred). */
#ifndef GL_ES
#define CAN_USE_NOPERSPECTIVE
#elif defined(GL_NV_shader_noperspective_interpolation)
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

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 1
    #endif
) BUFFER_OR_UNIFORM TransformationProjection {
    BUFFER_READONLY highp
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
flat out highp float halfSegmentLength;
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

/* Coming from Line.in.vert. That file is added after this one in order to have
   #extension directives before any code. */
highp vec2 expandLineVertex(
    in highp const vec2 transformedPosition,
    in highp const vec2 transformedPreviousPosition,
    in highp const vec2 transformedNextPosition,
    in lowp const uint annotation,
    in mediump const float width,
    in mediump const float smoothness,
    in highp const float miterLimit,
    in lowp const vec2 viewportSize,
    out highp vec2 centerDistanceSigned,
    out highp float halfSegmentLength,
    out highp float hasCap);

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
    /* With SSBOs MATERIAL_COUNT is defined to be empty, +0 makes the condition
       not cause a compile error */
    #if defined(SHADER_STORAGE_BUFFERS) || MATERIAL_COUNT+0 > 1
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

    highp const vec2 pointDirection = expandLineVertex(
        transformedPosition,
        transformedPreviousPosition,
        transformedNextPosition,
        annotation,
        width,
        smoothness,
        miterLimit,
        viewportSize,
        centerDistanceSigned,
        halfSegmentLength,
        hasCap);

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
