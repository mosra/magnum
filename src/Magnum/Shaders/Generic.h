#ifndef Magnum_Shaders_Generic_h
#define Magnum_Shaders_Generic_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Typedef @ref Magnum::Shaders::Generic, alias @ref Magnum::Shaders::Generic2D, @ref Magnum::Shaders::Generic3D
 * @m_deprecated_since_latest Use @ref Magnum/Shaders/GenericGL.h, the
 *      @ref Magnum::Shaders::GenericGL "GenericGL" class and
 *      related typedefs instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/GenericGL.h"

CORRADE_DEPRECATED_FILE("use Magnum/Shaders/GenericGL.h, the GenericGL class and related typedefs instead")

/* Deprecated aliases not present here but in GenericGL.h instead, as a lot of
   existing code relies on these being transitively included from Phong.h etc.,
   and there are no forward declarations in Shaders.h as the type is never used
   like that. While we *could* include Generic.h from Phong.h, we'd have to
   also temporarily disable the CORRADE_DEPRECATED_FILE() macro there and it's
   more pain than it's worth. */
#else
#error use Magnum/Shaders/GenericGL.h, the GenericGL class and related typedefs instead
#endif

#endif
