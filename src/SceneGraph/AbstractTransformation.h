#ifndef Magnum_SceneGraph_AbstractTransformation_h
#define Magnum_SceneGraph_AbstractTransformation_h
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
 * @brief Class Magnum::SceneGraph::AbstractTransformation, enum Magnum::SceneGraph::TransformationType, alias Magnum::SceneGraph::AbstractTransformation2D, Magnum::SceneGraph::AbstractTransformation3D
 */

#include <vector>

#include "DimensionTraits.h"
#include "SceneGraph.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for transformations

Provides transformation implementation for Object instances. See @ref scenegraph
for introduction.

@section AbstractTransformation-subclassing Subclassing

When sublassing, you have to:

- Implement all members described in **Subclass implementation** group above
- Provide implicit (parameterless) constructor

@see AbstractTransformation2D, AbstractTransformation3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint8_t dimensions, class T>
#else
template<std::uint8_t dimensions, class T = GLfloat>
#endif
class AbstractTransformation {
    public:
        /** @brief Underlying floating-point type */
        typedef T Type;

        /** @brief Dimension count */
        static const std::uint8_t Dimensions = dimensions;

        virtual ~AbstractTransformation() = 0;

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @{ @name Subclass implementation
         *
         * These members must be defined by the implementation.
         */

        /**
         * @todo Common way to call setClean() on the object after setting
         *      transformation & disallowing transformation setting on scene,
         *      so the implementer doesn't forget to do it? It could also
         *      allow to hide Object::isScene() from unwanted publicity.
         */

        /**
         * @brief Transformation data type
         *
         * The type must satisfy the following requirements:
         *
         * - Default constructor must create identity transformation
         *
         * Defined in subclasses.
         */
        typedef U DataType;

        /**
         * @brief Convert transformation to matrix
         *
         * Defined in subclasses.
         */
        static typename DimensionTraits<dimensions, T>::MatrixType toMatrix(const DataType& transformation);

        /**
         * @brief Convert transformation from matrix
         *
         * Defined in subclasses.
         */
        static DataType fromMatrix(const typename DimensionTraits<dimensions, T>::MatrixType& matrix);

        /**
         * @brief Compose transformations
         *
         * Defined in subclasses.
         */
        static DataType compose(const DataType& parent, const DataType& child);

        /**
         * @brief Inverted transformation
         *
         * Defined in subclasses.
         */
        static DataType inverted(const DataType& transformation);

        /**
         * @brief %Object transformation
         *
         * Relative to parent. Defined in subclasses.
         */
        DataType transformation() const;

        /**
         * @brief Absolute transformation
         *
         * Relative to root object. Defined in subclasses.
         */
        DataType absoluteTransformation() const;

        /*@}*/
        #endif
};

/** @brief Transformation type */
enum class TransformationType: std::uint8_t {
    /** Global transformation, applied after all other transformations. */
    Global = 0x00,

    /** Local transformation, applied before all other transformations. */
    Local = 0x01
};

template<std::uint8_t dimensions, class T> inline AbstractTransformation<dimensions, T>::~AbstractTransformation() {}

/**
@brief Base for two-dimensional transformations

Convenience alternative to <tt>%AbstractTransformation<2, T></tt>. See
AbstractTransformation for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractTransformation<2, T></tt>
    instead.
@see AbstractTransformation3D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractTransformation2D = AbstractTransformation<2, T>;
#endif
#else
typedef AbstractTransformation<2, T = GLfloat> AbstractTransformation2D;
#endif

/**
@brief Base for three-dimensional transformations

Convenience alternative to <tt>%AbstractTransformation<3, T></tt>. See
AbstractTransformation for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractTransformation<3, T></tt>
    instead.
@see AbstractTransformation2D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractTransformation3D = AbstractTransformation<3, T>;
#endif
#else
typedef AbstractTransformation<3, T = GLfloat> AbstractTransformation3D;
#endif

}}

#endif
