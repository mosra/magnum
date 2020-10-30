#ifndef Magnum_Trade_AbstractSceneConverter_h
#define Magnum_Trade_AbstractSceneConverter_h
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
 * @brief Class @ref Magnum::Trade::AbstractSceneConverter, enum @ref Magnum::Trade::SceneConverterFeature, enum set @ref Magnum::Trade::SceneConverterFeatures
 * @m_since{2020,06}
 */

#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Features supported by a scene converter
@m_since{2020,06}

@see @ref SceneConverterFeatures, @ref AbstractSceneConverter::features()
*/
enum class SceneConverterFeature: UnsignedByte {
    /**
     * Convert a mesh with
     * @ref AbstractSceneConverter::convert(const MeshData&).
     */
    ConvertMesh = 1 << 0,

    /**
     * Convert a mesh in-place with
     * @ref AbstractSceneConverter::convertInPlace(MeshData&).
     */
    ConvertMeshInPlace = 1 << 1,

    /**
     * Converting a mesh to a file with
     * @ref AbstractSceneConverter::convertToFile(const std::string&, const MeshData&).
     */
    ConvertMeshToFile = 1 << 2,

    /**
     * Converting a mesh to raw data with
     * @ref AbstractSceneConverter::convertToData(const MeshData&). Implies
     * @ref SceneConverterFeature::ConvertMeshToFile.
     */
    ConvertMeshToData = ConvertMeshToFile|(1 << 3)
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

/**
@brief Base for scene converter plugins
@m_since{2020,06}

Provides functionality for converting meshes and other scene data between
various formats or performing optimizations and other operations on them. See
@ref plugins for more information and `*SceneConverter` classes in the
@ref Trade namespace for available scene converter plugins.

@m_class{m-note m-success}

@par
    There's also a @ref magnum-sceneconverter "magnum-sceneconverter" tool,
    exposing functionality of all scene converter plugins on a command line as
    well as performing introspection of scene files.

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
@ref doConvert(), @ref doConvertInPlace(), @ref doConvertToData() or
@ref doConvertToFile() functions based on what features are supported.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The function @ref doConvert(const MeshData&) is called only if
    @ref SceneConverterFeature::ConvertMesh is supported.
-   The function @ref doConvertInPlace(MeshData&) is called only if
    @ref SceneConverterFeature::ConvertMeshInPlace is supported.
-   The function @ref doConvertToData(const MeshData&) is called only if
    @ref SceneConverterFeature::ConvertMeshToData is supported.
-   The function @ref doConvertToFile(const std::string&, const MeshData&) is
    called only if @ref SceneConverterFeature::ConvertMeshToFile is supported.

@m_class{m-block m-warning}

@par Dangling function pointers on plugin unload
    As @ref Trade-AbstractSceneConverter-data-dependency "mentioned above",
    @ref Corrade::Containers::Array instances returned from plugin
    implementations are not allowed to use anything else than the default
    deleter or the deleter used by @ref Trade::ArrayAllocator, otherwise this
    could cause dangling function pointer call on array destruction if the
    plugin gets unloaded before the array is destroyed. This is asserted by the
    base implementation on return.
*/
class MAGNUM_TRADE_EXPORT AbstractSceneConverter: public PluginManager::AbstractManagingPlugin<AbstractSceneConverter> {
    public:
        /**
         * @brief Plugin interface
         *
         * @snippet Magnum/Trade/AbstractSceneConverter.cpp interface
         */
        static std::string pluginInterface();

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
        static std::vector<std::string> pluginSearchPaths();
        #endif

        /** @brief Default constructor */
        explicit AbstractSceneConverter();

        /** @brief Constructor with access to plugin manager */
        explicit AbstractSceneConverter(PluginManager::Manager<AbstractSceneConverter>& manager);

        /** @brief Plugin manager constructor */
        explicit AbstractSceneConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

        /** @brief Features supported by this converter */
        SceneConverterFeatures features() const;

        /** @brief Converter flags */
        SceneConverterFlags flags() const { return _flags; }

        /**
         * @brief Set converter flags
         *
         * Some flags can be set only if the converter supports particular
         * features, see documentation of each @ref SceneConverterFlag for more
         * information. By default no flags are set.
         */
        void setFlags(SceneConverterFlags flags);

        /**
         * @brief Convert a mesh
         *
         * Depending on the plugin, can perform for example vertex format
         * conversion, overdraw optimization or decimation / subdivision.
         * Available only if @ref SceneConverterFeature::ConvertMesh is
         * supported.
         * @see @ref features(), @ref convertInPlace(MeshData&)
         */
        Containers::Optional<MeshData> convert(const MeshData& mesh);

        /**
         * @brief Convert a mesh in-place
         *
         * Depending on the plugin, can perform for example index buffer
         * reordering for better vertex cache use or overdraw optimization.
         * Available only if @ref SceneConverterFeature::ConvertMeshInPlace is
         * supported. Returns @cpp true @ce if the operation succeeded. On
         * failure the function prints an error message and returns
         * @cpp false @ce, @p mesh is guaranteed to stay unchanged.
         * @see @ref features(), @ref convert(const MeshData&)
         */
        bool convertInPlace(MeshData& mesh);

        /**
         * @brief Convert a mesh to a raw data
         *
         * Depending on the plugin, can convert the mesh to a file format that
         * can be saved to disk. Available only if
         * @ref SceneConverterFeature::ConvertMeshToData is supported. On
         * failure the function prints an error message and returns
         * @cpp nullptr @ce.
         * @see @ref features(), @ref convertToFile()
         */
        Containers::Array<char> convertToData(const MeshData& mesh);

        /**
         * @brief Convert a mesh to a file
         *
         * Available only if @ref SceneConverterFeature::ConvertMeshToFile or
         * @ref SceneConverterFeature::ConvertMeshToData is supported. Returns
         * @cpp true @ce on success, prints an error message and returns
         * @cpp false @ce otherwise.
         * @see @ref features(), @ref convertToData()
         */
        bool convertToFile(const std::string& filename, const MeshData& mesh);

    private:
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

        /** @brief Implementation for @ref convertToData(const MeshData&) */
        virtual Containers::Array<char> doConvertToData(const MeshData& mesh);

        /**
         * @brief Implementation for @ref convertToFile(const std::string&, const MeshData&)
         *
         * If @ref SceneConverterFeature::ConvertMeshToData is supported,
         * default implementation calls @ref doConvertToData(const MeshData&)
         * and saves the result to given file.
         */
        virtual bool doConvertToFile(const std::string& filename, const MeshData& mesh);

        SceneConverterFlags _flags;
};

}}

#endif
