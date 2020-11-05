#ifndef Magnum_ShaderTools_AbstractConverter_h
#define Magnum_ShaderTools_AbstractConverter_h
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
 * @brief Class @ref Magnum::ShaderTools::AbstractConverter, enum @ref Magnum::ShaderTools::ConverterFeature, @ref Magnum::ShaderTools::ConverterFlag, @ref Magnum::ShaderTools::Format, @ref Magnum::ShaderTools::Stage, enum set @ref Magnum::ShaderTools::ConverterFeatures, @ref Magnum::ShaderTools::ConverterFlags
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/ShaderTools/visibility.h"

namespace Magnum { namespace ShaderTools {

/**
@brief Features supported by a shader converter
@m_since_latest

@see @ref ConverterFeatures, @ref AbstractConverter::features()
*/
enum class ConverterFeature: UnsignedInt {
    /**
     * Validate shader file with @ref AbstractConverter::validateFile()
     */
    ValidateFile = 1 << 0,

    /**
     * Validate shader data with @ref AbstractConverter::validateData().
     * Implies @ref ConverterFeature::ValidateData.
     */
    ValidateData = ValidateFile|(1 << 1),

    /**
     * Convert shader file to a file with @ref AbstractConverter::convertFileToFile()
     */
    ConvertFile = 1 << 2,

    /**
     * Convert shader data to data with
     * @ref AbstractConverter::convertDataToData() or any of the other
     * @ref AbstractConverter::convertDataToFile(),
     * @ref AbstractConverter::convertFileToData() combinations. Implies
     * @ref ConverterFeature::ConvertFile.
     */
    ConvertData = ConvertFile|(1 << 3),

    /**
     * Link shader files together and output a file with
     * @ref AbstractConverter::linkFilesToFile()
     */
    LinkFile = 1 << 4,

    /**
     * Link shader data together and output a data with
     * @ref AbstractConverter::linkDataToData() or any of the other
     * @ref AbstractConverter::linkDataToFile(),
     * @ref AbstractConverter::linkFilesToData() combinations. Implies
     * @ref ConverterFeature::LinkFile.
     */
    LinkData = LinkFile|(1 << 5),

    /**
     * Specifying input file callbacks for additional files referenced from the
     * main file using @ref AbstractConverter::setInputFileCallback(). If the
     * converter doesn't expose this feature, the format is either single-file
     * or input file callbacks are not supported.
     *
     * See @ref ShaderTools-AbstractConverter-usage-callbacks and particular
     * converter documentation for more information.
     */
    InputFileCallback = 1 << 6,

    /**
     * Set preprocess definitions using @ref AbstractConverter::setDefinitions()
     * and the @ref ConverterFlag::PreprocessOnly flag.
     */
    Preprocess = 1 << 7,

    /**
     * Control code optimization using
     * @ref AbstractConverter::setOptimizationLevel()
     */
    Optimize = 1 << 8,

    /**
     * Control amount of debug info present in the output using
     * @ref AbstractConverter::setDebugInfoLevel()
     */
    DebugInfo = 1 << 9
};

/**
@brief Features supported by a shader converter
@m_since_latest

@see @ref AbstractConverter::features()
*/
typedef Containers::EnumSet<ConverterFeature> ConverterFeatures;

CORRADE_ENUMSET_OPERATORS(ConverterFeatures)

/**
@debugoperatorenum{ConverterFeature}
@m_since_latest
*/
MAGNUM_SHADERTOOLS_EXPORT Debug& operator<<(Debug& debug, ConverterFeature value);

/**
@debugoperatorenum{ConverterFeatures}
@m_since_latest
*/
MAGNUM_SHADERTOOLS_EXPORT Debug& operator<<(Debug& debug, ConverterFeatures value);

/**
@brief Shader converter flag
@m_since_latest

@see @ref ConverterFlags, @ref AbstractConverter::setFlags()
*/
enum class ConverterFlag: UnsignedInt {
    /**
     * Suppress warnings, print just errors. By default the converter prints
     * both warnings and errors. Corresponds to the `-q` / `--quiet` option in
     * @ref magnum-shaderconverter "magnum-shaderconverter".
     * @see @ref ConverterFlag::WarningAsError
     */
    Quiet = 1 << 0,

    /**
     * Print verbose diagnostic. By default the converter only prints warnings
     * and errors. Corresponds to the `-v` / `--verbose` option in
     * @ref magnum-shaderconverter "magnum-shaderconverter".
     */
    Verbose = 1 << 1,

    /**
     * Treat warnings as error. By default, if a warning occurs,
     * validation or conversion succeeds. With this flag set, it fails.
     * Corresponds to the `--warning-as-error` option in
     * @ref magnum-shaderconverter "magnum-shaderconverter".
     * @see @ref ConverterFlag::Quiet
     */
    WarningAsError = 1 << 2,

