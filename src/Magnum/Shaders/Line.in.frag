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

/* This file, along with *.in.vert, is copied verbatim between the Shaders
   and Ui libraries */

mediump float lineBlendFactor(
    in highp vec2 centerDistanceSigned,
    in highp float halfSegmentLength,
    in highp float hasCap,
    in mediump const float width,
    in mediump const float smoothness)
{
    /* Calculate a distance from the original line endpoint (B). Assuming a cap
       that's not a butt, actual quad vertices (2, 3 on the left diagram) would
       be at a distance `width/2` in both X and Y (in the space of the line
       segment, where X is in direction of the segment and Y is in direction to
       the line edges):

          ----------2                       --------2
                    |                               |
        [0,0]   B   |                     [0,0]     B
                    |                               |
          ----------3                       --------3

       For a butt cap, the endpoint B would be at the edge instead (right
       diagram) -- to have handling consistent for all cap styles, add
       `width/2` to the center distance in that case. For fragments on the left
       of B the X distance would be negative, make it 0 in that case
       instead. */
    highp const vec2 centerDistance = abs(centerDistanceSigned);
    highp vec2 endpointDistance = vec2(max(centerDistance.x
        #ifdef CAP_STYLE_BUTT
        + width*0.5
        #endif
        - halfSegmentLength, 0.0), centerDistance.y);

    /* If hasCap is negative, it means the nearest endpoint is a join, not a
       cap. Thus no smoothing happens in the direction of a cap, i.e. same as
       if we'd be at the center of the line. */
    if(hasCap < 0.0) endpointDistance.x = 0.0;

    /* Calculate a single distance factor out of the two-dimensional endpoint
       distance. This will form the cap shape. */
    #if defined(CAP_STYLE_BUTT) || defined(CAP_STYLE_SQUARE)
    highp const float distance1D = max(endpointDistance.x, endpointDistance.y);
    #elif defined(CAP_STYLE_ROUND)
    highp const float distance1D = length(endpointDistance);
    #elif defined(CAP_STYLE_TRIANGLE)
    highp const float distance1D = endpointDistance.x + endpointDistance.y;
    #else
    #error
    #endif

    return smoothstep(width*0.5 - smoothness, width*0.5 + smoothness, distance1D);
}
