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

#include "RectangularMatrix.h"

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<2, 2, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<3, 3, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<4, 4, Float>&);
#ifndef MAGNUM_TARGET_GLES
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<2, 2, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<3, 3, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<4, 4, Double>&);
#endif

template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<2, 3, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<3, 2, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<2, 4, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<4, 2, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<3, 4, Float>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<4, 3, Float>&);
#ifndef MAGNUM_TARGET_GLES
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<2, 3, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<3, 2, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<2, 4, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<4, 2, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<3, 4, Double>&);
template Corrade::Utility::Debug operator<<(Corrade::Utility::Debug, const Magnum::Math::RectangularMatrix<4, 3, Double>&);
#endif
#endif

}}

namespace Corrade { namespace Utility {

#ifndef DOXYGEN_GENERATING_OUTPUT
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Float>>;
#ifndef MAGNUM_TARGET_GLES
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Double>>;
#endif

template struct ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Float>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Float>>;
#ifndef MAGNUM_TARGET_GLES
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Double>>;
template struct ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Double>>;
#endif
#endif

}}
