#ifndef Magnum_Trade_AbstractImporter_h
#define Magnum_Trade_AbstractImporter_h
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
 * @brief Class @ref Magnum::Trade::AbstractImporter
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/PointerStl.h>

#include "Magnum/FileCallback.h"
#endif

namespace Magnum { namespace Trade {

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief InputFileCallbackPolicy
 * @deprecated Use @ref InputFileCallbackPolicy instead.
 */
typedef CORRADE_DEPRECATED("use InputFileCallbackPolicy instead") InputFileCallbackPolicy ImporterFileCallbackPolicy;
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

@subsection Trade-AbstractImporter-usage-callbacks Loading data from memory, using file callbacks

Besides loading data directly from the filesystem using @ref openFile() like
shown above, it's possible to use @ref openData() to import data from memory.
Note that the particular importer implementation must support
@ref Feature::OpenData for this method to work.

Complex scene files often reference other files such as images and in that case
you may want to intercept those references and load them in a custom way as
well. For importers that advertise support for this with @ref Feature::FileCallback
this is done by specifying a file loading callback using @ref setFileCallback().
The callback gets a filename, @ref InputFileCallbackPolicy and an user
pointer as parameters; returns a non-owning view on the loaded data or a
@ref Corrade::Containers::NullOpt "Containers::NullOpt" to indicate the file
loading failed. For example, loading a scene from memory-mapped files could
look like below. Note that the file loading callback affects @ref openFile() as
well --- you don't have to load the top-level file manually and pass it to
@ref openData(), any importer supporting the callback feature handles that
correctly.

@snippet MagnumTrade.cpp AbstractImporter-usage-callbacks

For importers that don't support @ref Feature::FileCallback directly, the base
@ref openFile() implementation will use the file callback to pass the loaded
data through to @ref openData(), in case the importer supports at least
@ref Feature::OpenData. If the importer supports neither @ref Feature::FileCallback
nor @ref Feature::OpenData, @ref setFileCallback() doesn't allow the callbacks
to be set.

@subsection Trade-AbstractImporter-usage-state Internal importer state

Some importers, especially ones that make use of well-known external libraries,
expose internal state through various accessors:

-   @ref importerState() can expose a pointer to the global importer
    state for currently opened file
-   @ref AbstractMaterialData::importerState() can expose importer state for
    given material imported by @ref material()
-   @ref AnimationData::importerState() can expose importer state for given
    animation imported by @ref animation()
-   @ref CameraData::importerState() can expose importer state for a camera
    importer by @ref camera()
-   @ref ImageData::importerState() can expose importer state for an image
    imported by @ref image1D(), @ref image2D() or @ref image3D()
-   @ref LightData::importerState() can expose importer state for a light
    imported by @ref light()
-   @ref MeshData3D::importerState() can expose importer state for a mesh
    imported by @ref mesh2D() or @ref mesh3D()
-   @ref ObjectData3D::importerState() can expose importer state for an object
    imported by @ref object2D() or @ref object3D()
-   @ref SceneData::importerState() can expose importer state for a scene
    imported by @ref scene()
-   @ref TextureData::importerState() can expose importer state for a texture
    imported by @ref texture()

Besides exposing internal state, importers that support the
@ref Feature::OpenState feature can also attach to existing importer state
using @ref openState(). See documentation of a particular importer for details
about concrete types returned and accepted by these functions.

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

@section Trade-AbstractImporter-subclassing Subclassing

The plugin needs to implement the @ref doFeatures(), @ref doIsOpened()
functions, at least one of @ref doOpenData() / @ref doOpenFile() /
@ref doOpenState() functions, function @ref doClose() and one or more tuples of
data access functions, based on what features are supported in given format.

In order to support @ref Feature::FileCallback, the importer needs to properly
use the callbacks to both load the top-level file in @ref doOpenFile() and also
load any external files when needed. The @ref doOpenFile() can delegate back
into the base implementation, but it should remember at least the base file
path to pass correct paths to subsequent file callbacks. The
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
-   The @ref doOpenData() function is called only if @ref Feature::OpenData is
    supported.
-   The @ref doOpenState() function is called only if @ref Feature::OpenState
    is supported.
-   The @ref doSetFileCallback() function is called only if
    @ref Feature::FileCallback is supported and there is no file opened.
-   All `do*()` implementations working on an opened file are called only if
    there is any file opened.
-   All `do*()` implementations taking data ID as parameter are called only if
    the ID is from valid range.

@attention
    @ref Corrade::Containers::Array instances returned from the plugin
    should *not* use anything else than the default deleter, otherwise this can
    cause dangling function pointer call on array destruction if the plugin
    gets unloaded before the array is destroyed.
@attention
    Similarly for interpolator functions passed through
    @ref Animation::TrackView instances to @ref AnimationData --- to avoid
    dangling pointers, be sure to always include an interpolator returned from
    @ref animationInterpolatorFor(), which guarantees the function is *not*
    instantiated in the plugin binary. Avoid using
    @ref Animation::interpolatorFor() (or indirectly it by specifying
    just @ref Animation::Interpolation), as it doesn't have such guarantee.
*/
class MAGNUM_TRADE_EXPORT AbstractImporter: public PluginManager::AbstractManagingPlugin<AbstractImporter> {
    public:
        /**
         * @brief Features supported by this importer
         *
         * @see @ref Features, @ref features()
         */
        enum class Feature: UnsignedByte {
            /** Opening files from raw data using @ref openData() */
            OpenData = 1 << 0,

