#ifndef Magnum_Trade_AbstractImporter_h
#define Magnum_Trade_AbstractImporter_h
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

/** @file
 * @brief Class @ref Magnum::Trade::AbstractImporter, enum @ref Magnum::Trade::ImporterFeature, enum set @ref Magnum::Trade::ImporterFeatures
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/PluginManager/AbstractManagingPlugin.h>
#include <Corrade/Utility/StlForwardString.h> /** @todo remove once file callbacks are std::string-free */

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* For APIs that used to take or return a std::string */
#include <Corrade/Containers/StringStl.h>
#endif

namespace Magnum { namespace Trade {

/**
@brief Features supported by an importer
@m_since{2020,06}

@see @ref ImporterFeatures, @ref AbstractImporter::features()
*/
enum class ImporterFeature: UnsignedByte {
    /**
     * Opening files from raw data or non-temporary memory using
     * @ref AbstractImporter::openData() or
     * @relativeref{AbstractImporter,openMemory()}
     */
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

/**
@brief Importer flag
@m_since{2020,06}

@see @ref ImporterFlags, @ref AbstractImporter::setFlags()
*/
enum class ImporterFlag: UnsignedByte {
    /**
     * Suppress warnings, print just errors. By default the converter prints
     * both warnings and errors.
     * @m_since_latest
     */
    Quiet = 1 << 1,

    /**
     * Print verbose diagnostic during import. By default the importer only
     * prints messages on error or when some operation might cause unexpected
     * data modification or loss.
     *
     * Corresponds to the `-v` / `--verbose` option in
     * @ref magnum-imageconverter "magnum-imageconverter",
     * @ref magnum-sceneconverter "magnum-sceneconverter" and
     * @ref magnum-player "magnum-player".
     */
    Verbose = 1 << 0,

    /** @todo is warning as error (like in ShaderConverter) usable for anything
        here? in case of a compiler it makes sense, in case of an importer not
        so much probably? it'd also mean expanding each and every Warning
        print (using Error, adding a return) which is a lot to maintain */