    /**
     * Only run the preprocessor. Available only if the converter supports
     * @ref ConverterFeature::Preprocess, not allowed in combination with
     * @ref AbstractConverter::linkDataToData(),
     * @ref AbstractConverter::linkDataToFile(),
     * @ref AbstractConverter::linkFilesToFile() or
     * @ref AbstractConverter::linkFilesToData(). Corresponds to the `-E` /
     * `--preprocess-only` option in
     * @ref magnum-shaderconverter "magnum-shaderconverter".
     */
    PreprocessOnly = 1 << 3
};

/**
@brief Shader converter flags
@m_since_latest

@see @ref AbstractConverter::setFlags()
*/
typedef Containers::EnumSet<ConverterFlag> ConverterFlags;

CORRADE_ENUMSET_OPERATORS(ConverterFlags)

/**
@debugoperatorenum{ConverterFlag}
@m_since_latest
*/
MAGNUM_SHADERTOOLS_EXPORT Debug& operator<<(Debug& debug, ConverterFlag value);

/**
@debugoperatorenum{ConverterFlags}
@m_since_latest
*/
MAGNUM_SHADERTOOLS_EXPORT Debug& operator<<(Debug& debug, ConverterFlags value);

/**
@brief Shader format
@m_since_latest

Describes input and output shader format.
@see @ref AbstractConverter::setInputFormat(),
    @ref AbstractConverter::setOutputFormat()
*/
enum class Format: UnsignedInt {
    /**
     * Either leaves format detection up to the implementation or describes a
     * format not fitting into any other categories. This includes various
     * application-specific languages and language flavors, compressed or
     * encrypted data and other.
     *
     * This value is guaranteed to be @cpp 0 @ce, which means you're encouraged
     * to simply use @cpp {} @ce in function calls and elsewhere.
     */
    Unspecified = 0,

    /** [GLSL](https://en.wikipedia.org/wiki/OpenGL_Shading_Language) */
    Glsl,

    /** [SPIR-V](https://en.wikipedia.org/wiki/Standard_Portable_Intermediate_Representation#SPIR-V) */
    Spirv,

    /**
     * Textual representation of [SPIR-V](https://en.wikipedia.org/wiki/Standard_Portable_Intermediate_Representation#SPIR-V).
     */
    SpirvAssembly,

    /**
     * [HLSL](https://en.wikipedia.org/wiki/High-Level_Shading_Language)
     * (High-Level Shading Language), used in D3D
     */
    Hlsl,

    /**
     * [MSL](https://en.wikipedia.org/wiki/Metal_(API)) (Metal Shading
     * Language)
     */
    Msl,

    /**
     * [WGSL](https://en.wikipedia.org/wiki/WebGPU) (WebGPU Shading Language)
     */
    Wgsl,

    /**
     * DXIL (DirectX Intermediate Language), produced by
     * [DirectX Shader Compiler](https://github.com/microsoft/DirectXShaderCompiler)
     */
    Dxil
};

/**
@debugoperatorenum{Format}
@m_since_latest
*/
MAGNUM_SHADERTOOLS_EXPORT Debug& operator<<(Debug& debug, Format value);

/**
@brief Shader stage
@m_since_latest

@see @ref AbstractConverter
*/
enum class Stage: UnsignedInt {
    /**
     * Unspecified stage. When used in the
     * @ref AbstractConverter::validateFile(),
     * @ref AbstractConverter::convertFileToFile() "convertFileToFile()",
     * @ref AbstractConverter::convertFileToData() "convertFileToData()",
     * @ref AbstractConverter::linkFilesToFile() "linkFilesToFile()" or
     * @ref AbstractConverter::linkFilesToData() "linkFilesToData()" APIs,
     * particular plugins may attempt to detect the stage from filename, the
     * shader stage might also be encoded directly in certain
     * @ref Format "Format"s. Leaving the stage unspecified might limit
     * validation and conversion capabilities, see documentation of a
     * particular converter for concrete behavior.
     *
     * This value is guaranteed to be @cpp 0 @ce, which means you're encouraged
     * to simply use @cpp {} @ce in function calls and elsewhere.
     */
    Unspecified = 0,

    Vertex,                     /**< Vertex stage */
    Fragment,                   /**< Fragment stage */
    Geometry,                   /**< Geometry stage */
    TessellationControl,        /**< Tessellation control stage */
    TessellationEvaluation,     /**< Tessellation evaluation stage */
    Compute,                    /**< Compute stage */

    RayGeneration,              /**< Ray generation stage */
    RayAnyHit,                  /**< Ray any hit stage */
    RayClosestHit,              /**< Ray closest hit stage */
    RayMiss,                    /**< Ray miss stage */
    RayIntersection,            /**< Ray intersection stage */
    RayCallable,                /**< Ray callable stage */

