#ifndef Magnum_Trade_AbstractImporter_h
#define Magnum_Trade_AbstractImporter_h
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
 * @brief Class @ref Magnum::Trade::AbstractImporter, enum @ref Magnum::Trade::ImporterFeature, enum set @ref Magnum::Trade::ImporterFeatures
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/FileCallback.h"
#endif

namespace Magnum { namespace Trade {

/**
@brief Features supported by an importer
@m_since{2020,06}

@see @ref ImporterFeatures, @ref AbstractImporter::features()
*/
enum class ImporterFeature: UnsignedByte {
    /** Opening files from raw data using @ref AbstractImporter::openData() */
    OpenData = 1 << 0,

    /** Opening already loaded state using @ref AbstractImporter::openState() */
    OpenState = 1 << 1,

    /**
     * Specifying callbacks for loading additional files referenced
     * from the main file using @ref AbstractImporter::setFileCallback(). If
     * the importer doesn't expose this feature, the format is either
     * single-file or loading via callbacks is not supported.
     *
     * See @ref Trade-AbstractImporter-usage-callbacks and particular importer
     * documentation for more information.
     */
    FileCallback = 1 << 2
};

/**
@brief Set of features supported by an importer
@m_since{2020,06}

@see @ref AbstractImporter::features()
*/
typedef Containers::EnumSet<ImporterFeature> ImporterFeatures;

CORRADE_ENUMSET_OPERATORS(ImporterFeatures)

/** @debugoperatorenum{ImporterFeatures} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ImporterFeature value);

/** @debugoperatorenum{ImporterFeatures} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ImporterFeatures value);

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief InputFileCallbackPolicy
 * @m_deprecated_since{2019,10} Use @ref InputFileCallbackPolicy instead.
 */
typedef CORRADE_DEPRECATED("use InputFileCallbackPolicy instead") InputFileCallbackPolicy ImporterFileCallbackPolicy;
#endif

/**
@brief Importer flag
@m_since{2020,06}

@see @ref ImporterFlags, @ref AbstractImporter::setFlags()
*/
enum class ImporterFlag: UnsignedByte {
    /**
     * Print verbose diagnostic during import. By default the importer only
     * prints messages on error or when some operation might cause unexpected
     * data modification or loss.
     */
    Verbose = 1 << 0,

