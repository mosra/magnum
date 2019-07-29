#ifndef Magnum_Audio_AbstractImporter_h
#define Magnum_Audio_AbstractImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Class @ref Magnum::Audio::AbstractImporter
 */

#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Audio/BufferFormat.h"

namespace Magnum { namespace Audio {

/**
@brief Base for audio importer plugins

Provides interface for importing various audio formats. See @ref plugins for
more information and `*Importer` classes in @ref Audio namespace for available
importer plugins.

@section Audio-AbstractImporter-subclassing Subclassing

Plugin implements function @ref doFeatures(), @ref doIsOpened(), one of or both
@ref doOpenData() and @ref doOpenFile() functions, function @ref doClose() and
data access functions @ref doFormat(), @ref doFrequency() and @ref doData().

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions @ref doOpenData() and @ref doOpenFile() are called after the
    previous file was closed, function @ref doClose() is called only if there
    is any file opened.
-   Function @ref doOpenData() is called only if @ref Feature::OpenData is
    supported.
-   All `do*()` implementations working on opened file are called only if
    there is any file opened.

@attention @ref Corrade::Containers::Array instances returned from the plugin
    should *not* use anything else than the default deleter, otherwise this can
    cause dangling function pointer call on array destruction if the plugin
    gets unloaded before the array is destroyed.
*/
class MAGNUM_AUDIO_EXPORT AbstractImporter: public PluginManager::AbstractManagingPlugin<AbstractImporter> {
    public:
        /**
         * @brief Features supported by this importer
         *
         * @see @ref Features, @ref features()
         */
        enum class Feature: UnsignedByte {
            /** Opening files from raw data using @ref openData() */
            OpenData = 1 << 0
        };

        /**
         * @brief Features supported by this importer
         *
         * @see @ref features()
         */
        typedef Containers::EnumSet<Feature> Features;

        /**
         * @brief Plugin interface
         *
         * @code{.cpp}
         * "cz.mosra.magnum.Audio.AbstractImporter/0.1"
         * @endcode
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * First looks in `magnum/audioimporters/` or `magnum-d/audioimporters/`
         * next to the executable (or, in case of Windows and a non-static
         * build, next to the DLL of the @ref Audio library) and as a fallback
         * in `magnum/audioimporters/` or `magnum-d/audioimporters/` in the
         * runtime install location (`lib[64]/` on Unix-like systems, `bin/` on
         * Windows). The system-wide plugin search directory is configurable
         * using the `MAGNUM_PLUGINS_DIR` CMake variables, see @ref building
         * for more information.
         *
         * Not defined on platforms without
         *      @ref CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT "dynamic plugin support".
         */
        static std::vector<std::string> pluginSearchPaths();
        #endif

        /** @brief Default constructor */
        explicit AbstractImporter();

        /** @brief Constructor with access to plugin manager */
        explicit AbstractImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AbstractImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

        /** @brief Features supported by this importer */
        Features features() const { return doFeatures(); }

        /** @brief Whether any file is opened */
        bool isOpened() const { return doIsOpened(); }

        /**
         * @brief Open raw data
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref Feature::OpenData is supported. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise.
         * @see @ref features(), @ref openFile()
         */
        bool openData(Containers::ArrayView<const char> data);

        /**
         * @brief Open file
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Returns @cpp true @ce on success, @cpp false @ce otherwise.
         * @see @ref features(), @ref openData()
         */
        bool openFile(const std::string& filename);

        /** @brief Close file */
        void close();

        /** @{ @name Data access */

        /** @brief Sample format */
        BufferFormat format() const;

        /** @brief Sample frequency */
        UnsignedInt frequency() const;

        /** @brief Sample data */
        Containers::Array<char> data();

        /*@}*/

    private:
        /** @brief Implementation for @ref features() */
        virtual Features doFeatures() const = 0;

        /** @brief Implementation for @ref isOpened() */
        virtual bool doIsOpened() const = 0;

        /** @brief Implementation for @ref openData() */
        virtual void doOpenData(Containers::ArrayView<const char> data);

        /**
         * @brief Implementation for @ref openFile()
         *
         * If @ref Feature::OpenData is supported, default implementation opens
         * the file and calls @ref doOpenData() with its contents.
         */
        virtual void doOpenFile(const std::string& filename);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /** @brief Implementation for @ref format() */
        virtual BufferFormat doFormat() const = 0;

        /** @brief Implementation for @ref frequency() */
        virtual UnsignedInt doFrequency() const = 0;

        /** @brief Implementation for @ref data() */
        virtual Containers::Array<char> doData() = 0;
};

}}

#endif
