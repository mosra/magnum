/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#ifndef UNIFORM_BUFFERS
#ifdef TWO_DIMENSIONS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 6)
#endif
uniform highp mat3 transformationProjectionMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;
#elif defined(THREE_DIMENSIONS)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 6)
#endif
uniform highp mat4 transformationMatrix
    #ifndef GL_ES
    = mat4(1.0)
    #endif
    ;
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 7)
#endif
uniform highp mat4 projectionMatrix
    #ifndef GL_ES
    = mat4(1.0)
    #endif
    ;
#else
#error
#endif

#ifdef VERTEX_ID
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 5)
#endif
uniform lowp vec2 colorMapOffsetScale
    #ifndef GL_ES
    = vec2(1.0/512.0, 1.0/256.0)
    #endif
    ;
#define colorMapOffset colorMapOffsetScale.x
#define colorMapScale colorMapOffsetScale.y
#endif

#if defined(TANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 8)
#endif
uniform highp mat3 normalMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 10)
#endif
uniform highp float lineLength
    #ifndef GL_ES
    = 1.0
    #endif
    ;
#endif

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

#ifdef TWO_DIMENSIONS
layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 1
    #endif
) uniform TransformationProjection {
    /* Can't be a mat3 because of ANGLE, see DrawUniform in Phong.vert for
       details */
    highp mat3x4 transformationProjectionMatrices[DRAW_COUNT];
};
#elif defined(THREE_DIMENSIONS)
layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 0
    #endif
) uniform Projection {
    highp mat4 projectionMatrix;
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 1
    #endif
) uniform Transformation {
    highp mat4 transformationMatrices[DRAW_COUNT];
};
#else
#error
#endif

/* Keep in sync with MeshVisualizer.geom and MeshVisualizer.frag. Can't
   "outsource" to a common file because the #extension directives need to be
   always before any code. */
struct DrawUniform {
    #ifdef THREE_DIMENSIONS
    /* Can't be a mat3 because of ANGLE, see Phong.vert for details */
    highp mat3x4 normalMatrix;
    #elif !defined(TWO_DIMENSIONS)
    #error
    #endif
    highp uvec4 materialIdReservedReservedReservedReserved;
    #define draw_materialIdReserved materialIdReservedReservedReservedReserved.x
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 2
    #endif
) uniform Draw {
    DrawUniform draws[DRAW_COUNT];
};

/* Keep in sync with MeshVisualizer.geom and MeshVisualizer.frag. Can't
   "outsource" to a common file because the #extension directives need to be
   always before any code. */
struct MaterialUniform {
    lowp vec4 color;
    lowp vec4 wireframeColor;
    lowp vec4 wireframeWidthColorMapOffsetColorMapScaleLineWidth;
    #define material_wireframeWidth wireframeWidthColorMapOffsetColorMapScaleLineWidth.x
    #define material_colorMapOffset wireframeWidthColorMapOffsetColorMapScaleLineWidth.y
    #define material_colorMapScale wireframeWidthColorMapOffsetColorMapScaleLineWidth.z
    #define material_lineWidth wireframeWidthColorMapOffsetColorMapScaleLineWidth.w
    lowp vec4 lineLengthSmoothnessReservedReserved;
    #define material_lineLength lineLengthSmoothnessReservedReserved.x
    #define material_smoothness lineLengthSmoothnessReservedReserved.y
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 4
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
in highp vec4 position;
#else
#error
#endif

#if defined(TANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION)
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TANGENT_ATTRIBUTE_LOCATION)
#endif
in highp vec4 tangent;
#endif

#ifdef BITANGENT_DIRECTION
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = BITANGENT_ATTRIBUTE_LOCATION)
#endif
in highp vec3 bitangent;
#endif

#if defined(NORMAL_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION)
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = NORMAL_ATTRIBUTE_LOCATION)
#endif
in highp vec3 normal;
#endif

#if defined(WIREFRAME_RENDERING) && defined(NO_GEOMETRY_SHADER)
#if (!defined(GL_ES) && __VERSION__ < 140) || (defined(GL_ES) && __VERSION__ < 300)
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 4)
#endif
in lowp float vertexIndex;
#define gl_VertexID int(vertexIndex)
#endif
#endif

#ifdef INSTANCED_OBJECT_ID
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = OBJECT_ID_ATTRIBUTE_LOCATION)
#endif
in highp uint instanceObjectId;
#endif

/* Outputs */

#if defined(WIREFRAME_RENDERING) && defined(NO_GEOMETRY_SHADER)
out vec3 barycentric;
#endif

