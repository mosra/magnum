#ifndef Magnum_Trade_AbstractSceneConverter_h
#define Magnum_Trade_AbstractSceneConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Trade::AbstractSceneConverter, enum @ref Magnum::Trade::SceneConverterFeature, enum set @ref Magnum::Trade::SceneConverterFeatures
 * @m_since{2020,06}
 */

#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* For *ToData() APIs that used to return just an Array before */
#include <Corrade/Containers/Optional.h>

/* So deprecated APIs taking a std::string don't fail to compile */
/** @todo remove once they are gone */
#include <Corrade/Utility/StlForwardString.h>
#endif

namespace Magnum { namespace Trade {

/**
@brief Features supported by a scene converter
@m_since{2020,06}

@see @ref SceneConverterFeatures, @ref AbstractSceneConverter::features()
*/
enum class SceneConverterFeature: UnsignedInt {
    /**
     * Convert a single mesh instance with
     * @ref AbstractSceneConverter::convert(const MeshData&).
     */
    ConvertMesh = 1 << 0, // TODO mention behavior w/ begin

    /**
     * Convert a single mesh instance in-place with
     * @ref AbstractSceneConverter::convertInPlace(MeshData&).
     */
    ConvertMeshInPlace = 1u << 1,

    /**
     * Convert a single mesh instance to a file with
     * @ref AbstractSceneConverter::convertToFile(const MeshData&, Containers::StringView).
     */
    ConvertMeshToFile = 1 << 2, // TODO mention behavior w/ beginFile

    /**
     * Convert a single mesh instance to raw data with
     * @ref AbstractSceneConverter::convertToData(const MeshData&). Implies
     * @ref SceneConverterFeature::ConvertMeshToFile.
     */
    ConvertMeshToData = ConvertMeshToFile|(1 << 3), // TODO mention behavior w/ beginData

    /**
     * Convert multiple data with
     * @ref AbstractSceneConverter::begin() and
     * @relativeref{AbstractSceneConverter,end()}.
     * @m_since_latest
     */
    ConvertMultiple = 1 << 4,

    /**
     * Convert multiple data to a file with
     * @ref AbstractSceneConverter::beginFile() and
     * @relativeref{AbstractSceneConverter,endFile()}.
     * @m_since_latest
     */
    ConvertMultipleToFile = 1 << 5,

    /**
     * Convert multiple data to raw data with
     * @ref AbstractSceneConverter::beginData() and
     * @relativeref{AbstractSceneConverter,endData()}. Implies
     * @ref SceneConverterFeature::ConvertMultipleToFile.
     * @m_since_latest
     */
    ConvertMultipleToData = ConvertMultipleToFile|(1 << 6),

    /**
     * Add scene instances with
     * @ref AbstractSceneConverter::add(const SceneData&, Containers::StringView),
     * together with @relativeref{AbstractSceneConverter,setSceneFieldName()},
     * @relativeref{AbstractSceneConverter,setObjectName()} and
     * @relativeref{AbstractSceneConverter,setDefaultScene()}.
     * @m_since_latest
     */
    /** @todo AddScene if target supports just one (OBJ, OpenGEX, Assimp) */
    AddScenes = 1 << 7,

    /**
     * Add animation instances with
     * @ref AbstractSceneConverter::add(const AnimationData&, Containers::StringView).
     * @m_since_latest
     */
    AddAnimations = 1 << 8,

    /**
     * Add light instances with
     * @ref AbstractSceneConverter::add(const LightData&, Containers::StringView).
     * @m_since_latest
     */
    AddLights = 1 << 9,

    /**
     * Add camera instances with
     * @ref AbstractSceneConverter::add(const CameraData&, Containers::StringView).
     * @m_since_latest
     */
    AddCameras = 1 << 10,

    /**
     * Add 2D skin instances with
     * @ref AbstractSceneConverter::add(const SkinData2D&, Containers::StringView).
     * @m_since_latest
     */
    AddSkins2D = 1 << 11,

    /**
     * Add 3D skin instances with
     * @ref AbstractSceneConverter::add(const SkinData3D&, Containers::StringView).
     * @m_since_latest
     */
    AddSkins3D = 1 << 12,

    /**
     * Add single-level mesh instances with
     * @ref AbstractSceneConverter::add(const MeshData&, Containers::StringView),
     * together with @relativeref{AbstractSceneConverter,setMeshAttributeName()}.
     * @m_since_latest
     * @see @ref SceneConverterFeature::MeshLevels
     */
    AddMeshes = 1 << 13, // TODO mention behavior with convert()

    /**
     * Add material instances with
     * @ref AbstractSceneConverter::add(const MaterialData&, Containers::StringView).
     * @m_since_latest
     */
    AddMaterials = 1 << 14,

    /**
     * Add texture instances with
     * @ref AbstractSceneConverter::add(const TextureData&, Containers::StringView).
     * @m_since_latest
     */
    AddTextures = 1 << 15,

    /**
     * Add single-level 1D image instances with
     * @ref AbstractSceneConverter::add(const ImageData1D&, Containers::StringView)
     * or @ref AbstractSceneConverter::add(const ImageView1D&, Containers::StringView).
     * @m_since_latest
     * @see @ref SceneConverterFeature::ImageLevels
     */
    AddImages1D = 1 << 16,

    /**
     * Add single-level 2D image instances with
     * @ref AbstractSceneConverter::add(const ImageData2D&, Containers::StringView)
     * or @ref AbstractSceneConverter::add(const ImageView2D&, Containers::StringView).
     * @m_since_latest
     * @see @ref SceneConverterFeature::ImageLevels
     */
    AddImages2D = 1 << 17,

    /**
     * Add single-level 3D image instances with
     * @ref AbstractSceneConverter::add(const ImageData3D&, Containers::StringView)
     * or @ref AbstractSceneConverter::add(const ImageView3D&, Containers::StringView).
     * @m_since_latest
     * @see @ref SceneConverterFeature::ImageLevels
     */
    AddImages3D = 1 << 18,

    /**
     * Add single-level compressed 1D image instances with
     * @ref AbstractSceneConverter::add(const ImageData1D&, Containers::StringView)
     * or @ref AbstractSceneConverter::add(const CompressedImageView1D&, Containers::StringView).
     * @m_since_latest
     * @see @ref SceneConverterFeature::ImageLevels
     */
    AddCompressedImages1D = 1 << 19,

    /**
     * Add single-level compressed 2D image instances with
     * @ref AbstractSceneConverter::add(const ImageData2D&, Containers::StringView)
     * or @ref AbstractSceneConverter::add(const CompressedImageView2D&, Containers::StringView).
     * @m_since_latest
     * @see @ref SceneConverterFeature::ImageLevels
     */
    AddCompressedImages2D = 1 << 20,

    /**
     * Add single-level compressed 3D image instances with
     * @ref AbstractSceneConverter::add(const ImageData3D&, Containers::StringView)
     * or @ref AbstractSceneConverter::add(const CompressedImageView3D&, Containers::StringView).
     * @m_since_latest
     * @see @ref SceneConverterFeature::ImageLevels
     */
    AddCompressedImages3D = 1 << 21,

    /**
     * Add multiple mesh levels with
     * @ref AbstractSceneConverter::add(Containers::ArrayView<const Containers::AnyReference<const MeshData>>, Containers::StringView)
     * if @ref SceneConverterFeature::AddMeshes is also supported.
     * @m_since_latest
     */
    MeshLevels = 1 << 22,

