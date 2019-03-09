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

Provides interface for importing various audio formats.

@section Audio-AbstractImporter-usage Usage

Importers are most commonly implemented as plugins. For example, loading an
audio track from the filesystem using the @ref AnyAudioImporter plugin can be
done like this, completely with all error handling:

@snippet MagnumAudio.cpp AbstractImporter-usage

See @ref plugins for more information about general plugin usage and
`*Importer` classes in the @ref Audio namespace for available importer plugins.

@subsection Audio-AbstractImporter-usage-callbacks Loading data from memory, using file callbacks

Besides loading data directly from the filesystem using @ref openFile() like
shown above, it's possible to use @ref openData() to import data from memory.
Note that the particular importer implementation must support
@ref Feature::OpenData for this method to work.

Some audio formats sometimes reference other files (such as songs of a
playlist) and in that case you may want to intercept those references and load
them in a custom way as well. For importers that advertise support for this
with @ref Feature::FileCallback this is done by specifying a file loading
callback using @ref setFileCallback(). The callback gets a filename,
@ref InputFileCallbackPolicy and an user pointer as parameters; returns a
non-owning view on the loaded data or a
@ref Corrade::Containers::NullOpt "Containers::NullOpt" to indicate the file
loading failed. For example, loading a scene from memory-mapped files could
look like below. Note that the file loading callback affects @ref openFile() as
well --- you don't have to load the top-level file manually and pass it to
@ref openData(), any importer supporting the callback feature handles that
correctly.

@snippet MagnumAudio.cpp AbstractImporter-usage-callbacks

For importers that don't support @ref Feature::FileCallback directly, the base
@ref openFile() implementation will use the file callback to pass the loaded
data through to @ref openData(), in case the importer supports at least
@ref Feature::OpenData. If the importer supports neither @ref Feature::FileCallback
nor @ref Feature::OpenData, @ref setFileCallback() doesn't allow the callbacks
to be set.

