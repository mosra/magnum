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

#if (defined(WIREFRAME_RENDERING) || defined(INSTANCED_OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)) && !defined(TBN_DIRECTION)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform lowp vec4 color
    #ifndef GL_ES
    = vec4(1.0, 1.0, 1.0, 1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 2)
#endif
uniform lowp vec4 wireframeColor
    #ifndef GL_ES
    = vec4(0.0, 0.0, 0.0, 1.0)
    #endif
    ;
#endif

#ifdef WIREFRAME_RENDERING
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 3)
#endif
uniform lowp float wireframeWidth
    #ifndef GL_ES
    = 1.0
    #endif
    ;
#elif defined(TBN_DIRECTION)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 9)
#endif
uniform lowp float lineWidth
    #ifndef GL_ES
    = 1.0
    #endif
    ;
#endif

#if defined(WIREFRAME_RENDERING) || defined(TBN_DIRECTION)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 4)
#endif
uniform lowp float smoothness
    #ifndef GL_ES
    = 2.0
    #endif
    ;
#endif

#if defined(INSTANCED_OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)
#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 4)
#endif
uniform lowp sampler2D colorMapTexture;
#endif

#if defined(INSTANCED_OBJECT_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 6)
#endif
uniform lowp vec2 colorMapOffsetScale
    #ifndef GL_ES
    = vec2(1.0/512.0, 1.0/256.0)
    #endif
    ;
#define colorMapOffset colorMapOffsetScale.x
#define colorMapScale colorMapOffsetScale.y
#endif

#if defined(WIREFRAME_RENDERING) || defined(TBN_DIRECTION)
#ifndef NO_GEOMETRY_SHADER
#if !defined(GL_ES) || defined(GL_NV_shader_noperspective_interpolation)
noperspective
#endif
in lowp vec3 dist;
#else
in lowp vec3 barycentric;
#endif
#endif

#ifdef INSTANCED_OBJECT_ID
flat in highp uint interpolatedInstanceObjectId;
#endif
#ifdef VERTEX_ID
in highp float interpolatedMappedVertexId;
#endif
#ifdef PRIMITIVE_ID_FROM_VERTEX_ID
flat in highp uint interpolatedPrimitiveId;
#endif

#ifdef TBN_DIRECTION
in lowp vec4 backgroundColor;
in lowp vec4 lineColor;
#endif

#ifdef NEW_GLSL
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = COLOR_OUTPUT_ATTRIBUTE_LOCATION)
#endif
out lowp vec4 fragmentColor;
#endif

void main() {
    /* Map object/vertex/primitive ID to a color. Will be either combined with
       the wireframe background color (if wireframe is enabled), ignored (if
       rendering TBN direction) or used as-is if nothing else is enabled */
    #if defined(INSTANCED_OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)
    lowp vec4 faceColor = texture(colorMapTexture, vec2(
        /* Object/primitive IDs are constant across the whole primitive so we
           do the offset/scale mapping here */
        #if defined(INSTANCED_OBJECT_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)
        colorMapOffset + float(
            #ifdef INSTANCED_OBJECT_ID
            interpolatedInstanceObjectId
            #elif defined(PRIMITIVE_ID)
            gl_PrimitiveID
            #elif defined(PRIMITIVE_ID_FROM_VERTEX_ID)
            interpolatedPrimitiveId
            #else
            #error mosra messed up
            #endif
        )*colorMapScale

        /* Vertex ID changes, so the offset/scale mapping was done already in
           a VS/GS and we just pass the interpolated result to a texture */
        #elif defined(VERTEX_ID)
        interpolatedMappedVertexId
        #else
        #error mosra messed up, again
        #endif
        , 0.0));
    #endif

    /* 1. For wireframe the line is on the triangle edges, thus dist = 0 at
          vertices and dist = w (= wireframeWidth) at center of smoothed edge.
       2. For antialiased TBN (drawn alone) the line is in the center of the
          bar, thus dist = 0 at the center, dist = ±w (= lineWidth) at center
          of the smoothed edge, and dist = ±S = w + s (= smoothness) at edge of
          the primitive.
       3. For non-antialised TBN (drawn together with wireframe) the bar width
          is without the extra padding for smoothness.

        0 _____________ 0          -S_-w_0__w_S         -w_0__w
          \  _______  /             | |     | |         |     |
           \ w     w /              | |    /| |         |    /|
            \ \   / /               | |   / | |         |   / |
             \ \w/ /                | |  /  | |         |  /  |
              \ v /                 | | /   | |         | /   |
               \ /                  | |/    | |         |/    |
                v                   |_|_____|_|         |_____|
                0                  -S -w 0  w S         -w 0  w
    */
    #if defined(WIREFRAME_RENDERING) || defined(TBN_DIRECTION)

    /* Fill with background color first:

       1. For wireframe alone the color is supplied directly from the color
          uniform
       2. For TBN alone the backgroundColor is supplied from the GS

       If primitive/object ID is enabled, it multiplies the color.
    */
    #if defined(WIREFRAME_RENDERING) && !defined(TBN_DIRECTION)
    fragmentColor = color;
    #else
    fragmentColor = backgroundColor;
    #endif
    #if defined(INSTANCED_OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)
    fragmentColor *= faceColor;
    #endif

    #ifndef NO_GEOMETRY_SHADER
    /* Distance to nearest side. If signed distance is involved when rendering
       TBN alone, we need to find an absolute distance */
    #if defined(TBN_DIRECTION) && !defined(WIREFRAME_RENDERING)
    lowp const float nearest = min(min(abs(dist.x), abs(dist.y)), abs(dist.z));
    #else
    lowp const float nearest = min(min(dist.x, dist.y), dist.z);
    #endif

    /* Mix in the line color:

       1. For wireframe alone the width and colors are supplied directly from
          wireframeWidth, wireframeColor, color uniforms
       2. For TBN alone the width is supplied directly from the lineWidth
          uniform and colors from the GS
       3. For TBN and wireframe together wireframeWidth is used to draw the
          wireframe, and since `nearest` is always 0 for the TBN bars, there it
          doesn't matter.
    */
    fragmentColor = mix(
        #if defined(WIREFRAME_RENDERING) && !defined(TBN_DIRECTION)
        wireframeColor,
        #else
        lineColor,
        #endif
        fragmentColor,
        #ifdef WIREFRAME_RENDERING
        smoothstep(wireframeWidth - smoothness,
                   wireframeWidth + smoothness, nearest)
        #elif defined(TBN_DIRECTION)
        smoothstep(lineWidth - smoothness,
                   lineWidth + smoothness, nearest)
        #else
        #error
        #endif
        );

    /* Wireframe rendering without a GS. No TBN in this case */
    #else
    const lowp vec3 d = fwidth(barycentric);
    const lowp vec3 factor = smoothstep(vec3(0.0), d*1.5, barycentric);
    const lowp float nearest = min(min(factor.x, factor.y), factor.z);
    fragmentColor = mix(wireframeColor, fragmentColor, nearest);
    #endif

    /* Object / Vertex / Primitive ID visualization using a colormap */
    #elif defined(INSTANCED_OBJECT_ID) || defined(VERTEX_ID) || defined(PRIMITIVE_ID) || defined(PRIMITIVE_ID_FROM_VERTEX_ID)
    fragmentColor = color*faceColor;

    #else
    #error no visualization enabled, huh?
    #endif
}
