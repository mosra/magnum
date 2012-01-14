#ifndef Magnum_MeshTools_AbstractTool_h
#define Magnum_MeshTools_AbstractTool_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::MeshTools::AbstractTool
 */

#include "MeshBuilder.h"

namespace Magnum {

/**
 * @brief %Mesh tools
 *
 * Tools for generating, optimizing and cleaning meshes.
 */
namespace MeshTools {

/**
 * @brief Base class for mesh tools
 *
 * Provides access to internals of MeshBuilder instance for subclasses. See
 * also AbstractIndexTool.
 */
template<class Vertex> class AbstractTool {
    public:
        /**
         * @brief Constructor
         * @param builder   %Mesh builder to operate on
         */
        inline AbstractTool(MeshBuilder<Vertex>& builder): builder(builder), vertices(builder._vertices), indices(builder._indices) {}

    protected:
        MeshBuilder<Vertex>& builder;       /**< @brief Builder instance */
        std::vector<Vertex>& vertices;      /**< @brief Builder vertices */
        std::vector<unsigned int>& indices; /**< @brief Builder indices */
};

/**
 * @brief Base class for mesh tools operating only on indices
 *
 * Provides access only to index array and vertex count. See also
 * AbstractTool.
 */
class AbstractIndexTool {
    public:
        /**
         * @brief Constructor
         * @param builder   %Mesh builder to operate on
         */
        template<class Vertex> inline AbstractIndexTool(MeshBuilder<Vertex>& builder): indices(builder._indices), vertexCount(builder.vertexCount()) {}

    protected:
        std::vector<unsigned int>& indices; /**< @brief Builder indices */
        const unsigned int vertexCount;     /**< @brief Count of builder vertices */
};

}}

#endif