    /** @todo ~~Y flip~~ Y up for images, "I want to import just once, don't copy" ... */
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

#ifdef MAGNUM_BUILD_DEPRECATED
namespace Implementation {
    /* Could be a concrete type as only MaterialData need this, but that would
       mean I'd need to include MaterialData here */
    template<class T> struct OptionalButAlsoPointer: Containers::Optional<T> {
        /*implicit*/ OptionalButAlsoPointer() = default;
        /*implicit*/ OptionalButAlsoPointer(Containers::Optional<T>&& optional): Containers::Optional<T>{Utility::move(optional)} {}
        CORRADE_DEPRECATED("use Containers::Optional instead of Containers::Pointer for a MaterialData") /*implicit*/ operator Containers::Pointer<T>() && {
            return Containers::Pointer<T>{new T{Utility::move(**this)}};
        }
    };
}
#endif

/**
@brief Base for importer plugins

Provides interface for importing generic scene data such as images, meshes or
animations.

A scene file can generally have an arbitrary amount of data of particular kind
and so the importer provides a set of @cpp thing(id) @ce accessors, where the
ID is from @cpp 0 @ce to @cpp thingCount() - 1 @ce. Certain kinds of data
can also reference each other via the ID (for example a material specifies the
ID of a texture it uses). The following kinds of data can be imported:

-   @ref AnimationData using @ref animation(UnsignedInt) up to
    @ref animationCount(). Each animation then references the 2D/3D object
    transformation it affects via its ID.
-   @ref CameraData using @ref camera(UnsignedInt) up to @ref cameraCount()
-   @ref ImageData1D / @ref ImageData2D / @ref ImageData3D using
    @ref image1D(UnsignedInt, UnsignedInt) /
    @ref image2D(UnsignedInt, UnsignedInt) /
    @ref image3D(UnsignedInt, UnsignedInt) up to @ref image1DCount() /
    @ref image2DCount() / @ref image3DCount(). Each image can also have
    multiple (mip) levels which are requested through the second parameter up
    to @ref image1DLevelCount() / @ref image2DLevelCount() /
    @ref image3DLevelCount().
-   @ref LightData using @ref light(UnsignedInt) up to @ref lightCount()
-   @ref MaterialData using @ref material(UnsignedInt) up to
    @ref materialCount(). A material can then reference textures via their IDs.
-   @ref MeshData using @ref mesh(UnsignedInt, UnsignedInt) up to
    @ref meshCount(). Similarly as with images, each mesh can also have
    multiple levels (LODs or for example separate edge/face data), which are
    requested through the second parameter up to @ref meshLevelCount().
-   @ref SceneData using @ref scene(UnsignedInt) up to @ref sceneCount(), with
    the default scene index exposed through @ref defaultScene(). A scene then
    contains all data for its objects such as transformations and parent/child
    hierarchy, particular objects are associated with meshes, materials,
    cameras, lights or skins via their IDs.
-   @ref SkinData2D / @ref SkinData3D using @ref skin2D(UnsignedInt) /
    @ref skin3D(UnsignedInt) up to @ref skin2DCount() / @ref skin3DCount()
-   @ref TextureData using @ref texture(UnsignedInt) up to @ref textureCount().
    Each texture then references the 1D/2D/3D image it uses via its ID.

Except for pure image formats that always have at least one image, in general
there's no guarantee that an imported file has always a mesh, a scene or a
camera. So unless the particular importer documentation says otherwise, you're
expected to always check the count before attempting an import.

@section Trade-AbstractImporter-usage Usage

Importers are commonly implemented as plugins, which means the concrete
importer implementation is loaded and instantiated through a @relativeref{Corrade,PluginManager::Manager}. A file is opened using either
@ref openFile(), @ref openData() / @ref openMemory() or, in rare cases,
@ref openState(). Then it stays open until the importer is destroyed,
@ref close() is called or another file is opened.

With a file open you can then query the importer for particular data. Where
possible, the import is performed lazily only when you actually request that
particular data, and thus it can be assumed that opening a file is relatively
cheap.

Since the importers deal with untrusted external data, it's needed to perform
explicit error handling on the application side. There are two cases where it
can fail --- during opening, in which case the function returns @cpp false @ce,
and during the actual data import, in which case you get an empty @relativeref{Corrade,Containers::Optional}. In both cases the actual failure
reason is printed to the error output. Everything else (IDs out of range,
calling functions without a file open, accessing an empty optional, ...) is
treated as a programmer error and will produce the usual assertions.

In the following example an image is loaded from the filesystem using the
@ref AnyImageImporter plugin, completely with all needed error handling:

@snippet Trade.cpp AbstractImporter-usage

See @ref plugins for more information about general plugin usage,
@ref file-formats to compare implementations of common file formats and the
list of @m_class{m-doc} <a href="#derived-classes">derived classes</a> for all
available importer plugins.

@m_class{m-note m-success}

@par
    There's also the @ref magnum-imageconverter "magnum-imageconverter" and
    @ref magnum-sceneconverter "magnum-sceneconverter" tools that you can use
    to perform introspection and conversion of image and scene files.

@subsection Trade-AbstractImporter-usage-callbacks Loading data from memory, using file callbacks

Besides loading data directly from the filesystem using @ref openFile() like
shown above, it's possible to use @ref openData() / @ref openMemory() to import
data from memory (for example from @relativeref{Corrade,Utility::Resource}).
Note that the particular importer implementation has to support
@ref ImporterFeature::OpenData for this method to work:

@snippet Trade.cpp AbstractImporter-usage-data

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

@snippet Trade.cpp AbstractImporter-usage-callbacks

For importers that don't support @ref ImporterFeature::FileCallback directly,
the base @ref openFile() implementation will use the file callback to pass the
loaded data through to @ref openData(), in case the importer supports at least
@ref ImporterFeature::OpenData. If the importer supports neither
@ref ImporterFeature::FileCallback nor @ref ImporterFeature::OpenData,
@ref setFileCallback() doesn't allow the callbacks to be set.

The input file callback signature is the same for @ref Trade::AbstractImporter,
@ref ShaderTools::AbstractConverter and @ref Text::AbstractFont to allow code
reuse.

@subsection Trade-AbstractImporter-usage-name-mapping Mapping between IDs and string names

Certain file formats have the ability to assign string names to objects,
materials and other parts of the scene. These are not imported as part of the
various @ref Trade::MeshData etc. structures, instead the importer itself
allows you to map IDs to names and vice versa. As a convenience feature, it's
also possible to import data directly using their name instead of having to map
the name to an IDs first. In that case the function will return an empty
@relativeref{Corrade,Containers::Optional} both if the import failed or if the
name doesn't exist.

-   Animation names can be retrieved using @ref animationName() and mapped to
    an ID using @ref animationForName(), imported with
    @ref animation(Containers::StringView). Animations can have custom
    animation track targets, for which the name mapping can be retrieved using
    @ref animationTrackTargetName() and @ref animationTrackTargetForName().
-   Camera names using @ref cameraName() & @ref cameraForName(), imported with
    @ref camera(Containers::StringView)
-   Image names using @ref image1DName() / @ref image2DName() /
    @ref image3DName() & @ref image1DForName() / @ref image2DForName() /
    @ref image3DForName(), imported with
    @ref image1D(Containers::StringView, UnsignedInt) /
    @ref image2D(Containers::StringView, UnsignedInt) /
    @ref image3D(Containers::StringView, UnsignedInt)
-   Light names using @ref lightName() & @ref lightForName(), imported with
    @ref light(Containers::StringView)
-   Material names using @ref materialName() & @ref materialForName(), imported
    with @ref material(Containers::StringView)
-   Mesh names using @ref meshName() & @ref meshForName(), imported with
    @ref mesh(Containers::StringView, UnsignedInt). Meshes themselves can have
    custom attributes, for which the name mapping can be retrieved using
    @ref meshAttributeName() and @ref meshAttributeForName().
-   Scene and object names using @ref sceneName() / @ref objectName() &
    @ref sceneForName() / @ref objectForName(), imported with
    @ref scene(Containers::StringView). Scenes themselves can have custom
    fields, for which the name mapping can be retrieved using
    @ref sceneFieldName() and @ref sceneFieldForName().
-   Skin names using @ref skin2DName() / @ref skin3DName() &
    @ref skin2DForName() / @ref skin3DForName(), imported with
    @ref skin2D(Containers::StringView) / @ref skin3D(Containers::StringView)
-   Texture names using @ref textureName() & @ref textureForName(), imported
    with @ref texture(Containers::StringView)

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
-   @ref SceneData::importerState() can expose importer state for a scene
    imported by @ref scene(), per-object importer state can then be stored in
    the @ref SceneField::ImporterState field
-   @ref SkinData::importerState() can expose importer state for a scene
    imported by @ref skin2D() or @ref skin3D()
-   @ref TextureData::importerState() can expose importer state for a texture
    imported by @ref texture()

Besides exposing internal state, importers that support the
@ref ImporterFeature::OpenState feature can also attach to existing importer
state using @ref openState(). See documentation of a particular importer for
details about concrete types returned and accepted by these functions.

@section Trade-AbstractImporter-data-dependency Data dependency

The `*Data` instances returned from various functions *by design* have no
dependency on the importer instance and neither on the dynamic plugin module.
In other words, you don't need to keep the importer instance (or the plugin
manager instance) around in order to have the `*Data` instances valid.
Moreover, all returned @ref Corrade::Containers::String instances and all @relativeref{Corrade,Containers::Array} instances returned through
@ref SceneData, @ref ImageData, @ref AnimationData, @ref MaterialData,
@ref MeshData and @ref SkinData are only allowed to have default deleters (or
be non-owning instances created from
@relativeref{Corrade,Containers::ArrayView}) --- this is to avoid potential
dangling function pointer calls when destructing such instances after the
plugin module has been unloaded.

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
@ref doSetFileCallback() can be overridden in case it's desired to respond to
file loading callback setup, but doesn't have to be.

For multi-data formats the file opening shouldn't take long and all parsing
should be done in the data parsing functions instead, because the user might
want to import only some data. This is obviously not the case for single-data
formats like images, as the file contains all the data the user wants to
import.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The @ref doOpenData(), @ref doOpenFile() and @ref doOpenState() functions
    are called after the previous file was closed, @ref doClose() is called
    only if there is any file opened.
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
    @relativeref{Corrade,Containers::Array} instances returned from plugin
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
         * @snippet Magnum/Trade/AbstractImporter.h interface
         *
         * @see @ref MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE
         */
        static Containers::StringView pluginInterface();

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
        static Containers::Array<Containers::String> pluginSearchPaths();
        #endif

        /** @brief Default constructor */
        explicit AbstractImporter();

        /** @brief Constructor with access to plugin manager */
        explicit AbstractImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        /* The plugin name is passed as a const& to make it possible for people
           to implement plugins without even having to include the StringView
           header. */
        explicit AbstractImporter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin);

        #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(DOXYGEN_GENERATING_OUTPUT)
        /* These twp needed because of the Array<CachedScenes> member
           (AnyImageImporter relies on the move), move assignment disabled by
           AbstractPlugin already */
        AbstractImporter(AbstractImporter&&) noexcept;
        ~AbstractImporter();
        #endif

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
         * more information. By default no flags are set. To avoid clearing
         * potential future default flags by accident, prefer to use
         * @ref addFlags() and @ref clearFlags() instead.
         *
         * Corresponds to the `-v` / `--verbose` option in
         * @ref magnum-imageconverter "magnum-imageconverter",
         * @ref magnum-sceneconverter "magnum-sceneconverter" and
         * @ref magnum-player "magnum-player".
         */
        void setFlags(ImporterFlags flags);

        /**
         * @brief Add importer flags
         * @m_since_latest
         *
         * Calls @ref setFlags() with the existing flags ORed with @p flags.
         * Useful for preserving the defaults.
         * @see @ref clearFlags()
         */
        void addFlags(ImporterFlags flags);

        /**
         * @brief Clear importer flags
         * @m_since_latest
         *
         * Calls @ref setFlags() with the existing flags ANDed with inverse of
         * @p flags. Useful for removing default flags.
         * @see @ref addFlags()
         */
        void clearFlags(ImporterFlags flags);

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
         * @snippet Trade.cpp AbstractImporter-setFileCallback
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
         * @snippet Trade.cpp AbstractImporter-setFileCallback-template
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

