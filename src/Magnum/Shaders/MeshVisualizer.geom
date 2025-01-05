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

#ifndef RUNTIME_CONST
#define const
#endif

#if defined(SHADER_STORAGE_BUFFERS) && !defined(GL_ES)
#extension GL_ARB_shader_storage_buffer_object: require
#endif

#ifdef GL_ES
#extension GL_EXT_geometry_shader: require
#ifdef GL_NV_shader_noperspective_interpolation
#extension GL_NV_shader_noperspective_interpolation: require
#endif
#endif

/* Uniforms */

/* This one is for both classic and UBOs, as it's usually set globally instead
   of changing per-draw */
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0)
#endif
uniform lowp vec2 viewportSize; /* defaults to zero */

#ifndef UNIFORM_BUFFERS
#if (defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)) && (defined(WIREFRAME_RENDERING) || defined(OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID))
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform lowp vec4 color
    #ifndef GL_ES
    = vec4(1.0, 1.0, 1.0, 1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 2)
#endif
uniform lowp vec4 wireframeColor
    #ifndef GL_ES
    = vec4(0.0, 0.0, 0.0, 1.0)
    #endif
    ;
#endif

#if defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 12)
#endif
uniform lowp float lineWidth
    #ifndef GL_ES
    = 1.0
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
#endif
uniform lowp float smoothness
    #ifndef GL_ES
    = 2.0
    #endif
    ;
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
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform highp uint drawOffset
    #ifndef GL_ES
    = 0u
    #endif
    ;
#endif

/* Keep in sync with MeshVisualizer.vert and MeshVisualizer.frag. Can't
   "outsource" to a common file because the #extension directives need to be
   always before any code. */