    /** @todo Y flip for images, "I want to import just once, don't copy" ... */
};

/**
@brief Importer flags
@m_since{2020,06}

@see @ref AbstractImporter::setFlags()
*/
typedef Containers::EnumSet<ImporterFlag> ImporterFlags;

CORRADE_ENUMSET_OPERATORS(ImporterFlags)

/**
@debugoperatorenum{ImporterFlag}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ImporterFlag value);

/**
@debugoperatorenum{ImporterFlags}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ImporterFlags value);

#if defined(MAGNUM_BUILD_DEPRECATED) && !defined(DOXYGEN_GENERATING_OUTPUT)
/* Could be a concrete type as only MaterialData need this, but that would
   mean I'd need to include MaterialData here */
template<class T> struct OptionalButAlsoPointer: Containers::Optional<T> {
    /*implicit*/ OptionalButAlsoPointer() = default;
    /*implicit*/ OptionalButAlsoPointer(Containers::Optional<T>&& optional): Containers::Optional<T>{std::move(optional)} {}
    CORRADE_DEPRECATED("use Containers::Optional instead of Containers::Pointer for a MaterialData") /*implicit*/ operator Containers::Pointer<T>() && {
        return Containers::Pointer<T>{new T{std::move(**this)}};
    }
};
#endif

/**
@brief Base for importer plugins

Provides interface for importing 2D/3D scene, camera, light, animation, mesh,
material, texture and image data.

@section Trade-AbstractImporter-usage Usage

Importers are most commonly implemented as plugins. For example, loading an
image from the filesystem using the @ref AnyImageImporter plugin can be done
like this, completely with all error handling:

@snippet MagnumTrade.cpp AbstractImporter-usage

See @ref plugins for more information about general plugin usage and
`*Importer` classes in the @ref Trade namespace for available importer plugins.

@m_class{m-note m-success}

@par
    There are also @ref magnum-imageconverter "magnum-imageconverter" and
    @ref magnum-sceneconverter "magnum-sceneconverter" tools which you can use
    to perform introspection of image and scene files.

@subsection Trade-AbstractImporter-usage-callbacks Loading data from memory, using file callbacks

Besides loading data directly from the filesystem using @ref openFile() like
shown above, it's possible to use @ref openData() to import data from memory.
Note that the particular importer implementation has to support
@ref ImporterFeature::OpenData for this method to work.

Complex scene files often reference other files such as images and in that case
you may want to intercept those references and load them in a custom way as
well. For importers that advertise support for this with
@ref ImporterFeature::FileCallback this is done by specifying a file loading
callback using @ref setFileCallback(). The callback gets a filename,
@ref InputFileCallbackPolicy and an user pointer as parameters; returns a
non-owning view on the loaded data or a
@ref Corrade::Containers::NullOpt "Containers::NullOpt" to indicate the file
loading failed. For example, loading a scene from memory-mapped files could
look like below. Note that the file loading callback affects @ref openFile() as
well --- you don't have to load the top-level file manually and pass it to
@ref openData(), any importer supporting the callback feature handles that
correctly.

@snippet MagnumTrade.cpp AbstractImporter-usage-callbacks

For importers that don't support @ref ImporterFeature::FileCallback directly,
the base @ref openFile() implementation will use the file callback to pass the
loaded data through to @ref openData(), in case the importer supports at least
@ref ImporterFeature::OpenData. If the importer supports neither
@ref ImporterFeature::FileCallback nor @ref ImporterFeature::OpenData,
@ref setFileCallback() doesn't allow the callbacks to be set.

The input file callback signature is the same for @ref Trade::AbstractImporter,
@ref ShaderTools::AbstractConverter and @ref Text::AbstractFont to allow code
reuse.

@subsection Trade-AbstractImporter-usage-state Internal importer state

Some importers, especially ones that make use of well-known external libraries,
expose internal state through various accessors:

-   @ref importerState() can expose a pointer to the global importer
    state for currently opened file
-   @ref MaterialData::importerState() can expose importer state for given
    material imported by @ref material()
-   @ref AnimationData::importerState() can expose importer state for given
    animation imported by @ref animation()
-   @ref CameraData::importerState() can expose importer state for a camera
    importer by @ref camera()
-   @ref ImageData::importerState() can expose importer state for an image
    imported by @ref image1D(), @ref image2D() or @ref image3D()
-   @ref LightData::importerState() can expose importer state for a light
    imported by @ref light()
-   @ref MeshData::importerState() can expose importer state for a mesh
    imported by @ref mesh()
-   @ref ObjectData3D::importerState() can expose importer state for an object
    imported by @ref object2D() or @ref object3D()
-   @ref SceneData::importerState() can expose importer state for a scene
    imported by @ref scene()
-   @ref SkinData::importerState() can expose importer state for a scene
    imported by @ref skin2D() or @ref skin3D()
-   @ref TextureData::importerState() can expose importer state for a texture
    imported by @ref texture()

Besides exposing internal state, importers that support the
@ref ImporterFeature::OpenState feature can also attach to existing importer
state using @ref openState(). See documentation of a particular importer for
details about concrete types returned and accepted by these functions.

@subsection Trade-AbstractImporter-usage-casting Polymorphic imported data types

Some data access functions return @ref Corrade::Containers::Pointer instead of
@ref Corrade::Containers::Optional because the result might be a particular
subclass of given type. Those functions are @ref material(), @ref object2D()
and @ref object3D(). You can cast the abstract base to a concrete type
depending on its reported type, for example:

@snippet MagnumTrade.cpp AbstractImporter-usage-cast

Another option is making use of the @ref Containers::pointerCast() utility, but
note that in that case the original @ref Corrade::Containers::Pointer will be
* *moved into* a new instance and that might not be desirable.

@section Trade-AbstractImporter-data-dependency Data dependency

The `*Data` instances returned from various functions *by design* have no
dependency on the importer instance and neither on the dynamic plugin module.
In other words, you don't need to keep the importer instance (or the plugin
manager instance) around in order to have the `*Data` instances valid.
Moreover, all @ref Corrade::Containers::Array instances returned through
@ref ImageData, @ref AnimationData, @ref MaterialData, @ref MeshData and
@ref SkinData are only allowed to have default deleters (or be non-owning
instances created from @ref Corrade::Containers::ArrayView) --- this is to
avoid potential dangling function pointer calls when destructing such instances
after the plugin module has been unloaded.

The only exception are various `importerState()` functions
@ref Trade-AbstractImporter-usage-state "described above", but in that case the
relation is *weak* --- these are valid only as long as the currently opened
file is kept open. If the file gets closed or the importer instance deleted,
the state pointers become dangling, and that's fine as long as you don't access
them.

@section Trade-AbstractImporter-subclassing Subclassing

The plugin needs to implement the @ref doFeatures(), @ref doIsOpened()
functions, at least one of @ref doOpenData() / @ref doOpenFile() /
@ref doOpenState() functions, function @ref doClose() and one or more tuples of
data access functions, based on what features are supported in given format.

In order to support @ref ImporterFeature::FileCallback, the importer needs to
properly use the callbacks to both load the top-level file in @ref doOpenFile()
and also load any external files when needed. The @ref doOpenFile() can
delegate back into the base implementation, but it should remember at least the
base file path to pass correct paths to subsequent file callbacks. The
@ref doSetFileCallback() can be overriden in case it's desired to respond to
file loading callback setup, but doesn't have to be.

For multi-data formats the file opening shouldn't take long and all parsing
should be done in the data parsing functions instead, because the user might
want to import only some data. This is obviously not the case for single-data
formats like images, as the file contains all the data the user wants to
import.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The @ref doOpenData(), @ref doOpenFile() and @ref doOpenState() functions
    are called after the previous file was closed, function @ref doClose() is
    called only if there is any file opened.
-   The @ref doOpenData() function is called only if
    @ref ImporterFeature::OpenData is supported.
-   The @ref doOpenState() function is called only if
    @ref ImporterFeature::OpenState is supported.
-   The @ref doSetFileCallback() function is called only if
    @ref ImporterFeature::FileCallback is supported and there is no file
    opened.
-   All `do*()` implementations working on an opened file as well as
    @ref doImporterState() are called only if there is any file opened.
-   All `do*()` implementations taking data ID as parameter are called only if
    the ID is from valid range.
-   For @ref doMesh() and `doImage*()` and @p level parameter being nonzero,
    implementations are called only if it is from valid range. Level zero is
    always expected to be present and thus no check is done in that case.

@m_class{m-block m-warning}

@par Dangling function pointers on plugin unload
    As @ref Trade-AbstractImporter-data-dependency "mentioned above",
    @ref Corrade::Containers::Array instances returned from plugin
    implementations are not allowed to use anything else than the default
    deleter or the deleter used by @ref Trade::ArrayAllocator, otherwise this
    could cause dangling function pointer call on array destruction if the
    plugin gets unloaded before the array is destroyed. This is asserted by the
    base implementation on return.
@par
    Similarly for interpolator functions passed through
    @ref Animation::TrackView instances to @ref AnimationData --- to avoid
    dangling pointers, be sure to always include an interpolator returned from
    @ref animationInterpolatorFor(), which guarantees the function is *not*
    instantiated in the plugin binary. Avoid using
    @ref Animation::interpolatorFor() (or indirectly using it by specifying
    just @ref Animation::Interpolation), as it doesn't have such a guarantee.
    Note that unlike with array instances, the base implementation can't easily
    check for this.
*/
class MAGNUM_TRADE_EXPORT AbstractImporter: public PluginManager::AbstractManagingPlugin<AbstractImporter> {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief ImporterFeature
         * @m_deprecated_since{2020,06} Use @ref ImporterFeature instead.
         */
        typedef CORRADE_DEPRECATED("use ImporterFeature instead") ImporterFeature Feature;

        /** @brief @copybrief ImporterFeatures
         * @m_deprecated_since{2020,06} Use @ref ImporterFeatures instead.
         */
        typedef CORRADE_DEPRECATED("use ImporterFeature instead") ImporterFeatures Features;
        #endif

        /**
         * @brief Plugin interface
         *
         * @snippet Magnum/Trade/AbstractImporter.cpp interface
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * Looks into `magnum/importers/` or `magnum-d/importers/` next to the
         * dynamic @ref Trade library, next to the executable and elsewhere
         * according to the rules documented in
         * @ref Corrade::PluginManager::implicitPluginSearchPaths(). The search
         * directory can be also hardcoded using the `MAGNUM_PLUGINS_DIR` CMake
         * variables, see @ref building for more information.
         *
         * Not defined on platforms without
         * @ref CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT "dynamic plugin support".
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

        /**
         * @brief Importer flags
         * @m_since{2020,06}
         */
        ImporterFlags flags() const { return _flags; }

        /**
         * @brief Set importer flags
         * @m_since{2020,06}
         *
         * It's expected that this function is called *before* a file is
         * opened. Some flags can be set only if the importer supports
         * particular features, see documentation of each @ref ImporterFlag for
         * more information. By default no flags are set.
         */
        void setFlags(ImporterFlags flags);

        /**
         * @brief File opening callback function
         *
         * @see @ref Trade-AbstractImporter-usage-callbacks
         */
        auto fileCallback() const -> Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*) { return _fileCallback; }

        /**
         * @brief File opening callback user data
         *
         * @see @ref Trade-AbstractImporter-usage-callbacks
         */
        void* fileCallbackUserData() const { return _fileCallbackUserData; }