            /** Opening already loaded state using @ref openState() */
            OpenState = 1 << 1,

            /**
             * Specifying callbacks for loading additional files referenced
             * from the main file using @ref setFileCallback(). If the importer
             * doesn't expose this feature, the format is either single-file or
             * loading via callbacks is not supported.
             *
             * See @ref Trade-AbstractImporter-usage-callbacks and particular
             * importer documentation for more information.
             */
            FileCallback = 1 << 2
        };

        /**
         * @brief Set of features supported by this importer
         *
         * @see @ref features()
         */
        typedef Containers::EnumSet<Feature> Features;

        /**
         * @brief Plugin interface
         *
         * @code{.cpp}
         * "cz.mosra.magnum.Trade.AbstractImporter/0.3"
         * @endcode
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * First looks in `magnum/importers/` or `magnum-d/importers/` next to
         * the executable and as a fallback in `magnum/importers/` or
         * `magnum-d/importers/` in the runtime install location (`lib[64]/` on
         * Unix-like systems, `bin/` on Windows). The system-wide plugin search
         * directory is configurable using the `MAGNUM_PLUGINS_DIR` CMake
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
        Features features() const { return doFeatures(); }

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
         * pass it to @p callback. Example usage:
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
         * data. Available only if @ref Feature::OpenData is supported. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise.
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
         * state. Available only if @ref Feature::OpenState is supported. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise.
         *
         * See documentation of a particular plugin for more information about
         * type and contents of the @p state parameter.
         * @see @ref features(), @ref openData()
         */
        bool openState(const void* state, const std::string& filePath = {});

        /**
         * @brief Open file
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Returns @cpp true @ce on success, @cpp false @ce otherwise.
         * If file loading callbacks are set via @ref setFileCallback() and
         * @ref Feature::OpenData is supported, this function uses the callback
         * to load the file and passes the memory view to @ref openData()
         * instead. See @ref setFileCallback() for more information.
         * @see @ref features(), @ref openData()
         */
        bool openFile(const std::string& filename);

        /** @brief Close file */
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
         *
         * @note The function is not const, because the value will probably
         *      be lazy-populated.
         */
        Int defaultScene();

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
         * @see @ref sceneName()
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
         */
        Containers::Optional<SceneData> scene(UnsignedInt id);

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
         * @see @ref animationName()
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
         */
        Containers::Optional<AnimationData> animation(UnsignedInt id);

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
         * @see @ref lightName()
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
         */
        Containers::Optional<LightData> light(UnsignedInt id);

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
         * @see @ref cameraName()
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
         */
        Containers::Optional<CameraData> camera(UnsignedInt id);

        /**
         * @brief Two-dimensional object count
         *
         * Expects that a file is opened.
         */
        UnsignedInt object2DCount() const;

        /**
         * @brief Two-dimensional object ID for given name
         *
         * If no scene for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref object2DName()
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
         */
        Containers::Pointer<ObjectData2D> object2D(UnsignedInt id);

