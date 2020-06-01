#ifndef Magnum_TextureTools_DistanceField_h
#define Magnum_TextureTools_DistanceField_h
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

/** @file
 * @brief Class @ref Magnum::TextureTools::DistanceField
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/Math/Vector2.h"
#endif
#include "Magnum/TextureTools/visibility.h"

namespace Magnum { namespace TextureTools {

/**
@brief Create a signed distance field

Converts a binary black/white image (stored in the red channel of @p input) to
a signed distance field (stored in the red channel of @p output @p rectangle).
The purpose of this function is to convert a high-resolution binary image (such
as vector artwork or font glyphs) to a low-resolution grayscale image. The
image will then occupy much less memory and can be scaled without aliasing
issues. Additionally it provides foundation for features like outlining, glow
or drop shadow essentially for free.

You can also use the @ref magnum-distancefieldconverter "magnum-distancefieldconverter"
utility to do distance field conversion on command-line. This functionality is
also used inside the @ref magnum-fontconverter "magnum-fontconverter" utility.

@section TextureTools-DistanceField-algorithm The algorithm

For each pixel inside the @p output sub-rectangle the algorithm looks at
corresponding pixel in the input and tries to find nearest pixel of opposite
color in an area defined @p radius. Signed distance between the points is then
saved as value of given pixel in @p output. Value of 1.0 means that the pixel
was originally colored white and nearest black pixel is farther than @p radius,
value of 0.0 means that the pixel was originally black and nearest white pixel
is farther than @p radius. Values around 0.5 are around edges.

The resulting texture can be used with bilinear filtering. It can be converted
back to binary form in shader using e.g. GLSL @glsl smoothstep() @ce function
with step around 0.5 to create antialiased edges. Or you can exploit the
distance field features to create many other effects. See also
@ref Shaders::DistanceFieldVector.

Based on: *Chris Green - Improved Alpha-Tested Magnification for Vector Textures
and Special Effects, SIGGRAPH 2007,
http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf*

@attention This is a GPU-only implementation, so it expects an active GL
    context.

@note If internal format of @p output texture is not renderable, this function
    prints a message to error output and does nothing. On desktop OpenGL and
    OpenGL ES 3.0 it's common to render to @ref GL::TextureFormat::R8. On
    OpenGL ES 2.0 you can use @ref GL::TextureFormat::Red if
    @gl_extension{EXT,texture_rg} is available; if not, the smallest but still
    inefficient supported format is in most cases @ref GL::TextureFormat::RGB,
    rendering to @ref GL::TextureFormat::Luminance is not supported in most
    cases.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class MAGNUM_TEXTURETOOLS_EXPORT DistanceField {
    public:
        /**
         * @brief Constructor
         * @param radius       Max lookup radius in the input texture
         *
         * Prepares the shader and other internal state for given @p radius.
         */
        explicit DistanceField(UnsignedInt radius);

        ~DistanceField();

        /** @brief Max lookup radius */
        UnsignedInt radius() const;

        /**
         * @brief Calculate the distance field
         * @param input        Input texture
         * @param output       Output texture
         * @param rectangle    Rectangle in output texture where to render
         * @param imageSize    Input texture size. Needed only for OpenGL ES,
         *      on desktop GL the information is gathered automatically using
         *      @ref GL::Texture2D::imageSize().
         */
        void operator()(GL::Texture2D& input, GL::Texture2D& output, const Range2Di& rectangle, const Vector2i& imageSize
            #ifndef MAGNUM_TARGET_GLES
            = {}
            #endif
        );

    private:
        struct State;
        Containers::Pointer<State> _state;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Create a signed distance field
@m_deprecated_since{2019,01} Deprecated due to inefficiency of its
    statelessness when doing batch processing. Use the @ref DistanceField class
    instead.
*/
inline CORRADE_DEPRECATED("use the DistanceField class instead") void distanceField(GL::Texture2D& input, GL::Texture2D& output, const Range2Di& rectangle, Int radius, const Vector2i& imageSize
    #ifndef MAGNUM_TARGET_GLES
    = Vector2i{}
    #endif
) {
    DistanceField{UnsignedInt(radius)}(input, output, rectangle, imageSize);
}
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
