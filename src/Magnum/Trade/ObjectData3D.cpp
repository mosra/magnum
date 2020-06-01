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

#include "ObjectData3D.h"

#include <Corrade/Containers/EnumSet.hpp>

namespace Magnum { namespace Trade {

ObjectData3D::ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, const ObjectInstanceType3D instanceType, const UnsignedInt instance, const void* const importerState): _children{std::move(children)}, _transformation{transformation}, _instanceType{instanceType}, _flags{}, _instance{Int(instance)}, _importerState{importerState} {}

ObjectData3D::ObjectData3D(std::vector<UnsignedInt> children, const Vector3& translation, const Quaternion& rotation, const Vector3& scaling, const ObjectInstanceType3D instanceType, const UnsignedInt instance, const void* const importerState): _children{std::move(children)}, _transformation{translation, rotation, scaling}, _instanceType{instanceType}, _flags{ObjectFlag3D::HasTranslationRotationScaling}, _instance{Int(instance)}, _importerState{importerState} {}

ObjectData3D::ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, const void* const importerState): _children{std::move(children)}, _transformation{transformation}, _instanceType{ObjectInstanceType3D::Empty}, _flags{}, _instance{-1}, _importerState{importerState} {}

ObjectData3D::ObjectData3D(std::vector<UnsignedInt> children, const Vector3& translation, const Quaternion& rotation, const Vector3& scaling, const void* const importerState): _children{std::move(children)}, _transformation{translation, rotation, scaling}, _instanceType{ObjectInstanceType3D::Empty}, _flags{ObjectFlag3D::HasTranslationRotationScaling}, _instance{-1}, _importerState{importerState} {}

ObjectData3D::ObjectData3D(ObjectData3D&&)
    #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
    noexcept
    #endif
    = default;

ObjectData3D::~ObjectData3D() = default;

ObjectData3D& ObjectData3D::operator=(ObjectData3D&&)
    #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
    noexcept
    #endif
    = default;

Vector3 ObjectData3D::translation() const {
    CORRADE_ASSERT(_flags & ObjectFlag3D::HasTranslationRotationScaling,
        "Trade::ObjectData3D::translation(): object has only a combined transformation", {});
    return _transformation.trs.translation;
}

Quaternion ObjectData3D::rotation() const {
    CORRADE_ASSERT(_flags & ObjectFlag3D::HasTranslationRotationScaling,
        "Trade::ObjectData3D::rotation(): object has only a combined transformation", {});
    return _transformation.trs.rotation;
}

Vector3 ObjectData3D::scaling() const {
    CORRADE_ASSERT(_flags & ObjectFlag3D::HasTranslationRotationScaling,
        "Trade::ObjectData3D::scaling(): object has only a combined transformation", {});
    return _transformation.trs.scaling;
}

Matrix4 ObjectData3D::transformation() const {
    if(_flags & ObjectFlag3D::HasTranslationRotationScaling)
        /* Has to be on a single line otherwise lcov reports an uncovered
           line. Ugh. */
        return Matrix4::from(_transformation.trs.rotation.toMatrix(), _transformation.trs.translation)*
               Matrix4::scaling(_transformation.trs.scaling);
    return _transformation.matrix;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const ObjectInstanceType3D value) {
    debug << "Trade::ObjectInstanceType3D" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case ObjectInstanceType3D::value: return debug << "::" #value;
        _c(Camera)
        _c(Light)
        _c(Mesh)
        _c(Empty)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ObjectFlag3D value) {
    debug << "Trade::ObjectFlag3D" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case ObjectFlag3D::value: return debug << "::" #value;
        _c(HasTranslationRotationScaling)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ObjectFlags3D value) {
    return enumSetDebugOutput(debug, value, "Trade::ObjectFlags3D{}", {
        ObjectFlag3D::HasTranslationRotationScaling});
}
#endif

}}
