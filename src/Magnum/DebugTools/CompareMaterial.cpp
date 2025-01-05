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

#include "CompareMaterial.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Math.h> /* max() that works with enums */
#include <Corrade/TestSuite/Comparator.h>

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector4.h"
/* Depending on MaterialTools isn't desirable at the moment, so it's just a
   header and an anonymous namespace to avoid clashes */
#include "Magnum/MaterialTools/Implementation/attributesEqual.h"
#include "Magnum/Trade/MaterialData.h"

namespace Corrade { namespace TestSuite {

using namespace Magnum;

namespace {

/* Higher values mean bigger change */
enum class MaterialState {
    Same,
    DifferentTypes,
    DifferentAttributeValues,
    DifferentAttributeTypes,
    DifferentAttributes,
    DifferentLayers
};

enum class AttributeState {
    Same,
    DifferentValue,
    DifferentType,
    OnlyInExpected,
    OnlyInActual
};

}

struct Comparator<DebugTools::CompareMaterial>::State {
    MaterialState materialState = MaterialState::Same;

    /* Second is attribute ID in the actual material (unless AttributeState is
       OnlyInExpected), third is attribute ID in the expected material (unless
       AttributeState is OnlyInActual). */
    Containers::Array<Containers::Triple<AttributeState, UnsignedInt, UnsignedInt>> attributes;
    /* Offsets into the attributes array for each layer. I.e., layer i is
       stored in `attributes[layerOffsets[i]]` to
       `attributes[layerOffsets[i + 1]]`. */
    Containers::Array<UnsignedInt> layerOffsets;

