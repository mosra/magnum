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
uniform highp mat4 transformationMatrix
    #ifndef GL_ES
    = mat4(1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform highp mat4 projectionMatrix
    #ifndef GL_ES
    = mat4(1.0)
    #endif
    ;

#ifdef HAS_LIGHTS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 2)
#endif
uniform mediump mat3 normalMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;
#endif

#ifdef TEXTURE_TRANSFORMATION
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 3)
#endif
uniform mediump mat3 textureMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;
#endif

#ifdef TEXTURE_ARRAYS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
#endif
/* mediump is just 2^10, which might not be enough, this is 2^16 */
uniform highp uint textureLayer; /* defaults to zero */
#endif

#ifdef JOINT_COUNT
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = JOINT_MATRICES_LOCATION)
#endif
uniform mat4 jointMatrices[JOINT_COUNT]
    #ifndef GL_ES
    = mat4[](JOINT_MATRIX_INITIALIZER)
    #endif
    ;

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

/* Keep in sync with Phong.frag. Can't "outsource" to a common file because
   the #extension directive needs to be always before any code. */
struct DrawUniform {
    /* Of all drivers, I made the crucial mistake of expecting ANGLE to have
       non-broken uniform packing. Of course everything including random phone
       drivers worked, except ANGLE with a D3D backend, which blew up when
       seeing `mat3` here. With all the coding guidelines, rules and automated
       bullying from Clang Tidy in place over at Google, it seems the false
       sense of security is so strong that they don't even bother testing what
       they wrote. Or, how to code, the Google way:

       1. Thoroughly document the packing rules and how the translation of
          every GLSL type to the D3D equivalent is performed:
          https://chromium.googlesource.com/angle/angle/+/refs/heads/main/src/libANGLE/renderer/d3d/d3d11/UniformBlockToStructuredBufferTranslation.md#std140-limitation
       2. Forget to actually implement and test the damn thing.
    */
    mediump mat3x4 normalMatrix;
    highp uvec4 materialIdReservedObjectIdLightOffsetLightCountJointOffsetPerInstanceJointCount;
    #define draw_materialIdReserved materialIdReservedObjectIdLightOffsetLightCountJointOffsetPerInstanceJointCount.x
    #define draw_objectId materialIdReservedObjectIdLightOffsetLightCountJointOffsetPerInstanceJointCount.y
    #define draw_lightOffsetLightCount materialIdReservedObjectIdLightOffsetLightCountJointOffsetPerInstanceJointCount.z
    #define draw_jointOffsetPerInstanceJointCount materialIdReservedObjectIdLightOffsetLightCountJointOffsetPerInstanceJointCount.w
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
    , binding = 0
    #endif
) BUFFER_OR_UNIFORM Projection {
    BUFFER_READONLY highp mat4 projectionMatrix;
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 1
    #endif
) BUFFER_OR_UNIFORM Transformation {
    BUFFER_READONLY highp mat4 transformationMatrices[DRAW_COUNT];
};

#ifdef JOINT_COUNT
layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 6
    #endif
) BUFFER_OR_UNIFORM Joint {
    BUFFER_READONLY highp mat4 jointMatrices[JOINT_COUNT];
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
in highp vec4 position;

#ifdef HAS_LIGHTS
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = NORMAL_ATTRIBUTE_LOCATION)
#endif
in mediump vec3 normal;

#ifdef NORMAL_TEXTURE
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TANGENT_ATTRIBUTE_LOCATION)
#endif
in mediump
    #ifndef BITANGENT
    vec4
    #else
    vec3
    #endif
    tangent;
#endif

#ifdef BITANGENT
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = BITANGENT_ATTRIBUTE_LOCATION)
#endif
in mediump vec3 bitangent;
#endif
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
in highp mat4 instancedTransformationMatrix;

#ifdef HAS_LIGHTS
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = NORMAL_MATRIX_ATTRIBUTE_LOCATION)
#endif
in highp mat3 instancedNormalMatrix;
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

#ifdef HAS_LIGHTS
out mediump vec3 transformedNormal;
#ifdef NORMAL_TEXTURE
#ifndef BITANGENT
out mediump vec4 transformedTangent;
#else
out mediump vec3 transformedTangent;
out mediump vec3 transformedBitangent;
#endif
#endif
out highp vec3 transformedPosition;
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

    highp const mat4 transformationMatrix = transformationMatrices[drawId];
    #ifdef HAS_LIGHTS
    mediump const mat3 normalMatrix = mat3(draws[drawId].normalMatrix);
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
    mat4 skinMatrix = mat4(0.0);
    #if PER_VERTEX_JOINT_COUNT
    for(uint i = 0u; i != PER_VERTEX_JOINT_COUNT
        #ifdef DYNAMIC_PER_VERTEX_JOINT_COUNT
        && i != perVertexJointCount.x
        #endif
    ; ++i)
        skinMatrix += weights[i]*jointMatrices[jointOffset + jointIds[i]];
    #endif
    #if SECONDARY_PER_VERTEX_JOINT_COUNT
    for(uint i = 0u; i != SECONDARY_PER_VERTEX_JOINT_COUNT
        #ifdef DYNAMIC_PER_VERTEX_JOINT_COUNT
        && i != perVertexJointCount.y
        #endif
    ; ++i)
        skinMatrix += secondaryWeights[i]*jointMatrices[jointOffset + secondaryJointIds[i]];
    #endif
    #endif

    /* Transformed vertex position */
    highp vec4 transformedPosition4 = transformationMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        #ifdef JOINT_COUNT
        skinMatrix*
        #endif
        position;
    #ifndef HAS_LIGHTS
    highp vec3
    #endif
    transformedPosition = transformedPosition4.xyz/transformedPosition4.w;

    #ifdef HAS_LIGHTS
    /* Transformed normal and tangent vector */
    transformedNormal = normalMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedNormalMatrix*
        #endif
        normal;
    #ifdef NORMAL_TEXTURE
    #ifndef BITANGENT
    transformedTangent = vec4(normalMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedNormalMatrix*
        #endif
        tangent.xyz, tangent.w);
    #else
    transformedTangent = normalMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedNormalMatrix*
        #endif
        tangent;
    transformedBitangent = normalMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedNormalMatrix*
        #endif
        bitangent;
    #endif
    #endif
    #endif

    /* Transform the position */
    gl_Position = projectionMatrix*transformedPosition4;

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
