#ifndef Magnum_MaterialTools_Filter_h
#define Magnum_MaterialTools_Filter_h
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
 * @brief Function @ref Magnum::MaterialTools::filterAttributes(), @ref Magnum::MaterialTools::filterLayers(), @ref Magnum::MaterialTools::filterAttributesLayers()
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/MaterialTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MaterialTools {

/**
@brief Filter material attributes
@m_since_latest

Returns a material with only the attributes for which the corresponding bit in
@p attributesToKeep was set. Attributes in additional layers are referenced by
bit ranges corresponding to @ref Trade::MaterialData::attributeDataOffset() for
a particular layer. The output layer ranges are then recalculated based on how
many attributes are left in those. Empty layers are kept,
@ref Trade::MaterialData::types() are ANDed with @p typesToKeep.

The size of @p attributesToKeep is expected to be equal to the number of
attributes in all layers (i.e., size of the
@ref Trade::MaterialData::attributeData() array).
@see @ref filterLayers(), @ref filterAttributesLayers()
*/
MAGNUM_MATERIALTOOLS_EXPORT Trade::MaterialData filterAttributes(const Trade::MaterialData& material, Containers::BitArrayView attributesToKeep, Trade::MaterialTypes typesToKeep = ~Trade::MaterialTypes{});

/**
@brief Filter material layers
@m_since_latest

Returns a material with only the layers for which the corresponding bit in
@p layersToKeep was set. The only exception is the base layer, which is left
empty if removed. Attributes in other layers are kept untouched,
@ref Trade::MaterialData::types() are ANDed with @p typesToKeep.

The size of @p layerCount is expected to be equal to
@ref Trade::MaterialData::layerCount().
@see @ref filterAttributes(), @ref filterAttributesLayers()
*/
MAGNUM_MATERIALTOOLS_EXPORT Trade::MaterialData filterLayers(const Trade::MaterialData& material, Containers::BitArrayView layersToKeep, Trade::MaterialTypes typesToKeep = ~Trade::MaterialTypes{});

/**
@brief Filter material attributes and layers
@m_since_latest

Performs what @ref filterAttributes() and @ref filterLayers() do, but in a
single step. Bits in @p attributesToKeep that correspond to layers that are
removed are ignored.
*/
MAGNUM_MATERIALTOOLS_EXPORT Trade::MaterialData filterAttributesLayers(const Trade::MaterialData& material, Containers::BitArrayView attributesToKeep, Containers::BitArrayView layersToKeep, Trade::MaterialTypes typesToKeep = ~Trade::MaterialTypes{});

}}

#endif
