/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/Math/TypeTraits.h"
#include "Magnum/MeshTools/CompressIndices.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CompressIndicesTest: TestSuite::Tester {
    explicit CompressIndicesTest();

    template<class T> void compressUnsignedByte();
    template<class T> void compressUnsignedShort();
    template<class T> void compressUnsignedInt();
    void compressUnsignedByteInflateToShort();
    /* No compressErased(), as that's tested in the templates above */
    void compressErasedNonContiguous();
    void compressErasedWrongIndexSize();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void compressDeprecated();
    #endif

    void compressAsShort();
};

CompressIndicesTest::CompressIndicesTest() {
    addTests({&CompressIndicesTest::compressUnsignedByte<UnsignedByte>,
              &CompressIndicesTest::compressUnsignedByte<UnsignedShort>,
              &CompressIndicesTest::compressUnsignedByte<UnsignedInt>,
              &CompressIndicesTest::compressUnsignedShort<UnsignedShort>,
              &CompressIndicesTest::compressUnsignedShort<UnsignedInt>,
              &CompressIndicesTest::compressUnsignedInt<UnsignedInt>,
              &CompressIndicesTest::compressUnsignedByteInflateToShort,
              &CompressIndicesTest::compressErasedNonContiguous,
              &CompressIndicesTest::compressErasedWrongIndexSize,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &CompressIndicesTest::compressDeprecated,
              #endif

              &CompressIndicesTest::compressAsShort});
}

template<class T> void CompressIndicesTest::compressUnsignedByte() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{1, 2, 3, 0, 4};
    /* By default it has 16-byte type as minimum, override */
    std::pair<Containers::Array<char>, MeshIndexType> out =
        compressIndices(indices, MeshIndexType::UnsignedByte);

    CORRADE_COMPARE(out.second, MeshIndexType::UnsignedByte);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(out.first),
        Containers::arrayView<UnsignedByte>({1, 2, 3, 0, 4}),
        TestSuite::Compare::Container);

    /* Test the type-erased variant as well */
    out = compressIndices(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)), MeshIndexType::UnsignedByte);

    CORRADE_COMPARE(out.second, MeshIndexType::UnsignedByte);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(out.first),
        Containers::arrayView<UnsignedByte>({1, 2, 3, 0, 4}),
        TestSuite::Compare::Container);
}

template<class T> void CompressIndicesTest::compressUnsignedShort() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{1, 256, 0, 5};
    std::pair<Containers::Array<char>, MeshIndexType> out = compressIndices(indices);

    CORRADE_COMPARE(out.second, MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(out.first),
        Containers::arrayView<UnsignedShort>({1, 256, 0, 5}),
        TestSuite::Compare::Container);

    /* Test the type-erased variant as well */
    out = compressIndices(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)));

    CORRADE_COMPARE(out.second, MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(out.first),
        Containers::arrayView<UnsignedShort>({1, 256, 0, 5}),
        TestSuite::Compare::Container);
}

template<class T> void CompressIndicesTest::compressUnsignedInt() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{65536, 3, 2};
    std::pair<Containers::Array<char>, MeshIndexType> out = compressIndices(indices);

    CORRADE_COMPARE(out.second, MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(out.first),
        Containers::arrayView<UnsignedInt>({65536, 3, 2}),
        TestSuite::Compare::Container);

    /* Test the type-erased variant as well */
    out = compressIndices(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)));

    CORRADE_COMPARE(out.second, MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(out.first),
        Containers::arrayView<UnsignedInt>({65536, 3, 2}),
        TestSuite::Compare::Container);
}

void CompressIndicesTest::compressUnsignedByteInflateToShort() {
    const UnsignedByte indices[]{1, 2, 3, 0, 4};
    /* That's the default */
    std::pair<Containers::Array<char>, MeshIndexType> out = compressIndices(indices);

    CORRADE_COMPARE(out.second, MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(out.first),
        Containers::arrayView<UnsignedShort>({1, 2, 3, 0, 4}),
        TestSuite::Compare::Container);
}

void CompressIndicesTest::compressErasedNonContiguous() {
    const char indices[6*4]{};

    std::stringstream out;
    Error redirectError{&out};
    compressIndices(Containers::StridedArrayView2D<const char>{indices, {6, 2}, {4, 2}});
    CORRADE_COMPARE(out.str(),
        "MeshTools::compressIndices(): second view dimension is not contiguous\n");
}

void CompressIndicesTest::compressErasedWrongIndexSize() {
    const char indices[6*3]{};

    std::stringstream out;
    Error redirectError{&out};
    compressIndices(Containers::StridedArrayView2D<const char>{indices, {6, 3}}.every(2));
    CORRADE_COMPARE(out.str(),
        "MeshTools::compressIndices(): expected index type size 1, 2 or 4 but got 3\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void CompressIndicesTest::compressDeprecated() {
    Containers::Array<char> data;
    MeshIndexType type;
    UnsignedInt start, end;
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::tie(data, type, start, end) = MeshTools::compressIndices(
        std::vector<UnsignedInt>{1, 256, 0, 5});
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(start, 0);
    CORRADE_COMPARE(end, 256);
    CORRADE_COMPARE(type, MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(data),
        Containers::arrayView<UnsignedShort>({1, 256, 0, 5}),
        TestSuite::Compare::Container);
}
#endif

void CompressIndicesTest::compressAsShort() {
    CORRADE_COMPARE_AS(MeshTools::compressIndicesAs<UnsignedShort>({123, 456}),
        Containers::arrayView<UnsignedShort>({123, 456}),
        TestSuite::Compare::Container);

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::compressIndicesAs<UnsignedShort>({65536});
    CORRADE_COMPARE(out.str(), "MeshTools::compressIndicesAs(): type too small to represent value 65536\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CompressIndicesTest)
