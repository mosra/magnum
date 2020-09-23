/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#ifndef NEW_GLSL
#define in attribute
#define out varying
#endif

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

#if LIGHT_COUNT
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

#if LIGHT_COUNT
/* Needs to be last because it uses locations 11 to 11 + LIGHT_COUNT - 1 */
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 11)
#endif
uniform highp vec4 lightPositions[LIGHT_COUNT]
    #ifndef GL_ES
    = vec4[](LIGHT_POSITION_INITIALIZER)
    #endif
    ;
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = POSITION_ATTRIBUTE_LOCATION)
#endif
in highp vec4 position;

#if LIGHT_COUNT
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

out mediump vec2 interpolatedTextureCoordinates;
#endif

#ifdef VERTEX_COLOR
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = COLOR_ATTRIBUTE_LOCATION)
#endif
in lowp vec4 vertexColor;

out lowp vec4 interpolatedVertexColor;
#endif

#ifdef INSTANCED_OBJECT_ID
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = OBJECT_ID_ATTRIBUTE_LOCATION)
#endif
in highp uint instanceObjectId;

flat out highp uint interpolatedInstanceObjectId;
#endif

#ifdef INSTANCED_TRANSFORMATION
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TRANSFORMATION_MATRIX_ATTRIBUTE_LOCATION)
#endif
in highp mat4 instancedTransformationMatrix;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = NORMAL_MATRIX_ATTRIBUTE_LOCATION)
#endif
in highp mat3 instancedNormalMatrix;
#endif

#ifdef INSTANCED_TEXTURE_OFFSET
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TEXTURE_OFFSET_ATTRIBUTE_LOCATION)
#endif
in mediump vec2 instancedTextureOffset;
#endif

#if LIGHT_COUNT
out mediump vec3 transformedNormal;
#ifdef NORMAL_TEXTURE
#ifndef BITANGENT
out mediump vec4 transformedTangent;
#else
out mediump vec3 transformedTangent;
out mediump vec3 transformedBitangent;
#endif
#endif
out highp vec4 lightDirections[LIGHT_COUNT];
out highp vec3 cameraDirection;
#endif

void main() {
    /* Transformed vertex position */
    highp vec4 transformedPosition4 = transformationMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        position;
    highp vec3 transformedPosition = transformedPosition4.xyz/transformedPosition4.w;

    #if LIGHT_COUNT
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

    /* Direction to the light. Directional lights have the last component set
       to 0, which gets used to ignore the transformed position. */
    for(int i = 0; i < LIGHT_COUNT; ++i)
        lightDirections[i] = vec4(lightPositions[i].xyz - transformedPosition*lightPositions[i].w, lightPositions[i].w);

    /* Direction to the camera */
    cameraDirection = -transformedPosition;
    #endif

    /* Transform the position */
    gl_Position = projectionMatrix*transformedPosition4;

    #ifdef TEXTURED
    /* Texture coordinates, if needed */
    interpolatedTextureCoordinates =
        #ifdef TEXTURE_TRANSFORMATION
        (textureMatrix*vec3(
            #ifdef INSTANCED_TEXTURE_OFFSET
            instancedTextureOffset +
            #endif
            textureCoordinates, 1.0)).xy
        #else
        textureCoordinates
        #endif
        ;
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
