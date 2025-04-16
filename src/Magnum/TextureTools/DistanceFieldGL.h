#ifndef Magnum_TextureTools_DistanceFieldGL_h
#define Magnum_TextureTools_DistanceFieldGL_h
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

/** @file
 * @brief Class @ref Magnum::TextureTools::DistanceFieldGL
 * @m_since_latest
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#include "Magnum/TextureTools/visibility.h"

namespace Magnum { namespace TextureTools {

/**
@brief Create a signed distance field using OpenGL
@m_since_latest

Converts a high-resolution black and white image (such as vector artwork or
font glyphs) to a low-resolution grayscale image with each pixel being a signed
distance to the nearest edge in the original image. Such a distance field image
then occupies much less memory as the spatial resolution is converted to pixel
values amd can be scaled without it being jaggy at small sizes or blurry when
large. It also makes it possible to implement outlining, glow or drop shadow
essentially for free.

@m_class{m-row}

@parblock

@m_div{m-col-m-6 m-nopadt}
@image html distancefield-src.png width=256px
@m_enddiv

@m_div{m-col-m-6 m-nopadt}
@image html distancefield-dst.png width=256px
@m_enddiv

@endparblock

You can use the @ref magnum-distancefieldconverter "magnum-distancefieldconverter"
utility to perform distance field conversion on a command line. Distance field
textures can be rendered with @ref Shaders::DistanceFieldVectorGL, this
functionality is also used to implement @ref Text::DistanceFieldGlyphCacheGL
for text rendering, which is then exposed in the
@ref magnum-fontconverter "magnum-fontconverter" utility.

Algorithm based on: *Chris Green - Improved Alpha-Tested Magnification for
Vector Textures and Special Effects, SIGGRAPH 2007,
http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf*

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section TextureTools-DistanceFieldGL-usage Example usage

The following snippet uploads an image to a @ref GL::Texture2D, creates a
second smaller @ref GL::Texture2D for the output and then performs the distance
field conversion with a radius of @cpp 12 @ce pixels and spanning the whole
output image area:

@snippet TextureTools-gl.cpp DistanceFieldGL

In the output (an example of which is shown above on the right, scaled up to
match the original), value of @cpp 1.0 @ce (when normalized from the actual
pixel format, so @cpp 255 @ce for the @ref GL::TextureFormat::R8 used above)
means that the pixel was originally colored white and nearest black pixel is
further away than the specified radius. Correspondingly, value of @cpp 0.0 @ce
means that the pixel was originally black and nearest white pixel is further
away than the radius. Edges are thus at values around @cpp 0.5 @ce.

The resulting texture is meant to be used with bilinear filtering, i.e. with
@ref GL::SamplerFilter::Linear. To get the original image back, the GLSL
@glsl smoothstep() @ce function can be used as shown in the following snippet,
with a step around @cpp 0.5 @ce and @cpp smoothness @ce being a configurable
factor controlling edge smoothness.

@code{.glsl}
float factor = smoothstep(0.5 - smoothness,
                          0.5 + smoothness,
                          texture(distanceFieldTexture, coordinates).r);
@endcode

The @ref Shaders::DistanceFieldVectorGL implements also outlining, edge dilate,
erode, and other effects with the distance field input.

@section TextureTools-DistanceFieldGL-parameters Parameter tuning

Quality of the generated distance field is affected by two variables --- the
ratio between input and output size, and the radius. A bigger size ratio will
result in bigger memory savings but at the cost of losing finer detail, so the
choice depends mainly on the content that's actually being processed. The image
shown above could get away with being reduced down even eight or sixteen times
without noticeable quality loss, on the other hand vector art consisting of
fine lines or for example CJK glyphs might likely have artifacts already with
the ratio of @cpp 4 @ce used above.

The radius should be at least as large as the size ratio in order to contribute
to at least one pixel on every side of an edge in the output, otherwise the
resulting rendering will be extremely blocky. After that, its value is dictated
mainly by the desired use of the output --- if you need to draw the output with
larger antialiasing smoothness, big outlines or shadows, the radius needs to
get bigger. With the size ratio of @cpp 4 @ce and radius of @cpp 12 @ce used
above, the output allows for smoothness, outline or other effect ±3 pixels
around the edge.

Finally, with very large radii you may run into quantization issues with 8-bit
texture formats, causing again blocky artifacts. A solution is then to use
@ref GL::TextureFormat::R16 instead. Using an even larger format probably won't
improve the result any further, and since the distance is normalized to a
@f$ [0, 1] @f$ range, a floating-point format such as
@ref GL::TextureFormat::R16F would also not, but rather resulting in worse
precision than the 16-bit normalized integer format.

@subsection TextureTools-DistanceFieldGL-parameters-rendering Effect of input parameters on final rendered image

In order to ensure consistent look when rendering regardless of the parameters
picked for distance field conversion, the rendering has to take the input size
and radius into account. Assuming @cpp image.size() @ce is size of the input
image and `renderedSize` is pixel size at which the distance field image is
drawn on the screen, the `ratio` calculated below is then distance that
corresponds to one pixel on the screen. Note that the ratio at which the
distance field output is sized down has no effect here, and thus it can be
chosen dynamically to achieve desired quality / memory use tradeoff.

@snippet TextureTools-gl.cpp DistanceFieldGL-parameters-rendering

For a concrete example, if the input was @cpp {256, 256} @ce, it's now rendered
at a size of @cpp {128, 128} @ce and it was converted with a radius of
@cpp 12 @ce, the `ratio` will be @cpp 1.0f/6 @ce. I.e., if you set the shader
`smoothness` to @cpp 1.0f/6 @ce, the edge smoothness radius will be exactly one
pixel.

@section TextureTools-DistanceFieldGL-incremental Incremental distance field calculation

Besides converting whole texture at once, it's possible to process just a part.
This is mainly useful with use cases like dynamically populated texture
atlases, where it'd be wasteful to repeatedly process already filled parts.
The *output* area to process is specified with the third argument to
@ref operator()() (which was above set to the whole output texture size). The
input texture is still taken as a whole, i.e. it's assumed that it contains
exactly the data meant to be processed and placed into the output area.
Additionally, to avoid needless OpenGL state changes, it's recommended to
supply a @ref GL::Framebuffer with the output texture attached so the
implementation doesn't need to create a temporary one each time:

@snippet TextureTools-gl.cpp DistanceFieldGL-incremental
*/
class MAGNUM_TEXTURETOOLS_EXPORT DistanceFieldGL {
    public:
        /**
         * @brief Constructor
         * @param radius        Distance field calculation radius
         *
         * Prepares the shader and other internal state for given @p radius.
         */
        explicit DistanceFieldGL(UnsignedInt radius);

