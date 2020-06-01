/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "ObjectData2D.h"

#include <Corrade/Containers/EnumSet.hpp>

namespace Magnum { namespace Trade {

ObjectData2D::ObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, const ObjectInstanceType2D instanceType, const UnsignedInt instance, const void* const importerState): _children{std::move(children)}, _transformation{transformation}, _instanceType{instanceType}, _flags{}, _instance{Int(instance)}, _importerState{importerState} {}

ObjectData2D::ObjectData2D(std::vector<UnsignedInt> children, const Vector2& translation, const Complex& rotation, const Vector2& scaling, const ObjectInstanceType2D instanceType, const UnsignedInt instance, const void* const importerState): _children{std::move(children)}, _transformation{translation, rotation, scaling}, _instanceType{instanceType}, _flags{ObjectFlag2D::HasTranslationRotationScaling}, _instance{Int(instance)}, _importerState{importerState} {}

ObjectData2D::ObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, const void* const importerState): _children{std::move(children)}, _transformation{transformation}, _instanceType{ObjectInstanceType2D::Empty}, _flags{}, _instance{-1}, _importerState{importerState} {}

ObjectData2D::ObjectData2D(std::vector<UnsignedInt> children, const Vector2& translation, const Complex& rotation, const Vector2& scaling, const void* const importerState): _children{std::move(children)}, _transformation{translation, rotation, scaling}, _instanceType{ObjectInstanceType2D::Empty}, _flags{ObjectFlag2D::HasTranslationRotationScaling}, _instance{-1}, _importerState{importerState} {}

ObjectData2D::ObjectData2D(ObjectData2D&&)
    #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
    noexcept
    #endif
    = default;

ObjectData2D::~ObjectData2D() = default;

ObjectData2D& ObjectData2D::operator=(ObjectData2D&&)
    #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
    noexcept
    #endif
    = default;

Vector2 ObjectData2D::translation() const {
    CORRADE_ASSERT(_flags & ObjectFlag2D::HasTranslationRotationScaling,
        "Trade::ObjectData2D::translation(): object has only a combined transformation", {});
    return _transformation.trs.translation;
}

Complex ObjectData2D::rotation() const {
    CORRADE_ASSERT(_flags & ObjectFlag2D::HasTranslationRotationScaling,
        "Trade::ObjectData2D::rotation(): object has only a combined transformation", {});
    return _transformation.trs.rotation;
}

Vector2 ObjectData2D::scaling() const {
    CORRADE_ASSERT(_flags & ObjectFlag2D::HasTranslationRotationScaling,
        "Trade::ObjectData2D::scaling(): object has only a combined transformation", {});
    return _transformation.trs.scaling;
}

Matrix3 ObjectData2D::transformation() const {
    if(_flags & ObjectFlag2D::HasTranslationRotationScaling)
        /* Has to be on a single line otherwise lcov reports an uncovered
           line. Ugh. */
        return Matrix3::from(_transformation.trs.rotation.toMatrix(), _transformation.trs.translation)*
               Matrix3::scaling(_transformation.trs.scaling);
    return _transformation.matrix;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const ObjectInstanceType2D value) {
    debug << "Trade::ObjectInstanceType2D" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case ObjectInstanceType2D::value: return debug << "::" #value;
        _c(Camera)
        _c(Mesh)
        _c(Empty)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ObjectFlag2D value) {
    debug << "Trade::ObjectFlag2D" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case ObjectFlag2D::value: return debug << "::" #value;
        _c(HasTranslationRotationScaling)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ObjectFlags2D value) {
    return enumSetDebugOutput(debug, value, "Trade::ObjectFlags2D{}", {
        ObjectFlag2D::HasTranslationRotationScaling});
}
#endif

}}