    MeshTask,                   /**< Mesh task stage */
    Mesh                        /**< Mesh stage */
};

/**
@debugoperatorenum{Stage}
@m_since_latest
*/
MAGNUM_SHADERTOOLS_EXPORT Debug& operator<<(Debug& debug, Stage value);

/**
@brief Base for shader converter plugins
@m_since_latest

Provides functionality for validating and converting shader code between
different representations or performing optimizations and other operations on
them. See @ref plugins for more information and `*ShaderConverter` classes in
the @ref ShaderTools namespace for available scene converter plugins.

@m_class{m-note m-success}

@par
    There's also a @ref magnum-shaderconverter "magnum-shaderconverter" tool, exposing functionality of all shader converter plugins on a command line.

@section ShaderTools-AbstractConverter-usage Usage

Shader converters are most commonly implemented as plugins. Depending on
exposed @ref features(), a plugin can support shader validation, conversion or
linking.

@m_class{m-block m-warning}

@par Multiple shader sources
    Compared to the (very broad) OpenGL API, only one shader source can be
    specified for a single stage. If you need to pass additional preprocessor
    flags, it's possible to do it via @ref setDefinitions() for plugins that
    support @ref ConverterFeature::Preprocess; if you have shader source
    scattered across multiple files either concatenate them together before
    processing or @cpp #include @ce the dependencies from the top-level file,
    potentially together with setting up @ref ShaderTools-AbstractConverter-usage-callbacks "file callbacks"
    (providing the particular converter implementation supports preprocessor
    includes).

@subsection ShaderTools-AbstractConverter-usage-validation Shader validation

As is common with other plugin interfaces, the
@ref AnyConverter "AnyShaderConverter" will detect a shader format based on
file extension, load an appropriate validator plugin for given format and
validate it:

@snippet MagnumShaderTools.cpp AbstractConverter-usage-validation

In most cases, the validation result depends on the format version and target
environment used. Formats and versions set by @ref setInputFormat() /
@ref setOutputFormat() get propagated by @ref AnyConverter "AnyShaderConverter"
to the particular plugin, however the interpretation is plugin-specific, so be
sure to read their docs.

@subsection ShaderTools-AbstractConverter-usage-conversion Shader conversion and linking

The following example shows converting a GLSL shader to SPIR-V together with
setting preprocessor defines and treating any warnings as errors. This time
it's not using @ref AnyConverter "AnyShaderConverter" but instead asking for a
plugin using the `GlslToSpirvShaderConverter` alias (which maps for example to
@ref GlslangConverter "GlslangShaderConverter") --- since we're operating
directly on data, the plugin would have no chance to know the desired input /
output format otherwise. Same goes for the shader stage, which has to be
supplied explicitly:

@snippet MagnumShaderTools.cpp AbstractConverter-usage-compilation

@todoc document linking when SpirvToolsShaderConverter implements that

@subsection ShaderTools-AbstractConverter-usage-callbacks Loading shaders from memory, using file callbacks

Besides loading shaders directly from the filesystem using @ref validateFile()
/ @ref convertFileToFile() / @ref linkFilesToFile() like shown above, it's
possible to use @ref validateData(), @ref convertDataToData(),
@ref linkDataToData() and variants to load data from memory. Note that the
particular converter implementation has to support
@ref ConverterFeature::ValidateData / @ref ConverterFeature::ConvertData /
@ref ConverterFeature::LinkData for this method to work.

Textual shader sources sometimes @cpp #include @ce other sources and in that
case you may want to intercept those references and load them in a custom way
as well. For converters that advertise support for this with
@ref ConverterFeature::InputFileCallback this is done by specifying an input
file callback using @ref setInputFileCallback(). The callback gets a filename,
@ref InputFileCallbackPolicy and an user pointer as parameters; returns a
non-owning view on the loaded data or a
@ref Corrade::Containers::NullOpt "Containers::NullOpt" to indicate the file
loading failed. For example, validating a shader from compiled-in resources
could look like below. Note that the input file callback affects
@ref validateFile() / @ref convertFileToFile() / @ref convertFileToData() /
@ref linkFilesToFile() / @ref linkFilesToData() as well --- you don't have to
load the top-level file manually and pass it to @ref validateData() /
@ref convertDataToData() / @ref linkDataToData(), any converter supporting the
callback feature handles that correctly.

@snippet MagnumShaderTools.cpp AbstractConverter-usage-callbacks

For converters that don't support @ref ConverterFeature::InputFileCallback
directly, the base @ref validateFile() / @ref convertFileToFile() /
@ref convertFileToData() / @ref linkFilesToFile() / @ref linkFilesToData()
implementations will use the file callback to pass the loaded data through to
@ref validateData() / @ref convertDataToData() / @ref linkDataToData(), in case
the converter supports at least @ref ConverterFeature::ValidateData
/ @ref ConverterFeature::ConvertData / @ref ConverterFeature::LinkData. If the
converter supports none of @ref ConverterFeature::InputFileCallback,
@ref ConverterFeature::ValidateData, @ref ConverterFeature::ConvertData or
@ref ConverterFeature::LinkData, @ref setInputFileCallback() doesn't allow the
callbacks to be set.

The input file callback signature is the same for
@ref ShaderTools::AbstractConverter, @ref Trade::AbstractImporter and
@ref Text::AbstractFont to allow code reuse.

@section ShaderTools-AbstractConverter-data-dependency Data dependency

The instances returned from various functions *by design* have no dependency on
the importer instance and neither on the dynamic plugin module. In other words,
you don't need to keep the importer instance (or the plugin manager instance)
around in order to have the returned data valid --- all returned
@ref Corrade::Containers::String and @ref Corrade::Containers::Array instances
are only allowed to have default deleters and this is to avoid potential
dangling function pointer calls when destructing such instances after the
plugin module has been unloaded.

@section ShaderTools-AbstractConverter-subclassing Subclassing

The plugin needs to implement the @ref doFeatures() function and one or more of
@ref doValidateData(), @ref doValidateFile(), @ref doConvertDataToData(),
@ref doConvertFileToData(), @ref doConvertFileToFile(),
@ref doLinkDataToData(), @ref doLinkFilesToData() or @ref doLinkFilesToFile()
functions based on what features are supported.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The function @ref doValidateData() is called only if
    @ref ConverterFeature::ValidateData is supported.
-   The function @ref doValidateFile() is called only if
    @ref ConverterFeature::ValidateFile is supported.
-   Functions @ref doConvertDataToData() and @ref doConvertFileToData() are
    called only if @ref ConverterFeature::ConvertData is supported.
-   The function @ref doConvertFileToFile() is called only if
    @ref ConverterFeature::ConvertFile is supported.
-   Functions @ref doLinkDataToData() and @ref doLinkFilesToData() are
    called only if @ref ConverterFeature::LinkData is supported.
-   The function @ref doLinkFilesToFile() is called only if
    @ref ConverterFeature::LinkFile is supported.
-   Functions @ref doLinkDataToData(), @ref doLinkFilesToData() and
    @ref doLinkFilesToFile() are called only if the data / file list passed is
    non-empty.

@m_class{m-block m-warning}

@par Dangling function pointers on plugin unload
    As @ref ShaderTools-AbstractConverter-data-dependency "mentioned above",
    @ref Corrade::Containers::String and @ref Corrade::Containers::Array
    instances returned from plugin implementations are not allowed to use
    anything else than the default deleter, otherwise this could cause dangling
    function pointer call on array destruction if the plugin gets unloaded
    before the array is destroyed. This is asserted by the base implementation
    on return.
*/
class MAGNUM_SHADERTOOLS_EXPORT AbstractConverter: public PluginManager::AbstractManagingPlugin<AbstractConverter> {
    public:
        /**
         * @brief Plugin interface
         *
         * @snippet Magnum/ShaderTools/AbstractConverter.cpp interface
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * Looks into `magnum/shaderconverters/` or `magnum-d/shaderconverters/`
         * next to the dynamic @ref ShaderTools library, next to the executable
         * and elsewhere according to the rules documented in
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
        explicit AbstractConverter();

        /** @brief Constructor with access to plugin manager */
        explicit AbstractConverter(PluginManager::Manager<AbstractConverter>& manager);