    /**
     * Add multiple image levels with
     * @ref AbstractSceneConverter::add(Containers::ArrayView<const Containers::AnyReference<const ImageData1D>>, Containers::StringView)
     * if @ref SceneConverterFeature::AddImages1D or
     * @relativeref{SceneConverterFeature,AddCompressedImages1D} is also
     * supported; with @ref AbstractSceneConverter::add(Containers::ArrayView<const Containers::AnyReference<const ImageData2D>>, Containers::StringView)
     * if @ref SceneConverterFeature::AddImages2D or
     * @relativeref{SceneConverterFeature,AddCompressedImages2D} is also
     * supported; or with @ref AbstractSceneConverter::add(Containers::ArrayView<const Containers::AnyReference<const ImageData1D>>, Containers::StringView)
     * if @ref SceneConverterFeature::AddImages3D or
     * @relativeref{SceneConverterFeature,AddCompressedImages3D} is also
     * supported.
     * @m_since_latest
     */
    ImageLevels = 1 << 23
};

/**
@brief Features supported by a scene converter
@m_since{2020,06}

@see @ref AbstractSceneConverter::features()
*/
typedef Containers::EnumSet<SceneConverterFeature> SceneConverterFeatures;

CORRADE_ENUMSET_OPERATORS(SceneConverterFeatures)

/** @debugoperatorenum{SceneConverterFeature} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneConverterFeature value);

/** @debugoperatorenum{SceneConverterFeatures} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneConverterFeatures value);

/**
@brief Scene converter flag
@m_since{2020,06}

@see @ref SceneConverterFlags, @ref AbstractSceneConverter::setFlags()
*/
enum class SceneConverterFlag: UnsignedByte {
    /**
     * Print verbose diagnostic during conversion. By default the converter
     * only prints messages on error or when some operation might cause
     * unexpected data modification or loss.
     *
     * Corresponds to the `-v` / `--verbose` option in
     * @ref magnum-sceneconverter "magnum-sceneconverter".
     */
    Verbose = 1 << 0

    /** @todo Y flip */
};

/**
@brief Scene converter flags
@m_since{2020,06}

@see @ref AbstractSceneConverter::setFlags()
*/
typedef Containers::EnumSet<SceneConverterFlag> SceneConverterFlags;

CORRADE_ENUMSET_OPERATORS(SceneConverterFlags)

/**
@debugoperatorenum{SceneConverterFlag}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneConverterFlag value);

/**
@debugoperatorenum{SceneConverterFlags}
@m_since{2020,06}
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneConverterFlags value);

#ifdef MAGNUM_BUILD_DEPRECATED
namespace Implementation {
    /* Could be a concrete type as it's always only char, but that would mean
       I'd need to include Optional and Array here. It's named like this
       because AbstractImageConverter and ShaderTools::AbstractConverter each
       have its own and introducing a common header containing just deprecated
       functionality seems silly. */
    template<class T> struct SceneConverterOptionalButAlsoArray: Containers::Optional<Containers::Array<T>> {
        /*implicit*/ SceneConverterOptionalButAlsoArray() = default;
        /*implicit*/ SceneConverterOptionalButAlsoArray(Containers::Optional<Containers::Array<T>>&& optional): Containers::Optional<Containers::Array<T>>{std::move(optional)} {}
        CORRADE_DEPRECATED("use Containers::Optional<Containers::Array<T>> instead") /*implicit*/ operator Containers::Array<T>() && {
            return *this ? Containers::Array<T>{std::move(**this)} : nullptr;
        }
    };
}
#endif

/**
@brief Base for scene converter plugins
@m_since{2020,06}

Provides functionality for converting meshes and other scene data between
various formats or performing optimizations and other operations on them.

The interface supports three main kinds of operation, with implementations
advertising support for a subset of them via @ref features():

-   Saving a mesh to a file / data using
    @ref convertToFile(const MeshData&, Containers::StringView) /
    @ref convertToData(const MeshData&). This is mostly for exporting the mesh
    data to a common format like OBJ or PLY in order to be used with an
    external tool. Advertised with @ref SceneConverterFeature::ConvertMeshToFile
    or @ref SceneConverterFeature::ConvertMeshToData
-   Performing an operation on the mesh data itself using
    @ref convert(const MeshData&), from which you get a @ref MeshData again.
    This includes operations like mesh decimation or topology cleanup.
    Advertised with @ref SceneConverterFeature::ConvertMesh.
-   Performing an operation on the mesh data *in place* using
    @ref convertInPlace(MeshData&). This is for operations like vertex cache
    optimization that don't need to change the mesh topology, only modify or
    shuffle the data around. Advertised with
    @ref SceneConverterFeature::ConvertMeshInPlace.

@section Trade-AbstractSceneConverter-usage Usage

Scene converters are commonly implemented as plugins, which means the concrete
converter implementation is loaded and instantiated through a
@relativeref{Corrade,PluginManager::Manager}. Then, based on the intent and on
what the particular converter supports, either a single-mesh conversion with
@ref convert() and related functions is performed, or a whole-scene conversion
with @ref begin(), various @ref add() APIs and @ref end() is done.

As each converter has different requirements on the input data, their layout
and formats, you're expected to perform error handling on the application side
--- if a conversion fails, you get an empty
@relativeref{Corrade,Containers::Optional} or @cpp false @ce and a reason
printed to @relativeref{Magnum,Error}. Everything else (using a feature not
implemented in the converter, ...) is treated as a programmer error and will
produce the usual assertions.

@subsection Trade-AbstractSceneConverter-usage-mesh-file Converting a single mesh to a file

In the following example a mesh is saved to a PLY file using the
@ref AnySceneConverter plugin, together with all needed error handling. In this
case we *know* that @ref AnySceneConverter supports
@ref SceneConverterFeature::ConvertMeshToFile, however in a more general case
it might be good to check against the reported @ref features() first.

@snippet MagnumTrade.cpp AbstractSceneConverter-usage-file

See @ref plugins for more information about general plugin usage,
@ref file-formats to compare implementations of common file formats and the
list of @m_class{m-doc} [derived classes](#derived-classes) for all available
scene converter plugins.

@m_class{m-note m-success}

@par
    There's also the @ref magnum-sceneconverter "magnum-sceneconverter" tool
    that exposes functionality of all scene converter plugins through a command
    line interface.

@subsection Trade-AbstractSceneConverter-usage-mesh-data Converting a single mesh data

In the following snippet we use the @ref MeshOptimizerSceneConverter to perform
a set of optimizations on the mesh to make it render faster. While
@ref AnySceneConverter can detect the desired format while writing to a file,
here it would have no way to know what we want and so we request the concrete
plugin name directly.

@snippet MagnumTrade.cpp AbstractSceneConverter-usage-mesh

Commonly, when operating directly on the mesh data, each plugin exposes a set
of configuration options to specify what actually gets done and how, and the
default setup may not even do anything. See @ref plugins-configuration for
details and a usage example.

@subsection Trade-AbstractSceneConverter-usage-mesh-in-place Converting a single mesh data in-place

Certain operations such as buffer reordering can be performed by directly
modifying the input data instead of having to allocate a copy of the whole
mesh. For that, there's @ref convertInPlace(), however compared to
@ref convert() it imposes additional requirements on the input. Depending on
the converter, it might require that either the index or the vertex data are
mutable, or that the mesh is interleaved and so on, so be sure to check the
plugin docs before use.

An equivalent to the above operation, but performed in-place, would be the
following:

@snippet MagnumTrade.cpp AbstractSceneConverter-usage-mesh-in-place

@subsection Trade-AbstractSceneConverter-usage-multiple Converting multiple data

While the operations shown above are convenient enough for simple cases
involving just a single mesh, general conversion of a whole scene needs much
more than that. Such conversion is done in a batch way --- first initializing
the conversion of desired kind, adding particular data, and finalizing the
conversion together with getting the output back. Similarly to the above cases
involving a single mesh, there are three kinds of batch conversions:

-   Scene-to-scene using @ref begin() and @ref end(). The output is an
    @ref AbstractImporter instance exposing arbitrary data. For example it can
    be an operation where a single mesh is converted into a set of LODs, or a
    scale-aware mesh and texture simplification taking into account the
    relative scale.
-   Scene-to-file using @ref beginFile() and @ref endFile(). Takes a filename,
    the output is a @cpp bool @ce denoting whether the saving was successful.
    For example for saving a whole scene as a glTF.
-   Scene-to-data using @ref beginData() and @ref endData(). Instead of saving
    a file, the output is an array with the converted file data.

Between the begin and end call, you're expected to call various @ref add()
functions to populate the output with actual data, depending on what the
particular converter supports.

For convenience, if a converter supports just the single-mesh operation, it's
exposed through the batch APIs as well. Conversely, if it supports just batch
conversion of meshes to files or data, those are transparently called from the
single-mesh @ref convertToData(const MeshData&) and
@ref convertToFile(const MeshData&, Containers::StringView) APIs.

@section Trade-AbstractSceneConverter-data-dependency Data dependency

The instances returned from various functions *by design* have no dependency on
the converter instance and neither on the dynamic plugin module. In other
words, you don't need to keep the converter instance (or the plugin manager
instance) around in order to have the `*Data` instances valid. Moreover, all
@ref Corrade::Containers::Array instances returned through @ref MeshData and
others are only allowed to have default deleters --- this is to avoid potential
dangling function pointer calls when destructing such instances after the
plugin module has been unloaded.

@section Trade-AbstractSceneConverter-subclassing Subclassing

The plugin needs to implement the @ref doFeatures() function and one or more of
@ref doConvert(), @ref doConvertInPlace(), @ref doConvertToData(),
@ref doConvertToFile() functions based on what single-mesh conversion features
are supported, or pairs of @ref doBegin() / @ref doEnd(), @ref doBeginData() /
@ref doEndData(), @ref doBeginFile() / @ref doEndFile() functions and one or
more @ref doAdd() functions based on what multiple-data features are supported.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The @ref doConvert(const MeshData&) function is called only if
    @ref SceneConverterFeature::ConvertMesh is supported.
-   The @ref doConvertInPlace(MeshData&) function is called only if
    @ref SceneConverterFeature::ConvertMeshInPlace is supported.
-   The @ref doConvertToData(const MeshData&) function is called only if
    @ref SceneConverterFeature::ConvertMeshToData is supported.
-   The @ref doConvertToFile(const MeshData&, Containers::StringView) function
    is called only if @ref SceneConverterFeature::ConvertMeshToFile is
    supported.
-   The @ref doBegin() and @ref doEnd() functions are called only if
    @ref SceneConverterFeature::ConvertMultiple is supported.
-   The @ref doBeginData() and @ref doEndData() functions are called only if
    @ref SceneConverterFeature::ConvertMultipleToData is supported.
-   The @ref doBeginFile() and @ref doEndFile() functions are called only if
    @ref SceneConverterFeature::ConvertMultipleToFile is supported.
-   The @ref doEnd(), @ref doEndData(), @ref doEndFile(), @ref doAbort() and
    @ref doAdd() functions are called only if a corresponding @ref begin(),
    @ref beginData() or @ref beginFile() was called before and @ref abort()
    wasn't called in the meantime.
-   The @ref doAdd() and various `doSet*()` functions are called only if a
    corresponding @ref SceneConverterFeature is supported.
-   The @ref doAdd((UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const MeshData>>, Containers::StringView)
    function is called only if the list has at least one mesh
-   All @ref doAdd() functions taking a single image are called only if the
    image has a non-zero size in all dimensions and the data is not
    @cpp nullptr @ce.
-   All @ref doAdd() functions taking multiple images are called only if the
    list has at least one image, each of the images has a non-zero size, the
    data are not @cpp nullptr @ce and additionally all images are either
    uncompressed or all compressed and they have the same pixel format.
    Since file formats have varying requirements on image level sizes and their
    order and some don't impose any requirements at all, the plugin
    implementation is expected to check the sizes on its own.

@m_class{m-block m-warning}

@par Dangling function pointers on plugin unload
    As @ref Trade-AbstractSceneConverter-data-dependency "mentioned above",
    @relativeref{Corrade::Containers,Array} instances returned from plugin
    implementations are not allowed to use anything else than the default
    deleter or the deleter used by @ref Trade::ArrayAllocator, otherwise this
    could cause dangling function pointer call on array destruction if the
    plugin gets unloaded before the array is destroyed. This is asserted by the
    base implementation on return.
@par
    The only exception is the @ref AbstractImporter instance returned by
    @ref end() --- since its implementation is in the plugin module itself, the
    plugin can't be unloaded until the returned instance is destroyed.
*/
class MAGNUM_TRADE_EXPORT AbstractSceneConverter: public PluginManager::AbstractManagingPlugin<AbstractSceneConverter> {
    public:
        /**
         * @brief Plugin interface
         *
         * @snippet Magnum/Trade/AbstractSceneConverter.cpp interface
         */
        static Containers::StringView pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * Looks into `magnum/sceneconverters/` or `magnum-d/sceneconverters/`
         * next to the dynamic @ref Trade library, next to the executable and
         * elsewhere according to the rules documented in
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
        explicit AbstractSceneConverter();