The input file callback signature is the same for @ref Audio::AbstractImporter,
@ref Trade::AbstractImporter and @ref Text::AbstractFont to allow code reuse.

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
            OpenData = 1 << 0,

            /**
             * Specifying callbacks for loading additional files referenced
             * from the main file using @ref setFileCallback(). If the importer
             * doesn't expose this feature, the format is either single-file or
             * loading via callbacks is not supported.
             *
             * See @ref Audio-AbstractImporter-usage-callbacks and particular
             * importer documentation for more information.
             */
            FileCallback = 1 << 1
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
         * next to the executable and as a fallback in `magnum/audioimporters/`
         * or `magnum-d/audioimporters/` in the runtime install location
         * (`lib[64]/` on Unix-like systems, `bin/` on Windows). The
         * system-wide plugin search directory is configurable using the
         * `MAGNUM_PLUGINS_DIR` CMake variables, see @ref building for more
         * information.
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

        /**
         * @brief File opening callback function
         *
         * @see @ref Audio-AbstractImporter-usage-callbacks
         */
        auto fileCallback() const -> Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*) { return _fileCallback; }

        /**
         * @brief File opening callback user data
         *
         * @see @ref Audio-AbstractImporter-usage-callbacks
         */
        void* fileCallbackUserData() const { return _fileCallbackUserData; }

        /**
         * @brief Set file opening callback
         *
         * In case the importer supports @ref Feature::FileCallback, files
         * opened through @ref openFile() will be loaded through the provided
         * callback. Besides that, all external files referenced by the
         * top-level file will be loaded through the callback function as well,
         * usually on demand. The callback function gets a filename,
         * @ref InputFileCallbackPolicy and the @p userData pointer as input
         * and returns a non-owning view on the loaded data as output or a
         * @ref Corrade::Containers::NullOpt if loading failed --- because
         * empty files might also be valid in some circumstances, @cpp nullptr @ce
         * can't be used to indicate a failure.
         *
         * In case the importer doesn't support @ref Feature::FileCallback but
         * supports at least @ref Feature::OpenData, a file opened through
         * @ref openFile() will be internally loaded through the provided
         * callback and then passed to @ref openData(). First the file is
         * loaded with @ref InputFileCallbackPolicy::LoadTemporary passed to
         * the callback, then the returned memory view is passed to
         * @ref openData() (sidestepping the potential @ref openFile()
         * implementation of that particular importer) and after that the
         * callback is called again with @ref InputFileCallbackPolicy::Close
         * because the semantics of @ref openData() don't require the data to
         * be alive after. In case you need a different behavior, use
         * @ref openData() directly.
         *
         * In case @p callback is @cpp nullptr @ce, the current callback (if
         * any) is reset. This function expects that the importer supports
         * either @ref Feature::FileCallback or @ref Feature::OpenData. If an
         * importer supports neither, callbacks can't be used.
         *
         * It's expected that this function is called *before* a file is
         * opened. It's also expected that the loaded data are kept in scope
         * for as long as the importer needs them, based on the value of
         * @ref InputFileCallbackPolicy. Documentation of particular importers
         * provides more information about the expected callback behavior.
         *
         * Following is an example of setting up a file loading callback for
         * fetching compiled-in resources from @ref Corrade::Utility::Resource.
         * See the overload below for a more convenient type-safe way to pass
         * the user data pointer.
         *
         * @snippet MagnumAudio.cpp AbstractImporter-setFileCallback
         *
         * @see @ref Audio-AbstractImporter-usage-callbacks
         */
        void setFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData = nullptr);

        /**
         * @brief Set file opening callback
         *
         * Equivalent to calling the above with a lambda wrapper that casts
         * @cpp void* @ce back to @cpp T* @ce and dereferences it in order to
         * pass it to @p callback. Example usage:
         *
         * @snippet MagnumAudio.cpp AbstractImporter-setFileCallback-template
         *
         * @see @ref Audio-AbstractImporter-usage-callbacks
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class T> void setFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, T&), T& userData);
        #else
        /* Otherwise the user would be forced to use the + operator to convert
           a lambda to a function pointer and (besides being weird and
           annoying) it's also not portable because it doesn't work on MSVC
           2015 and older versions of MSVC 2017. */
        template<class Callback, class T> void setFileCallback(Callback callback, T& userData);
        #endif

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

    protected:
        /**
         * @brief Implementation for @ref openFile()
         *
         * If @ref Feature::OpenData is supported, default implementation opens
         * the file and calls @ref doOpenData() with its contents. It is
         * allowed to call this function from your @ref doOpenFile()
         * implementation --- in particular, this implementation will also
         * correctly handle callbacks set through @ref setFileCallback().
         *
         * This function is not called when file callbacks are set through
         * @ref setFileCallback() and @ref Feature::FileCallback is not
         * supported --- instead, file is loaded though the callback and data
         * passed through to @ref doOpenData().
         */
        virtual void doOpenFile(const std::string& filename);

    private:
        /** @brief Implementation for @ref features() */
        virtual Features doFeatures() const = 0;

        /**
         * @brief Implementation for @ref setFileCallback()
         *
         * Useful when the importer needs to modify some internal state on
         * callback setup. Default implementation does nothing and this
         * function doesn't need to be implemented --- the callback function
         * and user data pointer are available through @ref fileCallback() and
         * @ref fileCallbackUserData().
         */
        virtual void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData);

        /** @brief Implementation for @ref isOpened() */
        virtual bool doIsOpened() const = 0;

        /** @brief Implementation for @ref openData() */
        virtual void doOpenData(Containers::ArrayView<const char> data);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /** @brief Implementation for @ref format() */
        virtual BufferFormat doFormat() const = 0;

        /** @brief Implementation for @ref frequency() */
        virtual UnsignedInt doFrequency() const = 0;

        /** @brief Implementation for @ref data() */
        virtual Containers::Array<char> doData() = 0;

        Containers::Optional<Containers::ArrayView<const char>>(*_fileCallback)(const std::string&, InputFileCallbackPolicy, void*){};
        void* _fileCallbackUserData{};

        /* Used by the templated version only */
        struct FileCallbackTemplate {
            void(*callback)();
            const void* userData;
        /* GCC 4.8 complains loudly about missing initializers otherwise */
        } _fileCallbackTemplate{nullptr, nullptr};
};

}}

#endif
