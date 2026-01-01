/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include "Magnum/ImageView.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Range.h"
#include "Magnum/TextureTools/DistanceFieldGL.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainTextureToolsGL();
void mainTextureToolsGL() {
/* On ES it requires one extra parameter with input image size */
#ifndef MAGNUM_TARGET_GLES
{
/* [DistanceFieldGL] */
ImageView2D image = DOXYGEN_ELLIPSIS(ImageView2D{PixelFormat{}, {}, nullptr});

GL::Texture2D input;
input
    .setMinificationFilter(GL::SamplerFilter::Nearest)
    .setMagnificationFilter(GL::SamplerFilter::Nearest)
    .setStorage(1, GL::textureFormat(image.format()), image.size())
    .setSubImage(0, {}, image);

GL::Texture2D output;
output.setStorage(1, GL::TextureFormat::R8, image.size()/4);

TextureTools::DistanceFieldGL distanceField{12};
distanceField(input, output, {{}, image.size()/4});
/* [DistanceFieldGL] */
}
#endif

{
ImageView2D image{PixelFormat{}, {}, nullptr};
TextureTools::DistanceFieldGL distanceField{0};
/* [DistanceFieldGL-parameters-rendering] */
Vector2 renderedSize = DOXYGEN_ELLIPSIS({});
Float ratio = renderedSize.x()/(image.size().x()*distanceField.radius());
/* [DistanceFieldGL-parameters-rendering] */
static_cast<void>(ratio);
}

/* On ES it requires one extra parameter with input image size */
#ifndef MAGNUM_TARGET_GLES
{
ImageView2D image{PixelFormat{}, {}, nullptr};
GL::Texture2D input, output;
/* [DistanceFieldGL-incremental] */
/* Construct and set up just once */
TextureTools::DistanceFieldGL distanceField{DOXYGEN_ELLIPSIS(0)};
GL::Framebuffer outputFramebuffer{{{}, image.size()/4}};
outputFramebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, output, 0);

/* Call the distance field processing each time the input texture is updated */
Range2Di updatedRange = DOXYGEN_ELLIPSIS({});
distanceField(input, output, updatedRange);
/* [DistanceFieldGL-incremental] */
}
#endif
}
