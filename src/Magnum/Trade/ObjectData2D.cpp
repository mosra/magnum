/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "ObjectData2D.h"

namespace Magnum { namespace Trade {

ObjectData2D::ObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, const ObjectInstanceType2D instanceType, const UnsignedInt instance, const void* const importerState): _children{std::move(children)}, _transformation{transformation}, _instanceType{instanceType}, _instance{Int(instance)}, _importerState{importerState} {}

ObjectData2D::ObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, const void* const importerState): _children{std::move(children)}, _transformation{transformation}, _instanceType{ObjectInstanceType2D::Empty}, _instance{-1}, _importerState{importerState} {}

ObjectData2D::ObjectData2D(ObjectData2D&&) = default;

ObjectData2D::~ObjectData2D() = default;

ObjectData2D& ObjectData2D::operator=(ObjectData2D&&) = default;

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, ObjectInstanceType2D value) {
    switch(value) {
        #define _c(value) case ObjectInstanceType2D::value: return debug << "Trade::ObjectInstanceType2D::" #value;
        _c(Camera)
        _c(Mesh)
        _c(Empty)
        #undef _c
    }

    return debug << "Trade::ObjectInstanceType2D::(invalid)";
}
#endif

}}