        /**
         * @brief Whether any file is opened
         *
         * Returns @cpp true @ce if a file is opened with @ref openData(),
         * @ref openMemory(), @ref openState() or @ref openFile() and
         * @ref close() wasn't called yet; @cpp false @ce otherwise.
         */
        bool isOpened() const { return doIsOpened(); }

        /**
         * @brief Open raw data
         *
         * Closes previous file, if it was opened, and tries to open given raw
         * data. Available only if @ref ImporterFeature::OpenData is supported.
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @cpp false @ce.
         *
         * The @p data is not expected to be alive after the function exits.
         * Using @ref openMemory() instead as can avoid unnecessary copies in
         * exchange for stricter requirements on @p data lifetime.
         * @see @ref features(), @ref openFile(), @ref openState()
         */
        bool openData(Containers::ArrayView<const void> data);

        /**
         * @brief Open a non-temporary memory
         * @m_since_latest
         *
         * Closes previous file, if it was opened, and tries to open given raw
         * data. Available only if @ref ImporterFeature::OpenData is supported.
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @cpp false @ce.
         *
         * Unlike @ref openData(), this function expects @p memory to stay in
         * scope until the importer is destructed, @ref close() is called or
         * another file is opened. This allows the implementation to directly
         * operate on the provided memory, without having to allocate a local
         * copy to extend its lifetime.
         * @see @ref features(), @ref openFile(), @ref openState()
         */
        bool openMemory(Containers::ArrayView<const void> memory);

        /**
         * @brief Open already loaded state
         * @param state     Pointer to importer-specific state
         * @param filePath  Base file directory for opening external data like
         *      textures and materials.
         *
         * Closes previous file, if it was opened, and tries to open given
         * state. Available only if @ref ImporterFeature::OpenState is
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @cpp false @ce.
         *
         * See documentation of a particular plugin for more information about
         * type and contents of the @p state parameter.
         * @see @ref features(), @ref openData(), @ref openMemory(),
         *      @ref openFile()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        bool openState(const void* state, Containers::StringView filePath = {});
        #else
        bool openState(const void* state, Containers::StringView filePath);
        bool openState(const void* state);
        #endif

        /**
         * @brief Open a file
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. On failure prints a message to @relativeref{Magnum,Error} and
         * returns @cpp false @ce. If file loading callbacks are set via
         * @ref setFileCallback() and @ref ImporterFeature::OpenData is
         * supported, this function uses the callback to load the file and
         * passes the memory view to @ref openData() instead. See
         * @ref setFileCallback() for more information.
         * @see @ref features(), @ref openData(), @ref openMemory(),
         *      @ref openState()
         */
        bool openFile(Containers::StringView filename);

        /**
         * @brief Close currently opened file
         *
         * On certain implementations an explicit call to this function when
         * the file is no longer needed but the importer is going to be reused
         * further may result in freed memory. This call is also done
         * automatically when the importer gets destructed or when another file
         * is opened. If no file is opened, does nothing. After this function
         * is called, @ref isOpened() is guaranteed to return @cpp false @ce.
         */
        void close();

        /** @{ @name Data accessors
         * Each function tuple provides access to given data.
         */

        /**
         * @brief Default scene
         * @return Scene ID from range [0, @ref sceneCount()) or @cpp -1 @ce if
         *      there's no default scene
         *
         * Expects that a file is opened.
         */
        Int defaultScene() const;

        /**
         * @brief Scene count
         *
         * Expects that a file is opened.
         */
        UnsignedInt sceneCount() const;

        /**
         * @brief Object count
         *
         * Total count of all (2D or 3D) objects in all scenes. An object can
         * be present in multiple scenes at the same time. Fields corresponding
         * to particular objects can be then accessed via the @ref SceneData
         * class returned from @ref scene(UnsignedInt). Expects that a file is
         * opened.
         */
        UnsignedLong objectCount() const;

        /**
         * @brief Scene ID for given name
         * @return Scene ID from range [0, @ref sceneCount()) or @cpp -1 @ce if
         *      no scene for given name exists
         *
         * Expects that a file is opened.
         * @see @ref sceneName(), @ref scene(Containers::StringView)
         */
        Int sceneForName(Containers::StringView name);

        /**
         * @brief Object ID for given name
         *
         * If no object for given name exists, returns @cpp -1 @ce. Expects
         * that a file is opened. Object IDs are shared among all scenes, an
         * object can be present in multiple scenes at the same time.
         * @see @ref objectName()
         */
        Long objectForName(Containers::StringView name);

        /**
         * @brief Scene name
         * @param id        Scene ID, from range [0, @ref sceneCount()).
         *
         * If the scene has no name or the importer doesn't support scene
         * names, returns an empty string. Expects that a file is opened.
         * @see @ref sceneForName()
         */
        Containers::String sceneName(UnsignedInt id);

        /**
         * @brief Object name
         * @param id        Object ID, from range [0, @ref objectCount()).
         *
         * Object IDs are shared among all scenes, an object can be present in
         * multiple scenes at the same time. If the object has no name or the
         * importer doesn't support object names, returns an empty string.
         * Expects that a file is opened.
         * @see @ref objectForName()
         */
        Containers::String objectName(UnsignedLong id);

        /**
         * @brief Scene
         * @param id        Scene ID, from range [0, @ref sceneCount()).
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref scene(Containers::StringView), @ref sceneName(),
         *      @ref objectName(), @ref sceneFieldName()
         */
        Containers::Optional<SceneData> scene(UnsignedInt id);

        /**
         * @brief Scene for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref sceneForName() and
         * @ref scene(UnsignedInt). If @ref sceneForName() returns @cpp -1 @ce,
         * prints a message to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}, otherwise propagates the
         * result from @ref scene(UnsignedInt). Expects that a file is opened.
         */
        Containers::Optional<SceneData> scene(Containers::StringView name);

        /**
         * @brief Scene field for given name
         * @m_since_latest
         *
         * If the name is not recognized, returns a zero (invalid)
         * @ref SceneField, otherwise returns a custom scene field. Note that
         * the value returned by this function may depend on whether a file is
         * opened or not and also be different for different files --- see
         * documentation of a particular importer for more information.
         * @see @ref sceneFieldName(), @ref isSceneFieldCustom()
         */
        SceneField sceneFieldForName(Containers::StringView name);

        /**
         * @brief String name for given custom scene field
         * @m_since_latest
         *
         * Given a custom @p name returned by @ref scene() in a @ref SceneData,
         * returns a string identifier. If a string representation is not
         * available or @p name is not recognized, returns an empty string.
         * Expects that @p name is custom. Note that the value returned by
         * this function may depend on whether a file is opened or not and also
         * be different for different files --- see documentation of a
         * particular importer for more information.
         * @see @ref sceneFieldForName(), @ref isSceneFieldCustom()
         */
        Containers::String sceneFieldName(SceneField name);

        /**
         * @brief Animation count
         *
         * Expects that a file is opened.
         */
        UnsignedInt animationCount() const;

