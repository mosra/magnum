#ifndef Magnum_Physics_ShapeGroup_h
#define Magnum_Physics_ShapeGroup_h
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
 * @brief Class Magnum::Physics::ShapeGroup
 */

#include "AbstractShape.h"

#include <type_traits>
#include <utility>

namespace Magnum { namespace Physics {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define enableIfIsBaseType typename std::enable_if<std::is_base_of<AbstractShape, T>::value, ShapeGroup>::type
#define enableIfAreBaseType typename std::enable_if<std::is_base_of<AbstractShape, T>::value && std::is_base_of<AbstractShape, U>::value, ShapeGroup>::type
#endif

/**
@brief Shape group

Result of logical operations on shapes.
See @ref collision-detection for brief introduction.
*/
class PHYSICS_EXPORT ShapeGroup: public AbstractShape {
    #ifndef DOXYGEN_GENERATING_OUTPUT
//     template<class T> friend constexpr enableIfIsBaseType operator~(const T& a);
//     template<class T> friend constexpr enableIfIsBaseType operator~(T&& a);
//     template<class T> friend constexpr enableIfIsBaseType operator~(T& a);
    template<class T> friend constexpr enableIfIsBaseType operator!(const T& a);
    template<class T> friend constexpr enableIfIsBaseType operator!(T&& a);
    template<class T> friend constexpr enableIfIsBaseType operator!(T& a);

    #define friendOp(char) \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(const T& a, const U& b); \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(const T& a, U&& b); \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(T&& a, const U& b); \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(T&& a, U&& b); \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(const T& a, std::reference_wrapper<U> b); \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(T&& a, std::reference_wrapper<U> b); \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(std::reference_wrapper<T> a, const U& b); \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(std::reference_wrapper<T> a, U&& b); \
        template<class T, class U> friend constexpr enableIfAreBaseType operator char(std::reference_wrapper<T> a, std::reference_wrapper<U> b);
//     friendOp(|)
//     friendOp(&)
//     friendOp(-)
//     friendOp(^)
    friendOp(&&)
    friendOp(||)
    #undef friendOp
    #endif

    ShapeGroup(const ShapeGroup& other) = delete;
    ShapeGroup& operator=(const ShapeGroup& other) = delete;

    private:
        enum Operation {
            RefA = 0x01,
            RefB = 0x02,
            RefAB = 0x03,
//             Complement = 1 << 2,
//             Union = 2 << 2,
//             Intersection = 3 << 2,
//             Difference = 4 << 2,
//             Xor = 5 << 2,
            And = 6 << 2,
            Or = 7 << 2,
            Not = 8 << 2,
            FirstObjectOnly = 9 << 2,
            AlwaysFalse = 10 << 2
        };

    public:
        /** @brief Default constructor */
        inline ShapeGroup(): operation(AlwaysFalse), a(nullptr), b(nullptr) {}

        /** @brief Move constructor */
        ShapeGroup(ShapeGroup&& other);

        /** @brief Destructor */
        ~ShapeGroup();

        /** @brief Move assignment */
        ShapeGroup& operator=(ShapeGroup&& other);

        void applyTransformation(const Matrix4& transformation);

        bool collides(const AbstractShape* other) const;

    protected:
        virtual Type type() const { return Type::ShapeGroup; }

    private:
        inline ShapeGroup(int operation, AbstractShape* a, AbstractShape* b): operation(operation), a(a), b(b) {}

