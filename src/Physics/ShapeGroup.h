#ifndef Magnum_Physics_ShapeGroup_h
#define Magnum_Physics_ShapeGroup_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Physics::ShapeGroup, enum Magnum::Physics::ShapeOperation
 */

#include <type_traits>
#include <utility>
#include <Utility/Assert.h>

#include "DimensionTraits.h"
#include "Physics/Physics.h"
#include "Physics/magnumPhysicsVisibility.h"
#include "Physics/shapeImplementation.h"

namespace Magnum { namespace Physics {

namespace Implementation {
    template<class> struct ObjectShapeHelper;

    template<UnsignedInt dimensions> inline AbstractShape<dimensions>* getAbstractShape(ShapeGroup<dimensions>& group, std::size_t i) {
        return group._shapes[i];
    }
    template<UnsignedInt dimensions> inline const AbstractShape<dimensions>* getAbstractShape(const ShapeGroup<dimensions>& group, std::size_t i) {
        return group._shapes[i];
    }
}

/** @brief Shape operation */
enum class ShapeOperation: UnsignedByte {
    Not,    /**< Boolean NOT */
    And,    /**< Boolean AND */
    Or      /**< Boolean OR */
};

/**
@brief Shape group

Result of logical operations on shapes.
See @ref collision-detection for brief introduction.
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT ShapeGroup {
    friend Implementation::AbstractShape<dimensions>* Implementation::getAbstractShape<>(ShapeGroup<dimensions>&, std::size_t);
    friend const Implementation::AbstractShape<dimensions>* Implementation::getAbstractShape<>(const ShapeGroup<dimensions>&, std::size_t);
    friend struct Implementation::ObjectShapeHelper<ShapeGroup<dimensions>>;

    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Dimension count */
        };

        /** @brief Shape type */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class Type {
            Point,          /**< Point */
            Line,           /**< Line */
            LineSegment,    /**< @ref LineSegment "Line segment" */
            Sphere,         /**< Sphere */
            Capsule,        /**< Capsule */
            AxisAlignedBox, /**< @ref AxisAlignedBox "Axis aligned box" */
            Box,            /**< Box */
            Plane           /**< Plane (3D only) */
        };
        #else
        typedef typename Implementation::ShapeDimensionTraits<dimensions>::Type Type;
        #endif

        /**
         * @brief Default constructor
         *
         * Creates empty hierarchy.
         */
        inline explicit ShapeGroup(): _shapeCount(0), _nodeCount(0), _shapes(nullptr), _nodes(nullptr) {}

        /**
         * @brief Unary operation constructor
         * @param operation Unary operation
         * @param a         Operand
         */
        template<class T> explicit ShapeGroup(ShapeOperation operation, T&& a);

        /**
         * @brief Binary operation constructor
         * @param operation Binary operation
         * @param a         Left operand
         * @param b         Right operand
         */
        template<class T, class U> explicit ShapeGroup(ShapeOperation operation, T&& a, U&& b);

        /** @brief Copy constructor */
        ShapeGroup(const ShapeGroup<dimensions>& other);

        /** @brief Move constructor */
        ShapeGroup(ShapeGroup<dimensions>&& other);

        ~ShapeGroup();

        /** @brief Assigment operator */
        ShapeGroup<dimensions>& operator=(const ShapeGroup<dimensions>& other);

        /** @brief Move assignment operator */
        ShapeGroup<dimensions>& operator=(ShapeGroup<dimensions>&& other);

        /** @brief Transformed shape */
        ShapeGroup<dimensions> transformed(const typename DimensionTraits<dimensions>::MatrixType& matrix) const;

        /** @brief Count of shapes in the hierarchy */
        inline std::size_t size() const { return _shapeCount; }

        /** @brief Type of shape at given position */
        inline Type type(std::size_t i) const { return _shapes[i]->type(); }

        /** @brief Shape at given position */
        template<class T> const T& get(std::size_t i) const;