        /**
         * @brief Animation for given name
         * @return Animation ID from range [0, @ref animationCount()) or
         *      @cpp -1 @ce if no animation for given name exists
         *
         * Expects that a file is opened.
         * @see @ref animationName(), @ref animation(Containers::StringView)
         */
        Int animationForName(Containers::StringView name);

        /**
         * @brief Animation name
         * @param id    Animation ID, from range [0, @ref animationCount()).
         *
         * Expects that a file is opened. If the animation has no name or the
         * importer doesn't support animation names, returns an empty string.
         * @see @ref animationForName()
         */
        Containers::String animationName(UnsignedInt id);

        /**
         * @brief Animation
         * @param id    Animation ID, from range [0, @ref animationCount()).
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref animation(Containers::StringView), @ref animationName(),
         *      @ref animationTrackTargetName()
         */
        Containers::Optional<AnimationData> animation(UnsignedInt id);

        /**
         * @brief Animation for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref animationForName() and
         * @ref animation(UnsignedInt). If @ref animationForName() returns
         * @cpp -1 @ce, prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref animation(UnsignedInt). Expects that
         * a file is opened.
         */
        Containers::Optional<AnimationData> animation(Containers::StringView name);

        /**
         * @brief Animation track target for given name
         * @m_since_latest
         *
         * If the @p name is not recognized, returns a zero (invalid)
         * @ref AnimationTrackTarget, otherwise returns a custom animation
         * track target. Note that the value returned by this function may
         * depend on whether a file is opened or not and also be different for
         * different files --- see documentation of a particular importer for
         * more information.
         * @see @ref animationTrackTargetName(),
         *      @ref isAnimationTrackTargetCustom()
         */
        AnimationTrackTarget animationTrackTargetForName(Containers::StringView name);

        /**
         * @brief String name for given custom animation track target
         * @m_since_latest
         *
         * Given a custom @p name returned by @ref animation() in an
         * @ref AnimationData, returns a string identifier. If a string
         * representation is not available or @p name is not recognized,
         * returns an empty string. Expects that @p name is custom. Note that
         * the value returned by this function may depend on whether a file is
         * opened or not and also be different for different files --- see
         * documentation of a particular importer for more information.
         * @see @ref animationTrackTargetForName(),
         *      @ref isAnimationTrackTargetCustom()
         */
        Containers::String animationTrackTargetName(AnimationTrackTarget name);

        /**
         * @brief Light count
         *
         * Expects that a file is opened.
         */
        UnsignedInt lightCount() const;

        /**
         * @brief Light for given name
         * @return Light ID from range [0, @ref lightCount()) or @cpp -1 @ce if
         *      no light for given name exists
         *
         * Expects that a file is opened.
         * @see @ref lightName(), @ref light(Containers::StringView)
         */
        Int lightForName(Containers::StringView name);

        /**
         * @brief Light name
         * @param id        Light ID, from range [0, @ref lightCount()).
         *
         * Expects that a file is opened. If the light has no name or the
         * importer doesn't support light names, returns an empty string.
         * @see @ref lightForName()
         */
        Containers::String lightName(UnsignedInt id);

        /**
         * @brief Light
         * @param id        Light ID, from range [0, @ref lightCount()).
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref light(Containers::StringView)
         */
        Containers::Optional<LightData> light(UnsignedInt id);

        /**
         * @brief Light for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref lightForName() and
         * @ref light(UnsignedInt). If @ref lightForName() returns @cpp -1 @ce,
         * prints a message to @relativeref{Magnum,Error} and returns
         * @relativeref{Corrade,Containers::NullOpt}, otherwise propagates the
         * result from @ref light(UnsignedInt). Expects that a file is opened.
         */
        Containers::Optional<LightData> light(Containers::StringView name);

        /**
         * @brief Camera count
         *
         * Expects that a file is opened.
         */
        UnsignedInt cameraCount() const;

        /**
         * @brief Camera for given name
         * @return Camera ID from range [0, @ref cameraCount()) or @cpp -1 @ce
         *      if no camera for given name exists
         *
         * Expects that a file is opened.
         * @see @ref cameraName(), @ref camera(Containers::StringView)
         */
        Int cameraForName(Containers::StringView name);

        /**
         * @brief Camera name
         * @param id        Camera ID, from range [0, @ref cameraCount()).
         *
         * Expects that a file is opened. If the camera has no name or the
         * importer doesn't support camera names, returns an empty string.
         * @see @ref cameraForName()
         */
        Containers::String cameraName(UnsignedInt id);

        /**
         * @brief Camera
         * @param id        Camera ID, from range [0, @ref cameraCount()).
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref camera(Containers::StringView)
         */
        Containers::Optional<CameraData> camera(UnsignedInt id);

        /**
         * @brief Camera for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref cameraForName() and
         * @ref camera(UnsignedInt). If @ref cameraForName() returns
         * @cpp -1 @ce, prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref camera(UnsignedInt). Expects that a
         * file is opened.
         */
        Containers::Optional<CameraData> camera(Containers::StringView name);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Two-dimensional object count
         *
         * Expects that a file is opened.
         * @m_deprecated_since_latest Use @ref objectCount() instead, which is
         *      shared for both 2D and 3D objects.
         */
        CORRADE_DEPRECATED("use objectCount() instead") UnsignedInt object2DCount() const;

        /**
         * @brief Two-dimensional object for given name
         * @return Object ID from range [0, @ref object2DCount()) or
         *      @cpp -1 @ce if no object for given name exists
         *
         * Expects that a file is opened.
         * @m_deprecated_since_latest Use @ref objectForName() instead, which
         *      is shared for both 2D and 3D objects.
         * @see @ref object2DName(), @ref object2D(const std::string&)
         */
        CORRADE_DEPRECATED("use objectForName() instead") Int object2DForName(const std::string& name);

        /**
         * @brief Two-dimensional object name
         * @param id        Object ID, from range [0, @ref object2DCount()).
         *
         * Expects that a file is opened. If the object has no name or the
         * importer doesn't support object names, returns an empty string.
         * @m_deprecated_since_latest Use @ref objectName() instead, which is
         *      shared for both 2D and 3D objects.
         * @see @ref object2DForName()
         */
        CORRADE_DEPRECATED("use objectName() instead") std::string object2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional object
         * @param id        Object ID, from range [0, @ref object2DCount()).
         *
         * Returns given object or @cpp nullptr @ce if importing failed.
         * Expects that a file is opened.
         * @m_deprecated_since_latest Query object fields on the @ref SceneData
         *      object returned from @ref scene() instead, which is shared for
         *      both 2D and 3D objects.
         * @see @ref object2D(const std::string&)
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        CORRADE_DEPRECATED("query object fields from scene() instead") Containers::Pointer<ObjectData2D> object2D(UnsignedInt id);
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Two-dimensional object for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref object2DForName() and
         * @ref object2D(UnsignedInt). If @ref object2DForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @cpp nullptr @ce, otherwise propagates the result from
         * @ref object2D(UnsignedInt). Expects that a file is opened.
         * @m_deprecated_since_latest Query object fields on the @ref SceneData
         *      object returned from @ref scene() instead, which is shared for
         *      both 2D and 3D objects.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        CORRADE_DEPRECATED("query object fields from scene() instead") Containers::Pointer<ObjectData2D> object2D(const std::string& name);
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Three-dimensional object count
         *
         * Expects that a file is opened.
         * @m_deprecated_since_latest Use @ref objectCount() instead, which is
         *      shared for both 2D and 3D objects.
         */
        CORRADE_DEPRECATED("use objectCount() instead") UnsignedInt object3DCount() const;