struct DrawUniform {
    #ifdef THREE_DIMENSIONS
    /* Can't be a mat3 because of ANGLE, see Phong.vert for details */
    highp mat3x4 normalMatrix;
    #elif !defined(TWO_DIMENSIONS)
    #error
    #endif
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

/* Keep in sync with MeshVisualizer.vert and MeshVisualizer.frag. Can't
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
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 4
    #endif
) BUFFER_OR_UNIFORM Material {
    BUFFER_READONLY MaterialUniform materials[MATERIAL_COUNT];
};
#endif

/* Inputs */

layout(triangles) in;

#ifdef TANGENT_DIRECTION
in highp vec4 tangentEndpoint[];
#endif
#ifdef BITANGENT_DIRECTION
in highp vec4 bitangentEndpoint[];
#endif
#ifdef NORMAL_DIRECTION
in highp vec4 normalEndpoint[];
#endif

#ifdef TEXTURED
in mediump
    #ifndef TEXTURE_ARRAYS
    vec2
    #else
    vec3
    #endif
    interpolatedVsTextureCoordinates[];
#endif
#ifdef INSTANCED_OBJECT_ID
flat in highp uint interpolatedVsInstanceObjectId[];
#endif
#ifdef VERTEX_ID
in highp float interpolatedVsMappedVertexId[];
#endif
#ifdef PRIMITIVE_ID_FROM_VERTEX_ID
flat in highp uint interpolatedVsPrimitiveId[];
#endif

#ifdef MULTI_DRAW
flat in highp uint vsDrawId[];
#endif

/* Outputs */

layout(triangle_strip, max_vertices = MAX_VERTICES) out;

/* Interpolate in screen space (if possible) */
#if !defined(GL_ES) || defined(GL_NV_shader_noperspective_interpolation)
noperspective
#endif
out lowp vec3 dist;

#ifdef TEXTURED
out mediump
    #ifndef TEXTURE_ARRAYS
    vec2
    #else
    vec3
    #endif
    interpolatedTextureCoordinates;
#endif
#ifdef INSTANCED_OBJECT_ID
flat out highp uint interpolatedInstanceObjectId;
#endif
#ifdef VERTEX_ID
out highp float interpolatedMappedVertexId;
#endif
#ifdef PRIMITIVE_ID_FROM_VERTEX_ID
flat out highp uint interpolatedPrimitiveId;
#endif

#ifdef MULTI_DRAW
flat out highp uint drawId;
#endif

#if defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
out lowp vec4 backgroundColor;
out lowp vec4 lineColor;

void emitQuad(
    #ifdef UNIFORM_BUFFERS
    uint materialId,
    #endif
    vec4 position, vec2 positionScreen, vec4 endpoint, vec2 endpointScreen
) {
    #if defined(UNIFORM_BUFFERS) && (defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION))
    lowp const float lineWidth = materials[materialId].material_lineWidth;
    lowp const float smoothness = materials[materialId].material_smoothness;
    #endif

    /* Calculate screen-space locations for the bar vertices and form two
       triangles out of them. In case TBN is rendered alone, half bar width is
       lineWidth + smoothness to allow for antialiasing, in case it's rendered
       together with wireframe, object ID, vertex ID or primitive ID, it's just
       lineWidth, as antialiasing would cause depth order issues.

        3 - e - 1     -S_-w_0__w_S         -w_0__w
        |      /|      | |     | |         |     |
        |     / |      | |    /| |         |    /|
        |    /  |      | |   / | |         |   / |
        |   /   |      | |  /  | |         |  /  |
        |  /    |      | | /   | |         | /   |
        | /     |      | |/    | |         |/    |
        |/      |      |_|_____|_|         |_____|
        2 - p - 0     -S -w 0  w S         -w 0  w

       The edgeDistance is set to a signed half-width for the antialiased case
       and to 0 otherwise. See the fragment shader for details.
    */
    vec2 direction = normalize(endpointScreen - positionScreen);
    #if defined(WIREFRAME_RENDERING) || defined(OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)
    float edgeDistance = 0.0;
    vec2 halfSide = lineWidth*vec2(-direction.y, direction.x);
    #else
    float edgeDistance = lineWidth + smoothness;
    vec2 halfSide = edgeDistance*vec2(-direction.y, direction.x);
    #endif

    /* 0 */
    dist = vec3(-edgeDistance);
    gl_Position = vec4((positionScreen - halfSide)*position.w/viewportSize, position.zw);
    EmitVertex();

    /* 1 */
    dist = vec3(-edgeDistance);
    gl_Position = vec4((endpointScreen - halfSide)*endpoint.w/viewportSize, endpoint.zw);
    EmitVertex();

    /* 2 */
    dist = vec3(edgeDistance);
    gl_Position = vec4((positionScreen + halfSide)*position.w/viewportSize, position.zw);
    EmitVertex();

    /* 3 */
    dist = vec3(edgeDistance);
    gl_Position = vec4((endpointScreen + halfSide)*endpoint.w/viewportSize, endpoint.zw);
    EmitVertex();

    EndPrimitive();
}
#endif

void main() {
    #ifdef UNIFORM_BUFFERS
    #ifdef MULTI_DRAW
    drawId = vsDrawId[0];
    #else
    #define drawId drawOffset
    #endif

    /* With SSBOs MATERIAL_COUNT is defined to be empty, +0 makes the condition
       not cause a compile error */
    #if defined(SHADER_STORAGE_BUFFERS) || MATERIAL_COUNT+0 > 1
    mediump const uint materialId = draws[drawId].draw_materialIdReserved & 0xffffu;
    #else
    #define materialId 0u
    #endif
    #if (defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)) && (defined(WIREFRAME_RENDERING) || defined(OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID))
    lowp const vec4 color = materials[materialId].color;
    lowp const vec4 wireframeColor = materials[materialId].wireframeColor;
    #endif
    #if defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
    lowp const float lineWidth = materials[materialId].material_lineWidth;
    lowp const float smoothness = materials[materialId].material_smoothness;
    #endif
    #endif

    /* Passthrough for unchanged variables */
    #ifdef INSTANCED_OBJECT_ID
    interpolatedInstanceObjectId = interpolatedVsInstanceObjectId[0];
    #endif
    #ifdef PRIMITIVE_ID_FROM_VERTEX_ID
    interpolatedPrimitiveId = interpolatedVsPrimitiveId[0];
    #elif defined(PRIMITIVE_ID)
    /* This has to be done explicitly, otherwise the fragment input is
       undefined. Interestingly enough this worked well on Mesa / Intel with
       the GS emitting always just 3 vertices, but not anymore when it emits
       also the TBN direction. */
    gl_PrimitiveID = gl_PrimitiveIDIn;
    #endif

    /* Screen position of each vertex + TBN direction */
    vec2 p[3];
    #ifdef TANGENT_DIRECTION
    vec2 t[3];
    #endif
    #ifdef BITANGENT_DIRECTION
    vec2 b[3];
    #endif
    #ifdef NORMAL_DIRECTION
    vec2 n[3];
    #endif
    for(int i = 0; i != 3; ++i) {
        p[i] = viewportSize*gl_in[i].gl_Position.xy/gl_in[i].gl_Position.w;
        #ifdef TANGENT_DIRECTION
        t[i] = viewportSize*tangentEndpoint[i].xy/tangentEndpoint[i].w;
        #endif
        #ifdef BITANGENT_DIRECTION
        b[i] = viewportSize*bitangentEndpoint[i].xy/bitangentEndpoint[i].w;
        #endif
        #ifdef NORMAL_DIRECTION
        n[i] = viewportSize*normalEndpoint[i].xy/normalEndpoint[i].w;
        #endif
    }

    #if defined(WIREFRAME_RENDERING) || defined(OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)
    /* Vector of each triangle side */
    const vec2 v[3] = vec2[3](
        p[2]-p[1],
        p[2]-p[0],
        p[1]-p[0]
    );

    /* Compute area using perp-dot product */
    const float area = abs(dot(vec2(-v[1].y, v[1].x), v[2]));

    /* If wireframe is enabled, add distance to opposite side to each vertex.
       Otherwise make all distances the same to avoid any lines being drawn.
       Propagate also mapped vertex ID, since that changes per-vertex also. */
    #if defined(VERTEX_ID) && defined(__GLSL_CG_DATA_TYPES)
    /* Without this, NVidia says "warning C7050: "interpolatedMappedVertexId"
       might be used before being initialized". Interestingly enough, it
       doesn't say that for interpolatedTextureCoordinates. */
    interpolatedMappedVertexId = 0.0;
    #endif
    for(int i = 0; i != 3; ++i) {
        dist = vec3(0.0, 0.0, 0.0);
        #ifdef WIREFRAME_RENDERING
        dist[i] = area/length(v[i]);
        #else
        dist = vec3(area/length(v[i]));
        #endif
        #ifdef VERTEX_ID
        interpolatedMappedVertexId = interpolatedVsMappedVertexId[i];
        #endif
        #ifdef TEXTURED
        interpolatedTextureCoordinates = interpolatedVsTextureCoordinates[i];
        #endif
        #if defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
        backgroundColor = color;
        lineColor = wireframeColor;
        #endif
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
    #endif

    #if defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
    backgroundColor = vec4(0.0, 0.0, 0.0, 0.0);
    for(int i = 0; i != 3; ++i) {
        #ifdef TANGENT_DIRECTION
        lineColor = vec4(1.0, 0.0, 0.0, 1.0);
        emitQuad(
            #ifdef UNIFORM_BUFFERS
            materialId,
            #endif
            gl_in[i].gl_Position, p[i], tangentEndpoint[i], t[i]
        );
        #endif

        #ifdef BITANGENT_DIRECTION
        lineColor = vec4(0.0, 1.0, 0.0, 1.0);
        emitQuad(
            #ifdef UNIFORM_BUFFERS
            materialId,
            #endif
            gl_in[i].gl_Position, p[i], bitangentEndpoint[i], b[i]
        );
        #endif

        #ifdef NORMAL_DIRECTION
        lineColor = vec4(0.0, 0.0, 1.0, 1.0);
        emitQuad(
            #ifdef UNIFORM_BUFFERS
            materialId,
            #endif
            gl_in[i].gl_Position, p[i], normalEndpoint[i], n[i]
        );
        #endif
    }
    #endif
}