        /** @brief Constructor with access to plugin manager */
        explicit AbstractSceneConverter(PluginManager::Manager<AbstractSceneConverter>& manager);

        /** @brief Plugin manager constructor */
        /* The plugin name is passed as a const& to make it possible for people
           to implement plugins without even having to include the StringView
           header. */
        explicit AbstractSceneConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin);

        ~AbstractSceneConverter();

        /** @brief Features supported by this converter */
        SceneConverterFeatures features() const;

        /** @brief Converter flags */
        SceneConverterFlags flags() const { return _flags; }

        /**
         * @brief Set converter flags
         *
         * Some flags can be set only if the converter supports particular
         * features, see documentation of each @ref SceneConverterFlag for more
         * information. By default no flags are set. To avoid clearing
         * potential future default flags by accident, prefer to use
         * @ref addFlags() and @ref clearFlags() instead.
         *
         * Corresponds to the `-v` / `--verbose` option in
         * @ref magnum-sceneconverter "magnum-sceneconverter".
         */
        void setFlags(SceneConverterFlags flags);

        /**
         * @brief Add converter flags
         * @m_since_latest
         *
         * Calls @ref setFlags() with the existing flags ORed with @p flags.
         * Useful for preserving the defaults.
         * @see @ref clearFlags()
         */
        void addFlags(SceneConverterFlags flags);

        /**
         * @brief Clear converter flags
         * @m_since_latest
         *
         * Calls @ref setFlags() with the existing flags ANDed with inverse of
         * @p flags. Useful for removing default flags.
         * @see @ref addFlags()
         */
        void clearFlags(SceneConverterFlags flags);

        /**
         * @brief Convert a mesh
         *
         * Depending on the plugin, can perform for example vertex format
         * conversion, overdraw optimization or decimation / subdivision.
         * Available only if @ref SceneConverterFeature::ConvertMesh is
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @ref Containers::NullOpt.
         *
         * If @ref SceneConverterFeature::AddMeshes is supported instead, you
         * have to use @ref begin(), @ref add(const MeshData&, Containers::StringView)
         * and retrieve the output from the importer returned by @ref end() ---
         * in such case the process can also return zero or more than one mesh
         * instead of always exactly one.
         * @see @ref features(), @ref convertInPlace(MeshData&)
         */
        Containers::Optional<MeshData> convert(const MeshData& mesh);

        /**
         * @brief Convert a mesh in-place
         *
         * Depending on the plugin, can perform for example index buffer
         * reordering for better vertex cache use or overdraw optimization.
         * Available only if @ref SceneConverterFeature::ConvertMeshInPlace is
         * supported. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @cpp false @ce, @p mesh is guaranteed to stay unchanged.
         * @see @ref features(), @ref convert(const MeshData&)
         */
        bool convertInPlace(MeshData& mesh);

        /**
         * @brief Convert a mesh to a raw data
         *
         * Depending on the plugin, can convert the mesh to a file format that
         * can be saved to disk. Available only if
         * @ref SceneConverterFeature::ConvertMeshToData or
         * both @ref SceneConverterFeature::ConvertMultipleToData and
         * @ref SceneConverterFeature::AddMeshes is supported. In case of the
         * latter is equivalent to a sequence of @ref beginData(),
         * @ref add(const MeshData&, Containers::StringView) and returning the
         * output of @ref endData(). On failure prints a message to
         * @relativeref{Magnum,Error} and returns @ref Containers::NullOpt.
         * @see @ref features(), @ref convertToFile()
         */
        #if !defined(MAGNUM_BUILD_DEPRECATED) || defined(DOXYGEN_GENERATING_OUTPUT)
        Containers::Optional<Containers::Array<char>>
        #else
        Implementation::SceneConverterOptionalButAlsoArray<char>
        #endif
        convertToData(const MeshData& mesh);