        /**
         * @brief Three-dimensional object for given name
         * @return Object ID from range [0, @ref object3DCount()) or
         *      @cpp -1 @ce if no object for given name exists
         *
         * Expects that a file is opened.
         * @m_deprecated_since_latest Use @ref objectForName() instead, which
         *      is shared for both 2D and 3D objects.
         * @see @ref object3DName(), @ref object3D(const std::string&)
         */
        CORRADE_DEPRECATED("use objectForName() instead") Int object3DForName(const std::string& name);

        /**
         * @brief Three-dimensional object name
         * @param id        Object ID, from range [0, @ref object3DCount()).
         *
         * Expects that a file is opened. If the object has no name or the
         * importer doesn't support object names, returns an empty string.
         * @m_deprecated_since_latest Use @ref objectName() instead, which is
         *      shared for both 2D and 3D objects.
         * @see @ref object3DForName()
         */
        CORRADE_DEPRECATED("use objectName() instead") std::string object3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional object
         * @param id        Object ID, from range [0, @ref object3DCount()).
         *
         * Returns given object or @cpp nullptr @ce if importing failed.
         * Expects that a file is opened.
         * @m_deprecated_since_latest Query object fields on the @ref SceneData
         *      object returned from @ref scene() instead, which is shared for
         *      both 2D and 3D objects.
         * @see @ref object3D(const std::string&)
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        CORRADE_DEPRECATED("query object fields from scene() instead") Containers::Pointer<ObjectData3D> object3D(UnsignedInt id);
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Three-dimensional object for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref object3DForName() and
         * @ref object3D(UnsignedInt). If @ref object3DForName() returns
         * @cpp -1 @ce, prints an error message and returns
         * @cpp nullptr @ce, otherwise propagates the result from
         * @ref object3D(UnsignedInt). Expects that a file is opened.
         * @m_deprecated_since_latest Query object fields on the @ref SceneData
         *      object returned from @ref scene() instead, which is shared for
         *      both 2D and 3D objects.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        CORRADE_DEPRECATED("query object fields from scene() instead") Containers::Pointer<ObjectData3D> object3D(const std::string& name);
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Two-dimensional skin count
         * @m_since_latest
         *
         * Expects that a file is opened.
         */
        UnsignedInt skin2DCount() const;

        /**
         * @brief Two-dimensional skin for given name
         * @return Skin ID from range [0, @ref skin2DCount()) or @cpp -1 @ce if
         *      no skin for given name exists
         * @m_since_latest
         *
         * Expects that a file is opened.
         * @see @ref skin2DName(), @ref skin2D(Containers::StringView)
         */
        Int skin2DForName(Containers::StringView name);

        /**
         * @brief Two-dimensional skin name
         * @param id        Skin ID, from range [0, @ref skin2DCount()).
         * @m_since_latest
         *
         * If the skin has no name or the importer doesn't support skin names,
         * returns an empty string. Expects that a file is opened.
         * @see @ref skin2DForName()
         */
        Containers::String skin2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional skin
         * @param id        Skin ID, from range [0, @ref skin2DCount()).
         * @m_since_latest
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref skin2D(Containers::StringView), @ref skin2DName()
         */
        Containers::Optional<SkinData2D> skin2D(UnsignedInt id);

        /**
         * @brief Two-dimensional skin for given name
         * @m_since_latest
         *
         * A convenience API combining @ref skin2DForName() and
         * @ref skin2D(UnsignedInt). If @ref skin2DForName() returns
         * @cpp -1 @ce, prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref skin2D(UnsignedInt). Expects that a
         * file is opened.
         */
        Containers::Optional<SkinData2D> skin2D(Containers::StringView name);

        /**
         * @brief Three-dimensional skin count
         * @m_since_latest
         *
         * Expects that a file is opened.
         */
        UnsignedInt skin3DCount() const;

        /**
         * @brief Three-dimensional skin for given name
         * @return Skin ID from range [0, @ref skin3DCount()) or @cpp -1 @ce if
         *      no skin for given name exists
         * @m_since_latest
         *
         * Expects that a file is opened.
         * @see @ref skin3DName(), @ref skin3D(Containers::StringView)
         */
        Int skin3DForName(Containers::StringView name);

        /**
         * @brief Three-dimensional skin name
         * @param id        Skin ID, from range [0, @ref skin3DCount()).
         * @m_since_latest
         *
         * If the skin has no name or the importer doesn't support skin names,
         * returns an empty string. Expects that a file is opened.
         * @see @ref skin3DForName()
         */
        Containers::String skin3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional skin
         * @param id        Skin ID, from range [0, @ref skin3DCount()).
         * @m_since_latest
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref skin3D(Containers::StringView), @ref skin3DName()
         */
        Containers::Optional<SkinData3D> skin3D(UnsignedInt id);

        /**
         * @brief Three-dimensional object for given name
         * @m_since_latest
         *
         * A convenience API combining @ref skin3DForName() and
         * @ref skin3D(UnsignedInt). If @ref skin3DForName() returns
         * @cpp -1 @ce, prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref skin3D(UnsignedInt). Expects that a
         * file is opened.
         */
        Containers::Optional<SkinData3D> skin3D(Containers::StringView name);

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
         * @brief Mesh for given name
         * @return Mesh ID from range [0, @ref meshCount()) or @cpp -1 @ce if
         *      no mesh for given name exists
         * @m_since{2020,06}
         *
         * Expects that a file is opened.
         * @see @ref meshName(), @ref mesh(Containers::StringView, UnsignedInt)
         */
        Int meshForName(Containers::StringView name);

        /**
         * @brief Mesh name
         * @param id        Mesh ID, from range [0, @ref meshCount()).
         * @m_since{2020,06}
         *
         * If the mesh has no name or the importer doesn't support mesh names,
         * returns an empty string. Expects that a file is opened.
         * @see @ref meshForName()
         */
        Containers::String meshName(UnsignedInt id);

        /**
         * @brief Mesh
         * @param id        Mesh ID, from range [0, @ref meshCount()).
         * @param level     Mesh level, from range [0, @ref meshLevelCount())
         * @m_since{2020,06}
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. The @p level
         * parameter allows access to additional data and is largely left as
         * importer-specific --- for example allowing access to per-instance,
         * per-face or per-edge data. Expects that a file is opened.
         * @see @ref mesh(Containers::StringView, UnsignedInt),
         *      @ref MeshPrimitive::Instances, @ref MeshPrimitive::Faces,
         *      @ref MeshPrimitive::Edges, @ref meshName(),
         *      @ref meshAttributeName()
         */
        Containers::Optional<MeshData> mesh(UnsignedInt id, UnsignedInt level = 0);

