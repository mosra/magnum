#ifndef Magnum_Audio_AbstractImporter_h
#define Magnum_Audio_AbstractImporter_h
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
 * @brief Class @ref Magnum::Audio::AbstractImporter, enum @ref Magnum::Audio::ImporterFeature, enum set @ref Magnum::Audio::ImporterFeatures
 */

#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Audio/BufferFormat.h"

namespace Magnum { namespace Audio {

/**
@brief Features supported by an audio importer
@m_since{2020,06}

@see @ref ImporterFeatures, @ref AbstractImporter::features()
*/
enum class ImporterFeature: UnsignedByte {
    /** Opening files from raw data using @ref AbstractImporter::openData() */
    OpenData = 1 << 0
};

/**
@brief Features supported by an audio importer
@m_since{2020,06}

@see @ref AbstractImporter::features()
*/
typedef Containers::EnumSet<ImporterFeature> ImporterFeatures;

CORRADE_ENUMSET_OPERATORS(ImporterFeatures)

/** @debugoperatorenum{ImporterFeatures} */
MAGNUM_AUDIO_EXPORT Debug& operator<<(Debug& debug, ImporterFeature value);

/** @debugoperatorenum{ImporterFeatures} */
MAGNUM_AUDIO_EXPORT Debug& operator<<(Debug& debug, ImporterFeatures value);

/**
@brief Base for audio importer plugins

Provides interface for importing various audio formats. See @ref plugins for
more information and `*Importer` classes in @ref Audio namespace for available
importer plugins.

@section Audio-AbstractImporter-data-dependency Data dependency

The data returned from various functions *by design* have no dependency on the
importer instance and neither on the dynamic plugin module. In other words, you don't need to keep the importer instance (or the plugin manager instance)
around in order to have the returned data valid. Moreover, all returned
@ref Corrade::Containers::Array instances are only allowed to have default
deleters --- this is to avoid potential dangling function pointer calls when
destructing such instances after the plugin module has been unloaded.

@section Audio-AbstractImporter-subclassing Subclassing

Plugin implements function @ref doFeatures(), @ref doIsOpened(), one of or both
@ref doOpenData() and @ref doOpenFile() functions, function @ref doClose() and
data access functions @ref doFormat(), @ref doFrequency() and @ref doData().

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions @ref doOpenData() and @ref doOpenFile() are called after the
    previous file was closed, function @ref doClose() is called only if there
    is any file opened.
-   Function @ref doOpenData() is called only if @ref ImporterFeature::OpenData
    is supported.
-   All `do*()` implementations working on opened file are called only if
    there is any file opened.

@m_class{m-block m-warning}

@par Dangling function pointers on plugin unload
    As @ref Trade-AbstractImporter-data-dependency "mentioned above",
    @ref Corrade::Containers::Array instances returned from plugin
    implementations are not allowed to use anything else than the default
    deleter, otherwise this could cause dangling function pointer call on array
    destruction if the plugin gets unloaded before the array is destroyed. This
    is asserted by the base implementation on return.
*/
class MAGNUM_AUDIO_EXPORT AbstractImporter: public PluginManager::AbstractManagingPlugin<AbstractImporter> {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief ImporterFeature
         * @m_deprecated_since{2020,06} Use @ref ImporterFeature instead.
         */
        typedef CORRADE_DEPRECATED("use ImporterFeature instead") ImporterFeature Feature;

        /** @brief @copybrief ImporterFeatures
         * @m_deprecated_since{2020,06} Use @ref ImporterFeatures instead.
         */
        typedef CORRADE_DEPRECATED("use ImporterFeatures instead") ImporterFeatures Features;
        #endif

        /**
         * @brief Plugin interface
         *
         * @snippet Magnum/Audio/AbstractImporter.cpp interface
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * Looks into `magnum/audioimporters/` or `magnum-d/audioimporters/`
         * next to the dynamic @ref Trade library, next to the executable and
         * elsewhere according to the rules documented in
         * @ref Corrade::PluginManager::implicitPluginSearchPaths(). The search
         * directory can be also hardcoded using the `MAGNUM_PLUGINS_DIR` CMake
         * variables, see @ref building for more information.
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
        ImporterFeatures features() const { return doFeatures(); }

        /** @brief Whether any file is opened */
        bool isOpened() const { return doIsOpened(); }

        /**
         * @brief Open raw data
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref ImporterFeature::OpenData is supported.
         * Returns @cpp true @ce on success, @cpp false @ce otherwise.
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

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    private:
        /** @brief Implementation for @ref features() */
        virtual ImporterFeatures doFeatures() const = 0;

        /** @brief Implementation for @ref isOpened() */
        virtual bool doIsOpened() const = 0;

        /** @brief Implementation for @ref openData() */
        virtual void doOpenData(Containers::ArrayView<const char> data);

        /**
         * @brief Implementation for @ref openFile()
         *
         * If @ref ImporterFeature::OpenData is supported, default
         * implementation opens the file and calls @ref doOpenData() with its
         * contents.
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
