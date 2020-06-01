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
#define value gl_FragColor.x
#define texture texture2D
#endif

#ifndef RUNTIME_CONST
#define const
#endif

#if (defined(GL_ES) && __VERSION__ >= 300) || (!defined(GL_ES) && __VERSION__ >= 150)
#define TEXELFETCH_USABLE
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0)
#endif
uniform mediump vec2 scaling;

#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 7)
#endif
uniform lowp sampler2D textureData;

#ifdef TEXELFETCH_USABLE
#ifndef GL_ES
layout(pixel_center_integer) in mediump vec4 gl_FragCoord;
#endif
#else
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform mediump vec2 imageSizeInverted;
#endif

#ifdef NEW_GLSL
out lowp float value;
#endif

#ifdef TEXELFETCH_USABLE
bool hasValue(const mediump ivec2 position, const mediump ivec2 offset) {
    return texelFetch(textureData, position + offset, 0).r > 0.5;
}
#else
bool hasValue(const mediump vec2 position, const mediump ivec2 offset) {
    return texture(textureData, position + (vec2(offset) + vec2(0.5))*imageSizeInverted).r > 0.5;
}
#endif

mediump int findMinDistanceSquared(const mediump
    #ifdef TEXELFETCH_USABLE
    ivec2
    #else
    vec2
    #endif
    position, const bool isInside)
{
    /* Initialize minimal distance to a value just outside the radius */
    mediump int minDistanceSquared = (RADIUS+1)*(RADIUS+1);

    /* Go in cocentric squares around the point */
    for(int i = 1; i <= RADIUS; ++i) {
        /* First check the nearest points, since that's only four combinations.
           If any of the four values is opposite of what is on `position`, we
           found the nearest value. If the distance is not less than what's
           found already, we don't even check the texture.

           i = 1    i = 2     i = 3

                                0
                      0
             0
            1o3     1 o 3    1  o  3
             2
                      2
                                2

           Since everything else in the cocentric square and all others will be
           further away, we can stop if we found something. */
        const mediump int centerDistanceSquared = i*i;
        if(centerDistanceSquared >= minDistanceSquared)
            return minDistanceSquared;
        if(hasValue(position, ivec2(0,  i)) != isInside ||
           hasValue(position, ivec2(-i, 0)) != isInside ||
           hasValue(position, ivec2(0, -i)) != isInside ||
           hasValue(position, ivec2(i,  0)) != isInside) {
            return centerDistanceSquared;
        }

        /* Now check for points further away, except for the corner points.
           Every iteration checks all eight rotations/reflections at the same
           distance. Again, if the distance is not less than what's found
           already, we don't even check the texture -- but can't return, since
           next iteration can still have closer values.

           i = 1    i = 2     i = 3
           (none)
                              91 08
                     1 0     a     f
                    2   7    2     7
             o        o         o
                    3   6    3     6
                     4 5     b     e
                              c4 5d

           Once we find something, it's the closest value possible in this
           cycle, so we stop the cycle. But next iterations can still have
           values that are closer, so can't return. */
        for(int j = 1; j < RADIUS; ++j) {
            /* Don't go further than current radius - 1 (i.e., excluding the
               corner). The loop needs to be compile-time bound otherwise some
               drivers crash on it, so we're breaking inside instead of having
               this directly in the loop condition. */
            if(j >= i) break;

            const mediump int sideDistanceSquared = i*i + j*j;
            if(sideDistanceSquared >= minDistanceSquared)
                break;
            if(hasValue(position, ivec2( j,  i)) != isInside ||
               hasValue(position, ivec2(-j,  i)) != isInside ||
               hasValue(position, ivec2(-i,  j)) != isInside ||
               hasValue(position, ivec2(-i, -j)) != isInside ||
               hasValue(position, ivec2(-j, -i)) != isInside ||
               hasValue(position, ivec2( j, -i)) != isInside ||
               hasValue(position, ivec2( i, -j)) != isInside ||
               hasValue(position, ivec2( i,  j)) != isInside) {
                minDistanceSquared = sideDistanceSquared;
                break;
            }
        }

        /* Finally, check for the corners, which is again just four cases:

           i = 1    i = 2     i = 3

                             1     0
                    1   0
            1 0
             o        o         o
            2 3
                    2   3
                             2     3

           If we find something, it's most probably not the nearest distance,
           since the following iterations can be much closer. */
        const mediump int cornerDistanceSquared = 2*i*i;
        if(cornerDistanceSquared >= minDistanceSquared)
            continue;
        if(hasValue(position, ivec2( i,  i)) != isInside ||
           hasValue(position, ivec2(-i,  i)) != isInside ||
           hasValue(position, ivec2(-i, -i)) != isInside ||
           hasValue(position, ivec2( i, -i)) != isInside) {
            minDistanceSquared = cornerDistanceSquared;
        }
    }

    return minDistanceSquared;
}

void main() {
    #ifdef TEXELFETCH_USABLE
    #ifndef GL_ES
    const mediump ivec2 position = ivec2(gl_FragCoord.xy*scaling);
    #else
    const mediump ivec2 position = ivec2((gl_FragCoord.xy - vec2(0.5))*scaling);
    #endif
    #else
    const mediump vec2 position = (gl_FragCoord.xy - vec2(0.5))*scaling*imageSizeInverted;
    #endif

    /* If pixel at the position is inside (its value > 0.5), we are looking for
       nearest pixel outside and the value will be positive (or > 0.5 after
       normalization). If it is outside (its value < 0), we are looking for
       nearest pixel inside and the value will be negative (or < 0.5). */
    const bool isInside = hasValue(position, ivec2(0, 0));
    const mediump float minDistance = sqrt(float(findMinDistanceSquared(position, isInside)));

    /* Final signed distance, normalized from [-radius-1, radius+1] to [0, 1] */
    const highp float halfSign = isInside ? 0.5 : -0.5;
    value = halfSign*minDistance/float(RADIUS + 1) + 0.5;
}