        /**
         * @brief Mesh for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref meshForName() and
         * @ref mesh(UnsignedInt, UnsignedInt). If @ref meshForName() returns
         * @cpp -1 @ce, prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref mesh(UnsignedInt, UnsignedInt).
         * Expects that a file is opened.
         */
        Containers::Optional<MeshData> mesh(Containers::StringView name, UnsignedInt level = 0);

        /**
         * @brief Mesh attribute for given name
         * @m_since{2020,06}
         *
         * If the @p name is not recognized, returns a zero (invalid)
         * @ref MeshAttribute, otherwise returns a custom mesh attribute. Note
         * that the value returned by this function may depend on whether a
         * file is opened or not and also be different for different files ---
         * see documentation of a particular importer for more information.
         * @see @ref meshAttributeName(), @ref isMeshAttributeCustom()
         */
        MeshAttribute meshAttributeForName(Containers::StringView name);

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
         * @see @ref meshAttributeForName(), @ref isMeshAttributeCustom()
         */
        Containers::String meshAttributeName(MeshAttribute name);

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
         * Returns given mesh or @relativeref{Corrade,Containers::NullOpt} if
         * importing failed. Expects that a file is opened.
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
         * Returns given mesh or @relativeref{Corrade,Containers::NullOpt} if
         * importing failed. Expects that a file is opened.
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
         * @brief Material for given name
         * @return Material ID from range [0, @ref materialCount()) or
         *      @cpp -1 @ce if no material for given name exists
         *
         * Expects that a file is opened.
         * @see @ref materialName()
         */
        Int materialForName(Containers::StringView name);

        /**
         * @brief Material name
         * @param id        Material ID, from range [0, @ref materialCount()).
         *
         * If the material has no name or the importer doesn't support material
         * names, returns an empty string. Expects that a file is opened.
         * @see @ref materialForName(), @ref material(Containers::StringView)
         */
        Containers::String materialName(UnsignedInt id);

        /**
         * @brief Material
         * @param id        Material ID, from range [0, @ref materialCount()).
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref material(Containers::StringView), @ref materialName()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<MaterialData>
        #else
        Implementation::OptionalButAlsoPointer<MaterialData>
        #endif
        material(UnsignedInt id);

        /**
         * @brief Material for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref materialForName() and
         * @ref material(UnsignedInt). If @ref materialForName() returns
         * @cpp -1 @ce, prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref material(UnsignedInt). Expects that
         * a file is opened.
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<MaterialData>
        #else
        Implementation::OptionalButAlsoPointer<MaterialData>
        #endif
        material(Containers::StringView name);

        /**
         * @brief Texture count
         *
         * Expects that a file is opened.
         */
        UnsignedInt textureCount() const;

        /**
         * @brief Texture for given name
         * @return Texture ID from range [0, @ref textureCount()) or
         *      @cpp -1 @ce if no texture for given name exists
         *
         * Expects that a file is opened.
         * @see @ref textureName(), @ref texture(Containers::StringView)
         */
        Int textureForName(Containers::StringView name);

        /**
         * @brief Texture name
         * @param id        Texture ID, from range [0, @ref textureCount()).
         *
         * If the texture has no name or the importer doesn't support texture
         * names, returns an empty string. Expects that a file is opened.
         * @see @ref textureForName()
         */
        Containers::String textureName(UnsignedInt id);

        /**
         * @brief Texture
         * @param id        Texture ID, from range [0, @ref textureCount()).
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref texture(Containers::StringView), @ref textureName()
         */
        Containers::Optional<TextureData> texture(UnsignedInt id);

        /**
         * @brief Texture for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref textureForName() and
         * @ref texture(UnsignedInt). If @ref textureForName() returns
         * @cpp -1 @ce, prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref texture(UnsignedInt). Expects that a
         * file is opened.
         */
        Containers::Optional<TextureData> texture(Containers::StringView name);

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
         * @brief One-dimensional image for given name
         * @return Image ID from range [0, @ref image1DCount()) or @cpp -1 @ce
         *      if no image for given name exists
         *
         * Expects that a file is opened.
         * @see @ref image1DName(), @ref image1D(Containers::StringView, UnsignedInt)
         */
        Int image1DForName(Containers::StringView name);

        /**
         * @brief One-dimensional image name
         * @param id        Image ID, from range [0, @ref image1DCount()).
         *
         * If the image has no name or the importer doesn't support image
         * names, returns an empty string. Expects that a file is opened.
         * @see @ref image1DForName()
         */
        Containers::String image1DName(UnsignedInt id);

        /**
         * @brief One-dimensional image
         * @param id        Image ID, from range [0, @ref image1DCount()).
         * @param level     Mip level, from range [0, @ref image1DLevelCount())
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref image1D(Containers::StringView, UnsignedInt),
         *      @ref image1DName()
         */
        Containers::Optional<ImageData1D> image1D(UnsignedInt id, UnsignedInt level = 0);

        /**
         * @brief One-dimensional image for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref image1DForName() and
         * @ref image1D(UnsignedInt, UnsignedInt). If @ref image1DForName()
         * returns @cpp -1 @ce, prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref image1D(UnsignedInt, UnsignedInt).
         * Expects that a file is opened.
         */
        Containers::Optional<ImageData1D> image1D(Containers::StringView name, UnsignedInt level = 0);

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
         * @brief Two-dimensional image for given name
         * @return Image ID from range [0, @ref image2DCount()) or @cpp -1 @ce
         *      if no image for given name exists
         *
         * Expects that a file is opened.
         * @see @ref image2DName(), @ref image2D(Containers::StringView, UnsignedInt)
         */
        Int image2DForName(Containers::StringView name);

        /**
         * @brief Two-dimensional image name
         * @param id        Image ID, from range [0, @ref image2DCount()).
         *
         * If the image has no name or the importer doesn't support image
         * names, returns an empty string. Expects that a file is opened.
         * @see @ref image2DForName()
         */
        Containers::String image2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional image
         * @param id        Image ID, from range [0, @ref image2DCount()).
         * @param level     Mip level, from range [0, @ref image2DLevelCount())
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref image2D(Containers::StringView, UnsignedInt),
         *      @ref image2DName()
         */
        Containers::Optional<ImageData2D> image2D(UnsignedInt id, UnsignedInt level = 0);

        /**
         * @brief Two-dimensional image for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref image2DForName() and
         * @ref image2D(UnsignedInt, UnsignedInt). If @ref image2DForName()
         * returns @cpp -1 @ce, prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref image2D(UnsignedInt, UnsignedInt).
         * Expects that a file is opened.
         */
        Containers::Optional<ImageData2D> image2D(Containers::StringView name, UnsignedInt level = 0);

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
         * @brief Three-dimensional image for given name
         * @return Image ID from range [0, @ref image3DCount()) or @cpp -1 @ce
         *      if no image for given name exists
         *
         * Expects that a file is opened.
         * @see @ref image3DName(), @ref image3D(Containers::StringView, UnsignedInt)
         */
        Int image3DForName(Containers::StringView name);

