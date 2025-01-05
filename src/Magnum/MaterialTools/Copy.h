#ifndef Magnum_MaterialTools_Copy_h
#define Magnum_MaterialTools_Copy_h
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
 * @brief Function @ref Magnum::MaterialTools::copy()
 * @m_since_latest
 */

#include "Magnum/MaterialTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MaterialTools {

/**
@brief Make an owned copy of the material
@m_since_latest

Allocates a copy of @ref Trade::MaterialData::attributeData() and
@relativeref{Trade::MaterialData,layerData()} and returns a new material with
those. All other properties such as material types or importer state are passed
through unchanged. The resulting @ref Trade::MaterialData::attributeDataFlags()
and @relativeref{Trade::MaterialData,layerDataFlags()} are always
@ref Trade::DataFlag::Owned and @ref Trade::DataFlag::Mutable.
@see @ref copy(Trade::MaterialData&&)
*/
MAGNUM_MATERIALTOOLS_EXPORT Trade::MaterialData copy(const Trade::MaterialData& material);

/**
@brief Make a material with owned data
@m_since_latest

If either @ref Trade::MaterialData::attributeDataFlags() or
@relativeref{Trade::MaterialData,layerDataFlags()} are not
@ref Trade::DataFlag::Owned and @ref Trade::DataFlag::Mutable, allocates a copy
of @ref Trade::MaterialData::attributeData() or
@relativeref{Trade::MaterialData,layerData()}, otherwise transfers their
ownership. The resulting data are always owned and mutable.
*/
MAGNUM_MATERIALTOOLS_EXPORT Trade::MaterialData copy(Trade::MaterialData&& material);

}}

#endif
