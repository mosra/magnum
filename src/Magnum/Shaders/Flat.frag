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

#ifndef NEW_GLSL
#define fragmentColor gl_FragColor
#define texture texture2D
#define in varying
#endif

#ifndef RUNTIME_CONST
#define const
#endif

/* Uniforms */

#ifndef UNIFORM_BUFFERS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 3)
#endif
uniform lowp vec4 color
    #ifndef GL_ES
    = vec4(1.0)
    #endif
    ;

#ifdef ALPHA_MASK
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
#endif
uniform lowp float alphaMask
    #ifndef GL_ES
    = 0.5
    #endif
    ;
#endif

#ifdef OBJECT_ID
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 5)
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
layout(location = 0)
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
    highp uvec4 materialIdReservedObjectIdJointOffsetPerInstanceJointCountReserved;
    #define draw_materialIdReserved materialIdReservedObjectIdJointOffsetPerInstanceJointCountReserved.x
    #define draw_objectId materialIdReservedObjectIdJointOffsetPerInstanceJointCountReserved.y
    #define draw_jointOffsetPerInstanceJointCount materialIdReservedObjectIdJointOffsetPerInstanceJointCountReserved.z
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 2
    #endif
) BUFFER_OR_UNIFORM Draw {
    BUFFER_READONLY DrawUniform draws[DRAW_COUNT];
};

struct MaterialUniform {
    lowp vec4 color;
    highp vec4 alphaMaskReservedReservedReserved;
    #define material_alphaMask alphaMaskReservedReservedReserved.x
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 4
    #endif
) BUFFER_OR_UNIFORM Material {
    BUFFER_READONLY MaterialUniform materials[MATERIAL_COUNT];
};
#endif

/* Textures */

#ifdef TEXTURED
#ifdef EXPLICIT_BINDING
layout(binding = 0)
#endif
uniform lowp
    #ifndef TEXTURE_ARRAYS
    sampler2D
    #else
    sampler2DArray
    #endif
    textureData;
#endif

#ifdef OBJECT_ID_TEXTURE
#ifdef EXPLICIT_BINDING
layout(binding = 5)
#endif
uniform lowp
    #ifndef TEXTURE_ARRAYS
    usampler2D
    #else
    usampler2DArray
    #endif
    objectIdTextureData;
#endif

/* Inputs */

#if defined(TEXTURED) || defined(OBJECT_ID_TEXTURE)
in mediump
    #ifndef TEXTURE_ARRAYS
    vec2
    #else
    vec3
    #endif
    interpolatedTextureCoordinates;
#endif

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
    lowp const vec4 color = materials[materialId].color;
    #ifdef ALPHA_MASK
    lowp const float alphaMask = materials[materialId].material_alphaMask;
    #endif
    #endif

    fragmentColor =
        #ifdef TEXTURED
        texture(textureData, interpolatedTextureCoordinates)*
        #endif
        #ifdef VERTEX_COLOR
        interpolatedVertexColor*
        #endif
        color;

    #ifdef ALPHA_MASK
    /* Using <= because if mask is set to 1.0, it should discard all, similarly
       as when using 0, it should only discard what's already invisible
       anyway. */
    if(fragmentColor.a <= alphaMask) discard;
    #endif

    #ifdef OBJECT_ID
    fragmentObjectId =
        #ifdef INSTANCED_OBJECT_ID
        interpolatedInstanceObjectId +
        #endif
        #ifdef OBJECT_ID_TEXTURE
        texture(objectIdTextureData, interpolatedTextureCoordinates).r +
        #endif
        objectId;
    #endif
}
