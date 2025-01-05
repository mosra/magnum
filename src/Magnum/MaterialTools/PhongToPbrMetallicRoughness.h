#ifndef Magnum_MaterialTools_PhongToPbrMetallicRoughness_h
#define Magnum_MaterialTools_PhongToPbrMetallicRoughness_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Enum @ref Magnum::MaterialTools::PhongToPbrMetallicRoughnessFlag, enum set @ref Magnum::MaterialTools::PhongToPbrMetallicRoughnessFlags, Function @ref Magnum::MaterialTools::phongToPbrMetallicRoughness()
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/MaterialTools/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace MaterialTools {

/**
@brief Phong to PBR metallic/roughness conversion flag
@m_since_latest

@see @ref PhongToPbrMetallicRoughnessFlags, @ref phongToPbrMetallicRoughness()
*/
enum class PhongToPbrMetallicRoughnessFlag {
    /**
     * Keep original attributes instead of removing all that were converted.
     */
    KeepOriginalAttributes = 1 << 0,

    /**
     * Drop attributes that can't be converted instead of keeping them in the
     * output. If
     * @relativeref{PhongToPbrMetallicRoughnessFlag,FailOnUnconvertibleAttributes}
     * is specified as well, it has a priority.
     */
    DropUnconvertibleAttributes = 1 << 1,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @copydoc PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes
     * @m_deprecated_since_latest Use @ref PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes
     *      instead.
     */
    DropUnconvertableAttributes CORRADE_DEPRECATED_ENUM("use DropUnconvertibleAttributes instead") = DropUnconvertibleAttributes,
    #endif

    /**
     * Fail if any attributes can't be converted instead of keeping them in the
     * output. Has a priority over
     * @relativeref{PhongToPbrMetallicRoughnessFlag,DropUnconvertibleAttributes}.
     */
    FailOnUnconvertibleAttributes = (1 << 2)|DropUnconvertibleAttributes,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @copydoc PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes
     * @m_deprecated_since_latest Use @ref PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes
     *      instead.
     */
    FailOnUnconvertableAttributes CORRADE_DEPRECATED_ENUM("use FailOnUnconvertibleAttributes instead") = FailOnUnconvertibleAttributes,
    #endif

    /** @todo flags to pick various shininess conversion alternatives */
};

/**
@brief Phong to PBR conversion flags
@m_since_latest

@see @ref phongToPbrMetallicRoughness()
*/
typedef Containers::EnumSet<PhongToPbrMetallicRoughnessFlag> PhongToPbrMetallicRoughnessFlags;

CORRADE_ENUMSET_OPERATORS(PhongToPbrMetallicRoughnessFlags)

/**
@brief Convert a Phong material to PBR metallic/roughness
@m_since_latest

Performs conversion of the following attributes. If the target attribute is
already present, it's passed through unchanged. The original attribute is
removed, unless @ref PhongToPbrMetallicRoughnessFlag::KeepOriginalAttributes is
set.

-   The value of @ref Trade::MaterialAttribute::DiffuseColor is used unchanged
    for @relativeref{Trade::MaterialAttribute,BaseColor}
-   The value of @ref Trade::MaterialAttribute::DiffuseTexture and related
    texture attributes is used unchanged for
    @relativeref{Trade::MaterialAttribute,BaseColorTexture} and corresponding
    related texture attributes

The following attributes currently aren't converted. If they are present in the
input material, a message is printed to @relativeref{Magnum,Warning}. The
attributes are passed through unchanged unless
@ref PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes is set; if
@relativeref{PhongToPbrMetallicRoughnessFlag,FailOnUnconvertibleAttributes} is
set instead, a message is printed to @relativeref{Magnum,Error} and the
function returns @relativeref{Corrade,Containers::NullOpt}.

-   @ref Trade::MaterialAttribute::AmbientColor,
    @relativeref{Trade::MaterialAttribute,AmbientTexture} and related texture
    attributes
-   @ref Trade::MaterialAttribute::SpecularColor,
    @relativeref{Trade::MaterialAttribute,SpecularTexture} and related texture
    attributes
-   @ref Trade::MaterialAttribute::Shininess

All other attributes (including ones common for Phong and PBR such as
@ref Trade::MaterialAttribute::NormalTexture) are passed through unchanged. The
resulting material has @ref Trade::MaterialType::PbrMetallicRoughness set and
@ref Trade::MaterialType::Phong removed.

@see @ref Trade::PbrMetallicRoughnessMaterialData
*/
MAGNUM_MATERIALTOOLS_EXPORT Containers::Optional<Trade::MaterialData> phongToPbrMetallicRoughness(const Trade::MaterialData& material, PhongToPbrMetallicRoughnessFlags flags = {});

}}

#endif
