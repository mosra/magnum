#ifndef Magnum_FileCallback_h
#define Magnum_FileCallback_h
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
 * @brief Enum @ref Magnum::InputFileCallbackPolicy
 */

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum {

/**
@brief Input file callback policy
@m_since{2019,10}

@see @ref Trade::AbstractImporter::setFileCallback(),
    @ref Trade-AbstractImporter-usage-callbacks
*/
enum class InputFileCallbackPolicy: UnsignedByte {
    /**
     * The requested file is used only during a call of given function and the
     * memory view is not referenced anymore once the function exits.
     *
     * This can be the case for example when importing image data using
     * @ref Trade::AbstractImporter::image2D() --- imported data are copied
     * into the returned @ref Trade::ImageData2D object and the original file
     * is not needed anymore. Note, however, that this might not be the case
     * for all importers --- see documentation of a particular plugin for
     * concrete info.
     */
    LoadTemporary,

    /**
     * The requested file may be used for loading most or all data in the next
     * steps, so the importer expects the memory view to be valid for as long
     * as data import functions are called on it, but at most until the
     * data importer is destroyed, its @ref Trade::AbstractImporter::close() "close()"
     * function is called or another file is opened.
     *
     * This can be the case for example when importing mesh data using
     * @ref Trade::AbstractImporter::mesh() --- all vertex data might be
     * combined in a single binary file and each mesh occupies only a portion
     * of it. Note, however, that this might not be the case for all importers
     * --- see documentation of a particular plugin for concrete info.
     */
    LoadPermanent,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @m_deprecated_since{2019,10} Use @ref InputFileCallbackPolicy::LoadPermanent
     *      instead.
     */
    LoadPernament CORRADE_DEPRECATED_ENUM("use LoadPermanent instead") = LoadPermanent,
    #endif

    /**
     * A file that has been previously loaded by this callback can be closed
     * now (and its memory freed). This is just a hint, it's not *required* for
     * the callback to close it. This policy is also only ever called with a
     * file that was previously opened with the same callback, so it's possible
     * to completely ignore it and just return the cached value.
     *
     * This can be the case for example when an importer is done parsing a text
     * file into an internal representation and the original data are no longer
     * needed (and, for example, other files need to be loaded and they could
     * repurpose the unused memory).
     */
    Close
};

/** @debugoperatorenum{InputFileCallbackPolicy} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, InputFileCallbackPolicy value);

}

#endif
