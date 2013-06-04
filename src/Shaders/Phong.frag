/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#ifndef NEW_GLSL
#define in varying
#define color gl_FragColor
#endif

#ifndef GL_ES
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4) uniform vec3 diffuseColor;
layout(location = 5) uniform vec3 ambientColor = vec3(0.0, 0.0, 0.0);
layout(location = 6) uniform vec3 specularColor = vec3(1.0, 1.0, 1.0);
layout(location = 7) uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
layout(location = 8) uniform float shininess = 80.0;
#else
uniform vec3 diffuseColor;
uniform vec3 ambientColor = vec3(0.0, 0.0, 0.0);
uniform vec3 specularColor = vec3(1.0, 1.0, 1.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform float shininess = 80.0;
#endif
#else
uniform lowp vec3 diffuseColor;
uniform lowp vec3 ambientColor;
uniform lowp vec3 specularColor;
uniform lowp vec3 lightColor;
uniform mediump float shininess;
#endif

in mediump vec3 transformedNormal;
in highp vec3 lightDirection;
in highp vec3 cameraDirection;

#ifdef NEW_GLSL
out lowp vec4 color;
#endif

void main() {
    /* Ambient color */
    color.rgb = ambientColor;

    mediump vec3 normalizedTransformedNormal = normalize(transformedNormal);
    highp vec3 normalizedLightDirection = normalize(lightDirection);

    /* Add diffuse color */
    lowp float intensity = max(0.0, dot(normalizedTransformedNormal, normalizedLightDirection));
    color.rgb += diffuseColor*lightColor*intensity;

    /* Add specular color, if needed */
    if(intensity > 0.001) {
        highp vec3 reflection = reflect(-normalizedLightDirection, normalizedTransformedNormal);
        mediump float specularity = pow(max(0.0, dot(normalize(cameraDirection), reflection)), shininess);
        color.rgb += specularColor*specularity;
    }

    /* Force alpha to 1 */
    color.a = 1.0;
}