        /**
         * @brief Construct without creating the internal OpenGL state
         * @m_since_latest
         *
         * The constructed instance is equivalent to moved-from state, i.e. no
         * APIs can be safely called on the object. Useful in cases where you
         * will overwrite the instance later anyway. Move another object over
         * it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit DistanceFieldGL(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        DistanceFieldGL(const DistanceFieldGL&) = delete;

        /**
         * @brief Move constructor
         * @m_since_latest
         *
         * Performs a destructive move, i.e. the original object isn't usable
         * afterwards anymore.
         */
        DistanceFieldGL(DistanceFieldGL&&) noexcept;

        ~DistanceFieldGL();

        /** @brief Copying is not allowed */
        DistanceFieldGL& operator=(const DistanceFieldGL&) = delete;

        /** @brief Move constructor */
        DistanceFieldGL& operator=(DistanceFieldGL&&) noexcept;

        /** @brief Distance field calculation radius */
        UnsignedInt radius() const;

        /**
         * @brief Calculate distance field to a framebuffer
         * @param input         Input texture
         * @param output        Output framebuffer
         * @param rectangle     Rectangle in the output where to render
         * @param imageSize     Input texture size. Needed only for OpenGL ES,
         *      on desktop GL the size is queried automatically using
         *      @ref GL::Texture2D::imageSize() and this parameter is ignored.
         * @m_since_latest
         *
         * The @p output texture is expected to have a framebuffer-drawable
         * @ref GL::TextureFormat. On desktop OpenGL and
         * OpenGL ES 3.0 it's common to render to @ref GL::TextureFormat::R8.
         * On OpenGL ES 2.0 you can use @ref GL::TextureFormat::Red if
         * @gl_extension{EXT,texture_rg} is available; if not, the smallest yet
         * still quite inefficient supported format is in most cases
         * @ref GL::TextureFormat::RGB. The @ref GL::TextureFormat::Luminance
         * format usually isn't renderable.
         *
         * Additionally, the ratio of the @p input size (or @p imageSize on
         * OpenGL ES) and @p rectangle size is expected to be a multiple of 2,
         * as that's what the generator shader relies on for correct pixel
         * addressing.
         *
         * @attention For consistent results on implementations that don't
         *      support @glsl texelFetch() @ce and have rely on regular texture
         *      filtering it's recommended to use @ref GL::SamplerFilter::Nearest
         *      on the @p input.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void operator()(GL::Texture2D& input, GL::Framebuffer& output, const Range2Di& rectangle, const Vector2i& imageSize
            #ifndef MAGNUM_TARGET_GLES
            = {}
            #endif
        );
        #else
        /* To avoid having to include Vector2 */
        void operator()(GL::Texture2D& input, GL::Framebuffer& output, const Range2Di& rectangle, const Vector2i& imageSize);
        #ifndef MAGNUM_TARGET_GLES
        void operator()(GL::Texture2D& input, GL::Framebuffer& output, const Range2Di& rectangle);
        #endif
        #endif

        /**
         * @brief Calculate distance field to a texture
         * @param input         Input texture
         * @param output        Output texture
         * @param rectangle     Rectangle in the output where to render
         * @param imageSize     Input texture size. Needed only for OpenGL ES,
         *      on desktop GL the information is gathered automatically using
         *      @ref GL::Texture2D::imageSize().
         *
         * Convenience variant of @ref operator()(GL::Texture2D&, GL::Framebuffer&, const Range2Di&, const Vector2i&)
         * that creates a temporary framebuffer with @p output attached and
         * destroys it again after the operation.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void operator()(GL::Texture2D& input, GL::Texture2D& output, const Range2Di& rectangle, const Vector2i& imageSize
            #ifndef MAGNUM_TARGET_GLES
            = {}
            #endif
        );
        #else
        /* To avoid having to include Vector2 */
        void operator()(GL::Texture2D& input, GL::Texture2D& output, const Range2Di& rectangle, const Vector2i& imageSize);
        #ifndef MAGNUM_TARGET_GLES
        void operator()(GL::Texture2D& input, GL::Texture2D& output, const Range2Di& rectangle);
        #endif
        #endif

    private:
        struct State;
        Containers::Pointer<State> _state;
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
