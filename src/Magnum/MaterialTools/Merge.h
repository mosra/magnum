#ifndef Magnum_MaterialTools_Merge_h
#define Magnum_MaterialTools_Merge_h
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
 * @brief Function @ref Magnum::MaterialTools::merge(), enum @ref Magnum::MaterialTools::MergeConflicts
 * @m_since_latest
 */

#include "Magnum/MaterialTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MaterialTools {

/**
@brief Material merge conflict resolution
@m_since_latest

@see @ref merge()
*/
enum MergeConflicts: UnsignedInt {
    /**
     * Print a message to @relativeref{Magnum,Error} and return
     * @relativeref{Corrade,Containers::NullOpt} in case both materials contain
     * an attribute of the same name in the same layer index. Neither its type
     * nor its value is checked, so this fails also in case the values are the
     * same.
     */
    Fail,

    /**
     * Keep the value from the first material in case both materials contain an
     * attribute of the same name in the same layer index and both attributes
     * have the same type. Print a message to @relativeref{Magnum,Error} and
     * return @relativeref{Corrade,Containers::NullOpt} if they have a
     * different type, for example in case of custom attributes.
     *
     * If you want to keep the value from the second material instead, call
     * @ref merge() with this option and the materials swapped.
     */
    KeepFirstIfSameType,

    /**
     * Keep the value from the first material in case both materials contain an
     * attribute of the same name in the same layer index, regardless of their
     * type. With this option the operation always succeeds.
     *
     * If you want to keep the value from the second material instead, call
     * @ref merge() with this option and the materials swapped.
     */
    KeepFirstIgnoreType
};

/**
@brief Merge two materials
@m_since_latest

Takes attributes from @p second and inserts them to layers of the same index in
@p first. If @p second has more layers than @p first, the additional layers are
added at the end of @p first. @ref Trade::MaterialTypes from @p first and
@p second are merged together. If both materials contain an attribute of the
same name in the same layer index, conflict resolution is performed according
to the @p conflicts option.

As the input materials have the attributes sorted already, the operation is
done in an @f$ \mathcal{O}(m + n) @f$ execution time and memory complexity,
with @f$ m @f$ and @f$ n @f$ being count of all attributes and layers in
@p first and @p second, respectively.
*/
Containers::Optional<Trade::MaterialData> MAGNUM_MATERIALTOOLS_EXPORT merge(const Trade::MaterialData& first, const Trade::MaterialData& second, MergeConflicts conflicts = MergeConflicts::Fail);

}}

#endif
