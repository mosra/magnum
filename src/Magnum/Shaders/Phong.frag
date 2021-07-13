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

#if defined(OBJECT_ID) && !defined(GL_ES) && !defined(NEW_GLSL)
#extension GL_EXT_gpu_shader4: require
#endif

#ifndef NEW_GLSL
#define in varying
#define fragmentColor gl_FragColor
#define texture texture2D
#endif

#ifndef RUNTIME_CONST
#define const
#endif

/* Uniforms */

#ifndef UNIFORM_BUFFERS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 5)
#endif
uniform lowp vec4 ambientColor
    #ifndef GL_ES
    #ifndef AMBIENT_TEXTURE
    = vec4(0.0)
    #else
    = vec4(1.0)
    #endif
    #endif
    ;

#if LIGHT_COUNT
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 6)
#endif
uniform lowp vec4 diffuseColor
    #ifndef GL_ES
    = vec4(1.0)
    #endif
    ;

#ifndef NO_SPECULAR
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 7)
#endif
uniform lowp vec4 specularColor
    #ifndef GL_ES
    = vec4(1.0, 1.0, 1.0, 0.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 8)
#endif
uniform mediump float shininess
    #ifndef GL_ES
    = 80.0
    #endif
    ;
#endif
#endif

#ifdef NORMAL_TEXTURE
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 9)
#endif
uniform mediump float normalTextureScale
    #ifndef GL_ES
    = 1.0
    #endif
    ;
#endif

#ifdef ALPHA_MASK
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 10)
#endif
uniform lowp float alphaMask
    #ifndef GL_ES
    = 0.5
    #endif
    ;
#endif

#ifdef OBJECT_ID
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 11)
#endif
/* mediump is just 2^10, which might not be enough, this is 2^16 */
uniform highp uint objectId; /* defaults to zero */
#endif

#if LIGHT_COUNT
/* Needs to be last because it uses locations 12 to 12 + LIGHT_COUNT - 1 */
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 12)
#endif
uniform highp vec4 lightPositions[LIGHT_COUNT]
    #ifndef GL_ES
    = vec4[](LIGHT_POSITION_INITIALIZER)
    #endif
    ;

/* Needs to be last because it uses locations 12 + LIGHT_COUNT to
   12 + 2*LIGHT_COUNT - 1. Location 12 is lightPositions. Also it can't be
   specified as 12 + LIGHT_COUNT because that requires ARB_enhanced_layouts.
   Same for lightSpecularColors and lightRanges below. */
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = LIGHT_COLORS_LOCATION) /* I fear this will blow up some drivers */
#endif
uniform lowp vec3 lightColors[LIGHT_COUNT]
    #ifndef GL_ES
    = vec3[](LIGHT_COLOR_INITIALIZER)
    #endif
    ;

#ifndef NO_SPECULAR
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = LIGHT_SPECULAR_COLORS_LOCATION)
#endif
uniform lowp vec3 lightSpecularColors[LIGHT_COUNT]
    #ifndef GL_ES
    = vec3[](LIGHT_COLOR_INITIALIZER)
    #endif
    ;
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = LIGHT_RANGES_LOCATION)
#endif
uniform lowp float lightRanges[LIGHT_COUNT]
    #ifndef GL_ES
    = float[](LIGHT_RANGE_INITIALIZER)
    #endif
    ;
#endif

/* Uniform buffers */

#else
#ifndef MULTI_DRAW
#if DRAW_COUNT > 1
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

/* Keep in sync with Phong.vert. Can't "outsource" to a common file because
   the #extension directive needs to be always before any code. */
struct DrawUniform {
    /* Can't be a mat3 because of ANGLE, see Phong.vert for details */
    mediump mat3x4 normalMatrix;
    highp uvec4 materialIdReservedObjectIdLightOffsetLightCount;
    #define draw_materialIdReserved materialIdReservedObjectIdLightOffsetLightCount.x
    #define draw_objectId materialIdReservedObjectIdLightOffsetLightCount.y
    #define draw_lightOffset materialIdReservedObjectIdLightOffsetLightCount.z
    #define draw_lightCount materialIdReservedObjectIdLightOffsetLightCount.w
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 2
    #endif
) uniform Draw {
    DrawUniform draws[DRAW_COUNT];
};

