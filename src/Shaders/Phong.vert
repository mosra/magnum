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
#define in attribute
#define out varying
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0) uniform mat4 transformationMatrix;
layout(location = 1) uniform mat4 projectionMatrix;
layout(location = 2) uniform mat3 normalMatrix;
layout(location = 3) uniform vec3 light;
#else
uniform highp mat4 transformationMatrix;
uniform highp mat4 projectionMatrix;
uniform mediump mat3 normalMatrix;
uniform highp vec3 light;
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 0) in highp vec4 position;
layout(location = 2) in mediump vec3 normal;
#else
in highp vec4 position;
in mediump vec3 normal;
#endif

#ifdef TEXTURED
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 1) in mediump vec2 textureCoords;
#else
in mediump vec2 textureCoords;
#endif
out mediump vec2 interpolatedTextureCoords;
#endif

out mediump vec3 transformedNormal;
out highp vec3 lightDirection;
out highp vec3 cameraDirection;

void main() {
    /* Transformed vertex position */
    highp vec4 transformedPosition4 = transformationMatrix*position;
    highp vec3 transformedPosition = transformedPosition4.xyz/transformedPosition4.w;

    /* Transformed normal vector */
    transformedNormal = normalMatrix*normal;

    /* Direction to the light */
    lightDirection = normalize(light - transformedPosition);

    /* Direction to the camera */
    cameraDirection = -transformedPosition;

    /* Transform the position */
    gl_Position = projectionMatrix*transformedPosition4;

    #ifdef TEXTURED
    /* Texture coordinates, if needed */
    interpolatedTextureCoords = textureCoords;
    #endif
}
