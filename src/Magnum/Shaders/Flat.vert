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

#if defined(INSTANCED_OBJECT_ID) && !defined(GL_ES) && !defined(NEW_GLSL)
#extension GL_EXT_gpu_shader4: require
#endif

#if defined(UNIFORM_BUFFERS) && defined(TEXTURE_ARRAYS) && !defined(GL_ES)
#extension GL_ARB_shader_bit_encoding: require
#endif

#if defined(SHADER_STORAGE_BUFFERS) && !defined(GL_ES)
#extension GL_ARB_shader_storage_buffer_object: require
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

/* Both classic uniforms and uniform buffers */

#ifdef DYNAMIC_PER_VERTEX_JOINT_COUNT
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = PER_VERTEX_JOINT_COUNT_LOCATION)
#endif
uniform mediump uvec2 perVertexJointCount
    #ifndef GL_ES
    = uvec2(PER_VERTEX_JOINT_COUNT, SECONDARY_PER_VERTEX_JOINT_COUNT)
    #endif
    ;
#endif

/* Uniforms */

#ifndef UNIFORM_BUFFERS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0)
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

#ifdef TEXTURE_TRANSFORMATION
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform mediump mat3 textureMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;
#endif

#ifdef TEXTURE_ARRAYS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 2)
#endif
/* mediump is just 2^10, which might not be enough, this is 2^16 */
uniform highp uint textureLayer; /* defaults to zero */
#endif

#ifdef JOINT_COUNT
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 6)
#endif
#ifdef TWO_DIMENSIONS
uniform mat3 jointMatrices[JOINT_COUNT]
    #ifndef GL_ES
    = mat3[](JOINT_MATRIX_INITIALIZER)
    #endif
    ;
#elif defined(THREE_DIMENSIONS)
uniform mat4 jointMatrices[JOINT_COUNT]
    #ifndef GL_ES
    = mat4[](JOINT_MATRIX_INITIALIZER)
    #endif
    ;
#else
#error
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = PER_INSTANCE_JOINT_COUNT_LOCATION)
#endif
uniform uint perInstanceJointCount; /* defaults to zero */
#endif

/* Uniform / shader storage buffers */

#else
/* For SSBOs, the per-draw and joint arrays are unbounded */
#ifdef SHADER_STORAGE_BUFFERS
#define DRAW_COUNT
/* Define JOINT_COUNT only if there are any per-vertex attributes, otherwise
   the buffer would be useless */
#if defined(PER_VERTEX_JOINT_COUNT) || defined(SECONDARY_PER_VERTEX_JOINT_COUNT)
#define JOINT_COUNT
#endif
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

#ifdef JOINT_COUNT
layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 6
    #endif
) BUFFER_OR_UNIFORM Joint {
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
    jointMatrices[JOINT_COUNT];
};
#endif

#ifdef TEXTURE_TRANSFORMATION
struct TextureTransformationUniform {
    highp vec4 rotationScaling;
    highp vec4 offsetLayerReserved;
    #define textureTransformation_offset offsetLayerReserved.xy
    #define textureTransformation_layer offsetLayerReserved.z
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 3
    #endif
) BUFFER_OR_UNIFORM TextureTransformation {
    BUFFER_READONLY TextureTransformationUniform textureTransformations[DRAW_COUNT];
};
#endif
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

#ifdef TEXTURED
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TEXTURECOORDINATES_ATTRIBUTE_LOCATION)
#endif
in mediump vec2 textureCoordinates;
#endif

#ifdef VERTEX_COLOR
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = COLOR_ATTRIBUTE_LOCATION)
#endif
in lowp vec4 vertexColor;
#endif

#ifdef JOINT_COUNT
#if PER_VERTEX_JOINT_COUNT
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = WEIGHTS_ATTRIBUTE_LOCATION)
#endif
in mediump vec4 weights;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = JOINTIDS_ATTRIBUTE_LOCATION)
#endif
in mediump uvec4 jointIds;
#endif

#if SECONDARY_PER_VERTEX_JOINT_COUNT
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = SECONDARY_WEIGHTS_ATTRIBUTE_LOCATION)
#endif
in mediump vec4 secondaryWeights;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = SECONDARY_JOINTIDS_ATTRIBUTE_LOCATION)
#endif
in mediump uvec4 secondaryJointIds;
#endif
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