    const Trade::MaterialData* actual{};
    const Trade::MaterialData* expected{};
};

Comparator<DebugTools::CompareMaterial>::Comparator(): _state{InPlaceInit} {}

Comparator<DebugTools::CompareMaterial>::~Comparator() = default;

TestSuite::ComparisonStatusFlags Comparator<DebugTools::CompareMaterial>::operator()(const Trade::MaterialData& actual, const Trade::MaterialData& expected) {
    _state->actual = &actual;
    _state->expected = &expected;

    if(actual.types() != expected.types())
        _state->materialState = MaterialState::DifferentTypes;

    /* The layer offset array has one extra item for the last layer count */
    const std::size_t layerMax = Math::max(actual.layerCount(), expected.layerCount());
    _state->layerOffsets = Containers::Array<UnsignedInt>{NoInit, layerMax + 1};

    /* Go over all layers that are in both materials */
    std::size_t layer = 0;
    for(const std::size_t layerMin = Math::min(actual.layerCount(), expected.layerCount()); layer != layerMin; ++layer) {
        _state->layerOffsets[layer] = _state->attributes.size();
        UnsignedInt inActual = 0;
        UnsignedInt inExpected = 0;

        /* Take the earliest-sorted attribute from either material */
        while(inActual != actual.attributeCount(layer) && inExpected != expected.attributeCount(layer)) {
            if(actual.attributeName(layer, inActual) == expected.attributeName(layer, inExpected)) {
                AttributeState attributeState;
                if(actual.attributeType(layer, inActual) != expected.attributeType(layer, inExpected)) {
                    attributeState = AttributeState::DifferentType;
                    _state->materialState = Utility::max(_state->materialState, MaterialState::DifferentAttributeTypes);
                } else if(!MaterialTools::Implementation::attributesEqual(actual.attributeData(layer, inActual), expected.attributeData(layer, inExpected))) {
                    attributeState = AttributeState::DifferentValue;
                    _state->materialState = Utility::max(_state->materialState, MaterialState::DifferentAttributeValues);
                } else {
                    attributeState = AttributeState::Same;
                }

                arrayAppend(_state->attributes, InPlaceInit, attributeState, inActual, inExpected);
                ++inActual;
                ++inExpected;
            } else if(actual.attributeName(layer, inActual) < expected.attributeName(layer, inExpected)) {
                arrayAppend(_state->attributes, InPlaceInit, AttributeState::OnlyInActual, inActual, ~UnsignedInt{});
                _state->materialState = Utility::max(_state->materialState, MaterialState::DifferentAttributes);
                ++inActual;
            } else if(actual.attributeName(layer, inActual) > expected.attributeName(layer, inExpected)) {
                arrayAppend(_state->attributes, InPlaceInit, AttributeState::OnlyInExpected, ~UnsignedInt{}, inExpected);
                _state->materialState = Utility::max(_state->materialState, MaterialState::DifferentAttributes);
                ++inExpected;
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Consume remaining leftover attributes in either. Only one of these
           loops get entered. */
        while(inActual < actual.attributeCount(layer)) {
            arrayAppend(_state->attributes, InPlaceInit, AttributeState::OnlyInActual, inActual, ~UnsignedInt{});
            _state->materialState = Utility::max(_state->materialState, MaterialState::DifferentAttributes);
            ++inActual;
        }
        while(inExpected < expected.attributeCount(layer)) {
            arrayAppend(_state->attributes, InPlaceInit, AttributeState::OnlyInExpected, ~UnsignedInt{}, inExpected);
            _state->materialState = Utility::max(_state->materialState, MaterialState::DifferentAttributes);
            ++inExpected;
        }
    }

    /* Go over remaining actual/expected layers which weren't in
       expected/actual. Only one of these loops get entered. */
    for(; layer < actual.layerCount(); ++layer) {
        _state->layerOffsets[layer] = _state->attributes.size();

        for(UnsignedInt inActual = 0, inActualMax = actual.attributeCount(layer); inActual != inActualMax; ++inActual)
            arrayAppend(_state->attributes, InPlaceInit, AttributeState::OnlyInActual, inActual, ~UnsignedInt{});

        _state->materialState = Utility::max(_state->materialState, MaterialState::DifferentLayers);
    }
    for(; layer < expected.layerCount(); ++layer) {
        _state->layerOffsets[layer] = _state->attributes.size();

        for(UnsignedInt inExpected = 0, inExpectedMax = expected.attributeCount(layer); inExpected != inExpectedMax; ++inExpected)
            arrayAppend(_state->attributes, InPlaceInit, AttributeState::OnlyInExpected, ~UnsignedInt{}, inExpected);

        _state->materialState = Utility::max(_state->materialState, MaterialState::DifferentLayers);
    }

    CORRADE_INTERNAL_ASSERT(layer == _state->layerOffsets.size() - 1);
    _state->layerOffsets[layer] = _state->attributes.size();

    /** @todo If there's a large sequence of Same attributes, elide the middle
        (mark it with SameEllipsis, e.g., and leave at most 3 before and 3
        after each difference), and skip them when printing unless Verbose is
        set. Would become practically useful only once there's really a lot
        material attributes in a single layer, currently not really. */

    return _state->materialState == MaterialState::Same ? TestSuite::ComparisonStatusFlags{} : TestSuite::ComparisonStatusFlag::Failed;
}

namespace {

void printAttribute(Debug& out, const Trade::MaterialAttributeData& attribute, const AttributeState state, const bool isActual) {
    out << Debug::newline << "      ";
    if(state == AttributeState::Same)
        out << " ";
    else if(isActual)
        out << Debug::color(Debug::Color::Green) << "+";
    else
        out << Debug::color(Debug::Color::Red) << "-";
    if(state == AttributeState::DifferentType ||
       state == AttributeState::DifferentValue)
        out << Debug::resetColor;

    out << "" << attribute.name() << "@";

    if(state == AttributeState::DifferentType)
        out << Debug::color(isActual ? Debug::Color::Green : Debug::Color::Red);
    out << Debug::packed << attribute.type();
    if(state == AttributeState::DifferentType)
        out << Debug::resetColor;
    out << Debug::nospace << ":";

    if(state == AttributeState::DifferentType ||
       state == AttributeState::DifferentValue)
        out << Debug::color(isActual ? Debug::Color::Green : Debug::Color::Red);
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(attribute.type()) {
        #define _c(type) case Trade::MaterialAttributeType::type:           \
            out << Debug::packed << attribute.value<type>();                \
            break;
        #define _ct(name, type) case Trade::MaterialAttributeType::name:    \
            out << Debug::packed << attribute.value<type>();                \
            break;
        _ct(Bool, bool)
        /* LCOV_EXCL_START */
        _c(Float)
        _c(Deg)
        _c(Rad)
        _c(UnsignedInt)
        _c(Int)
        _c(UnsignedLong)
        _c(Long)
        _c(Vector2)
        _c(Vector2ui)
        _c(Vector2i)
        _c(Vector3)
        _c(Vector3ui)
        _c(Vector3i)
        _c(Vector4)
        _c(Vector4ui)
        _c(Vector4i)
        _c(Matrix2x2)
        _c(Matrix2x3)
        _c(Matrix2x4)
        _c(Matrix3x2)
        _c(Matrix3x3)
        _c(Matrix3x4)
        _c(Matrix4x2)
        _c(Matrix4x3)
        /* LCOV_EXCL_STOP */
        _ct(Pointer, const void*)
        _ct(MutablePointer, void*)
        _ct(String, Containers::StringView)
        _ct(TextureSwizzle, Trade::MaterialTextureSwizzle)
        #undef _c
        #undef _ct
        case Trade::MaterialAttributeType::Buffer:
            out << Containers::arrayCast<const char>(attribute.value<Containers::ArrayView<const void>>());
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    if(state != AttributeState::Same)
        out << Debug::resetColor;
}

}

void Comparator<DebugTools::CompareMaterial>::printMessage(const TestSuite::ComparisonStatusFlags, Debug& out, const Containers::StringView actual, const Containers::StringView expected) const {
    out << "Materials" << actual << "and" << expected;

    if(_state->materialState == MaterialState::DifferentLayers) {
        out << "have different layers.";
    } else if(_state->materialState == MaterialState::DifferentAttributes) {
        out << "have different attributes.";
    } else if(_state->materialState == MaterialState::DifferentAttributeTypes) {
        out << "have different attribute types.";
    } else if(_state->materialState == MaterialState::DifferentAttributeValues) {
        out << "have different attribute values.";
    } else if(_state->materialState == MaterialState::DifferentTypes) {
        out << "have different types.";
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    out << Debug::color(Debug::Color::Green) << "Actual (+)"
        << Debug::resetColor << "vs" << Debug::color(Debug::Color::Red)
        << "expected (-)" << Debug::resetColor << Debug::nospace << ":";

    /* Print the type, or both if they differ */
    /** @todo Or maybe if it's MaterialState::DifferentType print just this and
        skip the attributes? Because everything else that's printed is the
        same... */
    if(_state->expected->types()) {
        out << Debug::newline << "      ";
        if(_state->expected->types() != _state->actual->types())
            out << Debug::color(Debug::Color::Red) << "-" << Debug::nospace;
        else
            out << "";
        if(_state->actual->types() && _state->expected->types() != _state->actual->types())
            out << Debug::resetColor;
        out << "Types:";
        if(_state->actual->types() && _state->expected->types() != _state->actual->types())
            out << Debug::color(Debug::Color::Red);
        out << Debug::packed << _state->expected->types() << Debug::resetColor;
    }
    if(_state->actual->types() && _state->actual->types() != _state->expected->types()) {
        out << Debug::newline << "      " << Debug::color(Debug::Color::Green) << "+" << Debug::nospace;
        if(_state->expected->types()) out << Debug::resetColor;
        out << "Types:";
        if(_state->expected->types()) out << Debug::color(Debug::Color::Green);
        out << Debug::packed << _state->actual->types() << Debug::resetColor;
    }

    /* Print content of both materials, interleaved, with layers and attributes
       that differ marked with +/- */
    for(UnsignedInt layer = 0, layerMax = _state->layerOffsets.size() - 1; layer != layerMax; ++layer) {
        /* Show layer header only if there's more than one and the base
           layer isn't empty */
        if(_state->layerOffsets.size() != 2 || _state->layerOffsets[1] != 0) {
            out << Debug::newline << "      ";
            if(layer >= _state->actual->layerCount())
                out << Debug::color(Debug::Color::Red) << "-" << Debug::nospace;
            else if(layer >= _state->expected->layerCount())
                out << Debug::color(Debug::Color::Green) << "+" << Debug::nospace;
            else
                out << "";
            if(layer == 0)
                out << "Base layer:";
            else
                out << "Layer" << layer << Debug::nospace << ":";
            if(layer >= _state->actual->layerCount() || layer >= _state->expected->layerCount())
                out << Debug::resetColor;
        }

        /* Print attribute values indented only if there's more than one
           layer */
        for(UnsignedInt id = _state->layerOffsets[layer], idMax = _state->layerOffsets[layer + 1]; id != idMax; ++id) {
            const Containers::Triple<AttributeState, UnsignedInt, UnsignedInt> attribute = _state->attributes[id];
            const AttributeState state =attribute.first();
            if(state != AttributeState::OnlyInActual)
                printAttribute(out, _state->expected->attributeData(layer, attribute.third()), state, false);
            if(state != AttributeState::Same && state != AttributeState::OnlyInExpected) {
                printAttribute(out, _state->actual->attributeData(layer, attribute.second()), state, true);
            }
        }
    }
}

}}