struct MaterialUniform {
    lowp vec4 ambientColor;
    lowp vec4 diffuseColor;
    lowp vec4 specularColor;
    mediump vec4 normalTextureScaleShininessAlphaMaskReserved;
    #define material_normalTextureScale normalTextureScaleShininessAlphaMaskReserved.x
    #define material_shininess normalTextureScaleShininessAlphaMaskReserved.y
    #define material_alphaMask normalTextureScaleShininessAlphaMaskReserved.z
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 4
    #endif
) uniform Material {
    MaterialUniform materials[MATERIAL_COUNT];
};

#if LIGHT_COUNT
struct LightUniform {
    highp vec4 position;
    lowp vec3 colorReserved;
    #define light_color colorReserved.xyz
    lowp vec4 specularColorReserved;
    #define light_specularColor specularColorReserved.xyz
    lowp vec4 rangeReservedReservedReserved;
    #define light_range rangeReservedReservedReserved.x
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 5
    #endif
) uniform Light {
    LightUniform lights[LIGHT_COUNT];
};
#endif
#endif

/* Textures */

#ifdef AMBIENT_TEXTURE
#ifdef EXPLICIT_BINDING
layout(binding = 0)
#endif
uniform lowp
    #ifndef TEXTURE_ARRAYS
    sampler2D
    #else
    sampler2DArray
    #endif
    ambientTexture;
#endif

#if LIGHT_COUNT
#ifdef DIFFUSE_TEXTURE
#ifdef EXPLICIT_BINDING
layout(binding = 1)
#endif
uniform lowp
    #ifndef TEXTURE_ARRAYS
    sampler2D
    #else
    sampler2DArray
    #endif
    diffuseTexture;
#endif

#ifdef SPECULAR_TEXTURE
#ifdef EXPLICIT_BINDING
layout(binding = 2)
#endif
uniform lowp
    #ifndef TEXTURE_ARRAYS
    sampler2D
    #else
    sampler2DArray
    #endif
    specularTexture;
#endif

#ifdef NORMAL_TEXTURE
#ifdef EXPLICIT_BINDING
layout(binding = 3)
#endif
uniform lowp
    #ifndef TEXTURE_ARRAYS
    sampler2D
    #else
    sampler2DArray
    #endif
    normalTexture;
#endif
#endif

/* Inputs */

#if LIGHT_COUNT
in mediump vec3 transformedNormal;
#ifdef NORMAL_TEXTURE
#ifndef BITANGENT
in mediump vec4 transformedTangent;
#else
in mediump vec3 transformedTangent;
in mediump vec3 transformedBitangent;
#endif
#endif
in highp vec3 transformedPosition;
#endif

