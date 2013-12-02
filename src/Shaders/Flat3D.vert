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
layout(location = 0) uniform mat4 transformationProjectionMatrix;
#else
uniform highp mat4 transformationProjectionMatrix;
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = POSITION_ATTRIBUTE_LOCATION) in highp vec4 position;
#else
in highp vec4 position;
#endif

#ifdef TEXTURED
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TEXTURECOORDINATES_ATTRIBUTE_LOCATION) in mediump vec2 textureCoords;
#else
in mediump vec2 textureCoords;
#endif
out mediump vec2 interpolatedTextureCoords;
#endif

void main() {
    gl_Position = transformationProjectionMatrix*position;

    #ifdef TEXTURED
    /* Texture coordinates, if needed */
    interpolatedTextureCoords = textureCoords;
    #endif
}
