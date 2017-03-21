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
#endif

#ifndef RUNTIME_CONST
#define const
#endif

#ifndef EXPLICIT_UNIFORM_LOCATION
#define layout(arg)
#endif

#if defined(WIREFRAME_RENDERING) && defined(GL_ES) && __VERSION__ < 300
#extension GL_OES_standard_derivatives : enable
#endif

#if defined(GL_ES) && !defined(NO_GEOMETRY_SHADER) && defined(GL_NV_shader_noperspective_interpolation)
#extension GL_NV_shader_noperspective_interpolation: require
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 2)
#endif
uniform lowp vec4 color
    #ifndef GL_ES
    = vec4(1.0, 1.0, 1.0, 1.0)
    #endif
    ;

#ifdef WIREFRAME_RENDERING
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 3)
#endif
uniform lowp vec4 wireframeColor
    #ifndef GL_ES
    = vec4(0.0, 0.0, 0.0, 1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
#endif
uniform lowp float wireframeWidth
    #ifndef GL_ES
    = 1.0
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 5)
#endif
uniform lowp float smoothness
    #ifndef GL_ES
    = 2.0
    #endif
    ;

#ifndef NO_GEOMETRY_SHADER
#ifdef GL_NV_shader_noperspective_interpolation
noperspective
#endif
in lowp vec3 dist;
#else
in lowp vec3 barycentric;
#endif
#endif

#ifdef NEW_GLSL
out lowp vec4 fragmentColor;
#endif

void main() {
    #ifdef WIREFRAME_RENDERING
    #ifndef NO_GEOMETRY_SHADER
    /* Distance to nearest side */
    lowp const float nearest = min(min(dist.x, dist.y), dist.z);

    /* Smooth step between face color and wireframe color based on distance */
    fragmentColor = mix(wireframeColor, color, smoothstep(wireframeWidth-smoothness, wireframeWidth+smoothness, nearest));
    #else
    const lowp vec3 d = fwidth(barycentric);
    const lowp vec3 factor = smoothstep(vec3(0.0), d*1.5, barycentric);
    const lowp float nearest = min(min(factor.x, factor.y), factor.z);
    fragmentColor = mix(wireframeColor, color, nearest);
    #endif

    #else
    fragmentColor = color;
    #endif
}
