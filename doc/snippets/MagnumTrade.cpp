/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/ObjectData2D.h"
#include "Magnum/Trade/ObjectData3D.h"
#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Texture.h"
#endif

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {

{
/* [ImageData-construction] */
Containers::Array<char> data;
Trade::ImageData2D image{PixelFormat::RGB8Unorm, {32, 32}, std::move(data)};
/* [ImageData-construction] */
}

{
/* [ImageData-construction-compressed] */
Containers::Array<char> data;
Trade::ImageData2D image{CompressedPixelFormat::Bc1RGBUnorm,
    {32, 32}, std::move(data)};
/* [ImageData-construction-compressed] */
}

#ifdef MAGNUM_TARGET_GL
{
/* [ImageData-usage] */
std::unique_ptr<Trade::AbstractImporter> importer;
Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
if(!image) Fatal{} << "Oopsie!";

GL::Texture2D texture;
// ...
if(!image->isCompressed())
    texture.setSubImage(0, {}, *image);
else
    texture.setCompressedSubImage(0, {}, *image);
/* [ImageData-usage] */
}
#endif

{
Trade::ObjectData2D& foo();
Trade::ObjectData2D& data = foo();
/* [ObjectData2D-transformation] */
Matrix3 transformation =
    Matrix3::from(data.rotation().toMatrix(), data.translation())*
    Matrix3::scaling(data.scaling());
/* [ObjectData2D-transformation] */
static_cast<void>(transformation);
}

{
Trade::ObjectData3D& bar();
Trade::ObjectData3D& data = bar();
/* [ObjectData3D-transformation] */
Matrix4 transformation =
    Matrix4::from(data.rotation().toMatrix(), data.translation())*
    Matrix4::scaling(data.scaling());
/* [ObjectData3D-transformation] */
static_cast<void>(transformation);
}

}
