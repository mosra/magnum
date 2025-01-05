#ifndef Magnum_Trade_Implementation_converterUtilities_h
#define Magnum_Trade_Implementation_converterUtilities_h
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

#include <chrono>
#include <sstream> /** @todo remove when Debug is stream-free */
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove when Debug is stream-free */
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/String.h> /* replaceAll() */
#include <Corrade/Utility/ConfigurationGroup.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade { namespace Implementation {

/* Used only in executables where we don't want it to be exported -- in
   particular magnum-imageconverter, magnum-sceneconverter and their tests */
namespace {

/** @todo move these to Magnum/Implementation/converterUtilities.h once also
    shaderconverter / fontconverter / etc implements --info-converter etc */
template<class T> void printPluginInfo(const Debug::Flags useColor, const T& plugin) {
        const PluginManager::PluginMetadata* metadata = plugin.metadata();
        CORRADE_INTERNAL_ASSERT(metadata);

        Debug d{useColor};
        d << Debug::boldColor(Debug::Color::Default) << "Plugin name:" << Debug::boldColor(Debug::Color::Yellow) << metadata->name() << Debug::resetColor;
        if(const Containers::StringIterable aliases = metadata->provides()) {
            d << Debug::newline << Debug::boldColor(Debug::Color::Default) << "Aliases:" << Debug::resetColor;
            for(const Containers::StringView alias: aliases) {
                d << Debug::newline << " ";
                if(alias == plugin.plugin())
                    d << Debug::color(Debug::Color::Yellow);
                d << alias;
                if(alias == plugin.plugin())
                    d << Debug::resetColor;
            }
        }

        /* Ugly, eh? */
        std::ostringstream out;
        Debug{&out, Debug::Flag::NoNewlineAtTheEnd} << Debug::packed << plugin.features();
        d << Debug::newline << Debug::boldColor(Debug::Color::Default) << "Features:" << Debug::color(Debug::Color::Cyan) << Debug::newline << " " << Utility::String::replaceAll(out.str(), "|", "\n  ") << Debug::resetColor;
}

void printPluginConfigurationInfo(Debug& d, const Utility::ConfigurationGroup& configuration, const Containers::StringView prefix) {
    using namespace Containers::Literals;

    for(Containers::Pair<Containers::StringView, Containers::StringView> i: configuration.valuesComments()) {
        if(i.first()) {
            d << Debug::newline << " " << Debug::boldColor(Debug::Color::Blue) << i.first() << Debug::nospace << Debug::color(Debug::Color::Blue) << "=" << Debug::nospace << Debug::color(Debug::Color::Red);
            /* Print the value wrapped in quotes if it contains spaces, indent
               also all newlines */
            if(i.second().contains('\n'))
                d << Utility::format("\"\"\"\n  {}\n  \"\"\"", Utility::String::replaceAll(i.second(), "\n", "\n  "));
            /** @todo less wasteful API for checking leading/trailing zeros? */
            else if(i.second().trimmed() != i.second())
                d << Utility::format("\"{}\"", i.second());
            else
                d << i.second();
            d << Debug::resetColor;
        } else {
            /* Configuration contents are delimited by these markers in order
               to include them in Doxygen-generated docs. Newly added values
               will however appear *after* these markers so we can't just
               return here. */
            if(i.second() == "# [configuration_]"_s)
                continue;

            /* Print leading space only if there's actually something */
            d << Debug::newline;
            if(i.second()) d << " " << Debug::boldColor(Debug::Color::Black) << i.second() << Debug::resetColor;
        }
    }

    for(Containers::Pair<Containers::StringView, Containers::Reference<const Utility::ConfigurationGroup>> i: configuration.groups()) {
        const Containers::String name = prefix ? "/"_s.join({prefix, i.first()}) : Containers::String{i.first()};

        d << Debug::newline << " " << Debug::color(Debug::Color::Blue) << "[" << Debug::nospace << Debug::boldColor(Debug::Color::Blue) << name << Debug::color(Debug::Color::Blue) << Debug::nospace << "]" << Debug::resetColor;

        printPluginConfigurationInfo(d, i.second(), name);
    }
}

void printPluginConfigurationInfo(const Debug::Flags useColor, const PluginManager::AbstractPlugin& plugin) {
    const Utility::ConfigurationGroup& configuration = plugin.configuration();
    if(configuration.isEmpty()) return;

    Debug d{useColor};
    d << Debug::boldColor(Debug::Color::Default) << "Configuration:" << Debug::resetColor;
    printPluginConfigurationInfo(d, configuration, {});
}

void printImporterInfo(const Debug::Flags useColor, const Trade::AbstractImporter& importer) {
    printPluginInfo(useColor, importer);
    printPluginConfigurationInfo(useColor, importer);
}

void printImageConverterInfo(const Debug::Flags useColor, const Trade::AbstractImageConverter& converter) {
    printPluginInfo(useColor, converter);

    Debug d{useColor|Debug::Flag::NoNewlineAtTheEnd};

    if(converter.features() & (ImageConverterFeature::Convert1DToFile|
                               ImageConverterFeature::Convert2DToFile|
                               ImageConverterFeature::Convert3DToFile|
                               ImageConverterFeature::ConvertCompressed1DToFile|
                               ImageConverterFeature::ConvertCompressed2DToFile|
                               ImageConverterFeature::ConvertCompressed3DToFile)) {
        if(const Containers::String extension = converter.extension())
            d << Debug::boldColor(Debug::Color::Default) << "File extension:" << Debug::resetColor << extension << Debug::newline;
        if(const Containers::String mimeType = converter.mimeType())
            d << Debug::boldColor(Debug::Color::Default) << "MIME type:" << Debug::resetColor << mimeType << Debug::newline;
    }

    printPluginConfigurationInfo(useColor, converter);
}

struct Duration {
    explicit Duration(std::chrono::high_resolution_clock::duration& output): _output(output), _t{std::chrono::high_resolution_clock::now()} {}

