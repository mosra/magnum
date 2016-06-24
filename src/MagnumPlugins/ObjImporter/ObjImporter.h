#ifndef Magnum_Trade_ObjImporter_h
#define Magnum_Trade_ObjImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Class @ref Magnum::Trade::ObjImporter
 */

#include "Magnum/Trade/AbstractImporter.h"

#include "MagnumPlugins/ObjImporter/configure.h"

#include <Corrade/Containers/Array.h>

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_OBJIMPORTER_BUILD_STATIC
    #if defined(ObjImporter_EXPORTS) || defined(ObjImporterObjects_EXPORTS)
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

This plugin depends on the @ref Trade library and is built if `WITH_OBJIMPORTER`
is enabled when building Magnum. To use as a dynamic plugin, you need to load
the @cpp "ObjImporter" @ce plugin from `MAGNUM_PLUGINS_IMPORTER_DIR`. To use as
a static plugin or as a dependency of another plugin with CMake, you need to
request the `ObjImporter` component of the `Magnum` package and link to the
`Magnum::ObjImporter` target. See @ref building, @ref cmake and @ref plugins
for more information.

@section Trade-ObjImporter-limitations Behavior and limitations

Polygons (quads etc.), automatic normal generation and material properties are
currently not supported.
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

        MAGNUM_OBJIMPORTER_LOCAL Features doFeatures() const override;

        MAGNUM_OBJIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_OBJIMPORTER_LOCAL void doOpenData(Containers::ArrayView<const char> data) override;
        MAGNUM_OBJIMPORTER_LOCAL void doOpenFile(const std::string& filename) override;
        MAGNUM_OBJIMPORTER_LOCAL void doClose() override;

        MAGNUM_OBJIMPORTER_LOCAL UnsignedInt doMesh3DCount() const override;
        MAGNUM_OBJIMPORTER_LOCAL Int doMesh3DForName(const std::string& name) override;
        MAGNUM_OBJIMPORTER_LOCAL std::string doMesh3DName(UnsignedInt id) override;
        MAGNUM_OBJIMPORTER_LOCAL Containers::Optional<MeshData3D> doMesh3D(UnsignedInt id) override;

        MAGNUM_OBJIMPORTER_LOCAL UnsignedInt doMaterialCount() const override;
        MAGNUM_OBJIMPORTER_LOCAL std::unique_ptr<AbstractMaterialData> doMaterial(UnsignedInt id) override;

        MAGNUM_OBJIMPORTER_LOCAL UnsignedInt doImage2DCount() const;
        MAGNUM_OBJIMPORTER_LOCAL std::optional<ImageData2D> doImage2D(UnsignedInt id);

        MAGNUM_OBJIMPORTER_LOCAL void parse();
        MAGNUM_OBJIMPORTER_LOCAL void parseMaterialLibrary(std::string libname);

        Containers::Array<char> _in;
        std::unique_ptr<struct ImporterState> _state;
        std::string _fileRoot;
};

}}

#endif
