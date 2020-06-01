/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Interpolation.h"

#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/DualQuaternion.h"

namespace Magnum { namespace Animation {

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const Interpolation value) {
    debug << "Animation::Interpolation" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Interpolation::value: return debug << "::" #value;
        _c(Constant)
        _c(Linear)
        _c(Spline)
        _c(Custom)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Extrapolation value) {
    debug << "Animation::Extrapolation" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Extrapolation::value: return debug << "::" #value;
        _c(DefaultConstructed)
        _c(Constant)
        _c(Extrapolated)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}
#endif

namespace Implementation {

template<class T> auto TypeTraits<Math::Complex<T>, Math::Complex<T>>::interpolator(Interpolation interpolation) -> Interpolator {
    switch(interpolation) {
        case Interpolation::Constant: return Math::select;
        case Interpolation::Linear: return Math::slerp;

        case Interpolation::Spline:
        case Interpolation::Custom: ; /* nope */
    }

    CORRADE_ASSERT_UNREACHABLE("Animation::interpolatorFor(): can't deduce interpolator function for" << interpolation, {});
}

template<class T> auto TypeTraits<Math::Quaternion<T>, Math::Quaternion<T>>::interpolator(Interpolation interpolation) -> Interpolator {
    switch(interpolation) {
        case Interpolation::Constant: return Math::select;
        case Interpolation::Linear: return Math::slerpShortestPath;

        case Interpolation::Spline:
        case Interpolation::Custom: ; /* nope */
    }

    CORRADE_ASSERT_UNREACHABLE("Animation::interpolatorFor(): can't deduce interpolator function for" << interpolation, {});
}

template<class T> auto TypeTraits<Math::DualQuaternion<T>, Math::DualQuaternion<T>>::interpolator(Interpolation interpolation) -> Interpolator {
    switch(interpolation) {
        case Interpolation::Constant: return Math::select;
        case Interpolation::Linear: return Math::sclerpShortestPath;

        case Interpolation::Spline:
        case Interpolation::Custom: ; /* nope */
    }

    CORRADE_ASSERT_UNREACHABLE("Animation::interpolatorFor(): can't deduce interpolator function for" << interpolation, {});
}

template<class T> auto TypeTraits<Math::CubicHermite<T>, T>::interpolator(Interpolation interpolation) -> Interpolator {
    switch(interpolation) {
        case Interpolation::Constant: return Math::select;
        case Interpolation::Linear: return Math::lerp;
        case Interpolation::Spline: return Math::splerp;

        case Interpolation::Custom: ; /* nope */
    }

    CORRADE_ASSERT_UNREACHABLE("Animation::interpolatorFor(): can't deduce interpolator function for" << interpolation, {});
}

template struct MAGNUM_EXPORT TypeTraits<Math::Complex<Float>, Math::Complex<Float>>;
template struct MAGNUM_EXPORT TypeTraits<Math::Quaternion<Float>, Math::Quaternion<Float>>;
template struct MAGNUM_EXPORT TypeTraits<Math::DualQuaternion<Float>, Math::DualQuaternion<Float>>;
template struct MAGNUM_EXPORT TypeTraits<Math::CubicHermite<Float>, Float>;
template struct MAGNUM_EXPORT TypeTraits<Math::CubicHermite<Math::Vector2<Float>>, Math::Vector2<Float>>;
template struct MAGNUM_EXPORT TypeTraits<Math::CubicHermite<Math::Vector3<Float>>, Math::Vector3<Float>>;
template struct MAGNUM_EXPORT TypeTraits<Math::CubicHermite<Math::Complex<Float>>, Math::Complex<Float>>;
template struct MAGNUM_EXPORT TypeTraits<Math::CubicHermite<Math::Quaternion<Float>>, Math::Quaternion<Float>>;

}

}}
