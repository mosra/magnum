/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
#define value gl_FragColor.x
#define texture texture2D
#endif

#ifndef RUNTIME_CONST
#define const
#endif

#if (defined(GL_ES) && __VERSION__ >= 300) || (!defined(GL_ES) && __VERSION__ >= 150)
#define TEXELFETCH_USABLE
#endif

#ifdef EXPLICIT_BINDING
layout(binding = 7)
#endif
uniform lowp sampler2D textureData;

#ifndef TEXELFETCH_USABLE
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0)
#endif
uniform mediump vec2 imageSizeInverted;
#endif

in mediump vec2 inputTextureCoordinates;

#ifdef NEW_GLSL
out lowp float value;
#endif

#ifdef TEXELFETCH_USABLE
bool hasValue(const mediump ivec2 position, const mediump ivec2 offset) {
    return texelFetch(textureData, position + offset, 0).r > 0.5;
}
#else
bool hasValue(const mediump vec2 position, const mediump ivec2 offset) {
    return texture(textureData, position + vec2(offset)*imageSizeInverted).r > 0.5;
}
#endif

mediump float findMinDistanceSquared(const mediump
    #ifdef TEXELFETCH_USABLE
    ivec2
    #else
    vec2
    #endif
    position, const bool isInside)
{
    /* Initialize minimal distance to a value just outside the radius. As the
       output coordinate is shifted by half a pixel from the input (see the
       diagram in main() below), the X/Y distances are always a whole pixel
       minus 0.5. Thus the max distance found in the below loop can be at most
       `RADIUS - 0.5`, and the next nearest distance is thus `RADIUS + 0.5`. */
    mediump float minDistanceSquared = (float(RADIUS)+0.5)*(float(RADIUS)+0.5);

    /* Go in cocentric squares around the point */
    for(int i = 2; i <= RADIUS; ++i) {
        /* Actual distance from the center for this iteration is half a pixel
           less (see the diagram in main() below) */
        const mediump float iF = float(i) - 0.5;

        /* Check for points further away from the initial 2x2 square, except
           for corners. Every iteration checks all eight rotations/reflections
           at the same distance. If the distance in given iteration is not less
           than what's found already, we don't even check the texture -- but
           can't return, since next iteration can still have closer values.

            i = 2   i = 3

                     9108
             10     a    f
            2  7    2    7
            3p 6    3 p  6
             45     b    e
                     c45d

           Once we find something, it's the closest value possible in this
           cycle, so we stop the cycle. But next iterations can still have
           values that are closer, so can't return.

           Note that the integer `position` isn't at the center, which means
           the offsets aren't symmetric. */
        for(int j = 1; j < RADIUS; ++j) {
            /* Don't go further than current radius - 1 (i.e., excluding the
               corner). The loop needs to be compile-time bound otherwise some
               drivers crash on it, so we're breaking inside instead of having
               this directly in the loop condition. */
            if(j >= i) break;

            /* Again, actual distance from the center is half a pixel less (see
               the diagram in main() below) */
            const mediump float jF = float(j) - 0.5;
            const mediump float sideDistanceSquared = iF*iF + jF*jF;
            if(sideDistanceSquared >= minDistanceSquared)
                break;
            if(hasValue(position, ivec2(  j,   i)) != isInside || /* 0, 8 */
               hasValue(position, ivec2(1-j,   i)) != isInside || /* 1, 9 */
               hasValue(position, ivec2(1-i,   j)) != isInside || /* 2, a */
               hasValue(position, ivec2(1-i, 1-j)) != isInside || /* 3, b */
               hasValue(position, ivec2(1-j, 1-i)) != isInside || /* 4, c */
               hasValue(position, ivec2(  j, 1-i)) != isInside || /* 5, d */
               hasValue(position, ivec2(  i, 1-j)) != isInside || /* 6, e */
               hasValue(position, ivec2(  i,   j)) != isInside) { /* 7, f */
                minDistanceSquared = sideDistanceSquared;
                break;
            }
        }

        /* Then check for the corners, which is just four cases. Again the
           integer `position` isn't at the center, which means the offsets
           aren't symmetric.

            i = 2   i = 3

                    1    0
            1  0

             p        p
            2  3
                    2    3

           If we find something, it's most probably not the nearest distance,
           since the following iterations can be much closer. */
        const mediump float cornerDistanceSquared = 2.0*iF*iF;
        if(cornerDistanceSquared >= minDistanceSquared)
            continue;
        if(hasValue(position, ivec2(  i,   i)) != isInside ||
           hasValue(position, ivec2(1-i,   i)) != isInside ||
           hasValue(position, ivec2(1-i, 1-i)) != isInside ||
           hasValue(position, ivec2(  i, 1-i)) != isInside) {
            minDistanceSquared = cornerDistanceSquared;
        }
    }

    return minDistanceSquared;
}

