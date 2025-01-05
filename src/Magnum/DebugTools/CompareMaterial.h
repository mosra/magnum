#ifndef Magnum_DebugTools_CompareMaterial_h
#define Magnum_DebugTools_CompareMaterial_h
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
 * @brief Class @ref Magnum::DebugTools::CompareMaterial
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>
#include <Corrade/TestSuite/TestSuite.h>

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace DebugTools {

class CompareMaterial;

}}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* If Doxygen sees this, all @ref Corrade::TestSuite links break (prolly
   because the namespace is undocumented in this project) */
namespace Corrade { namespace TestSuite {

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareMaterial> {
    public:
        explicit Comparator();
        ~Comparator();

        ComparisonStatusFlags operator()(const Magnum::Trade::MaterialData& actual, const Magnum::Trade::MaterialData& expected);

        void printMessage(ComparisonStatusFlags flags, Utility::Debug& out, Containers::StringView actual, Containers::StringView expected) const;

    private:
        struct State;
        Containers::Pointer<State> _state;
};

}}
#endif

namespace Magnum { namespace DebugTools {

/**
@brief Material comparator for @ref Corrade::TestSuite
@m_since_latest

Compares @ref Trade::MaterialData instances, printing the differences in the
two if they have a different type, different layer count, different attributes,
and different type or different value of the same attribute. Pass the
comparator to @ref CORRADE_COMPARE_AS() along with an actual and expected
material:

@snippet debugtools-comparematerial.cpp usage

Based on actual materials used, in case of a comparison failure the comparator
can give for example the following output:

@m_class{m-console-wrap}

@include debugtools-comparematerial.ansi

All @ref Trade::MaterialAttributeType are supported.

@see @ref CompareImage
*/
class MAGNUM_DEBUGTOOLS_EXPORT CompareMaterial {
    public:
        explicit CompareMaterial();

        #ifndef DOXYGEN_GENERATING_OUTPUT
        TestSuite::Comparator<CompareMaterial>& comparator() {
            return _c;
        }
        #endif

    private:
        TestSuite::Comparator<CompareMaterial> _c;
};

}}

#endif