        /**
         * @brief Convert a mesh to a file
         * @m_since_latest
         *
         * Available only if @ref SceneConverterFeature::ConvertMeshToFile or
         * both @ref SceneConverterFeature::ConvertMultipleToFile and
         * @ref SceneConverterFeature::AddMeshes is supported. In case of the
         * latter is equivalent to a sequence of @ref beginFile(),
         * @ref add(const MeshData&, Containers::StringView) and returning the
         * output of @ref endFile(). On failure prints a message to
         * @relativeref{Magnum,Error} and returns @cpp false @ce.
         * @see @ref features(), @ref convertToData()
         */
        bool convertToFile(const MeshData& mesh, Containers::StringView filename);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief convertToFile(const MeshData&, Containers::StringView)
         * @m_deprecated_since_latest Use @ref convertToFile(const MeshData&, Containers::StringView)
         *      instead.
         */
        CORRADE_DEPRECATED("use convertToFile(const MeshData&, Containers::StringView) instead") bool convertToFile(const std::string& filename, const MeshData& mesh);
        #endif

        /**
         * @brief Whether any conversion is in progress
         * @m_since_latest
         *
         * Returns @cpp true @ce if any conversion started by @ref begin(),
         * @ref beginData() or @ref beginFile() has not ended yet and
         * @ref abort() wasn't called; @cpp false @ce otherwise.
         */
        bool isConverting() const;

        /**
         * @brief Abort any in-progress conversion
         * @m_since_latest
         *
         * On particular implementations an explicit call to this function may
         * result in freed memory. If no conversion is currently in progress,
         * does nothing. After this function is called, @ref isConverting()
         * returns @cpp false @ce.
         */
        void abort();

        /**
         * @brief Begin converting a scene
         * @m_since_latest
         *
         * If a conversion is currently in progress, calls @ref abort() first.
         * The converted output of data supplied via various `add*()` and
         * `set*()` APIs is returned via an importer instance upon calling
         * @ref end(). Expects that
         * @ref SceneConverterFeature::ConvertMultiple is supported.
         * @see @ref features(), @ref beginData(), @ref beginFile()
         */
        // TODO mention behavior with ConvertMesh
        void begin();

        /**
         * @brief End converting a scene
         * @m_since_latest
         *
         * Expects that @ref begin() was called before. The returned importer
         * may contain arbitrary amounts of data depending on the particular
         * converter plugin. The instance is fully self-contained, with no
         * dependency on the originating converter instance or its internal
         * state, meaning you can immediately reuse the instance for another
         * conversion without corrupting the importer instance returned
         * earlier. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @cpp nullptr @ce.
         */
        // TODO mention behavior with ConvertMesh, especially with the importer
        //  behavior, also test that to be more graceful
        Containers::Pointer<AbstractImporter> end();

        /**
         * @brief Begin converting a scene to raw data
         * @m_since_latest
         *
         * If a conversion is currently in progress, calls @ref abort() first.
         * The converted output of data supplied via various `add*()` and
         * `set*()` APIs is returned upon calling @ref endData(). Expects that
         * @ref SceneConverterFeature::ConvertMultipleToData is supported.
         * @see @ref features(), @ref begin(), @ref beginFile()
         */
        // TODO mention behavior with ConvertMeshToData
        void beginData();

        /**
         * @brief End converting a scene to raw data
         * @m_since_latest
         *
         * Expects that @ref beginData() was called before. On failure prints a
         * message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt.
         */
        // TODO mention behavior with ConvertMeshToData
        Containers::Optional<Containers::Array<char>> endData();

        /**
         * @brief Begin converting a scene to a file
         * @m_since_latest
         *
         * If a conversion is currently in progress, calls @ref abort() first.
         * The converted output of data supplied via various `add*()` and
         * `set*()` APIs is returned upon calling @ref endFile(). Expects that
         * @ref SceneConverterFeature::ConvertMultipleToFile is supported.
         * @see @ref features(), @ref begin(), @ref beginData()
         */
        // TODO mention behavior with ConvertMeshToFile
        void beginFile(Containers::StringView filename);

        /**
         * @brief End converting a scene to raw data
         * @m_since_latest
         *
         * Expects that @ref beginData() was called before. On failure prints a
         * message to @relativeref{Magnum,Error} and returns @cpp false @ce.
         */
        // TODO mention behavior with ConvertMeshToFile, especially about when the file gets saved
        bool endFile();

        /**
         * @brief Count of added scenes
         * @m_since_latest
         *
         * Count of scenes successfully added with
         * @ref add(const SceneData&, Containers::StringView) since the initial
         * @ref begin(), @ref beginData() or @ref beginFile() call. Expects
         * that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddScenes is not supported, returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt sceneCount() const;

        /**
         * @brief Add a scene
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::AddScenes is supported. The returned ID
         * is implicitly equal to @ref sceneCount() before calling this
         * function and can be subsequently used in functions like
         * @ref setDefaultScene(). On failure prints a message to
         * @relativeref{Magnum,Error} and returns @ref Containers::NullOpt ---
         * the count of added animations doesn't change in that case.
         *
         * If the converter doesn't support scene naming, @p name is ignored.
         * @see @ref isConverting(), @ref features(),
         *      @ref setSceneFieldName(), @ref setObjectName(),
         *      @ref setDefaultScene()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const SceneData& data, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const SceneData& data, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const SceneData& data);
        #endif

        /**
         * @brief Set name of a custom scene field
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress,
         * @ref SceneConverterFeature::AddScenes is supported and @p field is a
         * custom field. The field name will get used only for scene data added
         * after this function has been called. If the converter doesn't
         * support custom scene fields or doesn't support naming them, the call
         * is ignored.
         * @see @ref isConverting(), @ref features(), @ref isSceneFieldCustom(),
         *      @ref setMeshAttributeName()
         */
        void setSceneFieldName(SceneField field, Containers::StringView name);

        /**
         * @brief Set object name
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress,
         * @ref SceneConverterFeature::AddScenes is supported and @p field is a
         * custom field. The object name will get used only for scene data
         * added after this function has been called. If the converter doesn't
         * support naming objects, the call is ignored.
         * @see @ref isConverting(), @ref features()
         */
        void setObjectName(UnsignedLong object, Containers::StringView name);

        /**
         * @brief Set default scene
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress,
         * @ref SceneConverterFeature::AddScenes is supported and
         * @ref sceneCount() is greater than @p id. If the converter doesn't
         * support multiple scenes or default scene selection, the call is
         * ignored.
         * @see @ref isConverting(), @ref features()
         */
        void setDefaultScene(UnsignedInt id);

