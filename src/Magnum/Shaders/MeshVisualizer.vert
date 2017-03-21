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
#define in attribute
#define out varying
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0)
#endif
uniform highp mat4 transformationProjectionMatrix;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = POSITION_ATTRIBUTE_LOCATION)
#endif
in highp vec4 position;

#if defined(WIREFRAME_RENDERING) && defined(NO_GEOMETRY_SHADER)
#if (!defined(GL_ES) && __VERSION__ < 140) || (defined(GL_ES) && __VERSION__ < 300)
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 3)
#endif
in lowp float vertexIndex;
#define gl_VertexID int(vertexIndex)
#endif

out vec3 barycentric;
#endif

void main() {
    gl_Position = transformationProjectionMatrix*position;

    #if defined(WIREFRAME_RENDERING) && defined(NO_GEOMETRY_SHADER)
    barycentric = vec3(0.0);

    #ifdef SUBSCRIPTING_WORKAROUND
    int i = int(mod(vertexIndex, 3.0));
         if(i == 0) barycentric.x = 1.0;
    else if(i == 1) barycentric.y = 1.0;
    else            barycentric.z = 1.0;
    #elif !defined(NEW_GLSL)
    barycentric[int(mod(vertexIndex, 3.0))] = 1.0;
    #else
    barycentric[gl_VertexID % 3] = 1.0;
    #endif

    #endif
}