        /** @brief Plugin manager constructor */
        explicit AbstractConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

        /** @brief Features supported by this converter */
        ConverterFeatures features() const;

        /** @brief Converter flags */
        ConverterFlags flags() const { return _flags; }

        /**
         * @brief Set converter flags
         *
         * Some flags can be set only if the converter supports particular
         * features, see documentation of each @ref ConverterFlag for more
         * information. By default no flags are set.
         *
         * Corresponds to the `-q` / `--quiet`, `-v` / `--verbose`,
         * `--warning-as-error` and `-E` / `--preprocess-only` options
         * in @ref magnum-shaderconverter "magnum-shaderconverter".
         */
        void setFlags(ConverterFlags flags);

        /**
         * @brief Input file callback function
         *
         * @see @ref ShaderTools-AbstractConverter-usage-callbacks
         */
        auto inputFileCallback() const -> Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*) { return _inputFileCallback; }

        /**
         * @brief Input file callback user data
         *
         * @see @ref ShaderTools-AbstractConverter-usage-callbacks
         */
        void* inputFileCallbackUserData() const { return _inputFileCallbackUserData; }

        /**
         * @brief Set input file callback
         *
         * In case the converter supports @ref ConverterFeature::InputFileCallback,
         * files opened through @ref validateFile(), @ref convertFileToData(),
         * @ref convertFileToFile(), @ref linkFilesToData() and
         * @ref linkFilesToFile() will be loaded through the provided callback.
         * Besides that, all external files referenced by the top-level file
         * will be loaded through the callback function as well, usually on
         * demand. The callback function gets a filename,
         * @ref InputFileCallbackPolicy and the @p userData pointer as input
         * and returns a non-owning view on the loaded data as output or a
         * @ref Corrade::Containers::NullOpt if loading failed --- because
         * empty files might also be valid in some circumstances,
         * @cpp nullptr @ce can't be used to indicate a failure.
         *
         * In case the converter doesn't support
         * @ref ConverterFeature::InputFileCallback but supports at least
         * @ref ConverterFeature::ValidateData /
         * @ref ConverterFeature::ConvertData /
         * @ref ConverterFeature::LinkData, a file opened through
         * @ref validateFile(), @ref convertFileToData(),
         * @ref convertFileToFile(), @ref linkFilesToData() or
         * @ref linkFilesToFile() will be internally loaded through the
         * provided callback and then passed to @ref validateData(),
         * @ref convertDataToData() or @ref linkDataToData(). First the file is
         * loaded with @ref InputFileCallbackPolicy::LoadTemporary passed to
         * the callback, then the returned memory view is passed to
         * @ref validateData() / @ref convertDataToData() /
         * @ref linkDataToData() (sidestepping the potential
         * @ref validateFile() / @ref convertFileToFile() /
         * @ref convertFileToData() / @ref linkFilesToFile() /
         * @ref linkFilesToData() implementation of that particular converter)
         * and after that the callback is called again with
         * @ref InputFileCallbackPolicy::Close. In case you need a different
         * behavior, use @ref validateData() / @ref convertDataToData() /
         * @ref linkDataToData() directly.
         *
         * In case @p callback is @cpp nullptr @ce, the current callback (if
         * any) is reset. This function expects that the converter supports
         * either @ref ConverterFeature::InputFileCallback or at least one of
         * @ref ConverterFeature::ValidateData,
         * @ref ConverterFeature::ConvertData, @ref ConverterFeature::LinkData.
         * If a converter supports neither, callbacks can't be used.
         *
         * Following is an example of setting up an input file callback for
         * fetching compiled-in resources from @ref Corrade::Utility::Resource.
         * See the overload below for a more convenient type-safe way to pass
         * the user data pointer.
         *
         * @snippet MagnumShaderTools.cpp AbstractConverter-setInputFileCallback
         *
         * @see @ref ShaderTools-AbstractConverter-usage-callbacks
         */
        /** @todo once porting away from std::string, it might make sense to
            also return a const void view (so it's always implicitly
            convertible) and not an Optional (as in, empty but non-null view
            would be used for empty files and null would be a failure); then it
            also might make sense to have fileCallback() return a char view
            again to avoid ugly casts in all user code */
        void setInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData = nullptr);