        /**
         * @brief Count of added animations
         * @m_since_latest
         *
         * Count of animations successfully added with
         * @ref add(const AnimationData&, Containers::StringView) since the
         * initial @ref begin(), @ref beginData() or @ref beginFile() call.
         * Expects that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddAnimations is not supported, returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt animationCount() const;

        /**
         * @brief Add an animation
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::AddAnimations is supported. The returned
         * ID is implicitly equal to @ref animationCount() before calling this
         * function. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @ref Containers::NullOpt --- the count of added
         * animations doesn't change in that case.
         *
         * If the converter doesn't support animation naming, @p name is
         * ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const AnimationData& animation, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const AnimationData& animation, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const AnimationData& animation);
        #endif

        /**
         * @brief Count of added lights
         * @m_since_latest
         *
         * Count of lights successfully added with
         * @ref add(const LightData&, Containers::StringView) since the initial
         * @ref begin(), @ref beginData() or @ref beginFile() call. Expects
         * that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddLights is not supported, returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt lightCount() const;

        /**
         * @brief Add a light
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::AddLights is supported. The returned ID
         * is implicitly equal to @ref lightCount() before calling this
         * function and can be subsequently used to for example reference a
         * light from a @ref SceneData passed to
         * @ref add(const SceneData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added lights doesn't
         * change in that case.
         *
         * If the converter doesn't support light naming, @p name is ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const LightData& light, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const LightData& light, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const LightData& light);
        #endif

        /**
         * @brief Count of added cameras
         * @m_since_latest
         *
         * Count of cameras successfully added with
         * @ref add(const CameraData&, Containers::StringView) since the
         * initial @ref begin(), @ref beginData() or @ref beginFile() call.
         * Expects that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddCameras is not supported, returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt cameraCount() const;

        /**
         * @brief Add a camera
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::AddCameras is supported. The returned ID
         * is implicitly equal to @ref cameraCount() before calling this
         * function and can be subsequently used to for example reference a
         * camera from a @ref SceneData passed to
         * @ref add(const SceneData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added cameras doesn't
         * change in that case.
         *
         * If the converter doesn't support camera naming, @p name is ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const CameraData& camera, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const CameraData& camera, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const CameraData& camera);
        #endif

        /**
         * @brief Count of added 2D skins
         * @m_since_latest
         *
         * Count of skins successfully added with
         * @ref add(const SkinData2D&, Containers::StringView) since the
         * initial @ref begin(), @ref beginData() or @ref beginFile() call.
         * Expects that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddSkins2D is not supported, returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt skin2DCount() const;

        /**
         * @brief Add a 2D skin
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::AddSkins2D is supported. The returned ID
         * is implicitly equal to @ref skin2DCount() before calling this
         * function. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @ref Containers::NullOpt --- the count of added skins
         * doesn't change in that case.
         *
         * If the converter doesn't support skin naming, @p name is ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const SkinData2D& skin, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const SkinData2D& skin, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const SkinData2D& skin);
        #endif

        /**
         * @brief Count of added 3D skins
         * @m_since_latest
         *
         * Count of skins successfully added with
         * @ref add(const SkinData3D&, Containers::StringView) since the
         * initial @ref begin(), @ref beginData() or @ref beginFile() call.
         * Expects that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddSkins3D is not supported, returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt skin3DCount() const;

        /**
         * @brief Add a 3D skin
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::AddSkins3D is supported. The returned ID
         * is implicitly equal to @ref skin3DCount() before calling this
         * function. On failure prints a message to @relativeref{Magnum,Error}
         * and returns @ref Containers::NullOpt --- the count of added skins
         * doesn't change in that case.
         *
         * If the converter doesn't support skin naming, @p name is ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const SkinData3D& skin, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const SkinData3D& skin, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const SkinData3D& skin);
        #endif

        /**
         * @brief Count of added meshes
         * @m_since_latest
         *
         * Count of meshes successfully added with
         * @ref add(const MeshData&, Containers::StringView) or
         * @ref add(Containers::ArrayView<const Containers::AnyReference<const MeshData>>, Containers::StringView)
         * since the initial @ref begin(), @ref beginData() or @ref beginFile()
         * call. Expects that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddMeshes is not supported and only the
         * singular @relativeref{SceneConverterFeature,ConvertMesh},
         * @relativeref{SceneConverterFeature,ConvertMeshToData} or
         * @relativeref{SceneConverterFeature,ConvertMeshToFile} is supported,
         * returns always either @cpp 0 @ce or @cpp 1 @ce. Otherwise returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt meshCount() const;

        /**
         * @brief Add a mesh
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and either
         * @ref SceneConverterFeature::AddMeshes,
         * @relativeref{SceneConverterFeature,ConvertMesh},
         * @relativeref{SceneConverterFeature,ConvertMeshToData} or
         * @relativeref{SceneConverterFeature,ConvertMeshToFile} is
         * supported. The returned ID is implicitly equal to @ref meshCount()
         * before calling this function and can be subsequently used to for
         * example reference a mesh from a @ref SceneData passed to
         * @ref add(const SceneData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added meshes doesn't
         * change in that case.
         *
         * If the converter doesn't support mesh naming, @p name is ignored.
         *
         * If only the singular @ref SceneConverterFeature::ConvertMesh,
         * @relativeref{SceneConverterFeature,ConvertMeshToData} or
         * @relativeref{SceneConverterFeature,ConvertMeshToFile} is supported,
         * the function can be called only exactly once to successfully produce
         * an output, and the process is equivalent to
         * @ref convert(const MeshData&),
         * @ref convertToData(const MeshData&) or
         * @ref convertToFile(const MeshData&, Containers::StringView), with
         * the @p name ignored.
         * @see @ref isConverting(), @ref features(),
         *      @ref setMeshAttributeName()
         */
        // TODO mention behavior with ConvertMeshToFile, especially when the file gets saved
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const MeshData& mesh, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const MeshData& mesh, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const MeshData& mesh);
        #endif

        /**
         * @brief Add a set of mesh levels
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::MeshLevels together with
         * @relativeref{SceneConverterFeature,AddMeshes} is supported. The
         * returned ID is implicitly equal to @ref meshCount() before calling
         * this function and can be subsequently used to for example reference
         * a mesh from a @ref SceneData passed to
         * @ref add(const SceneData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added meshes doesn't
         * change in that case.
         *
         * If the converter doesn't support mesh naming, @p name is ignored.
         * @see @ref isConverting(), @ref features(),
         *      @ref setMeshAttributeName()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const MeshData>> meshLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const MeshData>> meshLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const MeshData>> meshLevels);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const MeshData>> meshLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const MeshData>> meshLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const MeshData>> meshLevels);
        #endif

        /**
         * @brief Set name of a custom mesh attribute
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress,
         * @ref SceneConverterFeature::AddMeshes is supported and @p attribute
         * is a custom attribute. The attribute name will get used only mesh
         * data added after this function has been called. If the converter
         * doesn't support custom mesh attributes or doesn't support naming
         * them, the call is ignored.
         * @see @ref isConverting(), @ref features(),
         *      @ref isMeshAttributeCustom(), @ref setSceneFieldName()
         */
        void setMeshAttributeName(MeshAttribute attribute, Containers::StringView name);

        /**
         * @brief Count of added materials
         * @m_since_latest
         *
         * Count of materials successfully added with
         * @ref add(const MaterialData&, Containers::StringView) since the
         * initial @ref begin(), @ref beginData() or @ref beginFile() call.
         * Expects that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddMaterials is not supported, returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt materialCount() const;

        /**
         * @brief Add a material
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::AddMaterials is supported. The returned
         * ID is implicitly equal to @ref materialCount() before calling this
         * function and can be subsequently used to for example reference a
         * material from a @ref SceneData passed to
         * @ref add(const SceneData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added materials doesn't
         * change in that case.
         *
         * If the converter doesn't support material naming, @p name is
         * ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const MaterialData& material, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const MaterialData& material, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const MaterialData& material);
        #endif

        /**
         * @brief Count of added textures
         * @m_since_latest
         *
         * Count of textures successfully added with
         * @ref add(const TextureData&, Containers::StringView) since the
         * initial @ref begin(), @ref beginData() or @ref beginFile() call.
         * Expects that a conversion is currently in progress. If
         * @ref SceneConverterFeature::AddTextures is not supported, returns
         * @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt textureCount() const;

        /**
         * @brief Add a texture
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and
         * @ref SceneConverterFeature::AddTextures is supported. The returned
         * ID is implicitly equal to @ref textureCount() before calling this
         * function and can be subsequently used to for example reference a
         * texture from a @ref MaterialData passed to
         * @ref add(const MaterialData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added textures doesn't
         * change in that case.
         *
         * If the converter doesn't support texture naming, @p name is ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const TextureData& texture, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const TextureData& texture, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const TextureData& texture);
        #endif

        /**
         * @brief Count of added 1D images
         * @m_since_latest
         *
         * Count of images successfully added with
         * @ref add(const ImageData1D&, Containers::StringView) or
         * @ref add(Containers::ArrayView<const Containers::AnyReference<const ImageData1D>>, Containers::StringView)
         * and overloads since the initial @ref begin(), @ref beginData() or
         * @ref beginFile() call. Expects that a conversion is currently in
         * progress. If neither @ref SceneConverterFeature::AddImages1D nor
         * @relativeref{SceneConverterFeature,AddCompressedImages1D} is
         * supported, returns @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt image1DCount() const;

        /**
         * @brief Add a 1D image
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and either
         * @ref SceneConverterFeature::AddImages1D or
         * @relativeref{SceneConverterFeature,AddCompressedImages1D} is
         * supported based on whether @p image is compressed. The image is
         * expected to not be @cpp nullptr @ce and to have a non-zero size. The
         * returned ID is implicitly equal to @ref image1DCount() before
         * calling this function and can be subsequently used to for example
         * reference an image from a @ref TextureData passed to
         * @ref add(const TextureData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added images doesn't
         * change in that case.
         *
         * If the converter doesn't support image naming, @p name is ignored.
         * @see @ref isConverting(), @ref features(),
         *      @ref ImageData::isCompressed()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const ImageData1D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const ImageData1D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const ImageData1D& image);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const ImageView1D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const ImageView1D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const ImageView1D& image);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const CompressedImageView1D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const CompressedImageView1D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const CompressedImageView1D& image);
        #endif

        /**
         * @brief Add a set of 1D image levels
         * @m_since_latest
         *
         * The @p imageLevels are expected to have at least one image, with the
         * images either all uncompressed or all compressed and all sharing the
         * same pixel format. Expects that a conversion is currently in
         * progress and @relativeref{SceneConverterFeature,ImageLevels}
         * together with either @ref SceneConverterFeature::AddImages1D or
         * @relativeref{SceneConverterFeature,AddCompressedImages1D} is
         * supported based on whether @p imageLevels are compressed. The
         * returned ID is implicitly equal to @ref image1DCount() before
         * calling this function and can be subsequently used to for example
         * reference an image from a @ref TextureData passed to
         * @ref add(const TextureData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added images doesn't
         * change in that case.
         *
         * If the converter doesn't support image naming, @p name is ignored.
         * @see @ref isConverting(), @ref features(),
         *      @ref ImageData::isCompressed()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData1D>> imageLevels);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView1D> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView1D> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView1D> imageLevels);
        #endif

// TODO makes passing {ImageData} ambiguous, what to do?!
//         /**
//          * @overload
//          * @m_since_latest
//          */
//         #ifdef DOXYGEN_GENERATING_OUTPUT
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView1D> imageLevels, Containers::StringView name = {});
//         #else
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView1D> imageLevels, Containers::StringView name);
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView1D> imageLevels);
//         #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView1D> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView1D> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView1D> imageLevels);
        #endif

