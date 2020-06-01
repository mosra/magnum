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

#include "Screenshot.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/GL/AbstractFramebuffer.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/Trade/AbstractImageConverter.h"

namespace Magnum { namespace DebugTools {

bool screenshot(GL::AbstractFramebuffer& framebuffer, const std::string& filename) {
    PluginManager::Manager<Trade::AbstractImageConverter> manager;
    return screenshot(manager, framebuffer, filename);
}

bool screenshot(PluginManager::Manager<Trade::AbstractImageConverter>& manager, GL::AbstractFramebuffer& framebuffer, const std::string& filename) {
    /* Get the implementation-specific color read format for given framebuffer */
    const GL::PixelFormat format = framebuffer.implementationColorReadFormat();
    const GL::PixelType type = framebuffer.implementationColorReadType();
    auto genericFormat = [](GL::PixelFormat format, GL::PixelType type) -> Containers::Optional<PixelFormat> {
        #ifndef DOXYGEN_GENERATING_OUTPUT /* It gets *really* confused */
        #define _c(generic, glFormat, glType, glTextureFormat) if(format == GL::PixelFormat::glFormat && type == GL::PixelType::glType) return PixelFormat::generic;
        #define _n(generic, glFormat, glType) if(format == GL::PixelFormat::glFormat && type == GL::PixelType::glType) return PixelFormat::generic;
        #define _s(generic) return {};
        #include "Magnum/GL/Implementation/pixelFormatMapping.hpp"
        #undef _c
        #undef _n
        #undef _s
        #endif
        return {};
    }(format, type);
    if(!genericFormat) {
        Error{} << "DebugTools::screenshot(): can't map (" << Debug::nospace << format << Debug::nospace << "," << type << Debug::nospace << ") to a generic pixel format";
        return false;
    }

    return screenshot(manager, framebuffer, *genericFormat, filename);
}

bool screenshot(GL::AbstractFramebuffer& framebuffer, const PixelFormat format, const std::string& filename) {
    PluginManager::Manager<Trade::AbstractImageConverter> manager;
    return screenshot(manager, framebuffer, format, filename);
}

bool screenshot(PluginManager::Manager<Trade::AbstractImageConverter>& manager, GL::AbstractFramebuffer& framebuffer, const PixelFormat format, const std::string& filename) {
    Containers::Pointer<Trade::AbstractImageConverter> converter;
    if(!(converter = manager.loadAndInstantiate("AnyImageConverter")))
        return false;

    Image2D image = framebuffer.read(framebuffer.viewport(), {format});
    if(!converter->exportToFile(image, filename))
        return false;

    Debug{} << "DebugTools::screenshot(): saved a" << format << "image of size" << image.size() << "to" << filename;
    return true;
}

}}
