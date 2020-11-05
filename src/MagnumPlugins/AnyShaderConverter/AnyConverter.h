#ifndef Magnum_ShaderTools_AnyConverter_h
#define Magnum_ShaderTools_AnyConverter_h
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
 * @brief Class @ref Magnum::ShaderTools::AnyConverter
 * @m_since_latest
 */

#include "Magnum/ShaderTools/AbstractConverter.h"
#include "MagnumPlugins/AnyShaderConverter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_ANYSHADERCONVERTER_BUILD_STATIC
    #ifdef AnyShaderConverter_EXPORTS
        #define MAGNUM_ANYSHADERCONVERTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYSHADERCONVERTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_ANYSHADERCONVERTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_ANYSHADERCONVERTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_ANYSHADERCONVERTER_EXPORT
#define MAGNUM_ANYSHADERCONVERTER_LOCAL
#endif

namespace Magnum { namespace ShaderTools {

/**
@brief Any shader converter plugin
@m_since_latest

@m_keywords{AnyShaderConverter}

Detects file type based on file extension, loads corresponding plugin and then
tries to either validate or convert the file with it. Detected file formats:

-   GLSL (`*.glsl`, `*.vert`, `*.frag`, `*.geom`, `*.comp`, `*.tesc`, `*.tese`,
    `*.rgen`, `*.rint`, `*.rahit`, `*.rchit`, `*.rmiss`, `*.rcall`, `*.mesh`,
    `*.task`)
-   SPIR-V (`*.spv`)
-   SPIR-V Assembly (`*.spvasm`, `*.asm.vert`, ..., `*.asm.task`)

Supported validation scenarios:

-   GLSL, validated with any plugin that provides `GlslShaderConverter`
-   SPIR-V, validated with any plugin that provides `SpirvShaderConverter`
-   SPIR-V Assembly, validated with any plugin that provides
    `SpirvAssemblyShaderConverter`

Supported conversion paths:

-   GLSL to SPIR-V, converted with any plugin that provides
    `GlslToSpirvShaderConverter`
-   SPIR-V to SPIR-V, converted with any plugin that provides
    `SpirvShaderConverter`
-   SPIR-V to SPIR-V Assembly, converted with any plugin that provides
    `SpirvToSpirvAssemblyShaderConverter`
-   SPIR-V Assembly to SPIR-V, converted with any plugin that provides
    `SpirvAssemblyToSpirvShaderConverter`
-   SPIR-V Assembly to SPIR-V Assembly, converted with any plugin that provides
    `SpirvAssemblyShaderConverter`

Only validating and converting files is supported.

@section ShaderTools-AnyConverter-usage Usage

This plugin depends on the @ref ShaderTools library and is built if
`WITH_ANYSHADERCONVERTER` is enabled when building Magnum. To use as a dynamic
plugin, load @cpp "AnyShaderConverter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_ANYSHADERCONVERTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::AnyShaderConverter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `AnyShaderConverter` component of the `Magnum` package and
link to the `Magnum::AnyShaderConverter` target:

@code{.cmake}
find_package(Magnum REQUIRED AnyShaderConverter)

# ...
target_link_libraries(your-app PRIVATE Magnum::AnyShaderConverter)
@endcode

See @ref building, @ref cmake and @ref plugins for more information.
*/
class MAGNUM_ANYSHADERCONVERTER_EXPORT AnyConverter: public AbstractConverter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnyConverter(PluginManager::Manager<AbstractConverter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnyConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~AnyConverter();

    private:
        MAGNUM_ANYSHADERCONVERTER_LOCAL ConverterFeatures doFeatures() const override;

        MAGNUM_ANYSHADERCONVERTER_LOCAL void doSetInputFormat(Format, Containers::StringView version) override;
        MAGNUM_ANYSHADERCONVERTER_LOCAL void doSetOutputFormat(Format, Containers::StringView version) override;
        MAGNUM_ANYSHADERCONVERTER_LOCAL void doSetDefinitions(Containers::ArrayView<const std::pair<Containers::StringView, Containers::StringView>> definitions) override;
        MAGNUM_ANYSHADERCONVERTER_LOCAL void doSetDebugInfoLevel(Containers::StringView level) override;
        MAGNUM_ANYSHADERCONVERTER_LOCAL void doSetOptimizationLevel(Containers::StringView level) override;

        MAGNUM_ANYSHADERCONVERTER_LOCAL std::pair<bool, Containers::String> doValidateFile(Stage stage, Containers::StringView filename) override;
        MAGNUM_ANYSHADERCONVERTER_LOCAL bool doConvertFileToFile(Stage stage, Containers::StringView from, Containers::StringView to) override;

        struct State;
        Containers::Pointer<State> _state;
};

}}

#endif
