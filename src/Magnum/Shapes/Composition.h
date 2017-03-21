#ifndef Magnum_Shapes_Composition_h
#define Magnum_Shapes_Composition_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::Shapes::Composition, typedef @ref Magnum::Shapes::Composition2D, @ref Magnum::Shapes::Composition3D, enum @ref Magnum::Shapes::CompositionOperation
 */

#include <type_traits>
#include <utility>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/Shapes/Shapes.h"
#include "Magnum/Shapes/shapeImplementation.h"
#include "Magnum/Shapes/visibility.h"

namespace Magnum { namespace Shapes {

namespace Implementation {
    template<class> struct ShapeHelper;

    template<UnsignedInt dimensions> inline AbstractShape<dimensions>& getAbstractShape(Composition<dimensions>& group, std::size_t i) {
        return *group._shapes[i];
    }
    template<UnsignedInt dimensions> inline const AbstractShape<dimensions>& getAbstractShape(const Composition<dimensions>& group, std::size_t i) {
        return *group._shapes[i];
    }
}

/** @brief Shape operation */
enum class CompositionOperation: UnsignedByte {
    Not,    /**< Boolean NOT */
    And,    /**< Boolean AND */
    Or      /**< Boolean OR */
};

/**
@brief Composition of shapes

Result of logical operations on shapes. See @ref shapes for brief introduction.
*/
template<UnsignedInt dimensions> class MAGNUM_SHAPES_EXPORT Composition {
    friend Implementation::AbstractShape<dimensions>& Implementation::getAbstractShape<>(Composition<dimensions>&, std::size_t);
    friend const Implementation::AbstractShape<dimensions>& Implementation::getAbstractShape<>(const Composition<dimensions>&, std::size_t);
    friend Implementation::ShapeHelper<Composition<dimensions>>;

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
            InvertedSphere, /**< @ref InvertedSphere "Inverted sphere" */
            Cylinder,       /**< @ref Cylinder */
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
         * Creates empty composition.
         */
        explicit Composition() {}

        /**
         * @brief Unary operation constructor
         * @param operation Unary operation
         * @param a         Operand
         */
        template<class T> explicit Composition(CompositionOperation operation, T&& a);

        /**
         * @brief Binary operation constructor
         * @param operation Binary operation
         * @param a         Left operand
         * @param b         Right operand
         */
        template<class T, class U> explicit Composition(CompositionOperation operation, T&& a, U&& b);

        /** @brief Copy constructor */
        Composition(const Composition<dimensions>& other);

        /** @brief Move constructor */
        Composition(Composition<dimensions>&& other);

        ~Composition();

        /** @brief Assigment operator */
        Composition<dimensions>& operator=(const Composition<dimensions>& other);

        /** @brief Move assignment operator */
        Composition<dimensions>& operator=(Composition<dimensions>&& other);

        /** @brief Transformed shape */
        Composition<dimensions> transformed(const MatrixTypeFor<dimensions, Float>& matrix) const;

        /** @brief Count of shapes in the hierarchy */
        std::size_t size() const { return _shapes.size(); }

        /** @brief Type of shape at given position */
        Type type(std::size_t i) const { return _shapes[i]->type(); }

        /** @brief Shape at given position */
        template<class T> const T& get(std::size_t i) const;

        /** @brief Collision with another shape */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class T> bool operator%(const T& other) const {
        #else
        template<class T> auto operator%(const T& other) const -> typename std::enable_if<std::is_same<decltype(Implementation::TypeOf<T>::type()), typename Implementation::ShapeDimensionTraits<dimensions>::Type>::value, bool>::type {
        #endif
            return collides(Implementation::Shape<T>(other));
        }

    private:
        struct Node {
            std::size_t rightNode, rightShape;
            CompositionOperation operation;
        };

        bool collides(const Implementation::AbstractShape<dimensions>& a) const {
            return collides(a, 0, 0, _shapes.size());
        }

        bool collides(const Implementation::AbstractShape<dimensions>& a, std::size_t node, std::size_t shapeBegin, std::size_t shapeEnd) const;

        template<class T> constexpr static std::size_t shapeCount(const T&) {
            return 1;
        }
        constexpr static std::size_t shapeCount(const Composition<dimensions>& hierarchy) {
            return hierarchy._shapes.size();
        }
        template<class T> constexpr static std::size_t nodeCount(const T&) {
            return 0;
        }
        constexpr static std::size_t nodeCount(const Composition<dimensions>& hierarchy) {
            return hierarchy._nodes.size();
        }

        template<class T> void copyShapes(std::size_t offset, const T& shape) {
            _shapes[offset] = new Implementation::Shape<T>(shape);
        }
        void copyShapes(std::size_t offset, Composition<dimensions>&& other);
        void copyShapes(std::size_t offset, const Composition<dimensions>& other);

        template<class T> void copyNodes(std::size_t, const T&) {}
        void copyNodes(std::size_t offset, const Composition<dimensions>& other);

        Containers::Array<Implementation::AbstractShape<dimensions>*> _shapes;
        Containers::Array<Node> _nodes;
};

/** @brief Two-dimensional shape composition */
typedef Composition<2> Composition2D;

/** @brief Three-dimensional shape composition */
typedef Composition<3> Composition3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{Magnum::Shapes::Composition,Magnum::Shapes::Composition::Type} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, typename Composition<dimensions>::Type value);
#endif

/** @relates Composition
@brief Collision occurence of shape with Composition
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T> inline bool operator%(const T& a, const Composition<dimensions>& b) {
#else
template<UnsignedInt dimensions, class T> inline auto operator%(const T& a, const Composition<dimensions>& b) -> typename std::enable_if<std::is_same<decltype(Implementation::TypeOf<T>::type()), typename Implementation::ShapeDimensionTraits<dimensions>::Type>::value, bool>::type {
#endif
    return b % a;
}

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @relates Composition
@brief Logical NOT of shape
*/
template<class T> inline Composition<T::Dimensions> operator!(T a);

/** @relates Composition
@brief Logical AND of two shapes

[Short-circuit evaluation](http://en.wikipedia.org/wiki/Short-circuit_evaluation)
is used here, so this operation can be used for providing simplified shape
version, because collision with @p b is computed only if @p a collides.
See @ref shapes-simplification for an example.
*/
template<class T> inline Composition<T::Dimensions> operator&&(T a, T b);

/** @relates Composition
@brief Logical OR of two shapes

[Short-circuit evaluation](http://en.wikipedia.org/wiki/Short-circuit_evaluation)
is used, so if collision with @p a is detected, collision with @p b is not
computed.
*/
template<class T> inline Composition<T::Dimensions> operator||(T a, T b);
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#define enableIfIsShapeType typename std::enable_if< \
    std::is_same<decltype(Implementation::TypeOf<T>::type()), typename Implementation::ShapeDimensionTraits<T::Dimensions>::Type>::value, \
    Composition<T::Dimensions>>::type
#define enableIfAreShapeType typename std::enable_if< \
    std::is_same<decltype(Implementation::TypeOf<T>::type()), typename Implementation::ShapeDimensionTraits<T::Dimensions>::Type>::value && \
    std::is_same<decltype(Implementation::TypeOf<U>::type()), typename Implementation::ShapeDimensionTraits<T::Dimensions>::Type>::value, \
    Composition<T::Dimensions>>::type
template<class T> inline auto operator!(T&& a) -> enableIfIsShapeType {
    return Composition<T::Dimensions>(CompositionOperation::Not, std::forward<T>(a));
}
template<class T, class U> inline auto operator&&(T&& a, U&& b) -> enableIfAreShapeType {
    return Composition<T::Dimensions>(CompositionOperation::And, std::forward<T>(a), std::forward<U>(b));
}
template<class T, class U> inline auto operator||(T&& a, U&& b) -> enableIfAreShapeType {
    return Composition<T::Dimensions>(CompositionOperation::Or, std::forward<T>(a), std::forward<U>(b));
}
#undef enableIfIsShapeType
#undef enableIfAreShapeType
#endif

template<UnsignedInt dimensions> template<class T> Composition<dimensions>::Composition(CompositionOperation operation, T&& a): _shapes(shapeCount(a)), _nodes(nodeCount(a)+1) {
    CORRADE_ASSERT(operation == CompositionOperation::Not,
        "Shapes::Composition::Composition(): unary operation expected", );
    _nodes[0].operation = operation;

    /* 0 = no children, 1 = left child only */
    _nodes[0].rightNode = (nodeCount(a) == 0 ? 0 : 1);
    _nodes[0].rightShape = shapeCount(a);
    copyNodes(1, a);
    copyShapes(0, std::forward<T>(a));
}

template<UnsignedInt dimensions> template<class T, class U> Composition<dimensions>::Composition(CompositionOperation operation, T&& a, U&& b): _shapes(shapeCount(a) + shapeCount(b)), _nodes(nodeCount(a) + nodeCount(b) + 1) {
    CORRADE_ASSERT(operation != CompositionOperation::Not,
        "Shapes::Composition::Composition(): binary operation expected", );
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

template<UnsignedInt dimensions> template<class T> inline const T& Composition<dimensions>::get(std::size_t i) const {
    CORRADE_ASSERT(_shapes[i]->type() == Implementation::TypeOf<T>::type(),
        "Shapes::Composition::get(): given shape is not of type" << Implementation::TypeOf<T>::type() <<
        "but" << _shapes[i]->type(), *static_cast<T*>(nullptr));
    return static_cast<Implementation::Shape<T>*>(_shapes[i])->shape;
}

}}

#endif
