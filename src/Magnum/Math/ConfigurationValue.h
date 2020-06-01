#ifndef Magnum_Math_ConfigurationValue_h
#define Magnum_Math_ConfigurationValue_h
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

/** @file
 * @brief Configuration value parsers and writers for @ref Magnum::Math types
 * @m_since{2019,10}
 */

#include <string>
#include <Corrade/Utility/ConfigurationValue.h>

#include "Magnum/Math/Angle.h"
#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Complex.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/RectangularMatrix.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Math/Vector.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Vector4.h"

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::Deg} */
template<class T> struct ConfigurationValue<Magnum::Math::Deg<T>> {
    ConfigurationValue() = delete;

    /** @brief Writes degrees as a number */
    static std::string toString(const Magnum::Math::Deg<T>& value, ConfigurationValueFlags flags) {
        return ConfigurationValue<T>::toString(T(value), flags);
    }

    /** @brief Reads degrees as a number */
    static Magnum::Math::Deg<T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        return Magnum::Math::Deg<T>(ConfigurationValue<T>::fromString(stringValue, flags));
    }
};

/** @configurationvalue{Magnum::Math::Rad} */
template<class T> struct ConfigurationValue<Magnum::Math::Rad<T>> {
    ConfigurationValue() = delete;

    /** @brief Writes degrees as a number */
    static std::string toString(const Magnum::Math::Rad<T>& value, ConfigurationValueFlags flags) {
        return ConfigurationValue<T>::toString(T(value), flags);
    }

    /** @brief Reads degrees as a number */
    static Magnum::Math::Rad<T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        return Magnum::Math::Rad<T>(ConfigurationValue<T>::fromString(stringValue, flags));
    }
};

/** @configurationvalue{Magnum::Math::Vector} */
template<std::size_t size, class T> struct ConfigurationValue<Magnum::Math::Vector<size, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Vector<size, T>& value, ConfigurationValueFlags flags) {
        std::string output;

        for(std::size_t i = 0; i != size; ++i) {
            if(!output.empty()) output += ' ';
            output += ConfigurationValue<T>::toString(value[i], flags);
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Vector<size, T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        Magnum::Math::Vector<size, T> result;

        std::size_t oldpos = 0, pos = std::string::npos, i = 0;
        do {
            pos = stringValue.find(' ', oldpos);
            std::string part = stringValue.substr(oldpos, pos-oldpos);

            if(!part.empty()) {
                result[i] = ConfigurationValue<T>::fromString(part, flags);
                ++i;
            }

            oldpos = pos+1;
        } while(pos != std::string::npos && i != size);

        return result;
    }
};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::UnsignedInt>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::UnsignedInt>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::UnsignedInt>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Double>>;
#endif

/** @configurationvalue{Magnum::Math::Vector2} */
template<class T> struct ConfigurationValue<Magnum::Math::Vector2<T>>: public ConfigurationValue<Magnum::Math::Vector<2, T>> {};

/** @configurationvalue{Magnum::Math::Vector3} */
template<class T> struct ConfigurationValue<Magnum::Math::Vector3<T>>: public ConfigurationValue<Magnum::Math::Vector<3, T>> {};

/** @configurationvalue{Magnum::Math::Vector4} */
template<class T> struct ConfigurationValue<Magnum::Math::Vector4<T>>: public ConfigurationValue<Magnum::Math::Vector<4, T>> {};

/** @configurationvalue{Magnum::Color3} */
template<class T> struct ConfigurationValue<Magnum::Math::Color3<T>>: ConfigurationValue<Magnum::Math::Vector<3, T>> {};

/** @configurationvalue{Magnum::Color4} */
template<class T> struct ConfigurationValue<Magnum::Math::Color4<T>>: ConfigurationValue<Magnum::Math::Vector<4, T>> {};

/* No explicit instantiation for Vector[234] / Color[34] needed, as
   Vector<2, T> / Vector<3, T> / Vector<4, T> is instantiated already */

/** @configurationvalue{Magnum::Math::RectangularMatrix} */
template<std::size_t cols, std::size_t rows, class T> struct ConfigurationValue<Magnum::Math::RectangularMatrix<cols, rows, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::RectangularMatrix<cols, rows, T>& value, ConfigurationValueFlags flags) {
        std::string output;

        for(std::size_t row = 0; row != rows; ++row) {
            for(std::size_t col = 0; col != cols; ++col) {
                if(!output.empty()) output += ' ';
                output += ConfigurationValue<T>::toString(value[col][row], flags);
            }
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::RectangularMatrix<cols, rows, T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        Magnum::Math::RectangularMatrix<cols, rows, T> result;

        std::size_t oldpos = 0, pos = std::string::npos, i = 0;
        do {
            pos = stringValue.find(' ', oldpos);
            std::string part = stringValue.substr(oldpos, pos-oldpos);

            if(!part.empty()) {
                result[i%cols][i/cols] = ConfigurationValue<T>::fromString(part, flags);
                ++i;
            }

            oldpos = pos+1;
        } while(pos != std::string::npos && i != cols*rows);

        return result;
    }
};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
/* Square matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Double>>;

/* Rectangular matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Double>>;
#endif

/** @configurationvalue{Magnum::Math::Matrix} */
template<std::size_t size, class T> struct ConfigurationValue<Magnum::Math::Matrix<size, T>>: public ConfigurationValue<Magnum::Math::RectangularMatrix<size, size, T>> {};

/** @configurationvalue{Magnum::Math::Matrix3} */
template<class T> struct ConfigurationValue<Magnum::Math::Matrix3<T>>: public ConfigurationValue<Magnum::Math::Matrix3x3<T>> {};

