#ifndef Magnum_Trade_ObjImporter_h
#define Magnum_Trade_ObjImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

namespace Magnum { namespace Trade {

/**
@brief OBJ importer plugin

Supported features:
-   multiple objects
-   vertex positions, normals and 2D texture coordinates
-   triangles, lines and points

Polygons (quads etc.), automatic normal generation and material properties are
currently not supported.

This plugin is built if `WITH_OBJIMPORTER` is enabled when building Magnum. To
use dynamic plugin, you need to load `ObjImporter` plugin from
`MAGNUM_PLUGINS_IMPORTER_DIR`. To use static plugin or use this as a dependency
of another plugin, you need to request `ObjImporter` component of `Magnum`
package in CMake and link to `Magnum::ObjImporter` target. See @ref building,
@ref cmake and @ref plugins for more information.
*/
class ObjImporter: public AbstractImporter {
    public:
        /** @brief Default constructor */
        explicit ObjImporter();

        /** @brief Plugin manager constructor */
        explicit ObjImporter(PluginManager::AbstractManager& manager, std::string plugin);

        ~ObjImporter();

    private:
        struct File;

        Features doFeatures() const override;

        bool doIsOpened() const override;
        void doOpenData(Containers::ArrayView<const char> data) override;
        void doOpenFile(const std::string& filename) override;
        void doClose() override;

        UnsignedInt doMesh3DCount() const override;
        Int doMesh3DForName(const std::string& name) override;
        std::string doMesh3DName(UnsignedInt id) override;
        std::optional<MeshData3D> doMesh3D(UnsignedInt id) override;

        void parseMeshNames();

        std::unique_ptr<File> _file;
};

}}

#endif