// TODO makes passing {ImageData} ambiguous, what to do?!
//         /**
//          * @overload
//          * @m_since_latest
//          */
//         #ifdef DOXYGEN_GENERATING_OUTPUT
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView1D> imageLevels, Containers::StringView name = {});
//         #else
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView1D> imageLevels, Containers::StringView name);
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView1D> imageLevels);
//         #endif

        /**
         * @brief Count of added 2D images
         * @m_since_latest
         *
         * Count of images successfully added with
         * @ref add(const ImageData2D&, Containers::StringView) or
         * @ref add(Containers::ArrayView<const Containers::AnyReference<const ImageData2D>>, Containers::StringView)
         * and overloads since the initial @ref begin(), @ref beginData() or
         * @ref beginFile() call. Expects that a conversion is currently in
         * progress. If neither @ref SceneConverterFeature::AddImages2D nor
         * @relativeref{SceneConverterFeature,AddCompressedImages2D} is
         * supported, returns @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt image2DCount() const;

        /**
         * @brief Add a 2D image
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and either
         * @ref SceneConverterFeature::AddImages2D or
         * @relativeref{SceneConverterFeature,AddCompressedImages2D} is
         * supported based on whether @p image is compressed. The image is
         * expected to not be @cpp nullptr @ce and to have a non-zero size. The
         * returned ID is implicitly equal to @ref image2DCount() before
         * calling this function and can be subsequently used to for example
         * reference an image from a @ref TextureData passed to
         * @ref add(const TextureData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added images doesn't
         * change in that case.
         *
         * If the converter doesn't support image naming, @p name is ignored.
         * @see @ref isConverting(), @ref features(),
         *      @ref ImageData::isCompressed()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const ImageData2D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const ImageData2D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const ImageData2D& image);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const ImageView2D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const ImageView2D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const ImageView2D& image);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const CompressedImageView2D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const CompressedImageView2D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const CompressedImageView2D& image);
        #endif

        /**
         * @brief Add a set of 2D image levels
         * @m_since_latest
         *
         * The @p imageLevels are expected to have at least one image, with the
         * images either all uncompressed or all compressed and all sharing the
         * same pixel format. Expects that a conversion is currently in
         * progress and @relativeref{SceneConverterFeature,ImageLevels}
         * together with either @ref SceneConverterFeature::AddImages2D or
         * @relativeref{SceneConverterFeature,AddCompressedImages2D} is
         * supported based on whether @p imageLevels are compressed. The
         * returned ID is implicitly equal to @ref image2DCount() before
         * calling this function and can be subsequently used to for example
         * reference an image from a @ref TextureData passed to
         * @ref add(const TextureData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added images doesn't
         * change in that case.
         *
         * If the converter doesn't support image naming, @p name is ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData2D>> imageLevels);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView2D> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView2D> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView2D> imageLevels);
        #endif

// TODO makes passing {ImageData} ambiguous, what to do?!
//         /**
//          * @overload
//          * @m_since_latest
//          */
//         #ifdef DOXYGEN_GENERATING_OUTPUT
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView2D> imageLevels, Containers::StringView name = {});
//         #else
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView2D> imageLevels, Containers::StringView name);
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView2D> imageLevels);
//         #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView2D> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView2D> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView2D> imageLevels);
        #endif