        /**
         * @brief Three-dimensional object count
         *
         * Expects that a file is opened.
         */
        UnsignedInt object3DCount() const;

        /**
         * @brief Three-dimensional object ID for given name
         *
         * If no scene for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref object3DName()
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
         */
        Containers::Pointer<ObjectData3D> object3D(UnsignedInt id);

        /**
         * @brief Two-dimensional mesh count
         *
         * Expects that a file is opened.
         */
        UnsignedInt mesh2DCount() const;

        /**
         * @brief Two-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref mesh2DName()
         */
        Int mesh2DForName(const std::string& name);

        /**
         * @brief Two-dimensional mesh name
         * @param id        Mesh ID, from range [0, @ref mesh2DCount()).
         *
         * Expects that a file is opened.
         * @see @ref mesh2DForName()
         */
        std::string mesh2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional mesh
         * @param id        Mesh ID, from range [0, @ref mesh2DCount()).
         *
         * Returns given mesh or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         */
        Containers::Optional<MeshData2D> mesh2D(UnsignedInt id);

        /** @brief Three-dimensional mesh count */
        UnsignedInt mesh3DCount() const;

        /**
         * @brief Three-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref mesh3DName()
         */
        Int mesh3DForName(const std::string& name);

        /**
         * @brief Three-dimensional mesh name
         * @param id        Mesh ID, from range [0, @ref mesh3DCount()).
         *
         * Expects that a file is opened.
         * @see @ref mesh3DForName()
         */
        std::string mesh3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional mesh
         * @param id        Mesh ID, from range [0, @ref mesh3DCount()).
         *
         * Returns given mesh or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         */
        Containers::Optional<MeshData3D> mesh3D(UnsignedInt id);

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
         * @see @ref materialForName()
         */
        std::string materialName(UnsignedInt id);

        /**
         * @brief Material
         * @param id        Material ID, from range [0, @ref materialCount()).
         *
         * Returns given material or @cpp nullptr @ce if importing failed.
         * Expects that a file is opened.
         */
        Containers::Pointer<AbstractMaterialData> material(UnsignedInt id);

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
         * @see @ref textureName()
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
         */
        Containers::Optional<TextureData> texture(UnsignedInt id);

        /**
         * @brief One-dimensional image count
         *
         * Expects that a file is opened.
         */
        UnsignedInt image1DCount() const;

        /**
         * @brief One-dimensional image ID for given name
         *
         * If no image for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref image1DName()
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
         *
         * Returns given image or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         */
        Containers::Optional<ImageData1D> image1D(UnsignedInt id);

        /**
         * @brief Two-dimensional image count
         *
         * Expects that a file is opened.
         */
        UnsignedInt image2DCount() const;

        /**
         * @brief Two-dimensional image ID for given name
         *
         * If no image for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref image2DName()
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
         *
         * Returns given image or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         */
        Containers::Optional<ImageData2D> image2D(UnsignedInt id);

        /**
         * @brief Three-dimensional image count
         *
         * Expects that a file is opened.
         */
        UnsignedInt image3DCount() const;

        /**
         * @brief Three-dimensional image ID for given name
         *
         * If no image for given name exists, returns @cpp -1 @ce. Expects that
         * a file is opened.
         * @see @ref image3DName()
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
         *
         * Returns given image or @ref Containers::NullOpt if importing failed.
         * Expects that a file is opened.
         */
        Containers::Optional<ImageData3D> image3D(UnsignedInt id);

        /*@}*/

        /**
         * @brief Plugin-specific access to internal importer state
         *
         * The importer might provide access to its internal data structures
         * for currently opened document through this function. See
         * documentation of a particular plugin for more information about
         * returned type and contents. Returns @cpp nullptr @ce by default.
         * @see @ref AbstractMaterialData::importerState(),
         *      @ref AnimationData::importerState(), @ref CameraData::importerState(),
         *      @ref ImageData::importerState(), @ref LightData::importerState(),
         *      @ref MeshData2D::importerState(), @ref MeshData3D::importerState(),
         *      @ref ObjectData2D::importerState(), @ref ObjectData3D::importerState(),
         *      @ref SceneData::importerState(), @ref TextureData::importerState()
         */
        const void* importerState() const;

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