void main() {
    /* The -0.5 is to make the position aligned with the center of the input
       pixel, and in the integer case to make the conversion not round up */
    #ifdef TEXELFETCH_USABLE
    const mediump ivec2 position = ivec2(inputTextureCoordinates - vec2(0.5));
    #else
    const mediump vec2 position = inputTextureCoordinates - vec2(0.5)*imageSizeInverted;
    #endif

    /*  +=======+=====
        H | | | H | |    Assuming the ratio of input and output sizes is a
        H-+-+-+-H-+-+    multiple of 2 (in this diagram it's scaled 4x), the
        H |k|l| H |      center of the output pixel `o` is always between four
        H-+-o-+-H-+      input pixels `i`, `j`, `k`, `l`.Then, depending on
        H |i|j| H        which of the four input pixels have a value > 0.5, the
        H-+-+-+-H-       following six cases can happen. Other combinations are
        H | | |          just variants of these.
        +=====

       - In case A and F, the pixel is either inside or outside and we have to
         look further around to know the distance to the edge.
       - In case B and C, the pixel is exactly on the edge, i.e. distance is
         0, and we don't need to look further.
       - In case D and E, the pixel is at a distance of 0.5 or (0.5, 0.5) from
         the edge, and we don't need to look further.

          A        B        C        D      | E        F
        k---l    k---l    k   l    k   l   -k   l    k   l
        |   |    |  /        /     |
        | o |    | o        o      | o        o        o
        |   |    |/        /       |
        i---j    i   j    i   j    i   j    i   j    i   j

       The main complication is distinguishing cases C and D, in all other
       cases it's simply a matter of counting the number of pixels that have a
       value of > 0.5.

       Note that the integer `position` isn't at `o` (which is between pixels)
       but aliases `i`. Which means the offsets aren't symmetric. */
    const bool i = hasValue(position, ivec2(0, 0));
    const bool j = hasValue(position, ivec2(1, 0));
    const bool k = hasValue(position, ivec2(0, 1));
    const bool l = hasValue(position, ivec2(1, 1));

    mediump float minDistance;
    bool isInside;
    const int sum = int(i) + int(j) + int(k) + int(l);
    /* Case B */
    if(sum == 3) {
        isInside = false;
        minDistance = 0.0;
    /* Case C and D */
    } else if(sum == 2) {
        isInside = false;
        if((i && l) || (j && k))
            minDistance = 0.0;
        else
            minDistance = 0.5;
    /* Case E */
    } else if(sum == 1) {
        isInside = false;
        /* sqrt(0.5*0.5 + 0.5*0.5) */
        minDistance = 0.7071067811865475;
    /* Case A and F */
    } else {
        isInside = sum == 4;
        minDistance = sqrt(float(findMinDistanceSquared(position, isInside)));
    }

    /* Final signed distance, normalized from [-radius + 0.5, radius + 0.5] to
       [0, 1] */
    const highp float halfSign = isInside ? 0.5 : -0.5;
    value = halfSign*minDistance/(float(RADIUS) + 0.5) + 0.5;
}
