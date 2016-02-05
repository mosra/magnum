/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include "Attribute.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace Implementation {

UnsignedInt FloatAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
        case DataType::HalfFloat:
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
        case DataType::Float:
            return 4*components;
        #ifndef MAGNUM_TARGET_GLES
        case DataType::Double:
            return 8*components;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
}

#ifndef MAGNUM_TARGET_GLES2
UnsignedInt IntAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
            return 4*components;
    }

    CORRADE_ASSERT_UNREACHABLE();
}
#endif

#ifndef MAGNUM_TARGET_GLES
UnsignedInt DoubleAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::Double:
            return 8*components;
    }

    CORRADE_ASSERT_UNREACHABLE();
}
#endif

UnsignedInt Attribute<Math::Vector<3, Float>>::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
        case DataType::HalfFloat:
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
        case DataType::Float:
            return 4*components;
        #ifndef MAGNUM_TARGET_GLES
        case DataType::Double:
            return 8*components;
        case DataType::UnsignedInt10f11f11fRev:
            CORRADE_INTERNAL_ASSERT(components == 3);
            return 4;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
}

UnsignedInt Attribute<Math::Vector<4, Float>>::size(GLint components, DataType dataType) {
    #ifndef MAGNUM_TARGET_GLES
    if(components == GL_BGRA) components = 4;
    #endif

    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
        case DataType::HalfFloat:
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
        case DataType::Float:
            return 4*components;
        #ifndef MAGNUM_TARGET_GLES
        case DataType::Double:
            return 8*components;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case DataType::UnsignedInt2101010Rev:
        case DataType::Int2101010Rev:
            CORRADE_INTERNAL_ASSERT(components == 4);
            return 4;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
}

Debug& operator<<(Debug& debug, SizedAttribute<1, 1>::Components value) {
    switch(value) {
        case SizedAttribute<1, 1>::Components::One:
            return debug << "Attribute::Components::One";
    }

    return debug << "Attribute::Components::(invalid)";
}

Debug& operator<<(Debug& debug, SizedAttribute<1, 2>::Components value) {
    switch(value) {
        case SizedAttribute<1, 2>::Components::One:
            return debug << "Attribute::Components::One";
        case SizedAttribute<1, 2>::Components::Two:
            return debug << "Attribute::Components::Two";
    }

    return debug << "Attribute::Components::(invalid)";
}

Debug& operator<<(Debug& debug, SizedAttribute<1, 3>::Components value) {
    switch(value) {
        case SizedAttribute<1, 3>::Components::One:
            return debug << "Attribute::Components::One";
        case SizedAttribute<1, 3>::Components::Two:
            return debug << "Attribute::Components::Two";
        case SizedAttribute<1, 3>::Components::Three:
            return debug << "Attribute::Components::Three";
    }

    return debug << "Attribute::Components::(invalid)";
}

Debug& operator<<(Debug& debug, SizedAttribute<1, 4>::Components value) {
    switch(value) {
        case SizedAttribute<1, 4>::Components::One:
            return debug << "Attribute::Components::One";
        case SizedAttribute<1, 4>::Components::Two:
            return debug << "Attribute::Components::Two";
        case SizedAttribute<1, 4>::Components::Three:
            return debug << "Attribute::Components::Three";
        case SizedAttribute<1, 4>::Components::Four:
            return debug << "Attribute::Components::Four";
    }

    return debug << "Attribute::Components::(invalid)";
}

Debug& operator<<(Debug& debug, SizedMatrixAttribute<2>::Components value) {
    switch(value) {
        case SizedMatrixAttribute<2>::Components::Two:
            return debug << "Attribute::Components::Two";
    }

    return debug << "Attribute::Components::(invalid)";
}

Debug& operator<<(Debug& debug, SizedMatrixAttribute<3>::Components value) {
    switch(value) {
        case SizedMatrixAttribute<3>::Components::Three:
            return debug << "Attribute::Components::Three";
    }

    return debug << "Attribute::Components::(invalid)";
}

Debug& operator<<(Debug& debug, SizedMatrixAttribute<4>::Components value) {
    switch(value) {
        case SizedMatrixAttribute<4>::Components::Four:
            return debug << "Attribute::Components::Four";
    }

    return debug << "Attribute::Components::(invalid)";
}

Debug& operator<<(Debug& debug, Attribute<Math::Vector<4, Float>>::Components value) {
    switch(value) {
        case Attribute<Math::Vector<4, Float>>::Components::One:
            return debug << "Attribute::Components::One";
        case Attribute<Math::Vector<4, Float>>::Components::Two:
            return debug << "Attribute::Components::Two";
        case Attribute<Math::Vector<4, Float>>::Components::Three:
            return debug << "Attribute::Components::Three";
        case Attribute<Math::Vector<4, Float>>::Components::Four:
            return debug << "Attribute::Components::Four";
        #ifndef MAGNUM_TARGET_GLES
        case Attribute<Math::Vector<4, Float>>::Components::BGRA:
            return debug << "Attribute::Components::BGRA";
        #endif
    }

    return debug << "Attribute::Components::(invalid)";
}

Debug& operator<<(Debug& debug, FloatAttribute::DataType value) {
    switch(value) {
        #define _c(value) case FloatAttribute::DataType::value: return debug << "Attribute::DataType::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(HalfFloat)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #undef _c
    }

    return debug << "Attribute::DataType::(invalid)";
}

#ifndef MAGNUM_TARGET_GLES2
Debug& operator<<(Debug& debug, IntAttribute::DataType value) {
    switch(value) {
        #define _c(value) case IntAttribute::DataType::value: return debug << "Attribute::DataType::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        #undef _c
    }

    return debug << "Attribute::DataType::(invalid)";
}
#endif

#ifndef MAGNUM_TARGET_GLES
Debug& operator<<(Debug& debug, DoubleAttribute::DataType value) {
    switch(value) {
        #define _c(value) case DoubleAttribute::DataType::value: return debug << "Attribute::DataType::" #value;
        _c(Double)
        #undef _c
    }

    return debug << "Attribute::DataType::(invalid)";
}
#endif

Debug& operator<<(Debug& debug, Attribute<Math::Vector<3, Float>>::DataType value) {
    switch(value) {
        #define _c(value) case Attribute<Math::Vector<3, Float>>::DataType::value: return debug << "Attribute::DataType::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(HalfFloat)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        _c(UnsignedInt10f11f11fRev)
        #endif
        #undef _c
    }

    return debug << "Attribute::DataType::(invalid)";
}

Debug& operator<<(Debug& debug, Attribute<Math::Vector<4, Float>>::DataType value) {
    switch(value) {
        #define _c(value) case Attribute<Math::Vector<4, Float>>::DataType::value: return debug << "Attribute::DataType::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(HalfFloat)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UnsignedInt2101010Rev)
        _c(Int2101010Rev)
        #endif
        #undef _c
    }

    return debug << "Attribute::DataType::(invalid)";
}

}}