        /**
         * @brief Three-dimensional image name
         * @param id        Image ID, from range [0, @ref image3DCount()).
         *
         * If the image has no name or the importer doesn't support image
         * names, returns an empty string. Expects that a file is opened.
         * @see @ref image3DForName()
         */
        Containers::String image3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional image
         * @param id        Image ID, from range [0, @ref image3DCount()).
         * @param level     Mip level, from range [0, @ref image3DLevelCount())
         *
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @relativeref{Corrade,Containers::NullOpt}. Expects that a
         * file is opened.
         * @see @ref image3D(Containers::StringView, UnsignedInt),
         *      @ref image3DName()
         */
        Containers::Optional<ImageData3D> image3D(UnsignedInt id, UnsignedInt level = 0);

        /**
         * @brief Three-dimensional image for given name
         * @m_since{2020,06}
         *
         * A convenience API combining @ref image3DForName() and
         * @ref image3D(UnsignedInt, UnsignedInt). If @ref image3DForName()
         * returns @cpp -1 @ce, prints a message to @relativeref{Magnum,Error}
         * and returns @relativeref{Corrade,Containers::NullOpt}, otherwise
         * propagates the result from @ref image3D(UnsignedInt, UnsignedInt).
         * Expects that a file is opened.
         */
        Containers::Optional<ImageData3D> image3D(Containers::StringView name, UnsignedInt level = 0);

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
         * @see @ref MaterialData::importerState(),
         *      @ref AnimationData::importerState(),
         *      @ref CameraData::importerState(),
         *      @ref ImageData::importerState(),
         *      @ref LightData::importerState(),
         *      @ref MeshData::importerState(),
         *      @ref SceneData::importerState(),
         *      @ref SkinData2D::importerState(),
         *      @ref SkinData3D::importerState(),
         *      @ref TextureData::importerState()
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
        virtual void doOpenFile(Containers::StringView filename);

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

        /**
         * @brief Implementation for @ref openData() and @ref openMemory()
         * @m_since_latest
         *
         * The @p data is mutable or owned depending on the value of
         * @p dataFlags. This can be used for example to avoid allocating a
         * local copy in order to preserve its lifetime. The following cases
         * are possible:
         *
         * -    If @p dataFlags is empty, @p data is a @cpp const @ce
         *      non-owning view. This happens when the function is called from
         *      @ref openData(). You have to assume the data go out of scope
         *      after the function exists, so if the importer needs to access
         *      the data after, it has to allocate a local copy.
         * -    If @p dataFlags is @ref DataFlag::Owned and
         *      @ref DataFlag::Mutable, @p data is an owned memory. This
         *      happens when the implementation is called from the default
         *      @ref doOpenFile() implementation --- it reads a file into a
         *      newly allocated array and passes it to this function. You can
         *      take ownership of the @p data instance instead of allocating a
         *      local copy.
         * -    If @p dataFlags is @ref DataFlag::ExternallyOwned, it can be
         *      assumed that @p data will stay in scope until @ref doClose() is
         *      called or the importer is destructed. This happens when the
         *      function is called from @ref openMemory().
         *
         * Example workflow in a plugin that needs to preserve access to the
         * input data but wants to avoid allocating a copy if possible:
         *
         * @snippet Trade.cpp AbstractImporter-doOpenData-ownership
         *
         * The @p dataFlags can never be @ref DataFlag::Mutable without
         * any other flag set. The case of @ref DataFlag::Mutable with
         * @ref DataFlag::ExternallyOwned is currently unused but reserved for
         * future.
         */
        virtual void doOpenData(Containers::Array<char>&& data, DataFlags dataFlags);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Implementation for @ref openData()
         * @m_deprecated_since_latest Implement @ref doOpenData(Containers::Array<char>&&, DataFlags)
         *      instead.
         */
        /* MSVC warns when overriding such methods and there's no way to
           suppress that warning, making the RT build (which treats deprecation
           warnings as errors) fail and other builds extremely noisy. So
           disabling those on MSVC. */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doOpenData(Containers::Array<char>&&, DataFlags) instead")
        #endif
        virtual void doOpenData(Containers::ArrayView<const char> data);
        #endif

        /** @brief Implementation for @ref openState() */
        virtual void doOpenState(const void* state, Containers::StringView filePath);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /**
         * @brief Implementation for @ref defaultScene()
         *
         * Default implementation returns @cpp -1 @ce. This function isn't
         * expected to fail --- if an import error occus (for example because
         * the default scene index is out of range), it should be handled
         * already during file opening.
         */
        virtual Int doDefaultScene() const;

        /**
         * @brief Implementation for @ref sceneCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doScene() (with correct scene count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doSceneCount() const;

        /**
         * @brief Implementation for @ref objectCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doScene() (with correct object count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedLong doObjectCount() const;

        /**
         * @brief Implementation for @ref sceneForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doSceneForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref objectForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Long doObjectForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref sceneName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doSceneName(UnsignedInt id);

        /**
         * @brief Implementation for @ref objectName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doObjectName(UnsignedLong id);

        /** @brief Implementation for @ref scene() */
        virtual Containers::Optional<SceneData> doScene(UnsignedInt id);

        /**
         * @brief Implementation for @ref sceneFieldForName()
         * @m_since_latest
         *
         * Default implementation returns an invalid (zero) value.
         */
        virtual SceneField doSceneFieldForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref sceneFieldName()
         * @m_since_latest
         *
         * The @p name is always custom. Default implementation returns an
         * empty string.
         */
        virtual Containers::String doSceneFieldName(SceneField name);

        /**
         * @brief Implementation for @ref animationCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doAnimation() (with correct animation count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doAnimationCount() const;

        /**
         * @brief Implementation for @ref animationForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doAnimationForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref animationName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doAnimationName(UnsignedInt id);

        /** @brief Implementation for @ref animation() */
        virtual Containers::Optional<AnimationData> doAnimation(UnsignedInt id);

        /**
         * @brief Implementation for @ref animationTrackTargetForName()
         * @m_since_latest
         *
         * Default implementation returns an invalid (zero) value.
         */
        virtual AnimationTrackTarget doAnimationTrackTargetForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref animationTrackTargetName()
         * @m_since_latest
         *
         * The @p name is always custom. Default implementation returns an
         * empty string.
         */
        virtual Containers::String doAnimationTrackTargetName(AnimationTrackTarget name);

        /**
         * @brief Implementation for @ref lightCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doLight() (with correct light count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doLightCount() const;

        /**
         * @brief Implementation for @ref lightForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doLightForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref lightName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doLightName(UnsignedInt id);

        /** @brief Implementation for @ref light() */
        virtual Containers::Optional<LightData> doLight(UnsignedInt id);