        /**
         * @brief Set file opening callback
         *
         * Equivalent to calling the above with a lambda wrapper that casts
         * @cpp void* @ce back to @cpp T* @ce and dereferences it in order to
         * pass it to @p callback. Example usage --- this reuses an existing
         * @ref Corrade::Utility::Resource instance to avoid a potentially slow
         * resource group lookup every time:
         *
         * @snippet MagnumShaderTools.cpp AbstractConverter-setInputFileCallback-template
         *
         * @see @ref ShaderTools-AbstractConverter-usage-callbacks
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class T> void setInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, T&), T& userData);
        #else
        /* Otherwise the user would be forced to use the + operator to convert
           a lambda to a function pointer and (besides being weird and
           annoying) it's also not portable because it doesn't work on MSVC
           2015 and older versions of MSVC 2017. */
        template<class Callback, class T> void setInputFileCallback(Callback callback, T& userData);
        #endif

        /**
         * @brief Set input format version
         *
         * @ref Format::Unspecified and an empty version is always accepted,
         * other values are interpreted in a plugin-specific way. If a
         * format/version combination is not supported or recognized, the
         * following @ref validateData(), @ref validateFile(),
         * @ref convertDataToData(), @ref convertDataToFile(),
         * @ref convertFileToFile(), @ref convertFileToData(),
         * @ref linkDataToData(), @ref linkDataToFile(), @ref linkFilesToFile()
         * or @ref linkFilesToData() call will fail.
         *
         * The @p version parameter corresponds to the `--input-version` option
         * in @ref magnum-shaderconverter "magnum-shaderconverter", the
         * @p format isn't currently exposed there.
         * @see @ref setOutputFormat()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void setInputFormat(Format format, Containers::StringView version = {});
        #else /* To avoid including StringView */
        void setInputFormat(Format format, Containers::StringView version);
        void setInputFormat(Format format);
        #endif

        /**
         * @brief Set output format version
         *
         * @ref Format::Unspecified and an empty version is always accepted,
         * other values are interpreted in a plugin-specific way. If a
         * format/version combination is not supported or recognized, the
         * following @ref validateData(), @ref validateFile(),
         * @ref convertDataToData(), @ref convertDataToFile(),
         * @ref convertFileToFile(), @ref convertFileToData(),
         * @ref linkDataToData(), @ref linkDataToFile(), @ref linkFilesToFile()
         * or @ref linkFilesToData() call will fail.
         *
         * The @p version parameter corresponds to the `--output-version`
         * option in @ref magnum-shaderconverter "magnum-shaderconverter", the
         * @p format isn't currently exposed there.
         * @see @ref setInputFormat()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void setOutputFormat(Format format, Containers::StringView version = {});
        #else /* To avoid including StringView */
        void setOutputFormat(Format format, Containers::StringView version);
        void setOutputFormat(Format format);
        #endif

        /**
         * @brief Set preprocessor definitions
         *
         * Available only if @ref ConverterFeature::Preprocess is supported.
         * First string is macro name, second its value. If the second string
         * is empty (but not @cpp nullptr @ce), it's the same as
         * @cpp #define @ce without a value; if the second string is
         * @cpp nullptr @ce, it's the same as @cpp #undef @ce.
         *
         * Calling this function replaces the previous set, calling it with an
         * empty list will reset the definitions back to initial state.
         *
         * Corresponds to the `-D` / `--define` and `-U` / `--undefine` options
         * in @ref magnum-shaderconverter "magnum-shaderconverter".
         * @see @ref ConverterFlag::PreprocessOnly
         */
        void setDefinitions(Containers::ArrayView<const std::pair<Containers::StringView, Containers::StringView>> definitions);

        /** @overload */
        void setDefinitions(std::initializer_list<std::pair<Containers::StringView, Containers::StringView>> definitions);

        /**
         * @brief Set optimization level
         *
         * Available only if @ref ConverterFeature::Optimize is supported.
         * Interpreted in a plugin-specific way, if it's not recognized the
         * following @ref convertDataToData(), @ref convertDataToFile(),
         * @ref convertFileToFile(), @ref convertFileToData(),
         * @ref linkDataToData(), @ref linkDataToFile(), @ref linkFilesToFile()
         * or @ref linkFilesToData() call will fail.
         *
         * Has no effect for @ref validateData() or @ref validateFile().
         *
         * Corresponds to the `-O` / `--optimize` option in
         * @ref magnum-shaderconverter "magnum-shaderconverter".
         * @see @ref setDebugInfoLevel()
         */
        void setOptimizationLevel(Containers::StringView level);

        /**
         * @brief Set debug info level
         *
         * Available only if @ref ConverterFeature::DebugInfo is supported.
         * Interpreted in a plugin-specific way, if it's not recognized the
         * following @ref convertDataToData(), @ref convertDataToFile(),
         * @ref convertFileToFile(), @ref convertFileToData(),
         * @ref linkDataToData(), @ref linkDataToFile(), @ref linkFilesToFile()
         * or @ref linkFilesToData() call will fail.
         *
         * Has no effect for @ref validateData() or @ref validateFile().
         *
         * Corresponds to the `-g` / `--debug-info` option in
         * @ref magnum-shaderconverter "magnum-shaderconverter".
         * @see @ref setOptimizationLevel()
         */
        void setDebugInfoLevel(Containers::StringView level);

