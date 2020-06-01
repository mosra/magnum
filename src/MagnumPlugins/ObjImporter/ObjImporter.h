#ifndef Magnum_Trade_ObjImporter_h
#define Magnum_Trade_ObjImporter_h
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
 * @brief Class @ref Magnum::Trade::ObjImporter
 */

#include "Magnum/Trade/AbstractImporter.h"

#include "MagnumPlugins/ObjImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_OBJIMPORTER_BUILD_STATIC
    #ifdef ObjImporter_EXPORTS
        #define MAGNUM_OBJIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_OBJIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_OBJIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_OBJIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_OBJIMPORTER_EXPORT
#define MAGNUM_OBJIMPORTER_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief OBJ importer plugin

Loads Wavefront OBJ (`*.obj`) files, with the following supported features:

-   multiple objects
-   vertex positions, normals and 2D texture coordinates
-   triangles, lines and points

@section Trade-ObjImporter-usage Usage

This plugin depends on the @ref Trade library and is built if `WITH_OBJIMPORTER`
is enabled when building Magnum. To use as a dynamic plugin, load
@cpp "ObjImporter" @ce via @ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_OBJIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::ObjImporter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `ObjImporter` component of the `Magnum` package and link to
the `Magnum::ObjImporter` target:

@code{.cmake}
find_package(Magnum REQUIRED ObjImporter)

# ...
target_link_libraries(your-app PRIVATE Magnum::ObjImporter)
@endcode

See @ref building, @ref cmake, @ref plugins and @ref file-formats for more
information.

@section Trade-ObjImporter-behavior Behavior and limitations

Meshes are imported as @ref MeshPrimitive::Triangles with
@ref MeshIndexType::UnsignedInt indices, interleaved @ref VertexFormat::Vector3
positions with optional @ref VertexFormat::Vector3 normals and
@ref VertexFormat::Vector2 texture coordinates, if present in the source file.

Polygons (quads etc.) and material properties are currently not supported.
*/
class MAGNUM_OBJIMPORTER_EXPORT ObjImporter: public AbstractImporter {
    public:
        /** @brief Default constructor */
        explicit ObjImporter();

        /** @brief Plugin manager constructor */
        explicit ObjImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~ObjImporter();

    private:
        struct File;

        MAGNUM_OBJIMPORTER_LOCAL ImporterFeatures doFeatures() const override;

        MAGNUM_OBJIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_OBJIMPORTER_LOCAL void doOpenData(Containers::ArrayView<const char> data) override;
        MAGNUM_OBJIMPORTER_LOCAL void doOpenFile(const std::string& filename) override;
        MAGNUM_OBJIMPORTER_LOCAL void doClose() override;

        MAGNUM_OBJIMPORTER_LOCAL UnsignedInt doMeshCount() const override;
        MAGNUM_OBJIMPORTER_LOCAL Int doMeshForName(const std::string& name) override;
        MAGNUM_OBJIMPORTER_LOCAL std::string doMeshName(UnsignedInt id) override;
        MAGNUM_OBJIMPORTER_LOCAL Containers::Optional<MeshData> doMesh(UnsignedInt id, UnsignedInt level) override;

        MAGNUM_OBJIMPORTER_LOCAL void parseMeshNames();

        Containers::Pointer<File> _file;
};

}}

#endif