        /**
         * @brief Set file opening callback
         *
         * In case the importer supports @ref ImporterFeature::FileCallback,
         * files opened through @ref openFile() will be loaded through the
         * provided callback. Besides that, all external files referenced by
         * the top-level file will be loaded through the callback function as
         * well, usually on demand. The callback function gets a filename,
         * @ref InputFileCallbackPolicy and the @p userData pointer as input
         * and returns a non-owning view on the loaded data as output or a
         * @ref Corrade::Containers::NullOpt if loading failed --- because
         * empty files might also be valid in some circumstances, @cpp nullptr @ce
         * can't be used to indicate a failure.
         *
         * In case the importer doesn't support @ref ImporterFeature::FileCallback
         * but supports at least @ref ImporterFeature::OpenData, a file opened
         * through @ref openFile() will be internally loaded through the
         * provided callback and then passed to @ref openData(). First the file
         * is loaded with @ref InputFileCallbackPolicy::LoadTemporary passed to
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
         * either @ref ImporterFeature::FileCallback or
         * @ref ImporterFeature::OpenData. If an importer supports neither,
         * callbacks can't be used.
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
         * @snippet MagnumTrade.cpp AbstractImporter-setFileCallback
         *
         * @see @ref Trade-AbstractImporter-usage-callbacks
         */
        void setFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData = nullptr);

        /**
         * @brief Set file opening callback
         *
         * Equivalent to calling the above with a lambda wrapper that casts
         * @cpp void* @ce back to @cpp T* @ce and dereferences it in order to
         * pass it to @p callback. Example usage --- this reuses an existing
         * @ref Corrade::Utility::Resource instance to avoid a potentially slow
         * resource group lookup every time:
         *
         * @snippet MagnumTrade.cpp AbstractImporter-setFileCallback-template
         *
         * @see @ref Trade-AbstractImporter-usage-callbacks
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
         * Closes previous file, if it was opened, and tries to open given raw
         * data. Available only if @ref ImporterFeature::OpenData is supported.
         * Returns @cpp true @ce on success, @cpp false @ce otherwise. The
         * @p data is not expected to be alive after the function exits.
         * @see @ref features(), @ref openFile()
         */
        bool openData(Containers::ArrayView<const char> data);

        /**
         * @brief Open already loaded state
         * @param state     Pointer to importer-specific state
         * @param filePath  Base file directory for opening external data like
         *      textures and materials.
         *
         * Closes previous file, if it was opened, and tries to open given
         * state. Available only if @ref ImporterFeature::OpenState is
         * supported. Returns @cpp true @ce on success, @cpp false @ce
         * otherwise.
         *
         * See documentation of a particular plugin for more information about
         * type and contents of the @p state parameter.
         * @see @ref features(), @ref openData()
         */
        bool openState(const void* state, const std::string& filePath = {});

        /**
         * @brief Open a file
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Returns @cpp true @ce on success, @cpp false @ce otherwise.
         * If file loading callbacks are set via @ref setFileCallback() and
         * @ref ImporterFeature::OpenData is supported, this function uses the
         * callback to load the file and passes the memory view to
         * @ref openData() instead. See @ref setFileCallback() for more
         * information.
         * @see @ref features(), @ref openData()
         */
        bool openFile(const std::string& filename);

        /**
         * @brief Close currently opened file
         *
         * On particular implementations an explicit call to this function may
         * result in freed memory. This call is also done automatically when
         * the importer gets destructed or when another file is opened.
         */
        void close();

        /** @{ @name Data accessors
         * Each function tuple provides access to given data.
         */

        /**
         * @brief Default scene
         *
         * When there is more than one scene, returns ID of the default one.
         * If there is no default scene, returns @cpp -1 @ce. Expects that a
         * file is opened.
         */
        Int defaultScene() const;

        /**
         * @brief Scene count
         *
         * Expects that a file is opened.
         */
        UnsignedInt sceneCount() const;

        /**
         * @brief Scene ID for given name
         *
         * If no scene for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref sceneName(), @ref scene(const std::string&)
         */
        Int sceneForName(const std::string& name);

        /**
         * @brief Scene name
         * @param id        Scene ID, from range [0, @ref sceneCount()).
         *
         * Expects that a file is opened.
         * @see @ref sceneForName()
         */
        std::string sceneName(UnsignedInt id);

        /**
         * @brief Scene
         * @param id        Scene ID, from range [0, @ref sceneCount()).
         *
         * Returns given scene or @ref Containers::NullOpt if import failed.
         * Expects that a file is opened.
         * @see @ref scene(const std::string&)
         */
        Containers::Optional<SceneData> scene(UnsignedInt id);

        /**
         * @brief Scene for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref sceneForName() and
         * @ref scene(UnsignedInt). If @ref sceneForName() returns @cpp -1 @ce,
         * prints an error message and returns @ref Containers::NullOpt,
         * otherwise propagates the result from @ref scene(UnsignedInt).
         * Expects that a file is opened.
         */
        Containers::Optional<SceneData> scene(const std::string& name);

        /**
         * @brief Animation count
         *
         * Expects that a file is opened.
         */
        UnsignedInt animationCount() const;

        /**
         * @brief Animation ID for given name
         *
         * If no animation for given name exists, returns @cpp -1 @ce. Expects
         * that a file is opened.
         * @see @ref animationName(), @ref animation(const std::string&)
         */
        Int animationForName(const std::string& name);

        /**
         * @brief Animation name
         * @param id    Animation ID, from range [0, @ref animationCount()).
         *
         * Expects that a file is opened.
         * @see @ref animationForName()
         */
        std::string animationName(UnsignedInt id);

        /**
         * @brief Animation
         * @param id    Animation ID, from range [0, @ref animationCount()).
         *
         * Returns given animation or @ref Containers::NullOpt if importing
         * failed. Expects that a file is opened.
         * @see @ref animation(const std::string&)
         */
        Containers::Optional<AnimationData> animation(UnsignedInt id);

        /**
         * @brief Animation for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref animationForName() and
         * @ref animation(UnsignedInt). If @ref animationForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref animation(UnsignedInt). Expects that a file is opened.
         */
        Containers::Optional<AnimationData> animation(const std::string& name);

        /**
         * @brief Light count
         *
         * Expects that a file is opened.
         */
        UnsignedInt lightCount() const;

        /**
         * @brief Light ID for given name
         *
         * If no light for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref lightName(), @ref light(const std::string&)
         */
        Int lightForName(const std::string& name);

        /**
         * @brief Light name
         * @param id        Light ID, from range [0, @ref lightCount()).
         *
         * Expects that a file is opened.
         * @see @ref lightForName()
         */
        std::string lightName(UnsignedInt id);

        /**
         * @brief Light
         * @param id        Light ID, from range [0, @ref lightCount()).
         *
         * Returns given light or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         * @see @ref light(const std::string&)
         */
        Containers::Optional<LightData> light(UnsignedInt id);

        /**
         * @brief Light for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref lightForName() and
         * @ref light(UnsignedInt). If @ref lightForName() returns @cpp -1 @ce,
         * prints an error message and returns @ref Containers::NullOpt,
         * otherwise propagates the result from @ref light(UnsignedInt).
         * Expects that a file is opened.
         */
        Containers::Optional<LightData> light(const std::string& name);