/** @configurationvalue{Magnum::Math::Matrix4} */
template<class T> struct ConfigurationValue<Magnum::Math::Matrix4<T>>: public ConfigurationValue<Magnum::Math::Matrix4x4<T>> {};

/* No explicit instantiation for Matrix needed, as RectangularMatrix is
   instantiated already */

/** @configurationvalue{Magnum::Math::Range} */
template<Magnum::UnsignedInt dimensions, class T> struct ConfigurationValue<Magnum::Math::Range<dimensions, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Range<dimensions, T>& value, const ConfigurationValueFlags flags) {
        return ConfigurationValue<Magnum::Math::Vector<dimensions*2, T>>::toString(
            reinterpret_cast<const Magnum::Math::Vector<dimensions*2, T>&>(value), flags);
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Range<dimensions, T> fromString(const std::string& stringValue, const ConfigurationValueFlags flags) {
        const auto vec = ConfigurationValue<Magnum::Math::Vector<dimensions*2, T>>::fromString(stringValue, flags);
        return *reinterpret_cast<const Magnum::Math::Range<dimensions, T>*>(vec.data());
    }
};

/** @configurationvalue{Magnum::Math::Range2D} */
template<class T> struct ConfigurationValue<Magnum::Math::Range2D<T>>: public ConfigurationValue<Magnum::Math::Range<2, T>> {};

/** @configurationvalue{Magnum::Math::Range3D} */
template<class T> struct ConfigurationValue<Magnum::Math::Range3D<T>>: public ConfigurationValue<Magnum::Math::Range<3, T>> {};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Double>>;
#endif

/** @configurationvalue{Magnum::Math::Complex} */
template<class T> struct ConfigurationValue<Magnum::Math::Complex<T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Complex<T>& value, ConfigurationValueFlags flags) {
        return ConfigurationValue<Magnum::Math::Vector<2, T>>::toString(reinterpret_cast<const Magnum::Math::Vector<2, T>&>(value), flags);
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Complex<T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        const Magnum::Math::Vector<2, T> value = ConfigurationValue<Magnum::Math::Vector<2, T>>::fromString(stringValue, flags);
        return reinterpret_cast<const Magnum::Math::Complex<T>&>(value);
    }
};

/** @configurationvalue{Magnum::Math::DualComplex} */
template<class T> struct ConfigurationValue<Magnum::Math::DualComplex<T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::DualComplex<T>& value, ConfigurationValueFlags flags) {
        return ConfigurationValue<Magnum::Math::Vector<4, T>>::toString(reinterpret_cast<const Magnum::Math::Vector<4, T>&>(value), flags);
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::DualComplex<T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        const Magnum::Math::Vector<4, T> value = ConfigurationValue<Magnum::Math::Vector<4, T>>::fromString(stringValue, flags);
        return reinterpret_cast<const Magnum::Math::DualComplex<T>&>(value);
    }
};

/* No explicit instantiation for DualComplex needed, as Vector<4, T> is
   instantiated already */

/** @configurationvalue{Magnum::Math::Quaternion} */
template<class T> struct ConfigurationValue<Magnum::Math::Quaternion<T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Quaternion<T>& value, ConfigurationValueFlags flags) {
        return ConfigurationValue<Magnum::Math::Vector<4, T>>::toString(reinterpret_cast<const Magnum::Math::Vector<4, T>&>(value), flags);
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Quaternion<T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        const Magnum::Math::Vector<4, T> value = ConfigurationValue<Magnum::Math::Vector<4, T>>::fromString(stringValue, flags);
        return reinterpret_cast<const Magnum::Math::Quaternion<T>&>(value);
    }
};

/* No explicit instantiation needed for Quaternion, as Vector<4, T> is
   instantiated already */

/** @configurationvalue{Magnum::Math::DualQuaternion} */
template<class T> struct ConfigurationValue<Magnum::Math::DualQuaternion<T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::DualQuaternion<T>& value, ConfigurationValueFlags flags) {
        return ConfigurationValue<Magnum::Math::Vector<8, T>>::toString(reinterpret_cast<const Magnum::Math::Vector<8, T>&>(value), flags);
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::DualQuaternion<T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        const Magnum::Math::Vector<8, T> value = ConfigurationValue<Magnum::Math::Vector<8, T>>::fromString(stringValue, flags);
        return reinterpret_cast<const Magnum::Math::DualQuaternion<T>&>(value);
    }
};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::DualQuaternion<Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::DualQuaternion<Magnum::Double>>;
#endif

/** @configurationvalue{Magnum::Math::Bezier} */
template<Magnum::UnsignedInt order, Magnum::UnsignedInt dimensions, class T> struct ConfigurationValue<Magnum::Math::Bezier<order, dimensions, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Bezier<order, dimensions, T>& value, ConfigurationValueFlags flags) {
        std::string output;

        for(std::size_t o = 0; o != order + 1; ++o) {
            for(std::size_t i = 0; i != dimensions; ++i) {
                if(!output.empty()) output += ' ';
                output += ConfigurationValue<T>::toString(value[o][i], flags);
            }
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Bezier<order, dimensions, T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        Magnum::Math::Bezier<order, dimensions, T> result;

        std::size_t oldpos = 0, pos = std::string::npos, i = 0;
        do {
            pos = stringValue.find(' ', oldpos);
            std::string part = stringValue.substr(oldpos, pos-oldpos);

            if(!part.empty()) {
                result[i/dimensions][i%dimensions] = ConfigurationValue<T>::fromString(part, flags);
                ++i;
            }

            oldpos = pos+1;
        } while(pos != std::string::npos);

        return result;
    }
};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<2, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<2, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<3, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<3, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<2, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<2, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<3, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<3, 3, Magnum::Double>>;
#endif

}}

#endif
