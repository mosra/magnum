#ifndef Magnum_SizeTraits_h
#define Magnum_SizeTraits_h
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
 * @brief Class Magnum::SizeTraits, Magnum::SizeBasedCall, Magnum::Pow, Magnum::Log
 */

#include <Utility/Debug.h>

#include "Math/Math.h"
#include "Magnum.h"

namespace Magnum {

/** @todo Remove/internalize things used only in one place (Math::log, Pow, Log)? Simplify SizeTraits? */

/**
@brief Traits class providing suitable types for given data sizes
@tparam byte    Highest byte needed (counting from zero)

If you use indexed data, you would probably (for performance reasons) want to
use the smallest type which is able to store all indices in given range. This
class provides type suitable for given **logarithmic** size of data. For
example, if you want to store 289 elements, they occupy two bytes, so
`SizeTraits<1>::%SizeType` is `GLushort`. For convenience you can use Log class
to compute logarithms at compile time, e.g.
`SizeTraits<Log<256, 289>::%value>::%SizeType`.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<std::size_t byte> struct SizeTraits {
    /**
     * @brief (Unsigned) type able to index the data
     *
     * Not implemented for large sizes (@f$ > 2^{32} @f$ elements), because
     * OpenGL doesn't have any type which would be able to store the indices.
     */
    typedef T SizeType;
};
#else
template<std::size_t byte> struct SizeTraits: public SizeTraits<byte - 1> {};
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct SizeTraits<0> {
    typedef GLubyte SizeType;
};
template<> struct SizeTraits<1> {
    typedef GLushort SizeType;
};
template<> struct SizeTraits<2> {
    typedef GLuint SizeType;
};
template<> struct SizeTraits<4> {
    /* We don't have size type to store 2^32 values */
};
#endif

/**
@brief Functor for calling templated function with type based on size
@tparam Base    Base struct with templated function `run()`. See below for
    example.

If you have templated function which you want to call with type suitable for
indexing data of some size, you will probably use cascade of IFs, like this:
@code
std::size_t dataSize;
template<class IndexType> Bar foo(Arg1 arg1, Arg2 arg2, ...);

Bar bar;
if(dataSize < 256)
    bar = foo<GLubyte>(arg1, arg2, ...);
else if(dataSize < 65536)
    bar = foo<GLushort>(arg1, arg2, ...);
// ...
@endcode
But this approach leads to repetitive and unmaintainable code, especially if
there are many arguments needed to pass to each function. The solution is to
use this class. The only thing you need is to rename your function to `run()`
and wrap it in a `struct`:
@code
struct Foo {
    template<class IndexType> Bar run(Arg1 arg1, Arg2 arg2, ...);
};
@endcode
Then you can use this class to call the templated function with the right type
based on data size:
@code
bar = SizeBasedCall<Foo>(dataSize)(arg1, arg2, ...);
@endcode
*/
#ifndef CORRADE_GCC44_COMPATIBILITY
template<class Base> struct SizeBasedCall: public Base {
#else
template<class Return, class Base> struct SizeBasedCall: public Base {
#endif
    /**
     * @brief Constructor
     * @param size          Data size
     */
    SizeBasedCall(std::size_t size): size(size) {}

    /**
     * @brief Functor
     * @param arguments     Arguments passed to `Base::run()`
     * @return Return value of `Base::run()`
     *
     * Calls `Base::run()` based on data size (given in constructor). If there
     * is no suitable type for indexing given data size, prints message to
     * error output and returns default-constructed value.
     */
    #ifndef CORRADE_GCC45_COMPATIBILITY
    template<typename ...Args> auto operator()(Args&&... arguments) -> decltype(Base::template run<GLubyte>(std::forward<Args>(arguments)...)) {
    #else
    #ifdef CORRADE_GCC44_COMPATIBILITY
    template<typename Args> Return operator()(Args&& arguments) {
    #else
    template<typename Args> auto operator()(Args&& arguments) -> decltype(Base::template run<GLubyte>(std::forward<Args>(arguments))) {
    #endif
    #endif
        switch(Math::log(256, size)) {
            case 0:
                #ifndef CORRADE_GCC45_COMPATIBILITY
                return Base::template run<GLubyte>(std::forward<Args>(arguments)...);
                #else
                return Base::template run<GLubyte>(std::forward<Args>(arguments));
                #endif
            case 1:
                #ifndef CORRADE_GCC45_COMPATIBILITY
                return Base::template run<GLushort>(std::forward<Args>(arguments)...);
                #else
                return Base::template run<GLushort>(std::forward<Args>(arguments));
                #endif
            case 2:
            case 3:
                #ifndef CORRADE_GCC45_COMPATIBILITY
                return Base::template run<GLuint>(std::forward<Args>(arguments)...);
                #else
                return Base::template run<GLuint>(std::forward<Args>(arguments));
                #endif
        }

        Error() << "SizeBasedCall: no type able to index" << size << "elements.";
        #ifndef CORRADE_GCC45_COMPATIBILITY
        return decltype(Base::template run<GLubyte>(std::forward<Args>(arguments)...))();
        #else
        #ifdef CORRADE_GCC44_COMPATIBILITY
        return Return();
        #else
        return decltype(Base::template run<GLubyte>(std::forward<Args>(arguments)))();
        #endif
        #endif
    }

    private:
        std::size_t size;
};

/**
@brief Class for computing integral powers at compile time
@tparam base        Base
@tparam exponent    Exponent

Useful mainly for computing template parameter value, e.g. in conjunction with
SizeTraits class.
*/
template<std::uint32_t base, std::uint32_t exponent> struct Pow {
    /** @brief Value of the power */
    enum: std::uint32_t {
        #ifndef DOXYGEN_GENERATING_OUTPUT
        value = base*Pow<base, exponent-1>::value
        #else
        value
        #endif
    };
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint32_t base> struct Pow<base, 0> {
    enum: std::uint32_t { value = 1 };
};
#endif

/**
@brief Class for computing integral logarithms at compile time
@tparam base        Base
@tparam number      Number

Useful mainly for computing template parameter value, e.g. in conjunction with
SizeTraits class.
*/
template<std::uint32_t base, std::uint32_t number> struct Log {
    /** @brief Value of the logarithm */
    enum: std::uint32_t {
        #ifndef DOXYGEN_GENERATING_OUTPUT
        value = 1+Log<base, number/base>::value
        #else
        value
        #endif
    };
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint32_t base> struct Log<base, 1>: public Log<base, 0> {};
template<std::uint32_t base> struct Log<base, 0> {
    enum: std::uint32_t { value = 0 };
};
#endif

}

#endif