        /** @brief Implementation for @ref openState() */
        virtual void doOpenState(const void* state, const std::string& filePath);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /**
         * @brief Implementation for @ref defaultScene()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doDefaultScene();

        /**
         * @brief Implementation for @ref sceneCount()
         *
         * Default implementation returns @cpp 0 @ce.
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
         * Default implementation returns @cpp 0 @ce.
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
         * Default implementation returns @cpp 0 @ce.
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
         * Default implementation returns @cpp 0 @ce.
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
         * Default implementation returns @cpp 0 @ce.
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
         * Default implementation returns @cpp 0 @ce.
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
         * @brief Implementation for @ref mesh2DCount()
         *
         * Default implementation returns @cpp 0 @ce.
         */
        virtual UnsignedInt doMesh2DCount() const;

        /**
         * @brief Implementation for @ref mesh2DForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doMesh2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref mesh2DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doMesh2DName(UnsignedInt id);

        /** @brief Implementation for @ref mesh2D() */
        virtual Containers::Optional<MeshData2D> doMesh2D(UnsignedInt id);

        /**
         * @brief Implementation for @ref mesh3DCount()
         *
         * Default implementation returns @cpp 0 @ce.
         */
        virtual UnsignedInt doMesh3DCount() const;

        /**
         * @brief Implementation for @ref mesh3DForName()
         *
         * Default implementation returns @cpp -1 @ce.
         */
        virtual Int doMesh3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref mesh3DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doMesh3DName(UnsignedInt id);

        /** @brief Implementation for @ref mesh3D() */
        virtual Containers::Optional<MeshData3D> doMesh3D(UnsignedInt id);

        /**
         * @brief Implementation for @ref materialCount()
         *
         * Default implementation returns @cpp 0 @ce.
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
        virtual Containers::Pointer<AbstractMaterialData> doMaterial(UnsignedInt id);

        /**
         * @brief Implementation for @ref textureCount()
         *
         * Default implementation returns @cpp 0 @ce.
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
         * Default implementation returns @cpp 0 @ce.
         */
        virtual UnsignedInt doImage1DCount() const;

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
        virtual Containers::Optional<ImageData1D> doImage1D(UnsignedInt id);

        /**
         * @brief Implementation for @ref image2DCount()
         *
         * Default implementation returns @cpp 0 @ce.
         */
        virtual UnsignedInt doImage2DCount() const;

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
        virtual Containers::Optional<ImageData2D> doImage2D(UnsignedInt id);

        /**
         * @brief Implementation for @ref image3DCount()
         *
         * Default implementation returns @cpp 0 @ce.
         */
        virtual UnsignedInt doImage3DCount() const;

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
        virtual Containers::Optional<ImageData3D> doImage3D(UnsignedInt id);

        /** @brief Implementation for @ref importerState() */
        virtual const void* doImporterState() const;

    private:
        Containers::Optional<Containers::ArrayView<const char>>(*_fileCallback)(const std::string&, InputFileCallbackPolicy, void*){};
        void* _fileCallbackUserData{};

        /* Used by the templated version only */
        struct FileCallbackTemplate {
            void(*callback)();
            void* userData;
        } _fileCallbackTemplate{};
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class Callback, class T> void AbstractImporter::setFileCallback(Callback callback, T& userData) {
    /* Don't try to wrap a null function pointer. Need to cast first because
       MSVC (even 2017) can't apply ! to a lambda. Ugh. */
    const auto callbackPtr = static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, T&)>(callback);
    if(!callbackPtr) return setFileCallback(nullptr);

    _fileCallbackTemplate = { reinterpret_cast<void(*)()>(callbackPtr), &userData };
    setFileCallback([](const std::string& filename, const InputFileCallbackPolicy flags, void* const userData) {
        auto& s = *reinterpret_cast<FileCallbackTemplate*>(userData);
        return reinterpret_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, T&)>(s.callback)(filename, flags, *static_cast<T*>(s.userData));
    }, &_fileCallbackTemplate);
}
#endif

CORRADE_ENUMSET_OPERATORS(AbstractImporter::Features)

/** @debugoperatorclassenum{AbstractImporter,AbstractImporter::Feature} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AbstractImporter::Feature value);

/** @debugoperatorclassenum{AbstractImporter,AbstractImporter::Features} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AbstractImporter::Features value);

}}

#endif