        /**
         * @brief Validate a shader
         *
         * Available only if @ref ConverterFeature::ValidateData is
         * supported. Returns
         *
         * -    @cpp true @ce and an empty string if validation passes without
         *      warnings,
         * -    @cpp true @ce and a non-empty string if validation passes with
         *      warnings, and
         * -    @cpp false @ce if validation doesn't pass. If an external error
         *      occurs (for example a referenced file not being found), it may
         *      also happen that the returned string is empty and a message is
         *      printed to error output instead.
         *
         * @see @ref features(), @ref validateFile()
         */
        std::pair<bool, Containers::String> validateData(Stage stage, Containers::ArrayView<const void> data);

        /**
         * @brief Validate a shader
         *
         * Available only if @ref ConverterFeature::ValidateFile or
         * @ref ConverterFeature::ValidateData is supported. Returns
         *
         * -    @cpp true @ce and an empty string if validation passes without
         *      warnings,
         * -    @cpp true @ce and a non-empty string if validation passes with
         *      warnings, and
         * -    @cpp false @ce if validation doesn't pass. If an external error
         *      occurs (for example when a file cannot be read), it may also
         *      happen that the returned string is empty and a message is
         *      printed to error output instead.
         *
         * Corresponds to the `--validate` option in
         * @ref magnum-shaderconverter "magnum-shaderconverter".
         * @see @ref features(), @ref validateData()
         */
        std::pair<bool, Containers::String> validateFile(Stage stage, Containers::StringView filename);

        /**
         * @brief Convert shader data to a data
         *
         * Available only if @ref ConverterFeature::ConvertData is supported.
         * On failure the function prints an error message and returns
         * @cpp nullptr @ce.
         * @see @ref features(), @ref convertDataToFile(),
         *      @ref convertFileToData(), @ref convertFileToFile()
         */
        Containers::Array<char> convertDataToData(Stage stage, Containers::ArrayView<const void> data);

        /**
         * @brief Convert shader data to a file
         *
         * Available only if @ref ConverterFeature::ConvertData is supported.
         * Returns @cpp true @ce on success, prints an error message and
         * returns @cpp false @ce otherwise.
         * @see @ref features(), @ref convertDataToData(),
         *      @ref convertFileToData(), @ref convertFileToFile()
         */
        bool convertDataToFile(Stage stage, Containers::ArrayView<const void> data, Containers::StringView to);

        /**
         * @brief Convert shader file to a file
         *
         * Available only if @ref ConverterFeature::ConvertFile or
         * @ref ConverterFeature::ConvertData is supported. Returns
         * @cpp true @ce on success, prints an error message and returns
         * @cpp false @ce otherwise.
         *
         * Corresponds to the default behavior of
         * @ref magnum-shaderconverter "magnum-shaderconverter" when neither
         * `--validate` nor `--link` is specified.
         * @see @ref features(), @ref convertFileToData(),
         *      @ref convertDataToData(), @ref convertDataToFile()
         */
        bool convertFileToFile(Stage stage, Containers::StringView from, Containers::StringView to);

        /**
         * @brief Convert shader data to a file
         *
         * Available only if @ref ConverterFeature::ConvertData is supported.
         * On failure the function prints an error message and returns
         * @cpp nullptr @ce.
         * @see @ref features(), @ref convertFileToFile(),
         *      @ref convertDataToFile(), @ref convertDataToData()
         */
        Containers::Array<char> convertFileToData(Stage stage, const Containers::StringView from);

