#ifndef Magnum_DebugTools_Screenshot_h
#define Magnum_DebugTools_Screenshot_h
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
 * @brief Function @ref Magnum::DebugTools::screenshot()
 */

#include <string>
#include <Corrade/PluginManager/PluginManager.h>

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace DebugTools {

/**
@brief Save a screenshot to a file
@param framebuffer  Framebuffer which to read
@param filename     File where to save
@m_since{2019,10}

Reads a rectangle of given @p framebuffer, defined by its
@ref GL::AbstractFramebuffer::viewport() "viewport()". Pixel format is queried
using @ref GL::AbstractFramebuffer::implementationColorReadFormat() and
@ref GL::AbstractFramebuffer::implementationColorReadType() and then mapped
back to the generic @ref Magnum::PixelFormat "PixelFormat". If, for some
reason, the driver-suggested pixel format is not desired, use the
@ref screenshot(GL::AbstractFramebuffer&, PixelFormat, const std::string&)
overload instead.

The read pixel data are saved using the
@ref Trade::AnyImageConverter "AnyImageConverter" plugin, with format being
detected from @p filename. Note that every file format supports a different set
of pixel formats, it's the user responsibility to choose a file format that
matches the framebuffer pixel format.

Returns @cpp true @ce on success, @cpp false @ce in case it was not possible to
map the detected pixel format back to a generic format, if either the
@ref Trade::AnyImageConverter "AnyImageConverter" or the corresponding plugin
for given file format could not be loaded, or if the file saving fails (for
example due to unsupported pixel format). A message is printed in each case.
*/
bool MAGNUM_DEBUGTOOLS_EXPORT screenshot(GL::AbstractFramebuffer& framebuffer, const std::string& filename);

/** @overload
@m_since{2019,10}

Useful in case you already have an instance of the converter plugin manager in
your application or if you intend to save screenshots often, as the operation
doesn't involve costly dynamic library loading and unloading on every call.
*/
bool MAGNUM_DEBUGTOOLS_EXPORT screenshot(PluginManager::Manager<Trade::AbstractImageConverter>& manager, GL::AbstractFramebuffer& framebuffer, const std::string& filename);

/**
@brief Save a screenshot in requested pixel format to a file
@param framebuffer  Framebuffer which to read
@param format       Pixel format to use
@param filename     File where to save
@m_since{2019,10}

Similar to @ref screenshot(GL::AbstractFramebuffer&, PixelFormat, const std::string&)
but with an explicit pixel format. Useful in cases where the driver-suggested
pixel format is not desired, however note that supplying a format that's
incompatible with the framebuffer may result in GL errors.
*/
bool MAGNUM_DEBUGTOOLS_EXPORT screenshot(GL::AbstractFramebuffer& framebuffer, PixelFormat format, const std::string& filename);

/** @overload
@m_since{2019,10}

Useful in case you already have an instance of the converter plugin manager in
your application or if you intend to save screenshots often, as the operation
doesn't involve costly dynamic library loading and unloading on every call.
*/
bool MAGNUM_DEBUGTOOLS_EXPORT screenshot(PluginManager::Manager<Trade::AbstractImageConverter>& manager, GL::AbstractFramebuffer& framebuffer, PixelFormat format, const std::string& filename);

}}

#endif
