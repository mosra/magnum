/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#ifndef NEW_GLSL
#define in varying
#define fragmentColor gl_FragColor
#define texture texture2D
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform lowp vec4 color;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 2)
#endif
uniform lowp vec4 outlineColor;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 3)
#endif
uniform lowp vec2 outlineRange
    #ifndef GL_ES
    = vec2(0.5, 1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
#endif
uniform lowp float smoothness
    #ifndef GL_ES
    = 0.04
    #endif
    ;

#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 15)
#endif
uniform lowp sampler2D vectorTexture;

in mediump vec2 fragmentTextureCoordinates;

#ifdef NEW_GLSL
out lowp vec4 fragmentColor;
#endif

void main() {
    lowp float intensity = texture(vectorTexture, fragmentTextureCoordinates).r;

    /* Fill color */
    fragmentColor = smoothstep(outlineRange.x-smoothness, outlineRange.x+smoothness, intensity)*color;

    /* Outline */
    if(outlineRange.x > outlineRange.y) {
        lowp float mid = (outlineRange.x + outlineRange.y)/2.0;
        lowp float halfRange = (outlineRange.x - outlineRange.y)/2.0;
        fragmentColor += smoothstep(halfRange+smoothness, halfRange-smoothness, distance(mid, intensity))*outlineColor;
    }
}