        /**
         * @brief Camera count
         *
         * Expects that a file is opened.
         */
        UnsignedInt cameraCount() const;

        /**
         * @brief Camera ID for given name
         *
         * If no camera for given name exists, returns @cpp -1 @ce. Expects
         * that a file is opened.
         * @see @ref cameraName(), @ref camera(const std::string&)
         */
        Int cameraForName(const std::string& name);

        /**
         * @brief Camera name
         * @param id        Camera ID, from range [0, @ref cameraCount()).
         *
         * Expects that a file is opened.
         * @see @ref cameraForName()
         */
        std::string cameraName(UnsignedInt id);

        /**
         * @brief Camera
         * @param id        Camera ID, from range [0, @ref cameraCount()).
         *
         * Returns given camera or @ref Containers::NullOpt if importing
         * failed. Expects that a file is opened.
         * @see @ref camera(const std::string&)
         */
        Containers::Optional<CameraData> camera(UnsignedInt id);

        /**
         * @brief Camera for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref cameraForName() and
         * @ref camera(UnsignedInt). If @ref cameraForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref camera(UnsignedInt). Expects that a file is opened.
         */
        Containers::Optional<CameraData> camera(const std::string& name);

        /**
         * @brief Two-dimensional object count
         *
         * Expects that a file is opened.
         */
        UnsignedInt object2DCount() const;

        /**
         * @brief Two-dimensional object ID for given name
         *
         * If no object for given name exists, returns @cpp -1 @ce. Expects
         * that a file is opened.
         * @see @ref object2DName(), @ref object2D(const std::string&)
         */
        Int object2DForName(const std::string& name);

        /**
         * @brief Two-dimensional object name
         * @param id        Object ID, from range [0, @ref object2DCount()).
         *
         * Expects that a file is opened.
         * @see @ref object2DForName()
         */
        std::string object2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional object
         * @param id        Object ID, from range [0, @ref object2DCount()).
         *
         * Returns given object or @cpp nullptr @ce if importing failed.
         * Expects that a file is opened.
         * @see @ref object2D(const std::string&)
         */
        Containers::Pointer<ObjectData2D> object2D(UnsignedInt id);

        /**
         * @brief Two-dimensional object for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref object2DForName() and
         * @ref object2D(UnsignedInt). If @ref object2DForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @cpp nullptr @ce, otherwise propagates the result from
         * @ref object2D(UnsignedInt). Expects that a file is opened.
         */
        Containers::Pointer<ObjectData2D> object2D(const std::string& name);

        /**
         * @brief Three-dimensional object count
         *
         * Expects that a file is opened.
         */
        UnsignedInt object3DCount() const;

        /**
         * @brief Three-dimensional object ID for given name
         *
         * If no object for given name exists, returns @cpp -1 @ce. Expects
         * that a file is opened.
         * @see @ref object3DName(), @ref object3D(const std::string&)
         */
        Int object3DForName(const std::string& name);

        /**
         * @brief Three-dimensional object name
         * @param id        Object ID, from range [0, @ref object3DCount()).
         *
         * Expects that a file is opened.
         * @see @ref object3DForName()
         */
        std::string object3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional object
         * @param id        Object ID, from range [0, @ref object3DCount()).
         *
         * Returns given object or @cpp nullptr @ce if importing failed.
         * Expects that a file is opened.
         * @see @ref object3D(const std::string&)
         */
        Containers::Pointer<ObjectData3D> object3D(UnsignedInt id);

        /**
         * @brief Three-dimensional object for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref object3DForName() and
         * @ref object3D(UnsignedInt). If @ref object3DForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @cpp nullptr @ce, otherwise propagates the result from
         * @ref object3D(UnsignedInt). Expects that a file is opened.
         */
        Containers::Pointer<ObjectData3D> object3D(const std::string& name);

        /**
         * @brief Two-dimensional skin count
         * @m_since_latest
         *
         * Expects that a file is opened.
         */
        UnsignedInt skin2DCount() const;

        /**
         * @brief Two-dimensional skin ID for given name
         * @m_since_latest
         *
         * If no skin for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref skin2DName(), @ref skin2D(const std::string&)
         */
        Int skin2DForName(const std::string& name);

        /**
         * @brief Two-dimensional skin name
         * @param id        Skin ID, from range [0, @ref skin2DCount()).
         * @m_since_latest
         *
         * Expects that a file is opened.
         * @see @ref skin2DForName()
         */
        std::string skin2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional skin
         * @param id        Skin ID, from range [0, @ref skin2DCount()).
         * @m_since_latest
         *
         * Returns given skin or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         * @see @ref skin2D(const std::string&)
         */
        Containers::Optional<SkinData2D> skin2D(UnsignedInt id);

        /**
         * @brief Two-dimensional skin for given name
         * @m_since_latest
         *
         * A convenience API combining @ref skin2DForName() and
         * @ref skin2D(UnsignedInt). If @ref skin2DForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref skin2D(UnsignedInt). Expects that a file is opened.
         */
        Containers::Optional<SkinData2D> skin2D(const std::string& name);

        /**
         * @brief Three-dimensional skin count
         * @m_since_latest
         *
         * Expects that a file is opened.
         */
        UnsignedInt skin3DCount() const;

        /**
         * @brief Three-dimensional skin ID for given name
         * @m_since_latest
         *
         * If no skin for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref skin3DName(), @ref skin3D(const std::string&)
         */
        Int skin3DForName(const std::string& name);

        /**
         * @brief Three-dimensional skin name
         * @param id        Skin ID, from range [0, @ref skin3DCount()).
         * @m_since_latest
         *
         * Expects that a file is opened.
         * @see @ref skin3DForName()
         */
        std::string skin3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional skin
         * @param id        Skin ID, from range [0, @ref skin3DCount()).
         * @m_since_latest
         *
         * Returns given skin or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         * @see @ref skin3D(const std::string&)
         */
        Containers::Optional<SkinData3D> skin3D(UnsignedInt id);

        /**
         * @brief Three-dimensional object for given name
         * @m_since_latest
         *
         * A convenience API combining @ref skin3DForName() and
         * @ref skin3D(UnsignedInt). If @ref skin3DForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref skin3D(UnsignedInt). Expects that a file is opened.
         */
        Containers::Optional<SkinData3D> skin3D(const std::string& name);

        /**
         * @brief Mesh count
         * @m_since{2020,06}
         *
         * Expects that a file is opened.
         * @see @ref meshLevelCount()
         */
        UnsignedInt meshCount() const;

        /**
         * @brief Mesh level count
         * @param id        Mesh ID, from range [0, @ref meshCount()).
         * @m_since{2020,06}
         *
         * Always returns at least one level, import failures are deferred to
         * @ref mesh(). Expects that a file is opened.
         */
        UnsignedInt meshLevelCount(UnsignedInt id);

        /**
         * @brief Mesh ID for given name
         * @m_since{2020,06}
         *
         * If no mesh for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref meshName(), @ref mesh(const std::string&, UnsignedInt)
         */
        Int meshForName(const std::string& name);