        /**
         * @brief Link shader data together to a data
         *
         * Available only if @ref ConverterFeature::LinkData is supported. On
         * failure the function prints an error message and returns
         * @cpp nullptr @ce. Can't be called if
         * @ref ConverterFlag::PreprocessOnly is set --- in that case
         * @ref convertDataToData() has to be used instead.
         * @see @ref features() @ref linkDataToFile(), @ref linkFilesToFile()
         */
        Containers::Array<char> linkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const void>>> data);

        /** @overload */
        Containers::Array<char> linkDataToData(std::initializer_list<std::pair<Stage, Containers::ArrayView<const void>>> data);

        /**
         * @brief Link shader data together to a file
         *
         * Available only if @ref ConverterFeature::LinkData is supported. On
         * Returns @cpp true @ce on success, prints an error message and
         * returns @cpp false @ce otherwise. Can't be called if
         * @ref ConverterFlag::PreprocessOnly is set --- in that case
         * @ref convertDataToFile() has to be used instead.
         * @see @ref features(), @ref linkFilesToFile(),
         *      @ref linkFilesToData(), @ref linkDataToData()
         */
        bool linkDataToFile(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const void>>> data, Containers::StringView to);

        /** @overload */
        bool linkDataToFile(std::initializer_list<std::pair<Stage, Containers::ArrayView<const void>>> data, Containers::StringView to);

        /**
         * @brief Link shader files together to a file
         *
         * Available only if @ref ConverterFeature::LinkFile or
         * @ref ConverterFeature::LinkData is supported. Returns @cpp true @ce
         * on success, prints an error message and returns @cpp false @ce
         * otherwise. Can't be called if @ref ConverterFlag::PreprocessOnly is
         * set --- in that case  @ref convertFileToFile() has to be used
         * instead.
         *
         * Corresponds to the `--link` option in
         * @ref magnum-shaderconverter "magnum-shaderconverter".
         * @see @ref features(), @ref linkFilesToData(), @ref linkDataToFile(),
         *      @ref linkDataToData()
         */
        bool linkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from, Containers::StringView to);

        /** @overload */
        bool linkFilesToFile(std::initializer_list<std::pair<Stage, Containers::StringView>> from, Containers::StringView to);

        /**
         * @brief Link shader files together to a data
         *
         * Available only if @ref ConverterFeature::LinkData is supported, On
         * failure the function prints an error message and returns
         * @cpp nullptr @ce. Can't be called if
         * @ref ConverterFlag::PreprocessOnly is set --- in that case
         * @ref convertFileToData() has to be used instead.
         * @see @ref features(), @ref linkFilesToFile(), @ref linkDataToFile(),
         *      @ref linkDataToData()
         */
        Containers::Array<char> linkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from);

        /** @overload */
        Containers::Array<char> linkFilesToData(std::initializer_list<std::pair<Stage, Containers::StringView>> from);

    protected:
        /**
         * @brief Implementation for @ref validateFile()
         *
         * If @ref ConverterFeature::ValidateData is supported, default
         * implementation opens the file and calls @ref doValidateData() with
         * its contents. It is allowed to call this function from your
         * @ref doValidateFile() implementation --- in particular, this
         * implementation will also correctly handle callbacks set through
         * @ref setInputFileCallback().
         *
         * This function is not called when file callbacks are set through
         * @ref setInputFileCallback() and @ref ConverterFeature::InputFileCallback
         * is not supported --- instead, file is loaded though the callback and
         * data passed through to @ref doValidateData().
         */
        virtual std::pair<bool, Containers::String> doValidateFile(Stage stage, Containers::StringView filename);

        /**
         * @brief Implementation for @ref convertFileToFile()
         *
         * If @ref ConverterFeature::ConvertData is supported, default
         * implementation opens the file and calls @ref doConvertDataToData()
         * with its contents, then saving the output to a file. It is allowed
         * to call this function from your @ref doConvertFileToFile()
         * implementation --- in particular, this implementation will also
         * correctly handle callbacks set through @ref setInputFileCallback().
         *
         * This function is not called when file callbacks are set through
         * @ref setInputFileCallback() and @ref ConverterFeature::InputFileCallback
         * is not supported --- instead, file is loaded though the callback and
         * data passed through to @ref doConvertDataToData().
         */
        virtual bool doConvertFileToFile(Stage stage, Containers::StringView from, Containers::StringView to);

        /**
         * @brief Implementation for @ref convertFileToData()
         *
         * Default implementation opens the file and calls
         * @ref doConvertDataToData() with its contents --- you only need to
         * implement this if you need to do extra work with file inputs. It is
         * allowed to call this function from your @ref doConvertFileToData()
         * implementation --- in particular, this implementation will also
         * correctly handle callbacks set through @ref setInputFileCallback().
         *
         * This function is not called when file callbacks are set through
         * @ref setInputFileCallback() and @ref ConverterFeature::InputFileCallback
         * is not supported --- instead, file is loaded though the callback and
         * data passed through to @ref doConvertDataToData().
         */
        virtual Containers::Array<char> doConvertFileToData(Stage stage, Containers::StringView from);

        /**
         * @brief Implementation for @ref linkFilesToFile()
         *
         * If @ref ConverterFeature::LinkData is supported, default
         * implementation opens all files and calls @ref linkDataToData() with
         * their contents It is allowed to call this function from your
         * @ref doLinkFilesToFile() implementation --- in particular, this
         * implementation will also correctly handle callbacks set through
         * @ref setInputFileCallback().
         *
         * This function is not called when file callbacks are set through
         * @ref setInputFileCallback() and @ref ConverterFeature::InputFileCallback
         * is not supported --- instead, file is loaded though the callback and
         * data passed through to @ref doLinkDataToData().
         */
        virtual bool doLinkFilesToFile(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from, Containers::StringView to);

        /**
         * @brief Implementation for @ref linkFilesToData()
         *
         * Default implementation opens all files and calls
         * @ref doLinkDataToData() with their contents --- you only need to
         * implement this if you need to do extra work with file inputs. It is
         * allowed to call this function from your @ref doLinkFilesToData()
         * implementation --- in particular, this implementation will also
         * correctly handle callbacks set through @ref setInputFileCallback().
         *
         * This function is not called when file callbacks are set through
         * @ref setInputFileCallback() and @ref ConverterFeature::InputFileCallback
         * is not supported --- instead, file is loaded though the callback and
         * data passed through to @ref doConvertDataToData().
         */
        virtual Containers::Array<char> doLinkFilesToData(Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from);

    private:
        /**
         * @brief Implementation for @ref features()
         *
         * Has to be implemented always, the implementation is expected to
         * support at least one feature.
         */
        virtual ConverterFeatures doFeatures() const = 0;

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
        virtual void doSetFlags(ConverterFlags flags);

        /**
         * @brief Implementation for @ref setInputFileCallback()
         *
         * Useful when the converter needs to modify some internal state on
         * callback setup. Default implementation does nothing and this
         * function doesn't need to be implemented --- the callback function
         * and user data pointer are available through @ref inputFileCallback()
         * and @ref inputFileCallbackUserData().
         */
        virtual void doSetInputFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData);

        /**
         * @brief Implementation for @ref setInputFormat()
         *
         * Has to be implemented always. To simplify error handling on user
         * side, this function isn't expected to fail --- if the format/version
         * combination isn't recognized, the following @ref validateData(),
         * @ref validateFile(), @ref convertDataToData(),
         * @ref convertDataToFile(), @ref convertFileToFile(),
         * @ref convertFileToData(), @ref linkDataToData(),
         * @ref linkDataToFile(), @ref linkFilesToFile() or
         * @ref linkFilesToData() should fail instead.
         */
        virtual void doSetInputFormat(Format format, Containers::StringView version) = 0;

        /**
         * @brief Implementation for @ref setOutputFormat()
         *
         * Has to be implemented always. To simplify error handling on user
         * side, this function isn't expected to fail --- if the format/version
         * combination isn't recognized, the following @ref validateData(),
         * @ref validateFile(), @ref convertDataToData(),
         * @ref convertDataToFile(), @ref convertFileToFile(),
         * @ref convertFileToData(), @ref linkDataToData(),
         * @ref linkDataToFile(), @ref linkFilesToFile() or
         * @ref linkFilesToData() should fail instead.
         */
        virtual void doSetOutputFormat(Format format, Containers::StringView version) = 0;

        /**
         * @brief Implementation for @ref setDefinitions()
         *
         * Has to be implemented if @ref ConverterFeature::Preprocess is
         * supported. This function isn't expected to fail.
         */
        virtual void doSetDefinitions(Containers::ArrayView<const std::pair<Containers::StringView, Containers::StringView>> definitions);

        /**
         * @brief Implementation for @ref setOptimizationLevel()
         *
         * Has to be implemented if @ref ConverterFeature::Optimize is
         * supported. To simplify error handling on user side, this function
         * isn't expected to fail --- if the level isn't recognized, the
         * following @ref convertDataToData(), @ref convertDataToFile(),
         * @ref convertFileToFile(), @ref convertFileToData(),
         * @ref linkDataToData(), @ref linkDataToFile(), @ref linkFilesToFile()
         * or @ref linkFilesToData() should fail instead.
         */
        virtual void doSetOptimizationLevel(Containers::StringView level);

        /**
         * @brief Implementation for @ref setDebugInfoLevel()
         *
         * Has to be implemented if @ref ConverterFeature::DebugInfo is
         * supported. To simplify error handling on user side, this function
         * isn't expected to fail --- if the level isn't recognized, the
         * following @ref convertDataToData(), @ref convertDataToFile(),
         * @ref convertFileToFile(), @ref convertFileToData(),
         * @ref linkDataToData(), @ref linkDataToFile(), @ref linkFilesToFile()
         * or @ref linkFilesToData() should fail instead.
         */
        virtual void doSetDebugInfoLevel(Containers::StringView level);

        /**
         * @brief Implementation for @ref validateData()
         *
         * Has to be implemented if @ref ConverterFeature::ValidateData
         * is supported. While @ref validateData() uses a @cpp void @ce view in
         * order to accept any type, this function gets it cast to
         * @cpp char @ce for more convenience.
         */
        virtual std::pair<bool, Containers::String> doValidateData(Stage stage, Containers::ArrayView<const char> data);

        /* Used by convertFileToFile(), doConvertFileToFile(),
           convertFileToData() and doConvertFileToData() */
        MAGNUM_SHADERTOOLS_LOCAL Containers::Array<char> convertDataToDataUsingInputFileCallbacks(const char* prefix, const Stage stage, Containers::StringView from);

        /**
         * @brief Implementation for @ref convertDataToData()
         *
         * Has to be implemented if @ref ConverterFeature::ConvertData is
         * supported. While @ref convertDataToData() uses a @cpp void @ce view
         * in order to accept any type, this function gets it cast to
         * @cpp char @ce for more convenience.
         */
        virtual Containers::Array<char> doConvertDataToData(Stage stage, Containers::ArrayView<const char> data);

        /* Used by linkFilesToFile(), doLinkFilesToFile(), linkFilesToData()
           and doLinkFilesToData() */
        MAGNUM_SHADERTOOLS_LOCAL Containers::Array<char> linkDataToDataUsingInputFileCallbacks(const char* prefix, Containers::ArrayView<const std::pair<Stage, Containers::StringView>> from);

        /**
         * @brief Implementation for @ref linkDataToData()
         *
         * Has to be implemented if @ref ConverterFeature::LinkData is
         * supported. While @ref linkDataToData() uses a @cpp void @ce view in
         * order to accept any type, this function gets it cast to
         * @cpp char @ce for more convenience.
         */
        virtual Containers::Array<char> doLinkDataToData(Containers::ArrayView<const std::pair<Stage, Containers::ArrayView<const char>>> data);

        ConverterFlags _flags;

        Containers::Optional<Containers::ArrayView<const char>>(*_inputFileCallback)(const std::string&, InputFileCallbackPolicy, void*){};
        void* _inputFileCallbackUserData{};

        /* Used by the templated version only */
        struct FileCallbackTemplate {
            void(*callback)();
            const void* userData;
        /* GCC 4.8 complains loudly about missing initializers otherwise */
        } _inputFileCallbackTemplate{nullptr, nullptr};
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class Callback, class T> void AbstractConverter::setInputFileCallback(Callback callback, T& userData) {
    /* Don't try to wrap a null function pointer. Need to cast first because
       MSVC (even 2017) can't apply ! to a lambda. Ugh. */
    const auto callbackPtr = static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, T&)>(callback);
    if(!callbackPtr) return setInputFileCallback(nullptr);

    _inputFileCallbackTemplate = { reinterpret_cast<void(*)()>(callbackPtr), static_cast<const void*>(&userData) };
    setInputFileCallback([](const std::string& filename, const InputFileCallbackPolicy flags, void* const userData) {
        auto& s = *reinterpret_cast<FileCallbackTemplate*>(userData);
        return reinterpret_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, T&)>(s.callback)(filename, flags, *static_cast<T*>(const_cast<void*>(s.userData)));
    }, &_inputFileCallbackTemplate);
}
#endif

}}

#endif
