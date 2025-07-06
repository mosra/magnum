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
#include "Magnum/Text/Implementation/glyphCacheGLState.h"
#include "Magnum/TextureTools/DistanceFieldGL.h"

namespace Magnum { namespace Text {

struct DistanceFieldGlyphCacheGL::State: GlyphCacheGL::State {
    explicit State(const Vector2i& size, const Vector2i& processedSize, UnsignedInt radius);

    TextureTools::DistanceFieldGL distanceField;
};

DistanceFieldGlyphCacheGL::State::State(const Vector2i& size, const Vector2i& processedSize, const UnsignedInt radius):
    GlyphCacheGL::State{PixelFormat::R8Unorm, size,
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
    distanceField{radius}
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

DistanceFieldGlyphCacheGL::DistanceFieldGlyphCacheGL(const Vector2i& size, const Vector2i& processedSize, UnsignedInt radius): GlyphCacheGL{Containers::pointer<State>(size, processedSize, radius)} {}

DistanceFieldGlyphCacheGL::DistanceFieldGlyphCacheGL(NoCreateT) noexcept: GlyphCacheGL{NoCreate} {}

UnsignedInt DistanceFieldGlyphCacheGL::radius() const {
    return static_cast<const State&>(*_state).distanceField.radius();
}

#ifdef MAGNUM_BUILD_DEPRECATED
Vector2i DistanceFieldGlyphCacheGL::distanceFieldTextureSize() const {
    return processedSize().xy();
}
#endif

GlyphCacheFeatures DistanceFieldGlyphCacheGL::doFeatures() const {
    return GlyphCacheFeature::ImageProcessing
        #ifndef MAGNUM_TARGET_GLES
        |GlyphCacheFeature::ProcessedImageDownload
        #endif
        ;
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
namespace {

Range2Di paddedImageRange(const Vector3i& cacheSize, const Vector2i& imageOffset, const Vector2i& imageSize, const Vector2i& ratio) {
    const Vector2i paddedMin = imageOffset;
    const Vector2i paddedMax = imageOffset + imageSize;

    /* TextureTools::DistanceFieldGL expects the input size and output
       rectangle size ratio to be a multiple of 2 in order for the shader to
       perform pixel addressing correctly. That might not always be the case
       with the rectangle passed to flushImage(), so round the paddedMin *down*
       to a multiple of the ratio and paddedMax *up* to a multiple of the
       ratio. */
    const Vector2i paddedMinRounded = ratio*(paddedMin/ratio);
    const Vector2i paddedMaxRounded = ratio*((paddedMax + ratio - Vector2i{1})/ratio);
    /* As the size is also a multiple of ratio, the resulting size should not
       get larger */
    CORRADE_INTERNAL_ASSERT(paddedMaxRounded <= cacheSize.xy());
    #ifdef CORRADE_NO_ASSERT
    static_cast<void>(cacheSize);
    #endif
    return {paddedMinRounded, paddedMaxRounded};
}

}
#endif

void DistanceFieldGlyphCacheGL::doSetImage(const Vector2i&
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL)) && !defined(CORRADE_NO_ASSERT)
    offset
    #endif
, const ImageView2D& image) {
    auto& state = static_cast<State&>(*_state);

    /* Creating a temporary input texture that's deleted right after because I
       assume it's better than having a persistent one which would just occupy
       memory that was only ever used once. This way it can also be scaled to
       just exactly the input size being processed, not the whole unprocessed
       cache size, which can be quite big. */
    GL::Texture2D input;
    input
        /* In order to have correctly processed output, the input has to be
           sufficiently padded. But in case it isn't and texelFetch() isn't
           used, which clamps out-of-range reads to zero implicitly, clamp the
           out-of-range reads to the edge instead of repeat to avoid even worse
           artifacts. */
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
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
        state.distanceField(input, texture(), {{}, size().xy()/ratio}, size().xy());
    }
    #ifndef MAGNUM_TARGET_WEBGL
    else
    #endif
    #endif
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    {
        /* If EXT_unpack_subimage is supported, use the storage as-is but
           reset image height to 0 as that only matters with arrays which are
           not supported on ES2 at all. It's set by AbstractGlyphCache always
           because with array textures the upload may fail if not set. See
           DistanceFieldGlyphCacheGLTest::setImageArraySingleLayer() for a
           repro case. */
        PixelStorage storage{image.storage()};
        storage.setImageHeight(0);

        /* The image range was already expanded to include the padding in
           flushImage() */
        CORRADE_INTERNAL_ASSERT(storage.skip().xy() == offset);
        const Range2Di paddedRange = paddedImageRange(size(), storage.skip().xy(), image.size(), ratio);
        const ImageView2D paddedImage{
            storage.setSkip({paddedRange.min(), storage.skip().z()}),
            image.format(),
            paddedRange.size(),
            image.data()};

        /** @todo investigate if using setStorage() + setSubImage() is any
            faster or better, the assumption is that since the texture is
            temporary it doesn't matter much anyway; similarly with the
            temporary framebuffer created inside */
        input.setImage(0, GL::textureFormat(paddedImage.format()), paddedImage);
        state.distanceField(input, texture(), {paddedRange.min()/ratio, paddedRange.max()/ratio}, paddedRange.size());
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

#ifndef MAGNUM_TARGET_GLES2
struct DistanceFieldGlyphCacheArrayGL::State: GlyphCacheArrayGL::State {
    explicit State(const Vector3i& size, const Vector2i& processedSize, UnsignedInt radius);

    TextureTools::DistanceFieldGL distanceField;
};

DistanceFieldGlyphCacheArrayGL::State::State(const Vector3i& size, const Vector2i& processedSize, const UnsignedInt radius):
    GlyphCacheArrayGL::State{
        PixelFormat::R8Unorm, size, PixelFormat::R8Unorm,
        processedSize, Vector2i(radius)},
    distanceField{radius}
{
    /* Replicating the assertion from TextureTools::DistanceFieldGL so it gets
       checked during construction already instead of only later during the
       setImage() call */
    CORRADE_ASSERT(size.xy() % processedSize == Vector2i{0} &&
                   (size.xy()/processedSize) % 2 == Vector2i{0},
        "Text::DistanceFieldGlyphCacheArrayGL: expected source and processed size ratio to be a multiple of 2, got" << Debug::packed << size.xy() << "and" << Debug::packed << processedSize, );
}

DistanceFieldGlyphCacheArrayGL::DistanceFieldGlyphCacheArrayGL(const Vector3i& size, const Vector2i& processedSize, UnsignedInt radius): GlyphCacheArrayGL{Containers::pointer<State>(size, processedSize, radius)} {}

DistanceFieldGlyphCacheArrayGL::DistanceFieldGlyphCacheArrayGL(NoCreateT) noexcept: GlyphCacheArrayGL{NoCreate} {}

UnsignedInt DistanceFieldGlyphCacheArrayGL::radius() const {
    return static_cast<const State&>(*_state).distanceField.radius();
}

GlyphCacheFeatures DistanceFieldGlyphCacheArrayGL::doFeatures() const {
    return GlyphCacheFeature::ImageProcessing
        #ifndef MAGNUM_TARGET_GLES
        |GlyphCacheFeature::ProcessedImageDownload
        #endif
        ;
}

void DistanceFieldGlyphCacheArrayGL::doSetImage(const Vector3i& offset, const ImageView3D& image) {
    auto& state = static_cast<State&>(*_state);

    /* Like with DistanceFieldGlyphCacheGL above, the assumption is that a
       temporary texture instance is better than a persistent one */
    GL::Texture2D input;
    input
        /* Unlike with DistanceFieldGlyphCacheGL, neither wrapping nor nearest
           filter should be needed as we always use texelFetch(), but use it
           for consistency. The Base mipmap setting is however for some reason
           needed even for texelFetch() as with Nearest / Linear it results in
           zero output (likely due to setImage() being used below instead of
           setStorage()?). */
        /** @todo might want to clear this up once setStorage() is used? */
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base)
        .setMagnificationFilter(GL::SamplerFilter::Nearest);

    /* The constructor already checked that the ratio is an integer multiple,
       so this division should lead to no information loss */
    CORRADE_INTERNAL_ASSERT(size().xy() % processedSize().xy() == Vector2i{0});
    const Vector2i ratio = size().xy()/processedSize().xy();

    /* Upload the input texture and create a distance field from it */

    /* The image range was already expanded to include the padding in
       flushImage() */
    CORRADE_INTERNAL_ASSERT(image.storage().skip().xy() == offset.xy());
    const Range2Di paddedRange = paddedImageRange(size(), image.storage().skip().xy(), image.size().xy(), ratio);
    const ImageView3D paddedImage{
        PixelStorage{image.storage()}
            .setSkip({paddedRange.min(), image.storage().skip().z()}),
        image.format(),
        {paddedRange.size(), image.size().z()},
        image.data()};

    /* Properties needed for slicing the image to individual layers below */
    /** @todo clean this up once Image APIs stop being shit */
    const std::size_t firstLayerOffset = paddedImage.dataProperties().first.z();
    const std::size_t layerStride = paddedImage.dataProperties().second.xy().product();

    /* Cycle through all layers, for each upload slice of the input image,
       attach the corresponding output texture array layer to the framebuffer
       and run the distance field processing. Yes, this means a separate GPU
       call for each layer, but:

        -   The processing has to be done layer by layer anyway, as drawing to
            multiple layers at once is only possible with geometry shaders or
            image load/store. GS isn't available on WebGL or other ES3
            platforms we care about and generally has perf pitfalls unless a
            GS passthrough extension is available, which is basically just on
            NVidia. Image load/store is available only where compute is, so
            also just ES3.1+ or desktop, and generally fragment shader
            processing is always faster because the invocations are done in a
            more cache friendly manner than with compute. With compute one
            *can* emulate such behavior by hand, but it sidesteps the GPU's
            builtin implementation, likely always only playing catch up.
        -   Because only a single input layer is uploaded at a time, the GPU
            memory use is reduced compared to allocating the whole input
            texture array and then uploading and processing just a part. */
    for(Int i = 0; i != image.size().z(); ++i) {
        /** @todo like with DistanceFieldGlyphCacheGL above, investigate if
            using setStorage() + setSubImage() or a persistent framebuffer
            instance is any faster than this */
        input.setImage(0, GL::textureFormat(paddedImage.format()), ImageView2D{
            /* Ideally, with a sane API, I wouldn't need to reset the Z skip to
               0 and offset the data pointer, but with 2D images GL ignores the
               Z skip */
            /** @todo clean up all this once this useless GL-shaped API is
                dropped */
            PixelStorage{paddedImage.storage()}
                .setSkip({paddedRange.min(), 0}),
            paddedImage.format(),
            paddedImage.size().xy(),
            paddedImage.data().exceptPrefix(firstLayerOffset + i*layerStride)});
        state.distanceField(input, texture(), offset.z() + i, {paddedRange.min()/ratio, paddedRange.max()/ratio}, paddedRange.size());
    }
}
#endif

}}
