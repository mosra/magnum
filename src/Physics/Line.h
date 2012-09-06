#ifndef Magnum_Physics_Line_h
#define Magnum_Physics_Line_h
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
 * @brief Class Magnum::Physics::Line
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"

namespace Magnum { namespace Physics {

/** @brief Infinite line, defined by two points */
class PHYSICS_EXPORT Line: public AbstractShape {
    public:
        /** @brief Constructor */
        inline Line(const Vector3& a, const Vector3& b): _a(a), _transformedA(a), _b(b), _transformedB(b) {}

        void applyTransformation(const Matrix4& transformation);

        inline Vector3 a() const { return _a; }         /**< @brief First point */
        inline Vector3 b() const { return _a; }         /**< @brief Second point */

        inline void setA(const Vector3& a) { _a = a; }  /**< @brief Set first point */
        inline void setB(const Vector3& b) { _b = b; }  /**< @brief Set second point */

        /** @brief Transformed first point */
        inline Vector3 transformedA() const { return _transformedA; }

        /** @brief Transformed second point */
        inline Vector3 transformedB() const { return _transformedB; }

    protected:
        inline Type type() const { return Type::Line; }

    private:
        Vector3 _a, _transformedA,
            _b, _transformedB;
};

}}

#endif