        /** @brief Collision with another shape */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class T> inline bool operator%(const T& other) const {
        #else
        template<class T> inline auto operator%(const T& other) const -> typename std::enable_if<std::is_same<decltype(Implementation::TypeOf<T>::type()), typename Implementation::ShapeDimensionTraits<dimensions>::Type>::value, bool>::type {
        #endif
            Implementation::Shape<T> a(other);
            return collides(&a);
        }

    private:
        struct Node {
            std::size_t rightNode, rightShape;
            ShapeOperation operation;
        };

        inline bool collides(const Implementation::AbstractShape<dimensions>* a) const {
            return collides(a, 0, 0, _shapeCount);
        }

        bool collides(const Implementation::AbstractShape<dimensions>* a, std::size_t node, std::size_t shapeBegin, std::size_t shapeEnd) const;

        template<class T> inline constexpr static std::size_t shapeCount(const T&) {
            return 1;
        }
        inline constexpr static std::size_t shapeCount(const ShapeGroup<dimensions>& hierarchy) {
            return hierarchy._shapeCount;
        }
        template<class T> inline constexpr static std::size_t nodeCount(const T&) {
            return 0;
        }
        inline constexpr static std::size_t nodeCount(const ShapeGroup<dimensions>& hierarchy) {
            return hierarchy._nodeCount;
        }

        template<class T> inline void copyShapes(std::size_t offset, const T& shape) {
            _shapes[offset] = new Implementation::Shape<T>(shape);
        }
        void copyShapes(std::size_t offset, ShapeGroup<dimensions>&& other);
        void copyShapes(std::size_t offset, const ShapeGroup<dimensions>& other);

        template<class T> inline void copyNodes(std::size_t, const T&) {}
        void copyNodes(std::size_t offset, const ShapeGroup<dimensions>& other);

        std::size_t _shapeCount, _nodeCount;
        Implementation::AbstractShape<dimensions>** _shapes;
        Node* _nodes;
};

/** @brief Two-dimensional shape hierarchy */
typedef ShapeGroup<2> ShapeGroup2D;

/** @brief Three-dimensional shape hierarchy */
typedef ShapeGroup<3> ShapeGroup3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperator{Magnum::Physics::ShapeGroup} */
template<UnsignedInt dimensions> Debug operator<<(Debug debug, typename ShapeGroup<dimensions>::Type value);
#endif

/** @relates ShapeGroup
@brief Collision of shape with ShapeGroup
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T> inline bool operator%(const T& a, const ShapeGroup<dimensions>& b) {
#else
template<UnsignedInt dimensions, class T> inline auto operator%(const T& a, const ShapeGroup<dimensions>& b) -> typename std::enable_if<std::is_same<decltype(Implementation::TypeOf<T>::type()), typename Implementation::ShapeDimensionTraits<dimensions>::Type>::value, bool>::type {
#endif
    return b % a;
}

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @relates ShapeGroup
@brief Logical NOT of shape
*/
template<class T> inline ShapeGroup<T::Dimensions> operator!(T a);

/** @relates ShapeGroup
@brief Logical AND of two shapes

[Short-circuit evaluation](http://en.wikipedia.org/wiki/Short-circuit_evaluation)
is used here, so this operation can be used for providing simplified shape
version, because collision with @p b is computed only if @p a collides.
See @ref collision-detection-shape-simplification for an example.
*/
template<class T> inline ShapeGroup<T::Dimensions> operator&&(T a, T b);

/** @relates ShapeGroup
@brief Logical OR of two shapes

[Short-circuit evaluation](http://en.wikipedia.org/wiki/Short-circuit_evaluation)
is used, so if collision with @p a is detected, collision with @p b is not
computed.
*/
template<class T> inline ShapeGroup<T::Dimensions> operator||(T a, T b);
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#define enableIfIsShapeType typename std::enable_if< \
    std::is_same<decltype(Implementation::TypeOf<T>::type()), typename Implementation::ShapeDimensionTraits<T::Dimensions>::Type>::value, \
    ShapeGroup<T::Dimensions>>::type
#define enableIfAreShapeType typename std::enable_if< \
    std::is_same<decltype(Implementation::TypeOf<T>::type()), typename Implementation::ShapeDimensionTraits<T::Dimensions>::Type>::value && \
    std::is_same<decltype(Implementation::TypeOf<U>::type()), typename Implementation::ShapeDimensionTraits<T::Dimensions>::Type>::value, \
    ShapeGroup<T::Dimensions>>::type
template<class T> inline auto operator!(T&& a) -> enableIfIsShapeType {
    return ShapeGroup<T::Dimensions>(ShapeOperation::Not, std::forward<T>(a));
}
template<class T, class U> inline auto operator&&(T&& a, U&& b) -> enableIfAreShapeType {
    return ShapeGroup<T::Dimensions>(ShapeOperation::And, std::forward<T>(a), std::forward<U>(b));
}
template<class T, class U> inline auto operator||(T&& a, U&& b) -> enableIfAreShapeType {
    return ShapeGroup<T::Dimensions>(ShapeOperation::Or, std::forward<T>(a), std::forward<U>(b));
}
#undef enableIfIsShapeType
#undef enableIfAreShapeType
#endif

template<UnsignedInt dimensions> template<class T> ShapeGroup<dimensions>::ShapeGroup(ShapeOperation operation, T&& a): _shapeCount(shapeCount(a)), _nodeCount(nodeCount(a)+1), _shapes(new Implementation::AbstractShape<dimensions>*[_shapeCount]), _nodes(new Node[_nodeCount]) {
    CORRADE_ASSERT(operation == ShapeOperation::Not,
        "Physics::ShapeGroup::ShapeGroup(): unary operation expected", );
    _nodes[0].operation = operation;

    /* 0 = no children, 1 = left child only */
    _nodes[0].rightNode = (nodeCount(a) == 0 ? 0 : 1);
    _nodes[0].rightShape = shapeCount(a);
    copyNodes(1, a);
    copyShapes(0, std::forward<T>(a));
}

template<UnsignedInt dimensions> template<class T, class U> ShapeGroup<dimensions>::ShapeGroup(ShapeOperation operation, T&& a, U&& b): _shapeCount(shapeCount(a) + shapeCount(b)), _nodeCount(nodeCount(a) + nodeCount(b) + 1), _shapes(new Implementation::AbstractShape<dimensions>*[_shapeCount]), _nodes(new Node[_nodeCount]) {
    CORRADE_ASSERT(operation != ShapeOperation::Not,
        "Physics::ShapeGroup::ShapeGroup(): binary operation expected", );
    _nodes[0].operation = operation;

    /* 0 = no children, 1 = left child only, 2 = right child only, >2 = both */
    if(nodeCount(a) == 0 && nodeCount(b) == 0)
        _nodes[0].rightNode = 0;
    else if(nodeCount(b) == 0)
        _nodes[0].rightNode = 1;
    else _nodes[0].rightNode = nodeCount(a) + 2;

    _nodes[0].rightShape = shapeCount(a);
    copyNodes(1, a);
    copyNodes(nodeCount(a) + 1, b);
    copyShapes(shapeCount(a), std::forward<U>(b));
    copyShapes(0, std::forward<T>(a));
}

template<UnsignedInt dimensions> template<class T> inline const T& ShapeGroup<dimensions>::get(std::size_t i) const {
    CORRADE_ASSERT(_shapes[i]->type() == Implementation::TypeOf<T>::type(),
        "Physics::ShapeGroup::get(): given shape is not of type" << Implementation::TypeOf<T>::type() <<
        "but" << _shapes[i]->type(), *static_cast<T*>(nullptr));
    return static_cast<Implementation::Shape<T>*>(_shapes[i])->shape;
}

}}

#endif
