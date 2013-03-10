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

#include "Vector.h"

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<2, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<3, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<4, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<2, Int>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<3, Int>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<4, Int>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<2, UnsignedInt>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<3, UnsignedInt>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<4, UnsignedInt>&);
#ifndef MAGNUM_TARGET_GLES
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<2, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<3, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::Vector<4, Double>&);
#endif
#endif

}}

namespace Corrade { namespace Utility {

#ifndef DOXYGEN_GENERATING_OUTPUT
template struct ConfigurationValue<Magnum::Math::Vector<2, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::Vector<3, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::Vector<4, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::Vector<2, Magnum::Int>>;
template struct ConfigurationValue<Magnum::Math::Vector<3, Magnum::Int>>;
template struct ConfigurationValue<Magnum::Math::Vector<4, Magnum::Int>>;
template struct ConfigurationValue<Magnum::Math::Vector<2, Magnum::UnsignedInt>>;
template struct ConfigurationValue<Magnum::Math::Vector<3, Magnum::UnsignedInt>>;
template struct ConfigurationValue<Magnum::Math::Vector<4, Magnum::UnsignedInt>>;
#ifndef MAGNUM_TARGET_GLES
template struct ConfigurationValue<Magnum::Math::Vector<2, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::Vector<3, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::Vector<4, Magnum::Double>>;
#endif
#endif

}}

