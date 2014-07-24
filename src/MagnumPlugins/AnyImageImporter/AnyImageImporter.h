#ifndef Magnum_Trade_AnyImageImporter_h
#define Magnum_Trade_AnyImageImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
 * @brief Class Magnum::Trade::AnyImageImporter
 */

#include <Corrade/Containers/Array.h>
#include <Magnum/Trade/AbstractImporter.h>

#ifndef DOXYGEN_GENERATING_OUTPUT
    #if defined(AnyImageImporter_EXPORTS) || defined(AnyImageImporterObjects_EXPORTS)
        #define MAGNUM_ANYIMAGEIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYIMAGEIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
    #define MAGNUM_ANYIMAGEIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief Any image importer plugin

Detects file type based on file extension, then loads either
@ref TgaImporter, @ref PngImporter or @ref JpegImporter and tries to open the
file with it. See documentation of each particular plugin for more information.

Only loading from files is supported.
*/
class MAGNUM_ANYIMAGEIMPORTER_EXPORT AnyImageImporter: public AbstractImporter {
    public:
        /**
         * @brief Constructor
         *
         * The plugin needs access to plugin manager to function properly.
         */
        explicit AnyImageImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnyImageImporter(PluginManager::AbstractManager& manager, std::string plugin);

        ~AnyImageImporter();

    private:
        MAGNUM_ANYIMAGEIMPORTER_LOCAL Features doFeatures() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL void doClose() override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL void doOpenFile(const std::string& filename) override;

        MAGNUM_ANYIMAGEIMPORTER_LOCAL UnsignedInt doImage2DCount() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL std::optional<ImageData2D> doImage2D(UnsignedInt id) override;

    private:
        std::unique_ptr<AbstractImporter> _in;
};

}}

#endif
