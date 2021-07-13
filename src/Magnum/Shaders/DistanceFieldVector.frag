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
layout(location = 2)
#endif
uniform lowp vec4 color
    #ifndef GL_ES
    = vec4(1.0, 1.0, 1.0, 1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 3)
#endif
uniform lowp vec4 outlineColor; /* defaults to zero */

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
#endif
uniform lowp vec2 outlineRange
    #ifndef GL_ES
    = vec2(0.5, 1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 5)
#endif
uniform lowp float smoothness
    #ifndef GL_ES
    = 0.04
    #endif
    ;

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

struct DrawUniform {
    highp uvec4 materialIdReservedReservedReservedReserved;
    #define draw_materialIdReserved materialIdReservedReservedReservedReserved.x
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 2
    #endif
) uniform Draw {
    DrawUniform draws[DRAW_COUNT];
};

struct MaterialUniform {
    lowp vec4 color;
    lowp vec4 reserved;
    lowp vec4 outlineColor;
    lowp vec4 outlineRangeSmoothnessReserved;
    #define material_outlineRange outlineRangeSmoothnessReserved.xy
    #define material_smoothness outlineRangeSmoothnessReserved.z
};

layout(std140
    #ifdef EXPLICIT_BINDING
    , binding = 4
    #endif
) uniform Material {
    MaterialUniform materials[MATERIAL_COUNT];
};
#endif

/* Textures */

#ifdef EXPLICIT_BINDING
layout(binding = 6)
#endif
uniform lowp sampler2D vectorTexture;

/* Inputs */

in mediump vec2 interpolatedTextureCoordinates;

#ifdef MULTI_DRAW
flat in highp uint drawId;
#endif

/* OUtput */

#ifdef NEW_GLSL
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = COLOR_OUTPUT_ATTRIBUTE_LOCATION)
#endif
out lowp vec4 fragmentColor;
#endif

void main() {
    #ifdef UNIFORM_BUFFERS
    #if MATERIAL_COUNT > 1
    mediump const uint materialId = draws[drawId].draw_materialIdReserved & 0xffffu;
    #else
    #define materialId 0u
    #endif
    lowp const float smoothness = materials[materialId].material_smoothness;
    lowp const vec4 color = materials[materialId].color;
    lowp const vec4 outlineColor = materials[materialId].outlineColor;
    lowp const vec2 outlineRange = materials[materialId].material_outlineRange;
    #endif

    lowp float intensity = texture(vectorTexture, interpolatedTextureCoordinates).r;

    /* Fill color */
    fragmentColor = smoothstep(outlineRange.x-smoothness, outlineRange.x+smoothness, intensity)*color;

    /* Outline */
    if(outlineRange.x > outlineRange.y) {
        /* Doing *0.5 instead of /2.0 because the latter causes iOS / WebGL to
           complain that "Overflow in implicit constant conversion, minimum
           range for lowp float is (-2,2)" */
        lowp float mid = (outlineRange.x + outlineRange.y)*0.5;
        lowp float halfRange = (outlineRange.x - outlineRange.y)*0.5;
        fragmentColor += smoothstep(halfRange+smoothness, halfRange-smoothness, distance(mid, intensity))*outlineColor;
    }
}