        int operation;
        AbstractShape* a;
        AbstractShape* b;
};

// /* @brief Complement of shape */
// template<class T> inline constexpr enableIfIsBaseType operator~(const T& a) {
//     return ShapeGroup(ShapeGroup::Complement, new T(a), nullptr);
// }
// #ifndef DOXYGEN_GENERATING_OUTPUT
// template<class T> inline constexpr enableIfIsBaseType operator~(T&& a) {
//     return ShapeGroup(ShapeGroup::Complement, new T(std::forward<T>(a)), nullptr);
// }
// template<class T> inline constexpr enableIfIsBaseType operator~(T& a) {
//     return ShapeGroup(ShapeGroup::Complement|ShapeGroup::RefA, &a.get(), nullptr);
// }
// #endif

/** @relates ShapeGroup
@brief Logical NOT of shape
*/
template<class T> inline constexpr enableIfIsBaseType operator!(const T& a) {
    return ShapeGroup(ShapeGroup::Not, new T(a), nullptr);
}
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> inline constexpr enableIfIsBaseType operator!(T&& a) {
    return ShapeGroup(ShapeGroup::Not, new T(std::forward<T>(a)), nullptr);
}
template<class T> inline constexpr enableIfIsBaseType operator!(T& a) {
    return ShapeGroup(ShapeGroup::Not|ShapeGroup::RefA, &a.get(), nullptr);
}
#endif

#ifdef DOXYGEN_GENERATING_OUTPUT
// /* @brief Union of two shapes */
// template<class T, class U> inline constexpr ShapeGroup operator&(T a, U b);
//
// /* @brief Intersection of two shapes */
// template<class T, class U> inline constexpr ShapeGroup operator&(T a, U b);
//
// /* @brief Difference of two shapes */
// template<class T, class U> inline constexpr ShapeGroup operator-(T a, U b);
//
// /* @brief XOR of two shapes */
// template<class T, class U> inline constexpr ShapeGroup operator^(T a, U b);
/** @relates ShapeGroup
@brief Logical AND of two shapes

[Short-circuit evaluation](http://en.wikipedia.org/wiki/Short-circuit_evaluation)
is used here, so this operation can be used for providing simplified shape
version, because collision with @p b is computed only if @p a collides.
See @ref collision-detection-shape-simplification for an example.
*/
template<class T, class U> inline constexpr ShapeGroup operator&&(T a, U b);

/** @relates ShapeGroup
@brief Logical OR of two shapes

[Short-circuit evaluation](http://en.wikipedia.org/wiki/Short-circuit_evaluation)
is used, so if collision with @p a is detected, collision with @p b is not
computed.
*/
template<class T, class U> inline constexpr ShapeGroup operator||(T a, U b);
#else
#define op(type, char)                                                      \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(const T& a, const U& b) { \
    return ShapeGroup(ShapeGroup::type, new T(a), new U(b));                \
}                                                                           \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(const T& a, U&& b) { \
    return ShapeGroup(ShapeGroup::type, new T(a), new U(std::forward<U>(b))); \
}                                                                           \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(T&& a, const U& b) { \
    return ShapeGroup(ShapeGroup::type, new T(std::forward<T>(a)), new U(b)); \
}                                                                           \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(T&& a, U&& b) { \
    return ShapeGroup(ShapeGroup::type, new T(std::forward<T>(a)), new U(std::forward<U>(b))); \
}                                                                           \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(const T& a, std::reference_wrapper<U> b) { \
    return ShapeGroup(ShapeGroup::type|ShapeGroup::RefB, new T(a), &b.get()); \
}                                                                           \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(T&& a, std::reference_wrapper<U> b) { \
    return ShapeGroup(ShapeGroup::type|ShapeGroup::RefB, new T(std::forward<T>(a)), &b.get()); \
}                                                                           \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(std::reference_wrapper<T> a, const U& b) { \
    return ShapeGroup(ShapeGroup::type|ShapeGroup::RefA, &a.get(), new U(b)); \
}                                                                           \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(std::reference_wrapper<T> a, U&& b) { \
    return ShapeGroup(ShapeGroup::type|ShapeGroup::RefA, &a.get(), new U(std::forward<U>(b))); \
}                                                                           \
template<class T, class U> inline constexpr enableIfAreBaseType operator char(std::reference_wrapper<T> a, std::reference_wrapper<U> b) { \
    return ShapeGroup(ShapeGroup::type|ShapeGroup::RefAB, &a.get(), &b.get()); \
}
// op(Union, |)
// op(Intersection, &)
// op(Difference, -)
// op(Xor, ^)
op(And, &&)
op(Or, ||)
#undef op
#endif

#undef enableIfIsBaseType
#undef enableIfAreBaseType

}}

#endif
