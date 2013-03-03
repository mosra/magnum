#ifndef Magnum_Shaders_AbstractVectorShader_h
#define Magnum_Shaders_AbstractVectorShader_h
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
 * @brief Class Magnum::Shaders::AbstractVectorShader
 */

#include "AbstractShaderProgram.h"
#include "Color.h"
#include "DimensionTraits.h"

namespace Magnum { namespace Shaders {

/**
@brief Base for vector shaders

@see AbstractVectorShader2D, AbstractVectorShader3D
*/
template<UnsignedInt dimensions> class AbstractVectorShader: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef Attribute<0, typename DimensionTraits<dimensions>::VectorType> Position;

        /** @brief Texture coordinates */
        typedef Attribute<1, Vector2> TextureCoordinates;

        enum: GLint {
            VectorTextureLayer = 16 /**< Layer for vector texture */
        };

        ~AbstractVectorShader() = 0;
};

template<UnsignedInt dimensions> inline AbstractVectorShader<dimensions>::~AbstractVectorShader() {}

/** @brief Base for two-dimensional text shaders */
typedef AbstractVectorShader<2> AbstractVectorShader2D;

/** @brief Base for three-dimensional text shader */
typedef AbstractVectorShader<3> AbstractVectorShader3D;

}}

#endif