    ~Duration() {
        _output += std::chrono::high_resolution_clock::now() - _t;
    }

    private:
        std::chrono::high_resolution_clock::duration& _output;
        std::chrono::high_resolution_clock::time_point _t;
};

union ImageInfoFlags {
    /* Wow, C++, YOU FUCKING SUCK, how is this not the implicit behavior?!! */
    ImageInfoFlags(ImageFlags1D flags): one{flags} {}
    ImageInfoFlags(ImageFlags2D flags): two{flags} {}
    ImageInfoFlags(ImageFlags3D flags): three{flags} {}

    ImageFlags1D one;
    ImageFlags2D two;
    ImageFlags3D three;
};

struct ImageInfo {
    UnsignedInt image, level;
    bool compressed;
    PixelFormat format;
    CompressedPixelFormat compressedFormat;
    Vector3i size;
    std::size_t dataSize;
    Trade::DataFlags dataFlags;
    ImageInfoFlags flags;
    Containers::String name;
};

Containers::Array<ImageInfo> imageInfo(AbstractImporter& importer, bool& error, std::chrono::high_resolution_clock::duration& importTime) {
    Containers::Array<ImageInfo> infos;
    for(UnsignedInt i = 0; i != importer.image1DCount(); ++i) {
        const Containers::String name = importer.image1DName(i);
        const UnsignedInt levelCount = importer.image1DLevelCount(i);

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData1D> image;
            {
                Duration d{importTime};
                if(!(image = importer.image1D(i, j))) {
                    Error{} << "Can't import 1D image" << i << "level" << j;
                    error = true;
                    continue;
                }
            }
            arrayAppend(infos, InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                Vector3i::pad(image->size()),
                image->data().size(),
                image->dataFlags(),
                ImageInfoFlags{image->flags()},
                j ? "" : name);
        }
    }
    for(UnsignedInt i = 0; i != importer.image2DCount(); ++i) {
        const Containers::String name = importer.image2DName(i);
        const UnsignedInt levelCount = importer.image2DLevelCount(i);

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData2D> image;
            {
                Duration d{importTime};
                if(!(image = importer.image2D(i, j))) {
                    Error{} << "Can't import 2D image" << i << "level" << j;
                    error = true;
                    continue;
                }
            }
            arrayAppend(infos, InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                Vector3i::pad(image->size()),
                image->data().size(),
                image->dataFlags(),
                ImageInfoFlags{image->flags()},
                j ? "" : name);
        }
    }
    for(UnsignedInt i = 0; i != importer.image3DCount(); ++i) {
        const Containers::String name = importer.image3DName(i);
        const UnsignedInt levelCount = importer.image3DLevelCount(i);

        for(UnsignedInt j = 0; j != levelCount; ++j) {
            Containers::Optional<Trade::ImageData3D> image;
            {
                Duration d{importTime};
                if(!(image = importer.image3D(i, j))) {
                    Error{} << "Can't import 3D image" << i << "level" << j;
                    error = true;
                    continue;
                }
            }
            arrayAppend(infos, InPlaceInit, i, j,
                image->isCompressed(),
                image->isCompressed() ?
                    PixelFormat{} : image->format(),
                image->isCompressed() ?
                    image->compressedFormat() : CompressedPixelFormat{},
                image->size(),
                image->data().size(),
                image->dataFlags(),
                ImageInfoFlags{image->flags()},
                j ? "" : name);
        }
    }

    return infos;
}