        /**
         * @brief Mesh name
         * @param id        Mesh ID, from range [0, @ref meshCount()).
         * @m_since{2020,06}
         *
         * Expects that a file is opened.
         * @see @ref meshForName()
         */
        std::string meshName(UnsignedInt id);

        /**
         * @brief Mesh
         * @param id        Mesh ID, from range [0, @ref meshCount()).
         * @param level     Mesh level, from range [0, @ref meshLevelCount())
         * @m_since{2020,06}
         *
         * Returns given mesh or @ref Containers::NullOpt if importing failed.
         * The @p level parameter allows access to additional data and is
         * largely left as importer-specific --- for example allowing access to
         * per-instance, per-face or per-edge data. Expects that a file is
         * opened.
         * @see @ref mesh(const std::string&, UnsignedInt),
         *      @ref MeshPrimitive::Instances, @ref MeshPrimitive::Faces,
         *      @ref MeshPrimitive::Edges
         */
        Containers::Optional<MeshData> mesh(UnsignedInt id, UnsignedInt level = 0);

        /**
         * @brief Mesh for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref meshForName() and
         * @ref mesh(UnsignedInt, UnsignedInt). If @ref meshForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref mesh(UnsignedInt, UnsignedInt). Expects that a file is opened.
         */
        Containers::Optional<MeshData> mesh(const std::string& name, UnsignedInt level = 0);

        /**
         * @brief Mesh attribute for given name
         * @m_since{2020,06}
         *
         * If the name is not recognized, returns a zero (invalid)
         * @ref MeshAttribute, otherwise returns a custom mesh attribute. Note
         * that the value returned by this function may depend on whether a
         * file is opened or not and also be different for different files ---
         * see documentation of a particular importer for more information.
         * @see @ref isMeshAttributeCustom()
         */
        MeshAttribute meshAttributeForName(const std::string& name);

        /**
         * @brief String name for given custom mesh attribute
         * @m_since{2020,06}
         *
         * Given a custom @p name returned by @ref mesh() in a @ref MeshData,
         * returns a string identifier. If a string representation is not
         * available or @p name is not recognized, returns an empty string.
         * Expects that @p name is custom. Note that the value returned by
         * this function may depend on whether a file is opened or not and also
         * be different for different files --- see documentation of a
         * particular importer for more information.
         * @see @ref isMeshAttributeCustom()
         */
        std::string meshAttributeName(MeshAttribute name);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Two-dimensional mesh count
         *
         * Expects that a file is opened.
         * @m_deprecated_since{2020,06} Use @ref meshCount() instead.
         */
        CORRADE_DEPRECATED("use meshCount() instead") UnsignedInt mesh2DCount() const;

        /**
         * @brief Two-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @m_deprecated_since{2020,06} Use @ref meshForName() instead.
         * @see @ref mesh2DName()
         */
        CORRADE_DEPRECATED("use meshForName() instead") Int mesh2DForName(const std::string& name);

        /**
         * @brief Two-dimensional mesh name
         * @param id        Mesh ID, from range [0, @ref mesh2DCount()).
         *
         * Expects that a file is opened.
         * @m_deprecated_since{2020,06} Use @ref meshName() instead.
         * @see @ref mesh2DForName()
         */
        CORRADE_DEPRECATED("use meshName() instead") std::string mesh2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional mesh
         * @param id        Mesh ID, from range [0, @ref mesh2DCount()).
         *
         * Returns given mesh or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         * @m_deprecated_since{2020,06} Use @ref mesh() instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        CORRADE_DEPRECATED("use mesh() instead") Containers::Optional<MeshData2D> mesh2D(UnsignedInt id);
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Three-dimensional mesh count
         *
         * Expects that a file is opened.
         * @m_deprecated_since{2020,06} Use @ref meshCount() instead.
         */
        CORRADE_DEPRECATED("use meshCount() instead") UnsignedInt mesh3DCount() const;

        /**
         * @brief Three-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @m_deprecated_since{2020,06} Use @ref meshForName() instead.
         * @see @ref mesh3DName()
         */
        CORRADE_DEPRECATED("use meshForName() instead") Int mesh3DForName(const std::string& name);

        /**
         * @brief Three-dimensional mesh name
         * @param id        Mesh ID, from range [0, @ref mesh3DCount()).
         *
         * Expects that a file is opened.
         * @m_deprecated_since{2020,06} Use @ref meshName() instead.
         * @see @ref mesh3DForName()
         */
        CORRADE_DEPRECATED("use meshName() instead") std::string mesh3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional mesh
         * @param id        Mesh ID, from range [0, @ref mesh3DCount()).
         *
         * Returns given mesh or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         * @m_deprecated_since{2020,06} Use @ref meshName() instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        CORRADE_DEPRECATED("use mesh() instead") Containers::Optional<MeshData3D> mesh3D(UnsignedInt id);
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Material count
         *
         * Expects that a file is opened.
         */
        UnsignedInt materialCount() const;

        /**
         * @brief Material ID for given name
         *
         * If no material for given name exists, returns @cpp -1 @ce. Expects
         * that a file is opened.
         * @see @ref materialName()
         */
        Int materialForName(const std::string& name);

        /**
         * @brief Material name
         * @param id        Material ID, from range [0, @ref materialCount()).
         *
         * Expects that a file is opened.
         * @see @ref materialForName(), @ref material(const std::string&)
         */
        std::string materialName(UnsignedInt id);

        /**
         * @brief Material
         * @param id        Material ID, from range [0, @ref materialCount()).
         *
         * Returns given material or @ref Containers::NullOpt if importing
         * failed. Expects that a file is opened.
         * @see @ref material(const std::string&)
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<MaterialData>
        #else
        OptionalButAlsoPointer<MaterialData>
        #endif
        material(UnsignedInt id);

        /**
         * @brief Material for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref materialForName() and
         * @ref material(UnsignedInt). If @ref materialForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref material(UnsignedInt). Expects that a file is opened.
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<MaterialData>
        #else
        OptionalButAlsoPointer<MaterialData>
        #endif
        material(const std::string& name);

        /**
         * @brief Texture count
         *
         * Expects that a file is opened.
         */
        UnsignedInt textureCount() const;

        /**
         * @brief Texture ID for given name
         *
         * If no texture for given name exists, returns @cpp -1 @ce. Expects
         * that a file is opened.
         * @see @ref textureName(), @ref texture(const std::string&)
         */
        Int textureForName(const std::string& name);

        /**
         * @brief Texture name
         * @param id        Texture ID, from range [0, @ref textureCount()).
         *
         * Expects that a file is opened.
         * @see @ref textureForName()
         */
        std::string textureName(UnsignedInt id);

        /**
         * @brief Texture
         * @param id        Texture ID, from range [0, @ref textureCount()).
         *
         * Returns given texture or @ref Containers::NullOpt if importing
         * failed. Expects that a file is opened.
         * @see @ref texture(const std::string&)
         */
        Containers::Optional<TextureData> texture(UnsignedInt id);

