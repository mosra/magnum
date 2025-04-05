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

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section TextureTools-DistanceFieldGL-algorithm The algorithm

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
@ref Shaders::DistanceFieldVectorGL.

Based on: *Chris Green - Improved Alpha-Tested Magnification for Vector Textures
and Special Effects, SIGGRAPH 2007,
http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf*

@attention This is a GPU-only implementation, so it expects an active GL
    context.
*/
class MAGNUM_TEXTURETOOLS_EXPORT DistanceFieldGL {
    public:
        /**
         * @brief Constructor
         * @param radius       Max lookup radius in the input texture
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

        /** @brief Max lookup radius */
        UnsignedInt radius() const;

        /**
         * @brief Calculate the distance field to a framebuffer
         * @param input        Input texture
         * @param output       Output framebuffer
         * @param rectangle    Rectangle in output texture where to render
         * @param imageSize    Input texture size. Needed only for OpenGL ES,
         *      on desktop GL the information is gathered automatically using
         *      @ref GL::Texture2D::imageSize().
         * @m_since_latest
         *
         * The @p output texture is expected to have a framebuffer-drawable
         * @ref GL::TextureFormat. On desktop OpenGL and
         * OpenGL ES 3.0 it's common to render to @ref GL::TextureFormat::R8.
         * On OpenGL ES 2.0 you can use @ref GL::TextureFormat::Red if
         * @gl_extension{EXT,texture_rg} is available; if not, the smallest but
         * still inefficient supported format is in most cases
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
         * @brief Calculate the distance field to a texture
         * @param input        Input texture
         * @param output       Output texture
         * @param rectangle    Rectangle in output texture where to render
         * @param imageSize    Input texture size. Needed only for OpenGL ES,
         *      on desktop GL the information is gathered automatically using
         *      @ref GL::Texture2D::imageSize().
         *
         * Creates a framebuffer with @p output attached and calls
         * @ref operator()(GL::Texture2D&, GL::Framebuffer&, const Range2Di&, const Vector2i&).
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
