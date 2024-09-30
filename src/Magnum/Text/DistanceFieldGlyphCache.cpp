/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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

#include "DistanceFieldGlyphCache.h"

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef CORRADE_NO_ASSERT
#include "Magnum/GL/PixelFormat.h"
#endif
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Range.h"
#include "Magnum/TextureTools/DistanceField.h"

namespace Magnum { namespace Text {

DistanceFieldGlyphCache::DistanceFieldGlyphCache(const Vector2i& size, const Vector2i& processedSize, const UnsignedInt radius):
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    GlyphCache(GL::TextureFormat::R8, size, processedSize, Vector2i(radius)),
    #elif !defined(MAGNUM_TARGET_WEBGL)
    /* Luminance is not renderable in most cases. RGB is *theoretically*
       space-efficient but practically the driver uses RGBA internally anyway,
       so just use RGBA. */
    GlyphCache(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>() ?
        GL::TextureFormat::R8 : GL::TextureFormat::RGBA8, size, processedSize, Vector2i(radius)),
    #else
    GlyphCache(GL::TextureFormat::RGBA, size, processedSize, Vector2i(radius)),
    #endif
    _distanceField{radius}
{
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::texture_rg);
    #endif

    /* Replicating the assertion from TextureTools::DistanceField so it gets
       checked during construction already instead of only later during the
       setImage() call */
    CORRADE_ASSERT(size % processedSize == Vector2i{0} &&
                   (size/processedSize) % 2 == Vector2i{0},
        "Text::DistanceFieldGlyphCache: expected source and processed size ratio to be a multiple of 2, got" << Debug::packed << size << "and" << Debug::packed << processedSize, );

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Luminance is not renderable in most cases */
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        Warning() << "Text::DistanceFieldGlyphCache:" << GL::Extensions::EXT::texture_rg::string() << "not supported, using a full RGBA format for the distance field texture";
    #endif
}

DistanceFieldGlyphCache::DistanceFieldGlyphCache(NoCreateT) noexcept: GlyphCache{NoCreate}, _distanceField{NoCreate} {}

GlyphCacheFeatures DistanceFieldGlyphCache::doFeatures() const {
    return GlyphCacheFeature::ImageProcessing
        #ifndef MAGNUM_TARGET_GLES
        |GlyphCacheFeature::ProcessedImageDownload
        #endif
        ;
}

void DistanceFieldGlyphCache::doSetImage(const Vector2i& offset, const ImageView2D& image) {
    GL::Texture2D input;
    input.setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear);

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

        /* TextureTools::DistanceField expects the input size and output
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
void DistanceFieldGlyphCache::setDistanceFieldImage(const Vector2i& offset, const ImageView2D& image) {
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

void DistanceFieldGlyphCache::doSetProcessedImage(const Vector2i& offset, const ImageView2D& image) {
    ImageView2D imageToUse = image;

    /* On ES2, R8Unorm maps to Luminance, but here it's actually Red if
       EXT_texture_rg is supported. Reinterpret the image format in that
       case. If the format is something else (such as RGBA8Unorm), no
       reinterpret is done. WebGL doesn't have the EXT_texture_rg extension so
       there it isn't done either. */
    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(processedFormat() == PixelFormat::R8Unorm) {
        /* This is checked inside setProcessedImage() already */
        CORRADE_INTERNAL_ASSERT(image.format() == PixelFormat::R8Unorm);
        imageToUse = ImageView2D{image.storage(), GL::PixelFormat::Red, GL::PixelType::UnsignedByte, image.size(), image.data()};
    }
    #endif

    texture().setSubImage(0, offset, imageToUse);
}

#ifndef MAGNUM_TARGET_GLES
Image3D DistanceFieldGlyphCache::doProcessedImage() {
    Image2D out = _texture.image(0, PixelFormat::R8Unorm);
    return Image3D{out.format(), {out.size(), 1}, out.release()};
}
#endif

}}
