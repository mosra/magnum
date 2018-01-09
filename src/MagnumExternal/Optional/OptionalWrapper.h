#ifndef MagnumExternal_Optional_OptionalWrapper_hpp
#define MagnumExternal_Optional_OptionalWrapper_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#if !defined(Corrade_Containers_Optional_h) || !defined(Corrade_Utility_Macros_h) || !defined(MAGNUM_BUILD_DEPRECATED)
#error This file is not meant to be used directly.
#endif

/* Cover your eyes. This will hurt. And wash your hands after. */
#ifdef MAGNUM_BUILD_DEPRECATED
#ifdef __has_include
#if __has_include(<optional>) && CORRADE_CXX_STANDARD >= 201703L
#define _MAGNUM_HAS_STD_OPTIONAL
#include <optional>

namespace Corrade { namespace Containers { namespace Implementation {
    template<class T> struct OptionalConverter<T, std::optional<T>> {
        CORRADE_DEPRECATED("use Corrade::Containers::Optional instead") static std::optional<T> to(const Optional<T>& other) {
            if(other) return std::optional<T>{*other};
            else return std::nullopt;
        }
        CORRADE_DEPRECATED("use Corrade::Containers::Optional instead") static std::optional<T> to(Optional<T>&& other) {
            if(other) return std::optional<T>{std::move(*other)};
            else return std::nullopt;
        }
    };
}}}
#endif
#endif

#ifdef _MAGNUM_HAS_STD_OPTIONAL
#else
namespace std {
    template<class T> using optional CORRADE_DEPRECATED_ALIAS("use Corrade::Containers::Optional instead") = Corrade::Containers::Optional<T>;

    constexpr CORRADE_DEPRECATED("use Corrade::Containers::NullOpt instead") Corrade::Containers::NullOptT nullopt{Corrade::Containers::NullOptT::Init{}};

    template<class T> CORRADE_DEPRECATED("use Corrade::Containers::optional() instead") inline Corrade::Containers::Optional<typename std::decay<T>::type> make_optional(T&& value) {
        return Corrade::Containers::Optional<typename std::decay<T>::type>{std::forward<T>(value)};
    }
}
#endif
#endif

#endif
