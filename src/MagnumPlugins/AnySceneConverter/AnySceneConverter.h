#ifndef Magnum_Trade_AnySceneConverter_h
#define Magnum_Trade_AnySceneConverter_h
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
 * @brief Class @ref Magnum::Trade::AnySceneConverter
 */

#include "Magnum/Trade/AbstractSceneConverter.h"
#include "MagnumPlugins/AnySceneConverter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_ANYSCENECONVERTER_BUILD_STATIC
    #ifdef AnySceneConverter_EXPORTS
        #define MAGNUM_ANYSCENECONVERTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYSCENECONVERTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_ANYSCENECONVERTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_ANYSCENECONVERTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_ANYSCENECONVERTER_EXPORT
#define MAGNUM_ANYSCENECONVERTER_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief Any scene converter plugin

Detects file type based on file extension, loads corresponding plugin and then
tries to convert the file with it. Supported formats:

-   Stanford (`*.ply`), converted with @ref StanfordSceneConverter or any other
    plugin that provides it

Only converting to files is supported.

@section Trade-AnySceneConverter-usage Usage

This plugin depends on the @ref Trade library and is built if
`WITH_ANYSCENECONVERTER` is enabled when building Magnum. To use as a dynamic
plugin, load @cpp "AnySceneConverter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_ANYSCENECONVERTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::AnySceneConverter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `AnySceneConverter` component of the `Magnum` package and
link to the `Magnum::AnySceneConverter` target:

@code{.cmake}
find_package(Magnum REQUIRED AnySceneConverter)

# ...
target_link_libraries(your-app PRIVATE Magnum::AnySceneConverter)
@endcode

See @ref building, @ref cmake, @ref plugins and @ref file-formats for more
information.
*/
class MAGNUM_ANYSCENECONVERTER_EXPORT AnySceneConverter: public AbstractSceneConverter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnySceneConverter(PluginManager::Manager<AbstractSceneConverter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnySceneConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~AnySceneConverter();

    private:
        MAGNUM_ANYSCENECONVERTER_LOCAL SceneConverterFeatures doFeatures() const override;
        MAGNUM_ANYSCENECONVERTER_LOCAL bool doConvertToFile(const std::string& filename, const MeshData& mesh) override;
};

}}

#endif
