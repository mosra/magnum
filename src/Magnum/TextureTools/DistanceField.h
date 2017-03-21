#ifndef Magnum_TextureTools_DistanceField_h
#define Magnum_TextureTools_DistanceField_h
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

/** @file
 * @brief Function @ref Magnum::TextureTools::distanceField()
 */

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/Math/Vector2.h"
#endif
#include "Magnum/Magnum.h"

#include "Magnum/TextureTools/visibility.h"

namespace Magnum { namespace TextureTools {

/**
@brief Create signed distance field
@param input        Input texture
@param output       Output texture
@param rectangle    Rectangle in output texture where to render
@param radius       Max lookup radius in input texture
@param imageSize    Input texture size. Needed only in OpenGL ES, in desktop
    OpenGL the information is gathered automatically using
    @ref Texture2D::imageSize().

Converts binary image (stored in red channel of @p input) to signed distance
field (stored in red channel in @p rectangle of @p output). The purpose of this
function is to convert high-resolution binary image (such as vector artwork or
font glyphs) to low-resolution grayscale image. The image will then occupy much
less memory and can be scaled without aliasing issues. Additionally it provides
foundation for features like outlining, glow or drop shadow essentially for
free.

You can also use the @ref magnum-distancefieldconverter "magnum-distancefieldconverter"
utility to do distance field conversion on command-line. By extension, this
functionality is also provided through @ref magnum-fontconverter "magnum-fontconverter"
utility.

### The algorithm

For each pixel inside @p rectangle the algorithm looks at corresponding pixel in
@p input and tries to find nearest pixel of opposite color in area given by
@p radius. Signed distance between the points is then saved as value of given
pixel in @p output. Value of `1.0` means that the pixel was originally colored
white and nearest black pixel is farther than @p radius, value of `0.0` means
that the pixel was originally black and nearest white pixel is farther than
@p radius. Values around `0.5` are around edges.

The resulting texture can be used with bilinear filtering. It can be converted
back to binary form in shader using e.g. GLSL `smoothstep()` function with step
around `0.5` to create antialiased edges. Or you can exploit the distance field
features to create many other effects. See also @ref Shaders::DistanceFieldVector.

Based on: *Chris Green - Improved Alpha-Tested Magnification for Vector Textures
and Special Effects, SIGGRAPH 2007,
http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf*

@attention This is GPU-only implementation, so it expects active context.

@note If internal format of @p output texture is not renderable, this function
    prints message to error output and does nothing. In desktop OpenGL and
    OpenGL ES 3.0 it's common to render to @ref TextureFormat::R8. In OpenGL ES
    2.0 you can use @ref TextureFormat::Red if @extension{EXT,texture_rg} is
    available, if not, the smallest but still inefficient supported format is
    in most cases @ref TextureFormat::RGB, rendering to @ref TextureFormat::Luminance
    is not supported in most cases.

@bug ES (and maybe GL < 3.20) implementation behaves slightly different
    (jaggies, visible e.g. when rendering outlined fonts)
*/
#ifndef MAGNUM_TARGET_GLES
void MAGNUM_TEXTURETOOLS_EXPORT distanceField(Texture2D& input, Texture2D& output, const Range2Di& rectangle, Int radius, const Vector2i& imageSize = Vector2i());
#else
void MAGNUM_TEXTURETOOLS_EXPORT distanceField(Texture2D& input, Texture2D& output, const Range2Di& rectangle, Int radius, const Vector2i& imageSize);
#endif

}}

#endif