// TODO makes passing {ImageData} ambiguous, what to do?!
//         /**
//          * @overload
//          * @m_since_latest
//          */
//         #ifdef DOXYGEN_GENERATING_OUTPUT
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView2D> imageLevels, Containers::StringView name = {});
//         #else
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView2D> imageLevels, Containers::StringView name);
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView2D> imageLevels);
//         #endif

        /**
         * @brief Count of added 3D images
         * @m_since_latest
         *
         * Count of images successfully added with
         * @ref add(const ImageData3D&, Containers::StringView) or
         * @ref add(Containers::ArrayView<const Containers::AnyReference<const ImageData3D>>, Containers::StringView)
         * and overloads since the initial @ref begin(), @ref beginData() or
         * @ref beginFile() call. Expects that a conversion is currently in
         * progress. If neither @ref SceneConverterFeature::AddImages3D nor
         * @relativeref{SceneConverterFeature,AddCompressedImages3D} is
         * supported, returns @cpp 0 @ce.
         * @see @ref isConverting(), @ref features()
         */
        UnsignedInt image3DCount() const;

        /**
         * @brief Add a 3D image
         * @m_since_latest
         *
         * Expects that a conversion is currently in progress and either
         * @ref SceneConverterFeature::AddImages3D or
         * @relativeref{SceneConverterFeature,AddCompressedImages3D} is
         * supported based on whether @p image is compressed. The image is
         * expected to not be @cpp nullptr @ce and to have a non-zero size. The
         * returned ID is implicitly equal to @ref image3DCount() before
         * calling this function and can be subsequently used to for example
         * reference an image from a @ref TextureData passed to
         * @ref add(const TextureData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added images doesn't
         * change in that case.
         *
         * If the converter doesn't support image naming, @p name is ignored.
         * @see @ref isConverting(), @ref features(),
         *      @ref ImageData::isCompressed()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const ImageData3D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const ImageData3D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const ImageData3D& image);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const ImageView3D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const ImageView3D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const ImageView3D& image);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(const CompressedImageView3D& image, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(const CompressedImageView3D& image, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(const CompressedImageView3D& image);
        #endif

        /**
         * @brief Add a set of 3D image levels
         * @m_since_latest
         *
         * The @p imageLevels are expected to have at least one image, with the
         * images either all uncompressed or all compressed and all sharing the
         * same pixel format. Expects that a conversion is currently in
         * progress and @relativeref{SceneConverterFeature,ImageLevels}
         * together with either @ref SceneConverterFeature::AddImages3D or
         * @relativeref{SceneConverterFeature,AddCompressedImages3D} is
         * supported based on whether @p imageLevels are compressed. The
         * returned ID is implicitly equal to @ref image3DCount() before
         * calling this function and can be subsequently used to for example
         * reference an image from a @ref TextureData passed to
         * @ref add(const TextureData&, Containers::StringView). On failure
         * prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt --- the count of added images doesn't
         * change in that case.
         *
         * If the converter doesn't support image naming, @p name is ignored.
         * @see @ref isConverting(), @ref features()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(std::initializer_list<Containers::AnyReference<const ImageData3D>> imageLevels);
        #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView3D> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView3D> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const ImageView3D> imageLevels);
        #endif

// TODO makes passing {ImageData} ambiguous, what to do?!
//         /**
//          * @overload
//          * @m_since_latest
//          */
//         #ifdef DOXYGEN_GENERATING_OUTPUT
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView3D> imageLevels, Containers::StringView name = {});
//         #else
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView3D> imageLevels, Containers::StringView name);
//         Containers::Optional<UnsignedInt> add(std::initializer_list<ImageView3D> imageLevels);
//         #endif

        /**
         * @overload
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView3D> imageLevels, Containers::StringView name = {});
        #else
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView3D> imageLevels, Containers::StringView name);
        Containers::Optional<UnsignedInt> add(Containers::ArrayView<const CompressedImageView3D> imageLevels);
        #endif

// TODO makes passing {ImageData} ambiguous, what to do?!
//         /**
//          * @overload
//          * @m_since_latest
//          */
//         #ifdef DOXYGEN_GENERATING_OUTPUT
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView3D> imageLevels, Containers::StringView name = {});
//         #else
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView3D> imageLevels, Containers::StringView name);
//         Containers::Optional<UnsignedInt> add(std::initializer_list<CompressedImageView3D> imageLevels);
//         #endif

    protected:
        /**
         * @brief Implementation for @ref convertToFile(const MeshData&, Containers::StringView)
         *
         * If @ref SceneConverterFeature::ConvertMeshToData is supported,
         * default implementation calls @ref doConvertToData(const MeshData&)
         * and saves the result to given file. It is allowed to call this
         * function from your @ref doConvertToFile() implementation, for
         * example when you only need to do format detection based on file
         * extension.
         *
         * Otherwise, if both @ref SceneConverterFeature::ConvertMultipleToFile
         * and @ref SceneConverterFeature::AddMeshes is supported, default
         * implementation calls @ref beginData(),
         * @ref add(const MeshData&, Containers::StringView) and returns the
         * output of @ref endData(), or @ref Containers::NullOpt if any of
         * those failed.
         */
        virtual bool doConvertToFile(const MeshData& mesh, Containers::StringView filename);

        /**
         * @brief Implementation for @ref beginFile()
         * @m_since_latest
         *
         * If @ref SceneConverterFeature::ConvertMultipleToData is supported,
         * default implementation delegates to @ref doBeginData().
         *
         * Otherwise, if @ref SceneConverterFeature::ConvertMeshToFile is
         * supported, default implementation sets up internal state in order
         * to delegate @ref add(const MeshData&, Containers::StringView) to
         * @ref doConvertToFile(const MeshData&, Containers::StringView) and
         * return the result from @ref endFile().
         *
         * It is allowed to call this function from your @ref doBeginFile()
         * implementation, for example when you only need to do format
         * detection based on file extension.
         */
        virtual void doBeginFile(Containers::StringView filename);

        /**
         * @brief Implementation for @ref endFile()
         * @m_since_latest
         *
         * Receives the same @p filename as was passed to @ref doBeginFile()
         * earlier. Expected to save the output data and reset the internal
         * state for a potential new conversion to happen.
         *
         * If @ref SceneConverterFeature::ConvertMultipleToData is supported,
         * default implementation calls @ref doEndData() and saves the result
         * to given file.
         *
         * If @ref SceneConverterFeature::ConvertMeshToData is supported,
         * default implementation returns data converted via the
         * @ref add(const MeshData&, Containers::StringView) call, which
         * delegated to @ref doConvertToData(const MeshData&). If no mesh was
         * added, prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt.
         *
         * It is allowed to call this function from your @ref doEndFile()
         * implementation, for example when you only need to do format
         * detection based on file extension.
         */
        virtual bool doEndFile(Containers::StringView filename);

    private:
        struct State;

        /**
         * @brief Implementation for @ref features()
         *
         * The implementation is expected to support at least one feature.
         */
        virtual SceneConverterFeatures doFeatures() const = 0;

        /**
         * @brief Implementation for @ref setFlags()
         *
         * Useful when the converter needs to modify some internal state on
         * flag setup. Default implementation does nothing and this
         * function doesn't need to be implemented --- the flags are available
         * through @ref flags().
         *
         * To reduce the amount of error checking on user side, this function
         * isn't expected to fail --- if a flag combination is invalid /
         * unsuported, error reporting should be delayed to various conversion
         * functions, where the user is expected to do error handling anyway.
         */
        virtual void doSetFlags(SceneConverterFlags flags);

        /** @brief Implementation for @ref convert(const MeshData&) */
        virtual Containers::Optional<MeshData> doConvert(const MeshData& mesh);

        /** @brief Implementation for @ref convertInPlace(MeshData&) */
        virtual bool doConvertInPlace(MeshData& mesh);

        /**
         * @brief Implementation for @ref convertToData(const MeshData&)
         *
         * If both @ref SceneConverterFeature::ConvertMultipleToData and
         * @ref SceneConverterFeature::AddMeshes is supported, default
         * implementation calls @ref beginData(),
         * @ref add(const MeshData&, Containers::StringView) and returns the
         * output of @ref endData(), or @ref Containers::NullOpt if any of
         * those failed.
         */
        virtual Containers::Optional<Containers::Array<char>> doConvertToData(const MeshData& mesh);

        /**
         * @brief Implementation for @ref abort()
         * @m_since_latest
         *
         * Default implementation does nothing.
         */
        virtual void doAbort();

        /**
         * @brief Implementation for @ref begin()
         * @m_since_latest
         *
         * If @ref SceneConverterFeature::ConvertMesh is supported, default
         * implementation sets up internal state in order to delegate
         * @ref add(const MeshData&, Containers::StringView) to
         * @ref doConvert(const MeshData&) and return the result from
         * @ref end().
         */
        virtual void doBegin();

        /**
         * @brief Implementation for @ref end()
         * @m_since_latest
         *
         * Expected to return an importer instance owning all output data and
         * reset the internal state for a potential new conversion to happen.
         *
         * If @ref SceneConverterFeature::ConvertMesh is supported, default
         * implementation returns an importer exposing a single mesh that was
         * converted via the @ref add(const MeshData&, Containers::StringView)
         * call, which delegated to @ref doConvert(const MeshData&). If no mesh
         * was added, prints a message to @relativeref{Magnum,Error} and
         * returns @cpp nullptr @ce.
         */
        virtual Containers::Pointer<AbstractImporter> doEnd();

        /**
         * @brief Implementation for @ref beginData()
         * @m_since_latest
         *
         * If @ref SceneConverterFeature::ConvertMeshToData is supported,
         * default implementation sets up internal state in order to delegate
         * @ref add(const MeshData&, Containers::StringView) to
         * @ref doConvertToData(const MeshData&) and return the result from
         * @ref endData().
         */
        virtual void doBeginData();

        /**
         * @brief Implementation for @ref endData()
         * @m_since_latest
         *
         * Expected to return the output data and reset the internal state for
         * a potential new conversion to happen.
         *
         * If @ref SceneConverterFeature::ConvertMeshToData is supported,
         * default implementation returns data converted via the
         * @ref add(const MeshData&, Containers::StringView) call, which
         * delegated to @ref doConvertToData(const MeshData&). If no mesh was
         * added, prints a message to @relativeref{Magnum,Error} and returns
         * @ref Containers::NullOpt.
         */
        virtual Containers::Optional<Containers::Array<char>> doEndData();

        /**
         * @brief Implementation for @ref add(const SceneData&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref sceneCount() at the time this function is
         * called.
         */
        virtual bool doAdd(UnsignedInt id, const SceneData& scene, Containers::StringView name);

        /**
         * @brief Implementation for @ref setSceneFieldName()
         * @m_since_latest
         *
         * Receives the custom ID extracted via
         * @ref sceneFieldCustom(SceneField). Default implementation does
         * nothing.
         */
        virtual void doSetSceneFieldName(UnsignedInt field, Containers::StringView name);

        /**
         * @brief Implementation for @ref setObjectName()
         * @m_since_latest
         *
         * Default implementation does nothing.
         */
        virtual void doSetObjectName(UnsignedLong object, Containers::StringView name);

        /**
         * @brief Implementation for @ref setDefaultScene()
         * @m_since_latest
         *
         * Default implementation does nothing.
         */
        virtual void doSetDefaultScene(UnsignedInt id);

        /**
         * @brief Implementation for @ref add(const AnimationData&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref animationCount() at the time this
         * function is called.
         */
        virtual bool doAdd(UnsignedInt id, const AnimationData& animation, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const LightData&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref lightCount() at the time this function is
         * called.
         */
        virtual bool doAdd(UnsignedInt id, const LightData& light, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const CameraData&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref cameraCount() at the time this function
         * is called.
         */
        virtual bool doAdd(UnsignedInt id, const CameraData& camera, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const SkinData2D&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref skin2DCount() at the time this function
         * is called.
         */
        virtual bool doAdd(UnsignedInt id, const SkinData2D& skin, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const SkinData3D&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref skin3DCount() at the time this function
         * is called.
         */
        virtual bool doAdd(UnsignedInt id, const SkinData3D& skin, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const MeshData&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref meshCount() at the time this function is
         * called.
         *
         * If @ref SceneConverterFeature::AddMeshes together with
         * @relativeref{SceneConverterFeature,MeshLevels} is supported, default
         * implementation calls @ref doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const MeshData>>, Containers::StringView)
         * with just the single @p mesh.
         *
         * Otherwise, if @ref SceneConverterFeature::ConvertMesh,
         * @relativeref{SceneConverterFeature,ConvertMeshToData} or
         * @relativeref{SceneConverterFeature,ConvertMeshToFile} is supported,
         * default implementation delegates to @ref doConvert(const MeshData&),
         * @ref doConvertToData(const MeshData&) or
         * @ref doConvertToFile(const MeshData&, Containers::StringView) and
         * remembers the result to return it from @ref doEnd(),
         * @ref doEndData() or @ref doEndFile(). If the delegated-to function
         * fails, returns @cpp false @ce and the subsequent @ref doEnd(),
         * @ref doEndData() or @ref doEndFile() call prints a message to
         * @relativeref{Magnum,Error} and returns a @cpp nullptr @ce,
         * @ref Containers::NullOpt or @cpp false @ce. Since the delegation
         * operates just on a single mesh at a time, if this function is called
         * more than once after a @ref begin(), @ref beginData() or
         * @ref beginFile(), prints a message to @relativeref{Magnum,Error} and
         * returns @cpp false @ce.
         */
        virtual bool doAdd(UnsignedInt id, const MeshData& mesh, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(Containers::ArrayView<const Containers::AnyReference<const MeshData>>, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref meshCount() at the time this function
         * is called.
         */
        virtual bool doAdd(UnsignedInt id, Containers::ArrayView<const Containers::AnyReference<const MeshData>> meshLevels, Containers::StringView name);

        /**
         * @brief Implementation for @ref setMeshAttributeName()
         * @m_since_latest
         *
         * Receives the custom ID extracted via
         * @ref meshAttributeCustom(MeshAttribute). Default implementation does
         * nothing.
         */
        virtual void doSetMeshAttributeName(UnsignedShort attribute, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const MaterialData&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref materialCount() at the time this function
         * is called.
         */
        virtual bool doAdd(UnsignedInt id, const MaterialData& material, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const TextureData&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref textureCount() at the time this function
         * is called.
         */
        virtual bool doAdd(UnsignedInt id, const TextureData& texture, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const ImageData1D&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref image1DCount() at the time this function
         * is called. If @ref add(const ImageView1D&, Containers::StringView)
         * or @ref add(const CompressedImageView1D&, Containers::StringView)
         * was called, receives the view wrapped in a non-owning
         * @ref ImageData1D instance.
         *
         * If @ref SceneConverterFeature::ImageLevels is supported, default
         * implementation calls @ref doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData1D>>, Containers::StringView)
         * with just the single @p image.
         */
        virtual bool doAdd(UnsignedInt id, const ImageData1D& image, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(Containers::ArrayView<const Containers::AnyReference<const ImageData1D>>, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref image1DCount() at the time this function
         * is called. If @ref add(Containers::ArrayView<const ImageView1D>, Containers::StringView)
         * or @ref add(Containers::ArrayView<const CompressedImageView1D>, Containers::StringView)
         * was called, receives the views wrapped in non-owning
         * @ref ImageData1D instances.
         */
        virtual bool doAdd(UnsignedInt id, Containers::ArrayView<const Containers::AnyReference<const ImageData1D>> imageLevels, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const ImageData2D&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref image2DCount() at the time this function
         * is called. If @ref add(const ImageView2D&, Containers::StringView)
         * or @ref add(const CompressedImageView2D&, Containers::StringView)
         * was called, receives the view wrapped in a non-owning
         * @ref ImageData2D instance.
         *
         * If @ref SceneConverterFeature::ImageLevels is supported, default
         * implementation calls @ref doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData2D>>, Containers::StringView)
         * with just the single @p image.
         */
        virtual bool doAdd(UnsignedInt id, const ImageData2D& image, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(Containers::ArrayView<const Containers::AnyReference<const ImageData2D>>, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref image2DCount() at the time this function
         * is called. If @ref add(Containers::ArrayView<const ImageView2D>, Containers::StringView)
         * or @ref add(Containers::ArrayView<const CompressedImageView2D>, Containers::StringView)
         * was called, receives the views wrapped in non-owning
         * @ref ImageData2D instances.
         */
        virtual bool doAdd(UnsignedInt id, Containers::ArrayView<const Containers::AnyReference<const ImageData2D>> imageLevels, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(const ImageData3D&, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref image3DCount() at the time this function
         * is called. If @ref add(const ImageView3D&, Containers::StringView)
         * or @ref add(const CompressedImageView3D&, Containers::StringView)
         * was called, receives the view wrapped in a non-owning
         * @ref ImageData3D instance.
         *
         * If @ref SceneConverterFeature::ImageLevels is supported, default
         * implementation calls @ref doAdd(UnsignedInt, Containers::ArrayView<const Containers::AnyReference<const ImageData3D>>, Containers::StringView)
         * with just the single @p image.
         */
        virtual bool doAdd(UnsignedInt id, const ImageData3D& image, Containers::StringView name);

        /**
         * @brief Implementation for @ref add(Containers::ArrayView<const Containers::AnyReference<const ImageData3D>>, Containers::StringView)
         * @m_since_latest
         *
         * The @p id is equal to @ref image3DCount() at the time this function
         * is called. If @ref add(Containers::ArrayView<const ImageView3D>, Containers::StringView)
         * or @ref add(Containers::ArrayView<const CompressedImageView3D>, Containers::StringView)
         * was called, receives the views wrapped in non-owning
         * @ref ImageData3D instances.
         */
        virtual bool doAdd(UnsignedInt id, Containers::ArrayView<const Containers::AnyReference<const ImageData3D>> imageLevels, Containers::StringView name);

        SceneConverterFlags _flags;
        Containers::Pointer<State> _state;
};

}}

/* clang-cl complains about "explicit instantiation of undefined template"
   here, so don't define anything */
#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG)) && !defined(CORRADE_TARGET_CLANG_CL)
namespace Corrade { namespace PluginManager {
    extern template class MAGNUM_TRADE_EXPORT Manager<Magnum::Trade::AbstractSceneConverter>;
}}
#endif

#endif