        /**
         * @brief Texture for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref textureForName() and
         * @ref texture(UnsignedInt). If @ref textureForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref texture(UnsignedInt). Expects that a file is opened.
         */
        Containers::Optional<TextureData> texture(const std::string& name);

        /**
         * @brief One-dimensional image count
         *
         * Expects that a file is opened.
         * @see @ref image1DLevelCount()
         */
        UnsignedInt image1DCount() const;

        /**
         * @brief One-dimensional image mip level count
         * @param id        Image ID, from range [0, @ref image1DCount())
         * @m_since{2020,06}
         *
         * Always returns at least one level, import failures are deferred to
         * @ref image1D(). Expects that a file is opened.
         */
        UnsignedInt image1DLevelCount(UnsignedInt id);

        /**
         * @brief One-dimensional image ID for given name
         *
         * If no image for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref image1DName(), @ref image1D(const std::string&, UnsignedInt)
         */
        Int image1DForName(const std::string& name);

        /**
         * @brief One-dimensional image name
         * @param id        Image ID, from range [0, @ref image1DCount()).
         *
         * Expects that a file is opened.
         * @see @ref image1DForName()
         */
        std::string image1DName(UnsignedInt id);

        /**
         * @brief One-dimensional image
         * @param id        Image ID, from range [0, @ref image1DCount()).
         * @param level     Mip level, from range [0, @ref image1DLevelCount())
         *
         * Returns given image or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         * @see @ref image1D(const std::string&, UnsignedInt)
         */
        Containers::Optional<ImageData1D> image1D(UnsignedInt id, UnsignedInt level = 0);

        /**
         * @brief One-dimensional image for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref image1DForName() and
         * @ref image1D(UnsignedInt, UnsignedInt). If @ref image1DForName()
         * returns @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref image1D(UnsignedInt, UnsignedInt). Expects that a file is
         * opened.
         */
        Containers::Optional<ImageData1D> image1D(const std::string& name, UnsignedInt level = 0);

        /**
         * @brief Two-dimensional image count
         *
         * Expects that a file is opened.
         * @see @ref image2DLevelCount()
         */
        UnsignedInt image2DCount() const;

        /**
         * @brief Two-dimensional image mip level count
         * @param id        Image ID, from range [0, @ref image2DCount()).
         * @m_since{2020,06}
         *
         * Always returns at least one level, import failures are deferred to
         * @ref image2D(). Expects that a file is opened.
         */
        UnsignedInt image2DLevelCount(UnsignedInt id);

        /**
         * @brief Two-dimensional image ID for given name
         *
         * If no image for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref image2DName(), @ref image2D(const std::string&, UnsignedInt)
         */
        Int image2DForName(const std::string& name);

        /**
         * @brief Two-dimensional image name
         * @param id        Image ID, from range [0, @ref image2DCount()).
         *
         * Expects that a file is opened.
         * @see @ref image2DForName()
         */
        std::string image2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional image
         * @param id        Image ID, from range [0, @ref image2DCount()).
         * @param level     Mip level, from range [0, @ref image2DLevelCount())
         *
         * Returns given image or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         * @see @ref image2D(const std::string&, UnsignedInt)
         */
        Containers::Optional<ImageData2D> image2D(UnsignedInt id, UnsignedInt level = 0);

        /**
         * @brief Two-dimensional image for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref image2DForName() and
         * @ref image2D(UnsignedInt, UnsignedInt). If @ref image2DForName()
         * returns @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref image2D(UnsignedInt, UnsignedInt). Expects that a file is
         * opened.
         */
        Containers::Optional<ImageData2D> image2D(const std::string& name, UnsignedInt level = 0);

        /**
         * @brief Three-dimensional image count
         *
         * Expects that a file is opened.
         * @see @ref image3DLevelCount()
         */
        UnsignedInt image3DCount() const;

        /**
         * @brief Three-dimensional image mip level count
         * @param id        Image ID, from range [0, @ref image3DCount())
         * @m_since{2020,06}
         *
         * Always returns at least one level, import failures are deferred to
         * @ref image3D(). Expects that a file is opened.
         */
        UnsignedInt image3DLevelCount(UnsignedInt id);

        /**
         * @brief Three-dimensional image ID for given name
         *
         * If no image for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref image3DName(), @ref image3D(const std::string&, UnsignedInt)
         */
        Int image3DForName(const std::string& name);

        /**
         * @brief Three-dimensional image name
         * @param id        Image ID, from range [0, @ref image3DCount()).
         *
         * Expects that a file is opened.
         * @see @ref image3DForName()
         */
        std::string image3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional image
         * @param id        Image ID, from range [0, @ref image3DCount()).
         * @param level     Mip level, from range [0, @ref image3DLevelCount())
         *
         * Returns given image or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         * @see @ref image3D(const std::string&, UnsignedInt)
         */
        Containers::Optional<ImageData3D> image3D(UnsignedInt id, UnsignedInt level = 0);

        /**
         * @brief Three-dimensional image for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref image3DForName() and
         * @ref image3D(UnsignedInt, UnsignedInt). If @ref image3DForName()
         * returns @cpp -1 @ce, prints an error message and returns
         * @ref Containers::NullOpt, otherwise propagates the result from
         * @ref image3D(UnsignedInt, UnsignedInt). Expects that a file is
         * opened.
         */
        Containers::Optional<ImageData3D> image3D(const std::string& name, UnsignedInt level = 0);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /**
         * @brief Plugin-specific access to internal importer state
         *
         * The importer might provide access to its internal data structures
         * for currently opened document through this function. See
         * documentation of a particular plugin for more information about
         * returned type and contents. Returns @cpp nullptr @ce by default.
         * Expects that a file is opened.
         * @see @ref AbstractMaterialData::importerState(),
         *      @ref AnimationData::importerState(), @ref CameraData::importerState(),
         *      @ref ImageData::importerState(), @ref LightData::importerState(),
         *      @ref MeshData::importerState(),
         *      @ref ObjectData2D::importerState(), @ref ObjectData3D::importerState(),
         *      @ref SceneData::importerState(), @ref TextureData::importerState()
         */
        const void* importerState() const;

    protected:
        /**
         * @brief Implementation for @ref openFile()
         *
         * If @ref ImporterFeature::OpenData is supported, default
         * implementation opens the file and calls @ref doOpenData() with its
         * contents. It is allowed to call this function from your
         * @ref doOpenFile() implementation --- in particular, this
         * implementation will also correctly handle callbacks set through
         * @ref setFileCallback().
         *
         * This function is not called when file callbacks are set through
         * @ref setFileCallback() and @ref ImporterFeature::FileCallback is not
         * supported --- instead, file is loaded though the callback and data
         * passed through to @ref doOpenData().
         */
        virtual void doOpenFile(const std::string& filename);

    private:
        /** @brief Implementation for @ref features() */
        virtual ImporterFeatures doFeatures() const = 0;

