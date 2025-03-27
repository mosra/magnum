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

#include "DistanceFieldGlyphCacheGL.h"

#include <Corrade/Containers/Optional.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#endif
#if !defined(CORRADE_NO_ASSERT) || defined(MAGNUM_BUILD_DEPRECATED)
#include "Magnum/GL/PixelFormat.h"
#endif
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace Text {

DistanceFieldGlyphCacheGL::DistanceFieldGlyphCacheGL(const Vector2i& size, const Vector2i& processedSize, const UnsignedInt radius):
    GlyphCacheGL{PixelFormat::R8Unorm, size,
        #if !defined(MAGNUM_TARGET_GLES) || !defined(MAGNUM_TARGET_GLES2)
        PixelFormat::R8Unorm,
        #else
        #ifndef MAGNUM_TARGET_WEBGL
        /* Without EXT_texture_rg, PixelFormat::R8Unorm maps to Luminance which
           is not renderable in most cases. RGB is *theoretically* space-
           efficient but practically the driver uses RGBA internally anyway, so
           just use RGBA. */
        GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>() ?
            PixelFormat::R8Unorm :
        #endif
            PixelFormat::RGBA8Unorm,
        #endif
        processedSize, Vector2i(radius)},
    _distanceField{radius}
{
    /* Replicating the assertion from TextureTools::DistanceFieldGL so it gets
       checked during construction already instead of only later during the
       setImage() call */
    CORRADE_ASSERT(size % processedSize == Vector2i{0} &&
                   (size/processedSize) % 2 == Vector2i{0},
        "Text::DistanceFieldGlyphCacheGL: expected source and processed size ratio to be a multiple of 2, got" << Debug::packed << size << "and" << Debug::packed << processedSize, );

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* On ES2 print a warning to make it known that EXT_texture_rg wasn't
       available. On WebGL 1 this is the case always, so a warning would be
       just a noise. */
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        Warning() << "Text::DistanceFieldGlyphCacheGL:" << GL::Extensions::EXT::texture_rg::string() << "not supported, using a full RGBA format for the distance field texture";
    #endif
}

DistanceFieldGlyphCacheGL::DistanceFieldGlyphCacheGL(NoCreateT) noexcept: GlyphCacheGL{NoCreate}, _distanceField{NoCreate} {}

GlyphCacheFeatures DistanceFieldGlyphCacheGL::doFeatures() const {
    return GlyphCacheFeature::ImageProcessing
        #ifndef MAGNUM_TARGET_GLES
        |GlyphCacheFeature::ProcessedImageDownload
        #endif
        ;
}

void DistanceFieldGlyphCacheGL::doSetImage(const Vector2i& offset, const ImageView2D& image) {
    GL::Texture2D input;
    input.setWrapping(GL::SamplerWrapping::ClampToEdge)
        /* Use nearest filter to avoid minor rounding errors on ES2 compared to
           texelFetch() on ES3+ */
        .setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest);

    /* The constructor already checked that the ratio is an integer multiple,
       so this division should lead to no information loss */
    CORRADE_INTERNAL_ASSERT(size().xy() % processedSize().xy() == Vector2i{0});
    const Vector2i ratio = size().xy()/processedSize().xy();

    /* Upload the input texture and create a distance field from it. On ES2
       without EXT_unpack_subimage and on WebGL 1 there's no possibility to
       upload just a slice of the input, upload the whole image instead by
       ignoring the PixelStorage properties of the input and also process it as
       a whole. */
    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::unpack_subimage>())
    #endif
    {
        input.setImage(0, GL::textureFormat(image.format()), ImageView2D{image.format(), size().xy(), image.data()});
        _distanceField(input, texture(), {{}, size().xy()/ratio}, size().xy());
        #ifdef MAGNUM_TARGET_WEBGL
        static_cast<void>(offset);
        #endif
    }
    #ifndef MAGNUM_TARGET_WEBGL
    else
    #endif
    #endif
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    {
        /* The image range was already expanded to include the padding in
           flushImage() */
        CORRADE_INTERNAL_ASSERT(image.storage().skip().xy() == offset);
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(offset);
        #endif
        const Vector2i paddedMin = image.storage().skip().xy();
        const Vector2i paddedMax = image.size() + image.storage().skip().xy();

        /* TextureTools::DistanceFieldGL expects the input size and output
           rectangle size ratio to be a multiple of 2 in order for the shader
           to perform pixel addressing correctly. That might not always be the
           case with the rectangle passed to flushImage(), so round the
           paddedMin *down* to a multiple of the ratio and paddedMax *up* to a
           multiple of the ratio. */
        const Vector2i paddedMinRounded = ratio*(paddedMin/ratio);
        const Vector2i paddedMaxRounded = ratio*((paddedMax + ratio - Vector2i{1})/ratio);
        /* As the size is also a multiple of ratio, the resulting size should
           not get larger. */
        CORRADE_INTERNAL_ASSERT(paddedMaxRounded <= size().xy());

        const ImageView2D paddedImage{
            PixelStorage{image.storage()}
                .setSkip({paddedMinRounded, image.storage().skip().z()}),
            image.format(),
            paddedMaxRounded - paddedMinRounded,
            image.data()};

        input.setImage(0, GL::textureFormat(paddedImage.format()), paddedImage);
        _distanceField(input, texture(), Range2Di::fromSize(paddedMinRounded/ratio, paddedImage.size()/ratio), paddedImage.size());
    }
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
void DistanceFieldGlyphCacheGL::setDistanceFieldImage(const Vector2i& offset, const ImageView2D& image) {
    /* The original function accepted GL pixel formats as well, try to
       translate them back to the generic format. If that fails, pass the image
       as-is let the base implementation deal with that instead.

       Replacing the whole view instead of just the format so we don't need to
       do any special-casing for when the format stays implementation-specific
       and requires a pixel size to be specified externally. */
    ImageView2D imageToUse = image;
    if(isPixelFormatImplementationSpecific(image.format())) {
        if(const Containers::Optional<PixelFormat> candidateFormat = GL::genericPixelFormat(pixelFormatUnwrap<GL::PixelFormat>(image.format()), GL::PixelType(image.formatExtra())))
            imageToUse = ImageView2D{image.storage(), *candidateFormat, image.size(), image.data()};
    }

    setProcessedImage(offset, imageToUse);
}
#endif

}}
