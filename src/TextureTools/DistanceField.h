#ifndef Magnum_TextureTools_DistanceField_h
#define Magnum_TextureTools_DistanceField_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Function Magnum::TextureTools::distanceField()
 */

#include "Magnum.h"

#include "TextureTools/magnumTextureToolsVisibility.h"

namespace Magnum { namespace TextureTools {

/**
@brief Create signed distance field
@param input        Input texture
@param output       Output texture
@param rectangle    Rectangle in output texture where to render
@param radius       Max lookup radius in input texture

Converts binary image (stored in red channel of @p input) to signed distance
field (stored in red channel in @p rectangle of @p output). The purpose of this
function is to convert high-resolution binary image (such as vector artwork or
font glyphs) to low-resolution grayscale image. The image will then occupy much
less memory and can be scaled without aliasing issues. Additionally it provides
foundation for features like outlining, glow or drop shadow essentialy for free.

For each pixel inside @p rectangle the algorithm looks at corresponding pixel in
@p input and tries to find nearest pixel of opposite color in area given by
@p radius. Signed distance between the points is then saved as value of given
pixel in @p output. Value of `0` means that the pixel was originally colored
white and nearest black pixel is farther than @p radius, value of `1` means that
the pixel was originally black and nearest white pixel is farther than
@p radius. Values around `0.5` are around edges.

The resulting texture can be used with bilinear filtering. It can be converted
back to binary form in shader using e.g. GLSL `smoothstep()` function with step
around `0.5` to create antialiased edges. Or you can exploit the distance field
features to create many other effects.

Based on: *Chris Green - Improved Alpha-Tested Magnification for Vector Textures
and Special Effects, SIGGRAPH 2007,
http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf*

@attention This is GPU-only implementation, so it expects active context.
*/
void MAGNUM_TEXTURETOOLS_EXPORT distanceField(Texture2D* input, Texture2D* output, const Rectanglei& rectangle, const Int radius);

}}

#endif