void printImageInfo(const Debug::Flags useColor, const Containers::ArrayView<const ImageInfo> imageInfos, const Containers::ArrayView<const UnsignedInt> image1DReferenceCount, const Containers::ArrayView<const UnsignedInt> image2DReferenceCount, const Containers::ArrayView<const UnsignedInt> image3DReferenceCount) {
    std::size_t totalImageDataSize = 0;
    for(const Trade::Implementation::ImageInfo& info: imageInfos) {
        Debug d{useColor};
        if(info.level == 0) {
            d << Debug::boldColor(Debug::Color::Default);
            if(info.size.z()) d << "3D image";
            else if(info.size.y()) d << "2D image";
            else d << "1D image";
            d << info.image << Debug::resetColor;

            /* Print reference count only if there actually are any (i.e., the
               arrays are non-empty) otherwise this information is useless */
            Containers::Optional<UnsignedInt> count;
            if(info.size.z() && image3DReferenceCount) {
                count = image3DReferenceCount[info.image];
            } else if(info.size.y() && image2DReferenceCount) {
                count = image2DReferenceCount[info.image];
            } else if(image1DReferenceCount) {
                count = image1DReferenceCount[info.image];
            }
            if(count) {
                if(!*count) d << Debug::color(Debug::Color::Red);
                d << "(referenced by" << *count << "textures)";
                if(!*count) d << Debug::resetColor;
            }

            d << Debug::boldColor(Debug::Color::Default) << Debug::nospace << ":"
                << Debug::resetColor;
            if(info.name) d << Debug::boldColor(Debug::Color::Yellow)
                << info.name << Debug::resetColor;
            d << Debug::newline;
        }
        d << "  Level" << info.level << Debug::nospace << ":";
        if(info.flags.one) {
            d << Debug::packed << Debug::color(Debug::Color::Cyan);
            if(info.size.z()) d << info.flags.three;
            else if(info.size.y()) d << info.flags.two;
            else d << info.flags.one;
            d << Debug::resetColor;
        }
        d << Debug::packed;
        if(info.size.z()) d << info.size;
        else if(info.size.y()) d << info.size.xy();
        /* Kinda unnecessary, but makes the output more consistent if also 1D
           size is in {}s */
        else d << Math::Vector<1, Int>(info.size.x());
        d << Debug::color(Debug::Color::Blue) << "@" << Debug::resetColor;
        d << Debug::packed;
        /* Compressed formats are printed yellow. That kinda conflicts with
           custom fields / attributes elsewhere, but is significant enough to
           have it highlighted. */
        if(info.compressed) d << Debug::color(Debug::Color::Yellow) << info.compressedFormat;
        else d << Debug::color(Debug::Color::Cyan) << info.format;
        d << Debug::resetColor << "(" << Debug::nospace << Utility::format("{:.1f}", info.dataSize/1024.0f) << "kB";
        if(info.dataFlags != (Trade::DataFlag::Owned|Trade::DataFlag::Mutable))
            d << Debug::nospace << "," << Debug::packed
                << Debug::color(Debug::Color::Green) << info.dataFlags
                << Debug::resetColor;
        d << Debug::nospace << ")";

        totalImageDataSize += info.dataSize;
    }
    if(!imageInfos.isEmpty())
        Debug{} << "Total image data size:" << Utility::format("{:.1f}", totalImageDataSize/1024.0f) << "kB";
}

}

}}}

#endif