        /**
         * @brief Implementation for @ref setFlags()
         *
         * Useful when the importer needs to modify some internal state on
         * flag setup. Default implementation does nothing and this
         * function doesn't need to be implemented --- the flags are available
         * through @ref flags().
         *
         * To reduce the amount of error checking on user side, this function
         * isn't expected to fail --- if a flag combination is invalid /
         * unsuported, error reporting should be delayed to @ref openFile() and
         * others, where the user is expected to do error handling anyway.
         */
        virtual void doSetFlags(ImporterFlags flags);

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

        /** @brief Implementation for @ref openState() */
        virtual void doOpenState(const void* state, const std::string& filePath);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /**
         * @brief Implementation for @ref defaultScene()
         *
         * Default implementation returns @cpp -1 @ce. This function isn't
         * expected to fail --- if an import error occus (for example because
         * the default scene index is out of bounds), it should be handled
         * already during file opening.
         */
        virtual Int doDefaultScene() const;

        /**
         * @brief Implementation for @ref sceneCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doScene() (with correct scene count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doSceneCount() const;

        /**
         * @brief Implementation for @ref sceneForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doSceneForName(const std::string& name);

        /**
         * @brief Implementation for @ref sceneName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doSceneName(UnsignedInt id);

        /** @brief Implementation for @ref scene() */
        virtual Containers::Optional<SceneData> doScene(UnsignedInt id);

        /**
         * @brief Implementation for @ref animationCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doAnimation() (with correct animation count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doAnimationCount() const;

        /**
         * @brief Implementation for @ref animationForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doAnimationForName(const std::string& name);

        /**
         * @brief Implementation for @ref animationName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doAnimationName(UnsignedInt id);

        /** @brief Implementation for @ref animation() */
        virtual Containers::Optional<AnimationData> doAnimation(UnsignedInt id);

        /**
         * @brief Implementation for @ref lightCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doLight() (with correct light count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doLightCount() const;

        /**
         * @brief Implementation for @ref lightForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doLightForName(const std::string& name);

        /**
         * @brief Implementation for @ref lightName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doLightName(UnsignedInt id);

        /** @brief Implementation for @ref light() */
        virtual Containers::Optional<LightData> doLight(UnsignedInt id);

        /**
         * @brief Implementation for @ref cameraCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doCamera() (with correct camera count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doCameraCount() const;

        /**
         * @brief Implementation for @ref cameraForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doCameraForName(const std::string& name);

        /**
         * @brief Implementation for @ref cameraName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doCameraName(UnsignedInt id);

        /** @brief Implementation for @ref camera() */
        virtual Containers::Optional<CameraData> doCamera(UnsignedInt id);

        /**
         * @brief Implementation for @ref object2DCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doObject2D() (with correct object count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doObject2DCount() const;

        /**
         * @brief Implementation for @ref object2DForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doObject2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref object2DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doObject2DName(UnsignedInt id);

        /** @brief Implementation for @ref object2D() */
        virtual Containers::Pointer<ObjectData2D> doObject2D(UnsignedInt id);

        /**
         * @brief Implementation for @ref object3DCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doObject3D() (with correct object count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doObject3DCount() const;

        /**
         * @brief Implementation for @ref object3DForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doObject3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref object3DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doObject3DName(UnsignedInt id);

        /** @brief Implementation for @ref object3D() */
        virtual Containers::Pointer<ObjectData3D> doObject3D(UnsignedInt id);

        /**
         * @brief Implementation for @ref skin2DCount()
         * @m_since_latest
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doSkin2D() (with correct skin count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doSkin2DCount() const;

        /**
         * @brief Implementation for @ref skin2DForName()
         * @m_since_latest
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doSkin2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref skin2DName()
         * @m_since_latest
         *
         * Default implementation returns empty string.
         */
        virtual std::string doSkin2DName(UnsignedInt id);

        /**
         * @brief Implementation for @ref skin2D()
         * @m_since_latest
         */
        virtual Containers::Optional<SkinData2D> doSkin2D(UnsignedInt id);

        /**
         * @brief Implementation for @ref skin3DCount()
         * @m_since_latest
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doSkin3D() (with correct skin count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doSkin3DCount() const;

        /**
         * @brief Implementation for @ref skin3DForName()
         * @m_since_latest
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doSkin3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref skin3DName()
         * @m_since_latest
         *
         * Default implementation returns empty string.
         */
        virtual std::string doSkin3DName(UnsignedInt id);

        /**
         * @brief Implementation for @ref skin3D()
         * @m_since_latest
         */
        virtual Containers::Optional<SkinData3D> doSkin3D(UnsignedInt id);

        /**
         * @brief Implementation for @ref meshCount()
         * @m_since{2020,06}
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doMesh() (with correct mesh count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doMeshCount() const;

        /**
         * @brief Implementation for @ref meshLevelCount()
         * @m_since{2020,06}
         *
         * Default implementation returns @cpp 1 @ce. Similarly to all other
         * `*Count()` functions, this function isn't expected to fail --- if an
         * import error occus, this function should return @cpp 1 @ce and the
         * error state should be returned from @ref mesh() instead.
         *
         * Deliberately not @cpp const @ce to allow plugins cache decoded
         * data.
         */
        virtual UnsignedInt doMeshLevelCount(UnsignedInt id);

        /**
         * @brief Implementation for @ref meshForName()
         * @m_since{2020,06}
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doMeshForName(const std::string& name);

        /**
         * @brief Implementation for @ref meshName()
         * @m_since{2020,06}
         *
         * Default implementation returns an empty string.
         */
        virtual std::string doMeshName(UnsignedInt id);

        /**
         * @brief Implementation for @ref mesh()
         * @m_since{2020,06}
         */
        virtual Containers::Optional<MeshData> doMesh(UnsignedInt id, UnsignedInt level);

        /**
         * @brief Implementation for @ref meshAttributeForName()
         * @m_since{2020,06}
         *
         * Default implementation returns an invalid (zero) value.
         */
        virtual MeshAttribute doMeshAttributeForName(const std::string& name);

        /**
         * @brief Implementation for @ref meshAttributeName()
         * @m_since{2020,06}
         *
         * Receives the custom ID extracted via @ref meshAttributeCustom(MeshAttribute).
         * Default implementation returns an empty string.
         */
        virtual std::string doMeshAttributeName(UnsignedShort name);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Implementation for @ref mesh2DCount()
         *
         * Default implementation returns @cpp 0 @ce. There weren't any
         * importers in existence known to implement 2D mesh import, so unlike
         * @ref doMesh3DCount() this function doesn't delegate to
         * @ref doMeshCount().
         * @m_deprecated_since{2020,06} Implement @ref doMeshCount() instead.
         */
        /* MSVC warns when overriding such methods and there's no way to
           suppress that warning, making the RT build (which treats deprecation
           warnings as errors) fail and other builds extremely noisy. So
           disabling those on MSVC. */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doMeshCount() instead")
        #endif
        virtual UnsignedInt doMesh2DCount() const;