#ifdef INSTANCED_TEXTURE_OFFSET
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TEXTURE_OFFSET_ATTRIBUTE_LOCATION)
#endif
in mediump
    #ifndef TEXTURE_ARRAYS
    vec2
    #else
    vec3
    #endif
    instancedTextureOffset;
#endif

/* Outputs */

#ifdef TEXTURED
out mediump
    #ifndef TEXTURE_ARRAYS
    vec2
    #else
    vec3
    #endif
    interpolatedTextureCoordinates;
#endif

#ifdef VERTEX_COLOR
out lowp vec4 interpolatedVertexColor;
#endif

#ifdef INSTANCED_OBJECT_ID
flat out highp uint interpolatedInstanceObjectId;
#endif

#ifdef MULTI_DRAW
flat out highp uint drawId;
#endif

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
    #ifdef TEXTURE_TRANSFORMATION
    mediump const mat3 textureMatrix = mat3(textureTransformations[drawId].rotationScaling.xy, 0.0, textureTransformations[drawId].rotationScaling.zw, 0.0, textureTransformations[drawId].textureTransformation_offset, 1.0);
    #ifdef TEXTURE_ARRAYS
    highp const uint textureLayer = floatBitsToUint(textureTransformations[drawId].textureTransformation_layer);
    #endif
    #endif
    #ifdef JOINT_COUNT
    mediump const uint jointOffset = (draws[drawId].draw_jointOffsetPerInstanceJointCount & 0xffffu) + uint(gl_InstanceID)*(draws[drawId].draw_jointOffsetPerInstanceJointCount >> 16 & 0xffffu);
    #endif
    #else
    #ifdef JOINT_COUNT
    mediump const uint jointOffset = uint(gl_InstanceID)*perInstanceJointCount;
    #endif
    #endif

    #ifdef JOINT_COUNT
    #ifdef TWO_DIMENSIONS
    mat3 skinMatrix = mat3(0.0);
    #elif defined(THREE_DIMENSIONS)
    mat4 skinMatrix = mat4(0.0);
    #else
    #error
    #endif
    #if PER_VERTEX_JOINT_COUNT
    for(uint i = 0u; i != PER_VERTEX_JOINT_COUNT
        #ifdef DYNAMIC_PER_VERTEX_JOINT_COUNT
        && i != perVertexJointCount.x
        #endif
    ; ++i)
        skinMatrix += weights[i]*
            #ifdef TWO_DIMENSIONS
            mat3 /* need to slice because it's a mat3x4 due to ANGLE, see
                    DrawUniform in Phong.vert for details */
            #endif
            (jointMatrices[jointOffset + jointIds[i]]);
    #endif
    #if SECONDARY_PER_VERTEX_JOINT_COUNT
    for(uint i = 0u; i != SECONDARY_PER_VERTEX_JOINT_COUNT
        #ifdef DYNAMIC_PER_VERTEX_JOINT_COUNT
        && i != perVertexJointCount.y
        #endif
    ; ++i)
        skinMatrix += secondaryWeights[i]*
            #ifdef TWO_DIMENSIONS
            mat3 /* need to slice because it's a mat3x4 due to ANGLE, see
                    DrawUniform in Phong.vert for details */
            #endif
            (jointMatrices[jointOffset + secondaryJointIds[i]]);
    #endif
    #endif

    #ifdef TWO_DIMENSIONS
    gl_Position.xywz = vec4(transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        #ifdef JOINT_COUNT
        skinMatrix*
        #endif
        vec3(position, 1.0), 0.0);
    #elif defined(THREE_DIMENSIONS)
    gl_Position = transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        #ifdef JOINT_COUNT
        skinMatrix*
        #endif
        position;
    #else
    #error
    #endif

    #ifdef TEXTURED
    /* Texture coordinates, if needed */
    interpolatedTextureCoordinates.xy =
        #ifdef TEXTURE_TRANSFORMATION
        (textureMatrix*vec3(
            #ifdef INSTANCED_TEXTURE_OFFSET
            instancedTextureOffset.xy +
            #endif
            textureCoordinates, 1.0)).xy
        #else
        textureCoordinates
        #endif
        ;
    #ifdef TEXTURE_ARRAYS
    interpolatedTextureCoordinates.z = float(
        #ifdef INSTANCED_TEXTURE_OFFSET
        uint(instancedTextureOffset.z) +
        #endif
        textureLayer
    );
    #endif
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
