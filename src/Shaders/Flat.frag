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
#define fragmentColor gl_FragColor
#define texture texture2D
#define in varying
#endif

#ifdef TEXTURED
#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 0) uniform sampler2D textureData;
#else
uniform sampler2D textureData;
#endif
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
#   ifndef GL_ES
layout(location = 1) uniform vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
#   else
layout(location = 1) uniform vec4 color;
#   endif
#else
#   ifndef GL_ES
uniform lowp vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
#   else
unfirom lowp vec4 color;
#   endif
#endif

#ifdef TEXTURED
in mediump vec2 interpolatedTextureCoordinates;
#endif

#ifdef NEW_GLSL
out lowp vec4 fragmentColor;
#endif

void main() {
    #ifdef TEXTURED
    fragmentColor = color * texture(textureData, interpolatedTextureCoordinates);
    #else
    fragmentColor = color;
    #endif
}
