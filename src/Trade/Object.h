#ifndef Magnum_Trade_Object_h
#define Magnum_Trade_Object_h
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
 * @brief Class Magnum::Trade::Object
 */

#include "Magnum.h"

namespace Magnum { namespace Trade {

/**
@brief Object

Provides access to object transformation and hierarchy.
*/
struct MAGNUM_EXPORT Object {
    Object(const Object& other) = delete;
    Object(Object&& other) = delete;
    Object& operator=(const Object& other) = delete;
    Object& operator=(Object&& other) = delete;

    public:
        /**
         * @brief Constructor
         */
        Object(size_t parent, const Matrix4& transformation): _parent(parent), _transformation(transformation) {}

        inline size_t parent() const { return _parent; }
        inline size_t transformation() const { return _transformation; }

    private:
        size_t _parent;
        Matrix4 _transformation;
};

}}

#endif