        /**
         * @brief Implementation for @ref cameraCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doCamera() (with correct camera count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doCameraCount() const;

        /**
         * @brief Implementation for @ref cameraForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doCameraForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref cameraName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doCameraName(UnsignedInt id);

        /** @brief Implementation for @ref camera() */
        virtual Containers::Optional<CameraData> doCamera(UnsignedInt id);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Implementation for @ref object2DCount()
         *
         * Default implementation returns @cpp 0 @ce. There weren't any
         * importers in existence known to implement 2D scene import, so unlike
         * @ref doObject3DCount() this function doesn't delegate to
         * @ref doObjectCount().
         * @m_deprecated_since_latest Implement @ref doObjectCount() instead.
         */
        /* MSVC warns when overriding such methods and there's no way to
           suppress that warning, making the RT build (which treats deprecation
           warnings as errors) fail and other builds extremely noisy. So
           disabling those on MSVC. */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doObjectCount() instead")
        #endif
        virtual UnsignedInt doObject2DCount() const;

        /**
         * @brief Implementation for @ref object2DForName()
         *
         * Default implementation returns @cpp -1 @ce. There weren't any
         * importers in existence known to implement 2D scene import, so unlike
         * @ref doObject3DForName() this function doesn't delegate to
         * @ref doObjectForName().
         * @m_deprecated_since_latest Implement @ref doObjectForName() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doObjectForName() instead")
        #endif /* See above */
        virtual Int doObject2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref object2DName()
         *
         * Default implementation returns an empty string. There weren't any
         * importers in existence known to implement 2D scene import, so unlike
         * @ref doObject3DName() this function doesn't delegate to
         * @ref doObjectName().
         * @m_deprecated_since_latest Implement @ref doObjectName() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doObjectName() instead")
        #endif /* See above */
        virtual std::string doObject2DName(UnsignedInt id);

        /**
         * @brief Implementation for @ref object2D()
         *
         * There weren't any importers in existence known to implement 2D scene
         * import, so unlike @ref doObject3D() this function doesn't proxy
         * per-object data returned from @ref doScene().
         * @m_deprecated_since_latest Implement @ref doScene() instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doScene() instead")
        #endif /* See above */
        virtual Containers::Pointer<ObjectData2D> doObject2D(UnsignedInt id);
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Implementation for @ref object3DCount()
         *
         * Default implementation returns @ref doObjectCount() for backwards
         * compatibility.
         * @m_deprecated_since_latest Implement @ref doObjectCount() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doObjectCount() instead")
        #endif /* See above */
        virtual UnsignedInt doObject3DCount() const;

        /**
         * @brief Implementation for @ref object3DForName()
         *
         * Default implementation returns @ref doObjectForName() for backwards
         * compatibility.
         * @m_deprecated_since_latest Implement @ref doObjectForName() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doObjectForName() instead")
        #endif /* See above */
        virtual Int doObject3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref object3DName()
         *
         * Default implementation returns @ref doObjectName() for backwards
         * compatibility.
         * @m_deprecated_since_latest Implement @ref doObjectName() instead.
         */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doObjectName() instead")
        #endif /* See above */
        virtual std::string doObject3DName(UnsignedInt id);

        /**
         * @brief Implementation for @ref object3D()
         *
         * Default implementation retrieves and caches scenes returned from
         * @ref doScene(), finds the first scene that contains any fields for
         * object @p id and then returns a subset of the data that's
         * representable with a @ref ObjectData3D / @ref MeshObjectData3D
         * instance.
         * @m_deprecated_since_latest Implement @ref doScene() instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* Clang doesn't warn, but GCC does */
        #if !(defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG))
        CORRADE_DEPRECATED("implement doScene() instead")
        #endif /* See above */
        virtual Containers::Pointer<ObjectData3D> doObject3D(UnsignedInt id);
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Implementation for @ref skin2DCount()
         * @m_since_latest
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doSkin2D() (with correct skin count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doSkin2DCount() const;

        /**
         * @brief Implementation for @ref skin2DForName()
         * @m_since_latest
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doSkin2DForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref skin2DName()
         * @m_since_latest
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doSkin2DName(UnsignedInt id);

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
         * preferably during @ref doSkin3D() (with correct skin count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doSkin3DCount() const;

        /**
         * @brief Implementation for @ref skin3DForName()
         * @m_since_latest
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doSkin3DForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref skin3DName()
         * @m_since_latest
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doSkin3DName(UnsignedInt id);

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
         * preferably during @ref doMesh() (with correct mesh count
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
        virtual Int doMeshForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref meshName()
         * @m_since{2020,06}
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doMeshName(UnsignedInt id);

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
        virtual MeshAttribute doMeshAttributeForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref meshAttributeName()
         * @m_since{2020,06}
         *
         * The @p name is always custom. Default implementation returns an
         * empty string.
         */
        virtual Containers::String doMeshAttributeName(MeshAttribute name);

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
         * Default implementation returns an empty string. There weren't any
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
         * @ref MeshData3D for backwards compatibility.
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
         * preferably during @ref doMaterial() (with correct material count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doMaterialCount() const;

        /**
         * @brief Implementation for @ref materialForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doMaterialForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref materialName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doMaterialName(UnsignedInt id);

        /** @brief Implementation for @ref material() */
        virtual Containers::Optional<MaterialData> doMaterial(UnsignedInt id);

        /**
         * @brief Implementation for @ref textureCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doTexture() (with correct texture count
         * reported), and if not possible, already during file opening.
         */
        virtual UnsignedInt doTextureCount() const;

        /**
         * @brief Implementation for @ref textureForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doTextureForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref textureName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doTextureName(UnsignedInt id);

        /** @brief Implementation for @ref texture() */
        virtual Containers::Optional<TextureData> doTexture(UnsignedInt id);

        /**
         * @brief Implementation for @ref image1DCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doImage1D() (with correct image count
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
        virtual Int doImage1DForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref image1DName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doImage1DName(UnsignedInt id);

        /** @brief Implementation for @ref image1D() */
        virtual Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level);

        /**
         * @brief Implementation for @ref image2DCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doImage2D() (with correct image count
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
        virtual Int doImage2DForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref image2DName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doImage2DName(UnsignedInt id);

        /** @brief Implementation for @ref image2D() */
        virtual Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level);

        /**
         * @brief Implementation for @ref image3DCount()
         *
         * Default implementation returns @cpp 0 @ce. This function isn't
         * expected to fail --- if an import error occus, it should be handled
         * preferably during @ref doImage3D() (with correct image count
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
        virtual Int doImage3DForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref image3DName()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doImage3DName(UnsignedInt id);

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

        #ifdef MAGNUM_BUILD_DEPRECATED
        struct CachedScenes;
        Containers::Pointer<CachedScenes> _cachedScenes;
        void populateCachedScenes();
        #endif
};

/**
@brief Importer plugin interface
@m_since_latest

Same string as returned by
@relativeref{Magnum::Trade,AbstractImporter::pluginInterface()}, meant to be
used inside @ref CORRADE_PLUGIN_REGISTER() to avoid having to update the
interface string by hand every time the version gets bumped:

@snippet Trade.cpp MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE

The interface string version gets increased on every ABI break to prevent
silent crashes and memory corruption. Plugins built against the previous
version will then fail to load, a subsequent rebuild will make them pick up the
updated interface string.
*/
/* Silly indentation to make the string appear in pluginInterface() docs */
#define MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE /* [interface] */ \
"cz.mosra.magnum.Trade.AbstractImporter/0.5.2"
/* [interface] */

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
