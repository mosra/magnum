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

#ifdef AMBIENT_TEXTURE
#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 0)
#endif
uniform lowp sampler2D ambientTexture;
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
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
#ifdef DIFFUSE_TEXTURE
#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 1)
#endif
uniform lowp sampler2D diffuseTexture;
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 5)
#endif
uniform lowp vec4 diffuseColor
    #ifndef GL_ES
    = vec4(1.0)
    #endif
    ;

#ifdef SPECULAR_TEXTURE
#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 2)
#endif
uniform lowp sampler2D specularTexture;
#endif

#ifdef NORMAL_TEXTURE
#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 3)
#endif
uniform lowp sampler2D normalTexture;
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 6)
#endif
uniform lowp vec4 specularColor
    #ifndef GL_ES
    = vec4(1.0, 1.0, 1.0, 0.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 7)
#endif
uniform mediump float shininess
    #ifndef GL_ES
    = 80.0
    #endif
    ;
#endif

#ifdef NORMAL_TEXTURE
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 8)
#endif
uniform mediump float normalTextureScale
    #ifndef GL_ES
    = 1.0
    #endif
    ;
#endif

#ifdef ALPHA_MASK
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 9)
#endif
uniform lowp float alphaMask
    #ifndef GL_ES
    = 0.5
    #endif
    ;
#endif

#ifdef OBJECT_ID
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 10)
#endif
/* mediump is just 2^10, which might not be enough, this is 2^16 */
uniform highp uint objectId; /* defaults to zero */
#endif

#if LIGHT_COUNT
/* Needs to be last because it uses locations 11 + LIGHT_COUNT to
   11 + 2*LIGHT_COUNT - 1. Location 11 is lightPositions. Also it can't be
   specified as 11 + LIGHT_COUNT because that requires ARB_enhanced_layouts.
   Same for lightSpecularColors and lightRanges below. */
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = LIGHT_COLORS_LOCATION) /* I fear this will blow up some drivers */
#endif
uniform lowp vec3 lightColors[LIGHT_COUNT]
    #ifndef GL_ES
    = vec3[](LIGHT_COLOR_INITIALIZER)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = LIGHT_SPECULAR_COLORS_LOCATION)
#endif
uniform lowp vec3 lightSpecularColors[LIGHT_COUNT]
    #ifndef GL_ES
    = vec3[](LIGHT_COLOR_INITIALIZER)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = LIGHT_RANGES_LOCATION)
#endif
uniform lowp float lightRanges[LIGHT_COUNT]
    #ifndef GL_ES
    = float[](LIGHT_RANGE_INITIALIZER)
    #endif
    ;
#endif

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
in highp vec4 lightDirections[LIGHT_COUNT];
in highp vec3 cameraDirection;
#endif

#if defined(AMBIENT_TEXTURE) || defined(DIFFUSE_TEXTURE) || defined(SPECULAR_TEXTURE) || defined(NORMAL_TEXTURE)
in mediump vec2 interpolatedTextureCoordinates;
#endif

#ifdef VERTEX_COLOR
in lowp vec4 interpolatedVertexColor;
#endif

#ifdef INSTANCED_OBJECT_ID
flat in highp uint interpolatedInstanceObjectId;
#endif

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
    lowp const vec4 finalSpecularColor =
        #ifdef SPECULAR_TEXTURE
        texture(specularTexture, interpolatedTextureCoordinates)*
        #endif
        specularColor;
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

    /* Add diffuse color for each light */
    for(int i = 0; i < LIGHT_COUNT; ++i) {
        /* Attenuation. Directional lights have the .w component set to 0, use
           that to make the distance zero -- which will then ensure the
           attenuation is always 1.0 */
        highp float dist = length(lightDirections[i].xyz)*lightDirections[i].w;
        /* If range is 0 for whatever reason, clamp it to a small value to
           avoid a NaN when dist is 0 as well (which is the case for
           directional lights). */
        highp float attenuation = clamp(1.0 - pow(dist/max(lightRanges[i], 0.0001), 4.0), 0.0, 1.0);
        attenuation = attenuation*attenuation/(1.0 + dist*dist);

        highp vec3 normalizedLightDirection = normalize(lightDirections[i].xyz);
        lowp float intensity = max(0.0, dot(normalizedTransformedNormal, normalizedLightDirection))*attenuation;
        fragmentColor += vec4(finalDiffuseColor.rgb*lightColors[i]*intensity, finalDiffuseColor.a/float(LIGHT_COUNT));

        /* Add specular color, if needed */
        if(intensity > 0.001) {
            highp vec3 reflection = reflect(-normalizedLightDirection, normalizedTransformedNormal);
            /* Use attenuation for the specularity as well */
            mediump float specularity = clamp(pow(max(0.0, dot(normalize(cameraDirection), reflection)), shininess), 0.0, 1.0)*attenuation;
            fragmentColor += vec4(finalSpecularColor.rgb*lightSpecularColors[i].rgb*specularity, finalSpecularColor.a);
        }
    }
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