#ifdef INSTANCED_OBJECT_ID
#ifdef NO_GEOMETRY_SHADER
flat out highp uint interpolatedInstanceObjectId;
#else
flat out highp uint interpolatedVsInstanceObjectId;
#endif
#endif

#ifdef VERTEX_ID
#ifdef NO_GEOMETRY_SHADER
out highp float interpolatedMappedVertexId;
#else
out highp float interpolatedVsMappedVertexId;
#endif
#endif

#ifdef PRIMITIVE_ID_FROM_VERTEX_ID
#ifdef NO_GEOMETRY_SHADER
flat out highp uint interpolatedPrimitiveId;
#else
flat out highp uint interpolatedVsPrimitiveId;
#endif
#endif

#ifdef TANGENT_DIRECTION
out highp vec4 tangentEndpoint;
#endif
#if defined(BITANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION)
out highp vec4 bitangentEndpoint;
#endif
#ifdef NORMAL_DIRECTION
out highp vec4 normalEndpoint;
#endif

#ifdef MULTI_DRAW
flat out highp uint
    #ifdef NO_GEOMETRY_SHADER
    drawId
    #else
    vsDrawId
    #endif
    ;
#endif

void main() {
    #ifdef UNIFORM_BUFFERS
    #ifdef MULTI_DRAW
    #ifdef NO_GEOMETRY_SHADER
    drawId
    #else
    vsDrawId
    #define drawId vsDrawId
    #endif
    = drawOffset + uint(
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
    highp const mat4 transformationMatrix = transformationMatrices[drawId];
    #else
    #error
    #endif
    #if defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
    mediump const mat3 normalMatrix = mat3(draws[drawId].normalMatrix);
    #endif
    mediump const uint materialId = draws[drawId].draw_materialIdReserved & 0xffffu;
    lowp float colorMapOffset = materials[materialId].material_colorMapOffset;
    lowp float colorMapScale = materials[materialId].material_colorMapScale;
    highp float lineLength = materials[materialId].material_lineLength;
    #endif

    #ifdef TWO_DIMENSIONS
    gl_Position.xywz = vec4(transformationProjectionMatrix*vec3(position, 1.0), 0.0);
    #elif defined(THREE_DIMENSIONS)
    gl_Position = projectionMatrix*transformationMatrix*position;
    #else
    #error
    #endif

    #ifdef TANGENT_DIRECTION
    tangentEndpoint = projectionMatrix*(transformationMatrix*position + vec4(normalize(normalMatrix*tangent.xyz)*lineLength, 0.0));
    #endif
    #ifdef BITANGENT_FROM_TANGENT_DIRECTION
    vec3 bitangent = cross(normal, tangent.xyz)*tangent.w;
    #endif
    #if defined(BITANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION)
    bitangentEndpoint = projectionMatrix*(transformationMatrix*position + vec4(normalize(normalMatrix*bitangent)*lineLength, 0.0));
    #endif
    #ifdef NORMAL_DIRECTION
    normalEndpoint = projectionMatrix*(transformationMatrix*position + vec4(normalize(normalMatrix*normal)*lineLength, 0.0));
    #endif

    #if defined(WIREFRAME_RENDERING) && defined(NO_GEOMETRY_SHADER)
    barycentric = vec3(0.0);

    #ifdef SUBSCRIPTING_WORKAROUND
    #ifndef NEW_GLSL
    int i = int(mod(vertexIndex, 3.0));
    #else
    int i = gl_VertexID % 3;
    #endif
         if(i == 0) barycentric.x = 1.0;
    else if(i == 1) barycentric.y = 1.0;
    else            barycentric.z = 1.0;
    #elif !defined(NEW_GLSL)
    barycentric[int(mod(vertexIndex, 3.0))] = 1.0;
    #else
    barycentric[gl_VertexID % 3] = 1.0;
    #endif
    #endif

    #ifdef INSTANCED_OBJECT_ID
    #ifdef NO_GEOMETRY_SHADER
    interpolatedInstanceObjectId
    #else
    interpolatedVsInstanceObjectId
    #endif
        = instanceObjectId;
    #endif
    #ifdef VERTEX_ID
    #ifdef NO_GEOMETRY_SHADER
    interpolatedMappedVertexId
    #else
    interpolatedVsMappedVertexId
    #endif
        = colorMapOffset + float(gl_VertexID)*colorMapScale;
    #endif
    #ifdef PRIMITIVE_ID_FROM_VERTEX_ID
    #ifdef NO_GEOMETRY_SHADER
    interpolatedPrimitiveId
    #else
    interpolatedVsPrimitiveId
    #endif
        = uint(gl_VertexID/3);
    #endif
}