        /**
         * @brief Implementation for @ref mesh2DForName()
         *
         * Default implementation returns @cpp -1 @ce. There weren't any
         * importers in existence known to implement 2D mesh import, so unlike
         * @ref doMesh3DForName() this function doesn't delegate to
         * @ref doMeshForName().
         * @m_deprecated_since{2020,06} Implement @ref doMeshForName() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doMeshForName() instead")
        #endif /* See above */
        virtual Int doMesh2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref mesh2DName()
         *
         * Default implementation returns empty string. There weren't any
         * importers in existence known to implement 2D mesh import, so unlike
         * @ref doMesh3DName() this function doesn't delegate to
         * @ref doMeshName().
         * @m_deprecated_since{2020,06} Implement @ref doMeshName() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doMeshName() instead")
        #endif /* See above */
        virtual std::string doMesh2DName(UnsignedInt id);

        /**
         * @brief Implementation for @ref mesh2D()
         *
         * There weren't any importers in existence known to implement 2D mesh
         * import, so unlike @ref doMesh3D() this function doesn't
         * delegate to @ref doMesh().
         * @m_deprecated_since{2020,06} Implement @ref doMesh() instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doMesh() instead")
        #endif /* See above */
        virtual Containers::Optional<MeshData2D> doMesh2D(UnsignedInt id);
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Implementation for @ref mesh3DCount()
         *
         * Default implementation returns @ref doMeshCount() for backwards
         * compatibility.
         * @m_deprecated_since{2020,06} Implement @ref doMeshCount() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doMeshCount() instead")
        #endif /* See above */
        virtual UnsignedInt doMesh3DCount() const;

        /**
         * @brief Implementation for @ref mesh3DForName()
         *
         * Default implementation returns @ref doMeshForName() for backwards
         * compatibility.
         * @m_deprecated_since{2020,06} Implement @ref doMeshForName() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doMeshForName() instead")
        #endif /* See above */
        virtual Int doMesh3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref mesh3DName()
         *
         * Default implementation returns @ref doMeshName() for backwards
         * compatibility.
         * @m_deprecated_since{2020,06} Implement @ref doMeshName() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doMeshName() instead")
        #endif /* See above */
        virtual std::string doMesh3DName(UnsignedInt id);

        /**
         * @brief Implementation for @ref mesh3D()
         *
         * Default implementation returns @ref doMesh() converted to
         * @cpp MeshData3D @ce for backwards compatibility.
         * @m_deprecated_since{2020,06} Implement @ref doMesh() instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doMesh() instead")
        #endif /* See above */
        virtual Containers::Optional<MeshData3D> doMesh3D(UnsignedInt id);
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Implementation for @ref materialCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doMaterial() (with correct material count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doMaterialCount() const;

        /**
         * @brief Implementation for @ref materialForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doMaterialForName(const std::string& name);

        /**
         * @brief Implementation for @ref materialName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doMaterialName(UnsignedInt id);

        /** @brief Implementation for @ref material() */
        virtual Containers::Optional<MaterialData> doMaterial(UnsignedInt id);

        /**
         * @brief Implementation for @ref textureCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doTexture() (with correct texture count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doTextureCount() const;

        /**
         * @brief Implementation for @ref textureForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doTextureForName(const std::string& name);

        /**
         * @brief Implementation for @ref textureName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doTextureName(UnsignedInt id);

        /** @brief Implementation for @ref texture() */
        virtual Containers::Optional<TextureData> doTexture(UnsignedInt id);

        /**
         * @brief Implementation for @ref image1DCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doImage1D() (with correct image count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doImage1DCount() const;

        /**
         * @brief Implementation for @ref image1DLevelCount()
         * @m_since{2020,06}
         *
         * Default implementation returns @cpp 1 @ce. See
         * @ref doImage2DLevelCount() for expected implementation behavior.
         */
        virtual UnsignedInt doImage1DLevelCount(UnsignedInt id);

        /**
         * @brief Implementation for @ref image1DForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doImage1DForName(const std::string& name);

        /**
         * @brief Implementation for @ref image1DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doImage1DName(UnsignedInt id);

        /** @brief Implementation for @ref image1D() */
        virtual Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level);

        /**
         * @brief Implementation for @ref image2DCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doImage2D() (with correct image count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doImage2DCount() const;

        /**
         * @brief Implementation for @ref image2DLevelCount()
         * @m_since{2020,06}
         *
         * Default implementation returns @cpp 1 @ce. Similarly to all other
         * `*Count()` functions, this function isn't expected to fail --- if an
         * import error occus, this function should return @cpp 1 @ce and the
         * error state should be returned from @ref image2D() instead; if a
         * file really contains a zero-level image, the implementation should
         * exclude that image from @ref doImage2DCount() instead of returning
         * @cpp 0 @ce here.
         *
         * Deliberately not @cpp const @ce to allow plugins cache decoded
         * data.
         */
        virtual UnsignedInt doImage2DLevelCount(UnsignedInt id);

        /**
         * @brief Implementation for @ref image2DForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doImage2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref image2DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doImage2DName(UnsignedInt id);

        /** @brief Implementation for @ref image2D() */
        virtual Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level);

        /**
         * @brief Implementation for @ref image3DCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferrably during @ref doImage3D() (with correct image count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doImage3DCount() const;

        /**
         * @brief Implementation for @ref image3DLevelCount()
         * @m_since{2020,06}
         *
         * Default implementation returns @cpp 1 @ce. See
         * @ref doImage2DLevelCount() for expected implementation behavior.
         */
        virtual UnsignedInt doImage3DLevelCount(UnsignedInt id);

        /**
         * @brief Implementation for @ref image3DForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doImage3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref image3DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doImage3DName(UnsignedInt id);

        /** @brief Implementation for @ref image3D() */
        virtual Containers::Optional<ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level);

        /** @brief Implementation for @ref importerState() */
        virtual const void* doImporterState() const;

        ImporterFlags _flags;

        Containers::Optional<Containers::ArrayView<const char>>(*_fileCallback)(const std::string&, InputFileCallbackPolicy, void*){};
        void* _fileCallbackUserData{};

        /* Used by the templated version only */
        struct FileCallbackTemplate {
            void(*callback)();
            const void* userData;
        /* GCC 4.8 complains loudly about missing initializers otherwise */
        } _fileCallbackTemplate{nullptr, nullptr};
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class Callback, class T> void AbstractImporter::setFileCallback(Callback callback, T& userData) {
    /* Don't try to wrap a null function pointer. Need to cast first because
       MSVC (even 2017) can't apply ! to a lambda. Ugh. */
    const auto callbackPtr = static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, T&)>(callback);
    if(!callbackPtr) return setFileCallback(nullptr);

    _fileCallbackTemplate = { reinterpret_cast<void(*)()>(callbackPtr), static_cast<const void*>(&userData) };
    setFileCallback([](const std::string& filename, const InputFileCallbackPolicy flags, void* const userData) {
        auto& s = *reinterpret_cast<FileCallbackTemplate*>(userData);
        return reinterpret_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, T&)>(s.callback)(filename, flags, *static_cast<T*>(const_cast<void*>(s.userData)));
    }, &_fileCallbackTemplate);
}
#endif

}}

#endif
