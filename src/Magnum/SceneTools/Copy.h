#ifndef Magnum_SceneTools_Copy_h
#define Magnum_SceneTools_Copy_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Function @ref Magnum::SceneTools::copy()
 * @m_since_latest
 */

#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace SceneTools {

/**
@brief Make an owned copy of the scene
@m_since_latest

Allocates a copy of @ref Trade::SceneData::data() and
@relativeref{Trade::SceneData,fieldData()} and returns a new scene with them.
All other properties such as the mapping bound or importer state are passed
through unchanged, the data layout isn't changed in any way. The resulting
@ref Trade::SceneData::dataFlags() are always @ref Trade::DataFlag::Owned and
@ref Trade::DataFlag::Mutable.
@see @ref copy(Trade::SceneData&&)
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData copy(const Trade::SceneData& material);

/**
@brief Make a scene with owned data
@m_since_latest

If @ref Trade::SceneData::dataFlags() are not @ref Trade::DataFlag::Owned and
@ref Trade::DataFlag::Mutable or the field data don't have the default deleter,
allocates a copy of @ref Trade::SceneData::data() or
@relativeref{Trade::SceneData,fieldData()}, otherwise transfers their
ownership. The resulting data are always owned and mutable, the data layout
isn't changed in any way.
*/
MAGNUM_SCENETOOLS_EXPORT Trade::SceneData copy(Trade::SceneData&& material);

}}

#endif