#if defined(AMBIENT_TEXTURE) || defined(DIFFUSE_TEXTURE) || defined(SPECULAR_TEXTURE) || defined(NORMAL_TEXTURE)
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
    #if MATERIAL_COUNT > 1
    mediump const uint materialId = draws[drawId].draw_materialIdReserved & 0xffffu;
    #else
    #define materialId 0u
    #endif
    lowp const vec4 ambientColor = materials[materialId].ambientColor;
    #if LIGHT_COUNT
    lowp const vec4 diffuseColor = materials[materialId].diffuseColor;
    lowp const vec4 specularColor = materials[materialId].specularColor;
    mediump const float shininess = materials[materialId].material_shininess;
    #endif
    #ifdef NORMAL_TEXTURE
    mediump float normalTextureScale = materials[materialId].material_normalTextureScale;
    #endif
    #ifdef ALPHA_MASK
    lowp const float alphaMask = materials[materialId].material_alphaMask;
    #endif
    #if LIGHT_COUNT
    mediump const uint lightOffset = draws[drawId].draw_lightOffset;
    #endif
    #endif

    lowp const vec4 finalAmbientColor =
        #ifdef AMBIENT_TEXTURE
        texture(ambientTexture, interpolatedTextureCoordinates)*
        #endif
        #ifdef VERTEX_COLOR
        interpolatedVertexColor*
        #endif
        ambientColor;
    #if LIGHT_COUNT
    lowp const vec4 finalDiffuseColor =
        #ifdef DIFFUSE_TEXTURE
        texture(diffuseTexture, interpolatedTextureCoordinates)*
        #endif
        #ifdef VERTEX_COLOR
        interpolatedVertexColor*
        #endif
        diffuseColor;
    #ifndef NO_SPECULAR
    lowp const vec4 finalSpecularColor =
        #ifdef SPECULAR_TEXTURE
        texture(specularTexture, interpolatedTextureCoordinates)*
        #endif
        specularColor;
    #endif
    #endif

    /* Ambient color */
    fragmentColor = finalAmbientColor;

    #if LIGHT_COUNT
    /* Normal */
    mediump vec3 normalizedTransformedNormal = normalize(transformedNormal);
    #ifdef NORMAL_TEXTURE
    #ifndef BITANGENT
    mediump vec3 normalizedTransformedTangent = normalize(transformedTangent.xyz);
    #else
    mediump vec3 normalizedTransformedTangent = normalize(transformedTangent);
    mediump vec3 normalizedTransformedBitangent = normalize(transformedBitangent);
    #endif
    mediump mat3 tbn = mat3(
        normalizedTransformedTangent,
        #ifndef BITANGENT
        normalize(cross(normalizedTransformedNormal,
                        normalizedTransformedTangent)*transformedTangent.w),
        #else
        normalizedTransformedBitangent,
        #endif
        normalizedTransformedNormal
    );
    normalizedTransformedNormal = tbn*(normalize((texture(normalTexture, interpolatedTextureCoordinates).rgb*2.0 - vec3(1.0))*vec3(normalTextureScale, normalTextureScale, 1.0)));
    #endif

    highp const vec3 cameraDirection = normalize(-transformedPosition);

    /* Add diffuse color for each light */
    #ifndef LIGHT_CULLING
    for(int i = 0; i < LIGHT_COUNT; ++i)
    #else
    for(uint i = 0u, actualLightCount = min(uint(LIGHT_COUNT), draws[drawId].draw_lightCount); i < actualLightCount; ++i)
    #endif
    {
        lowp const vec3 lightColor =
            #ifndef UNIFORM_BUFFERS
            lightColors[i]
            #else
            lights[
                #ifdef LIGHT_CULLING
                lightOffset +
                #endif
                i].light_color
            #endif
            ;
        #ifndef NO_SPECULAR
        lowp const vec3 lightSpecularColor =
            #ifndef UNIFORM_BUFFERS
            lightSpecularColors[i]
            #else
            lights[
                #ifdef LIGHT_CULLING
                lightOffset +
                #endif
                i].light_specularColor
            #endif
            ;
        #endif
        lowp const float lightRange =
            #ifndef UNIFORM_BUFFERS
            lightRanges[i]
            #else
            lights[
                #ifdef LIGHT_CULLING
                lightOffset +
                #endif
                i].light_range
            #endif
            ;

        highp const vec4 lightPosition =
            #ifndef UNIFORM_BUFFERS
            lightPositions[i]
            #else
            lights[
                #ifdef LIGHT_CULLING
                lightOffset +
                #endif
                i].position
            #endif
            ;
        highp const vec4 lightDirection = vec4(lightPosition.xyz - transformedPosition*lightPosition.w, lightPosition.w);

        /* Attenuation. Directional lights have the .w component set to 0, use
           that to make the distance zero -- which will then ensure the
           attenuation is always 1.0 */
        highp const float len = length(lightDirection.xyz);
        highp const float dist = len*lightDirection.w;
        /* If range is 0 for whatever reason, clamp it to a small value to
           avoid a NaN when dist is 0 as well (which is the case for
           directional lights). */
        highp float attenuation = clamp(1.0 - pow(dist/max(lightRange, 0.0001), 4.0), 0.0, 1.0);
        attenuation = attenuation*attenuation/(1.0 + dist*dist);

        highp vec3 normalizedLightDirection = lightDirection.xyz/len;
        lowp float intensity = max(0.0, dot(normalizedTransformedNormal, normalizedLightDirection))*attenuation;
        fragmentColor.rgb += finalDiffuseColor.rgb*lightColor*intensity;

        #ifndef NO_SPECULAR
        /* Add specular color, if needed */
        if(intensity > 0.001) {
            highp vec3 reflection = reflect(-normalizedLightDirection, normalizedTransformedNormal);
            /* Use attenuation for the specularity as well */
            mediump float specularity = clamp(pow(max(0.0, dot(cameraDirection, reflection)), shininess), 0.0, 1.0)*attenuation;
            fragmentColor += vec4(finalSpecularColor.rgb*lightSpecularColor.rgb*specularity, finalSpecularColor.a);
        }
        #endif
    }

    fragmentColor.a += finalDiffuseColor.a;
    #endif

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
        objectId;
    #endif
}
