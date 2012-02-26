#ifndef Magnum_Trade_ObjectData_h
#define Magnum_Trade_ObjectData_h
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
 * @brief Class Magnum::Trade::ObjectData
 */

#include "Magnum.h"

namespace Magnum { namespace Trade {

/**
@brief %Object data

Provides access to object transformation and hierarchy.
*/
class MAGNUM_EXPORT ObjectData {
    ObjectData(const ObjectData& other) = delete;
    ObjectData(ObjectData&& other) = delete;
    ObjectData& operator=(const ObjectData& other) = delete;
    ObjectData& operator=(ObjectData&& other) = delete;

    public:
        /**
         * @brief Constructor
         */
        ObjectData(size_t parent, const Matrix4& transformation): _parent(parent), _transformation(transformation) {}

        /** @brief Parent object */
        inline size_t parent() const { return _parent; }

        /** @brief Transformation relative to parent */
        inline size_t transformation() const { return _transformation; }

    private:
        size_t _parent;
        Matrix4 _transformation;
};

}}

#endif
