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

#ifndef RUNTIME_CONST
#define const
#endif

#ifdef GL_ES
#extension GL_EXT_geometry_shader: require
#ifdef GL_NV_shader_noperspective_interpolation
#extension GL_NV_shader_noperspective_interpolation: require
#endif
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform lowp vec2 viewportSize;

layout(triangles) in;

layout(triangle_strip, max_vertices = 3) out;

/* Interpolate in screen space (if possible) */
#ifdef GL_NV_shader_noperspective_interpolation
noperspective
#endif
out lowp vec3 dist;

void main() {
    /* Screen position of each vertex */
    vec2 p[3];
    for(int i = 0; i != 3; ++i)
        p[i] = viewportSize*gl_in[i].gl_Position.xy/gl_in[i].gl_Position.w;

    /* Vector of each triangle side */
    const vec2 v[3] = vec2[3](
        p[2]-p[1],
        p[2]-p[0],
        p[1]-p[0]
    );

    /* Compute area using perp-dot product */
    const float area = abs(dot(vec2(-v[1].y, v[1].x), v[2]));

    /* Add distance to opposite side to each vertex */
    for(int i = 0; i != 3; ++i) {
        dist = vec3(0.0, 0.0, 0.0);
        dist[i] = area/length(v[i]);
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}
