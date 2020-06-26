#ifndef Magnum_Magnum_h
#define Magnum_Magnum_h
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

/** @file
 * @brief Forward declarations and basic types for the @ref Magnum namespace
 */

#include <Corrade/Utility/Utility.h>

#include "Magnum/Types.h"
#include "Magnum/Math/Math.h"

namespace Magnum {

/* Bring whole Corrade namespace */
using namespace Corrade;

/* Bring debugging facility from Corrade::Utility namespace */
using Corrade::Utility::Debug;
using Corrade::Utility::Warning;
using Corrade::Utility::Error;
using Corrade::Utility::Fatal;

#ifdef DOXYGEN_GENERATING_OUTPUT
/**
@brief Build with deprecated API included

Defined if the library contains deprecated API (which will be removed in the
future). To preserve backward compatibility, Magnum is by default built with
deprecated API included.
@see @ref building, @ref cmake
*/
#define MAGNUM_BUILD_DEPRECATED
/* (enabled by default) */

/**
@brief Static library build

Defined if built as static libraries. Default are shared libraries.
@see @ref building, @ref cmake
*/
#define MAGNUM_BUILD_STATIC
#undef MAGNUM_BUILD_STATIC

/**
@brief Static library build with globals unique across shared libraries
@m_since{2020,06}

Enabled by default in a static build.
@see @ref building, @ref cmake
*/
#define MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS
#undef MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief Multi-threaded build
 * @m_deprecated_since{2019,10} Use @ref CORRADE_BUILD_MULTITHREADED instead.
 */
#define MAGNUM_BUILD_MULTITHREADED
#undef MAGNUM_BUILD_MULTITHREADED
#endif

/**
@brief OpenGL interoperability

Defined if the engine is built with OpenGL interoperability enabled --- extra
APIs in various libraries interacting with the @ref Magnum::GL "GL" library.
Enabled by default in case the @ref Magnum::GL "GL" library is built.
@see @ref MAGNUM_TARGET_GLES2, @ref MAGNUM_TARGET_GLES3,
    @ref MAGNUM_TARGET_DESKTOP_GLES, @ref building, @ref cmake
*/
#define MAGNUM_TARGET_GL
/* (enabled by default) */

/**
@brief OpenGL ES target

Defined if the engine is built for OpenGL ES 3.0 or OpenGL ES 2.0.
@see @ref MAGNUM_TARGET_GLES2, @ref MAGNUM_TARGET_GLES3,
    @ref MAGNUM_TARGET_DESKTOP_GLES, @ref building, @ref cmake
*/
#define MAGNUM_TARGET_GLES
#undef MAGNUM_TARGET_GLES

/**
@brief OpenGL ES 2.0 target

Defined if the engine is built for OpenGL ES 2.0. Implies also
@ref MAGNUM_TARGET_GLES.
@see @ref MAGNUM_TARGET_GLES3, @ref MAGNUM_TARGET_DESKTOP_GLES, @ref building,
    @ref cmake
*/
#define MAGNUM_TARGET_GLES2
#undef MAGNUM_TARGET_GLES2

/**
@brief OpenGL ES 3.0 target

Defined if the engine is built for OpenGL ES 3.0. Implies also
@ref MAGNUM_TARGET_GLES.
@see @ref MAGNUM_TARGET_GLES2, @ref MAGNUM_TARGET_DESKTOP_GLES, @ref building,
    @ref cmake
*/
#define MAGNUM_TARGET_GLES3
#undef MAGNUM_TARGET_GLES3

/**
@brief Desktop emulation of OpenGL ES target

Defined if the engine is built for OpenGL ES 3.0 or OpenGL ES 2.0 emulated
within standard desktop OpenGL. Implies also @ref MAGNUM_TARGET_GLES.
@see @ref MAGNUM_TARGET_GLES2, @ref MAGNUM_TARGET_GLES3, @ref building,
    @ref cmake
*/
#define MAGNUM_TARGET_DESKTOP_GLES
#undef MAGNUM_TARGET_DESKTOP_GLES

/**
@brief WebGL target

Defined if the engine is built for WebGL (using Emscripten). WebGL is nearly
equivalent to OpenGL ES 2.0, thus in most cases you don't need to treat it
differently, but there are some
[specific restrictions and features](http://www.khronos.org/registry/webgl/specs/latest/1.0/#6)
which you might want to be aware of. Implies also @ref MAGNUM_TARGET_GLES and
@ref MAGNUM_TARGET_GLES2.
@see @ref CORRADE_TARGET_EMSCRIPTEN, @ref building, @ref cmake
*/
#define MAGNUM_TARGET_WEBGL
#undef MAGNUM_TARGET_WEBGL

/**
@brief Headless target

Defined if the engine is built for use on a headless machine (without any
graphical desktop environment). Basically it means that EGL with no display
attachment is being used everywhere instead of platform-specific toolkits like
CGL, GLX or WGL. Note that this might not be supported on all platforms, see
@ref Magnum::Platform::WindowlessEglApplication "Platform::WindowlessEglApplication"
for more information.
*/
#define MAGNUM_TARGET_HEADLESS
#undef MAGNUM_TARGET_HEADLESS

/**
@brief Vulkan interoperability

Defined if the engine is built with Vulkan interoperability enabled --- extra
APIs in various libraries interacting with the @ref Magnum::Vk "Vk" library.
Enabled by default in case the @ref Magnum::Vk "Vk" library is built.
@see @ref building, @ref cmake
*/
#define MAGNUM_TARGET_VK
#undef MAGNUM_TARGET_VK
#endif

/** @{ @name Basic type definitions

See @ref types for more information.
*/

/** @brief Unsigned byte (8bit) */
typedef std::uint8_t UnsignedByte;

/** @brief Signed byte (8bit) */
typedef std::int8_t Byte;

/** @brief Unsigned short (16bit) */
typedef std::uint16_t UnsignedShort;

/** @brief Signed short (16bit) */
typedef std::int16_t Short;

/**
@brief Unsigned int (32bit)

Equivalent to GLSL @glsl uint @ce.
@m_keyword{uint,GLSL uint,}
*/
typedef std::uint32_t UnsignedInt;

/**
@brief Signed int (32bit)

Equivalent to GLSL @glsl int @ce.
@m_keyword{int,GLSL int,}
*/
typedef std::int32_t Int;

/** @brief Unsigned long (64bit) */
typedef std::uint64_t UnsignedLong;

/** @brief Signed long (64bit) */
typedef std::int64_t Long;

/**
@brief Float (32bit)

Equivalent to GLSL @glsl float @ce.
@m_keyword{float,GLSL float,}
*/
typedef float Float;

/** @brief Half (16bit) */
typedef Math::Half Half;

/**
@brief Two-component bool vector
@m_since{2019,10}

Equivalent to GLSL @glsl bvec2 @ce.
@m_keyword{bvec2,GLSL bvec2,}
*/
typedef Math::BoolVector<2> BoolVector2;

/**
@brief Three-component bool vector
@m_since{2019,10}

Equivalent to GLSL @glsl bvec3 @ce.
@m_keyword{bvec3,GLSL bvec3,}
*/
typedef Math::BoolVector<3> BoolVector3;

/**
@brief Four-component bool vector
@m_since{2019,10}

Equivalent to GLSL @glsl bvec4 @ce.
@m_keyword{bvec4,GLSL bvec4,}
*/
typedef Math::BoolVector<4> BoolVector4;

/**
@brief Two-component float vector

Equivalent to GLSL @glsl vec2 @ce.
@m_keyword{vec2,GLSL vec2,}
*/
typedef Math::Vector2<Float> Vector2;

/**
@brief Three-component float vector

Equivalent to GLSL @glsl vec3 @ce.
@m_keyword{vec3,GLSL vec3,}
*/
typedef Math::Vector3<Float> Vector3;

/**
@brief Four-component float vector

Equivalent to GLSL @glsl vec4 @ce.
@m_keyword{vec4,GLSL vec4,}
*/
typedef Math::Vector4<Float> Vector4;

/**
@brief Two-component unsigned byte vector
@m_since{2020,06}
*/
typedef Math::Vector2<UnsignedByte> Vector2ub;

/**
@brief Three-component unsigned byte vector
@m_since{2020,06}
*/
typedef Math::Vector3<UnsignedByte> Vector3ub;

/**
@brief Four-component unsigned byte vector
@m_since{2020,06}
*/
typedef Math::Vector4<UnsignedByte> Vector4ub;

/**
@brief Two-component signed byte vector
@m_since{2020,06}
*/
typedef Math::Vector2<Byte> Vector2b;

/**
@brief Three-component signed byte vector
@m_since{2020,06}
*/
typedef Math::Vector3<Byte> Vector3b;

/**
@brief Four-component signed byte vector
@m_since{2020,06}
*/
typedef Math::Vector4<Byte> Vector4b;

/**
@brief Two-component unsigned short vector
@m_since{2020,06}
*/
typedef Math::Vector2<UnsignedShort> Vector2us;

/**
@brief Three-component unsigned short vector
@m_since{2020,06}
*/
typedef Math::Vector3<UnsignedShort> Vector3us;

/**
@brief Four-component unsigned short vector
@m_since{2020,06}
*/
typedef Math::Vector4<UnsignedShort> Vector4us;

/**
@brief Two-component signed short vector
@m_since{2020,06}
*/
typedef Math::Vector2<Short> Vector2s;

/**
@brief Three-component signed short vector
@m_since{2020,06}
*/
typedef Math::Vector3<Short> Vector3s;

/**
@brief Four-component signed short vector
@m_since{2020,06}
*/
typedef Math::Vector4<Short> Vector4s;

/**
@brief Two-component unsigned integer vector

Equivalent to GLSL @glsl uvec2 @ce.
@m_keyword{uvec2,GLSL uvec2,}
*/
typedef Math::Vector2<UnsignedInt> Vector2ui;

/**
@brief Three-component unsigned integer vector

Equivalent to GLSL @glsl uvec3 @ce.
@m_keyword{uvec3,GLSL uvec3,}
*/
typedef Math::Vector3<UnsignedInt> Vector3ui;

/**
@brief Four-component unsigned integer vector

Equivalent to GLSL @glsl uvec4 @ce.
@m_keyword{uvec4,GLSL uvec4,}
*/
typedef Math::Vector4<UnsignedInt> Vector4ui;

/**
@brief Two-component signed integer vector

Equivalent to GLSL @glsl ivec2 @ce.
@m_keyword{ivec2,GLSL ivec2,}
*/
typedef Math::Vector2<Int> Vector2i;

/**
@brief Three-component signed integer vector

Equivalent to GLSL @glsl ivec3 @ce.
@m_keyword{ivec3,GLSL ivec3,}
*/
typedef Math::Vector3<Int> Vector3i;

/**
@brief Four-component signed integer vector

Equivalent to GLSL @glsl ivec4 @ce.
@m_keyword{ivec4,GLSL ivec4,}
*/
typedef Math::Vector4<Int> Vector4i;

/** @brief Three-component (RGB) float color */
typedef Math::Color3<Float> Color3;

/** @brief Four-component (RGBA) float color */
typedef Math::Color4<Float> Color4;

/** @brief HSV float color */
typedef Math::ColorHsv<Float> ColorHsv;

/**
@brief Three-component (RGB) unsigned byte color

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. Use @ref Color3::fromSrgb() and @ref Color3::toSrgb()
    for proper sRGB handling.
*/
typedef Math::Color3<UnsignedByte> Color3ub;

/**
@brief Three-component (RGB) unsigned short color
@m_since{2020,06}
*/
typedef Math::Color3<UnsignedShort> Color3us;

/**
@brief Four-component (RGBA) unsigned byte color

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. Use @ref Color4::fromSrgbAlpha() and
    @ref Color4::toSrgbAlpha() for proper sRGB handling.
*/
typedef Math::Color4<UnsignedByte> Color4ub;

/**
@brief Four-component (RGB) unsigned short color
@m_since{2020,06}
*/
typedef Math::Color4<UnsignedShort> Color4us;

/**
@brief 3x3 float transformation matrix

Equivalent to GLSL @glsl mat3 @ce.
@see @ref Matrix3x3
@m_keyword{mat3,GLSL mat3,}
*/
typedef Math::Matrix3<Float> Matrix3;

/**
@brief 4x4 float transformation matrix

Equivalent to GLSL @glsl mat4 @ce.
@see @ref Matrix4x4
@m_keyword{mat4,GLSL mat4,}
*/
typedef Math::Matrix4<Float> Matrix4;

/**
@brief 2x2 float matrix

Equivalent to GLSL @glsl mat2x2 @ce.
@m_keyword{mat2x2,GLSL mat2x2,}
*/
typedef Math::Matrix2x2<Float> Matrix2x2;

/**
@brief 3x3 float matrix

Equivalent to GLSL @glsl mat3x3 @ce. Note that this is different from
@ref Matrix3, which contains additional functions for transformations in 2D.
@m_keyword{mat3x3,GLSL mat3x3,}
*/
typedef Math::Matrix3x3<Float> Matrix3x3;

/**
@brief 4x4 float matrix

Equivalent to GLSL @glsl mat4x4 @ce. Note that this is different from
@ref Matrix4, which contains additional functions for transformations in 3D.
@m_keyword{mat4x4,GLSL mat4x4,}
*/
typedef Math::Matrix4x4<Float> Matrix4x4;

/**
@brief Float matrix with 2 columns and 3 rows

Equivalent to GLSL @glsl mat2x3 @ce.
@m_keyword{mat2x3,GLSL mat2x3,}
*/
typedef Math::Matrix2x3<Float> Matrix2x3;

/**
@brief Float matrix with 3 columns and 2 rows

Equivalent to GLSL @glsl mat3x2 @ce.
@m_keyword{mat3x2,GLSL mat3x2,}
*/
typedef Math::Matrix3x2<Float> Matrix3x2;

/**
@brief Float matrix with 2 columns and 4 rows

Equivalent to GLSL @glsl mat2x4 @ce.
@m_keyword{mat2x4,GLSL mat2x4,}
*/
typedef Math::Matrix2x4<Float> Matrix2x4;

/**
@brief Float matrix with 4 columns and 2 rows

Equivalent to GLSL @glsl mat2x4 @ce.
@m_keyword{mat2x4,GLSL mat2x4,}
*/
typedef Math::Matrix4x2<Float> Matrix4x2;

/**
@brief Float matrix with 3 columns and 4 rows

Equivalent to GLSL @glsl mat3x4 @ce.
@m_keyword{mat3x4,GLSL mat3x4,}
*/
typedef Math::Matrix3x4<Float> Matrix3x4;

/**
@brief Float matrix with 4 columns and 3 rows

Equivalent to GLSL @glsl mat4x3 @ce.
@m_keyword{mat4x3,GLSL mat4x3,}
*/
typedef Math::Matrix4x3<Float> Matrix4x3;

/**
@brief Signed byte matrix with 2 columns and 2 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix2x4b and ignore the bottom two rows.
*/
typedef Math::Matrix2x2<Byte> Matrix2x2b;

/**
@brief Signed byte matrix with 2 columns and 3 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix2x4b and ignore the bottom row.
*/
typedef Math::Matrix2x3<Byte> Matrix2x3b;

/**
@brief Signed byte matrix with 2 columns and 4 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix2x4<Byte> Matrix2x4b;

/**
@brief Signed byte matrix with 3 columns and 2 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix3x4b and ignore the bottom two rows.
*/
typedef Math::Matrix3x2<Byte> Matrix3x2b;

/**
@brief Signed byte matrix with 3 columns and 3 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix3x4b and ignore the bottom row.
*/
typedef Math::Matrix3x3<Byte> Matrix3x3b;

/**
@brief Signed byte matrix with 3 columns and 4 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix3x4<Byte> Matrix3x4b;

/**
@brief Signed byte matrix with 4 columns and 2 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix4x4b and ignore the bottom two rows.
*/
typedef Math::Matrix4x2<Byte> Matrix4x2b;

/**
@brief Signed byte matrix with 4 columns and 3 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix4x4b and ignore the bottom row.
*/
typedef Math::Matrix4x3<Byte> Matrix4x3b;

/**
@brief Signed byte matrix with 4 columns and 4 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 8-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix4x4<Byte> Matrix4x4b;

/**
@brief Signed short matrix with 2 columns and 2 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix2x2<Short> Matrix2x2s;

/**
@brief Signed short matrix with 2 columns and 3 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix2x4s and ignore the bottom row.
*/
typedef Math::Matrix2x3<Short> Matrix2x3s;

/**
@brief Signed short matrix with 2 columns and 4 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix2x4<Short> Matrix2x4s;

/**
@brief Signed short matrix with 3 columns and 2 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix3x2<Short> Matrix3x2s;

/**
@brief Signed short matrix with 3 columns and 3 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix3x4s and ignore the bottom row.
*/
typedef Math::Matrix3x3<Short> Matrix3x3s;

/**
@brief Signed short matrix with 3 columns and 4 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix3x4<Short> Matrix3x4s;

/**
@brief Signed short matrix with 4 columns and 2 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix4x2<Short> Matrix4x2s;

/**
@brief Signed short matrix with 4 columns and 3 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix4x4s and ignore the bottom row.
*/
typedef Math::Matrix4x3<Short> Matrix4x3s;

/**
@brief Signed short matrix with 4 columns and 4 rows
@m_since{2020,06}

Storage only, in ordet to support matrices packed into 16-bit types. For
performing arithmetic on this type use @ref Math::unpack() / @ref Math::unpackInto() to convert to a float type first and then @ref Math::pack()
/ @ref Math::packInto() back again.
*/
typedef Math::Matrix4x4<Short> Matrix4x4s;

/** @brief Float two-dimensional quadratic Bézier curve */
typedef Math::QuadraticBezier2D<Float> QuadraticBezier2D;

/** @brief Float three-dimensional quadratic Bézier curve */
typedef Math::QuadraticBezier3D<Float> QuadraticBezier3D;

/** @brief Float two-dimensional cubic Bézier curve */
typedef Math::CubicBezier2D<Float> CubicBezier2D;

/** @brief Float three-dimensional cubic Bézier curve */
typedef Math::CubicBezier3D<Float> CubicBezier3D;

/** @brief Float scalar cubic Hermite spline point */
typedef Math::CubicHermite1D<Float> CubicHermite1D;

/** @brief Float two-dimensional cubic Hermite spline point */
typedef Math::CubicHermite2D<Float> CubicHermite2D;

/** @brief Float three-dimensional cubic Hermite spline point */
typedef Math::CubicHermite3D<Float> CubicHermite3D;

/** @brief Float cubic Hermite spline complex number */
typedef Math::CubicHermiteComplex<Float> CubicHermiteComplex;

/** @brief Float cubic Hermite spline quaternion */
typedef Math::CubicHermiteQuaternion<Float> CubicHermiteQuaternion;

/** @brief Float complex number */
typedef Math::Complex<Float> Complex;

/** @brief Float dual complex number */
typedef Math::DualComplex<Float> DualComplex;

/** @brief Float quaternion */
typedef Math::Quaternion<Float> Quaternion;

/** @brief Float dual quaternion */
typedef Math::DualQuaternion<Float> DualQuaternion;

/** @brief Float constants */
typedef Math::Constants<Float> Constants;

/** @brief Angle in float degrees */
typedef Math::Deg<Float> Deg;

/** @brief Angle in float radians */
typedef Math::Rad<Float> Rad;

/** @brief One-dimensional float range */
typedef Math::Range1D<Float> Range1D;

/** @brief Two-dimensional float range */
typedef Math::Range2D<Float> Range2D;

/** @brief Three-dimensional float range */
typedef Math::Range3D<Float> Range3D;

/** @brief One-dimensional signed integer range */
typedef Math::Range1D<Int> Range1Di;

/** @brief Two-dimensional signed integer range */
typedef Math::Range2D<Int> Range2Di;

/** @brief Three-dimensional signed integer range */
typedef Math::Range3D<Int> Range3Di;

/** @brief Float frustum */
typedef Math::Frustum<Float> Frustum;

/* Since 1.8.17, the original short-hand group closing doesn't work anymore.
   FFS. */
/**
 * @}
 */

/** @{ @name Half-precision types

These types are for storage and conversion from / to single-precision types,
no arithmetic operations are implemented. See @ref types for more information,
for performing arithmetic on these types use @ref Math::unpackHalf() / @ref Math::unpackHalfInto() to convert to a 32-bit float type first and then
@ref Math::packHalf() / @ref Math::packHalfInto() back again.
*/

/**
@brief Two-component half-float vector
@m_since{2020,06}
*/
typedef Math::Vector2<Half> Vector2h;

/**
@brief Three-component half-float vector
@m_since{2020,06}
*/
typedef Math::Vector3<Half> Vector3h;

/**
@brief Four-component half-float vector
@m_since{2020,06}
*/
typedef Math::Vector4<Half> Vector4h;

/**
@brief Three-component (RGB) half-float color
@m_since{2020,06}
*/
typedef Math::Color3<Half> Color3h;

/**
@brief Four-component (RGBA) half-float color
@m_since{2020,06}
*/
typedef Math::Color4<Half> Color4h;

/**
@brief Half-float matrix with 2 columns and 2 rows
@m_since{2020,06}
*/
typedef Math::Matrix2x2<Half> Matrix2x2h;

/**
@brief Half-float matrix with 2 columns and 3 rows
@m_since{2020,06}

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix2x4h and ignore the bottom row.
*/
typedef Math::Matrix2x3<Half> Matrix2x3h;

/**
@brief Half-float matrix with 2 columns and 4 rows
@m_since{2020,06}
*/
typedef Math::Matrix2x4<Half> Matrix2x4h;

/**
@brief Half-float matrix with 3 columns and 2 rows
@m_since{2020,06}
*/
typedef Math::Matrix3x2<Half> Matrix3x2h;

/**
@brief Half-float matrix with 3 columns and 3 rows
@m_since{2020,06}

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix3x4h and ignore the bottom row.
*/
typedef Math::Matrix3x3<Half> Matrix3x3h;

/**
@brief Half-float matrix with 3 columns and 4 rows
@m_since{2020,06}
*/
typedef Math::Matrix3x4<Half> Matrix3x4h;

/**
@brief Half-float matrix with 4 columns and 2 rows
@m_since{2020,06}
*/
typedef Math::Matrix4x2<Half> Matrix4x2h;

/**
@brief Half-float matrix with 4 columns and 3 rows
@m_since{2020,06}

Note that this type doesn't have the columns four-byte aligned, which may
negatively affect performance in some cases. For better alignment use
@ref Matrix4x4h and ignore the bottom row.
*/
typedef Math::Matrix4x3<Half> Matrix4x3h;

/**
@brief Half-float matrix with 4 columns and 4 rows
@m_since{2020,06}
*/
typedef Math::Matrix4x4<Half> Matrix4x4h;

/* Since 1.8.17, the original short-hand group closing doesn't work anymore.
   FFS. */
/**
 * @}
 */

/** @{ @name Double-precision types

See @ref types for more information.
*/

/**
@brief Double (64bit)

Equivalent to GLSL @glsl double @ce.
@m_keyword{double,GLSL double,}
*/
typedef double Double;

/**
@brief Two-component double vector

Equivalent to GLSL @glsl dvec2 @ce.
@m_keyword{dvec2,GLSL dvec2,}
*/
typedef Math::Vector2<Double> Vector2d;

/**
@brief Three-component double vector

Equivalent to GLSL @glsl dvec3 @ce.
@m_keyword{dvec3,GLSL dvec3,}
*/
typedef Math::Vector3<Double> Vector3d;

/**
@brief Four-component double vector

Equivalent to GLSL @glsl dvec4 @ce.
@m_keyword{dvec4,GLSL dvec4,}
*/
typedef Math::Vector4<Double> Vector4d;

/**
@brief 3x3 double transformation matrix

Equivalent to GLSL @glsl dmat3 @ce.
@see @ref Matrix3x3d
@m_keyword{dmat3,GLSL dmat3,}
*/
typedef Math::Matrix3<Double> Matrix3d;

/**
@brief 4x4 double transformation matrix

Equivalent to GLSL @glsl dmat4 @ce.
@see @ref Matrix4x4d
@m_keyword{dmat4,GLSL dmat4,}
*/
typedef Math::Matrix4<Double> Matrix4d;

/**
@brief 2x2 double matrix

Equivalent to GLSL @glsl dmat2x2 @ce.
@m_keyword{dmat2x2,GLSL dmat2x2,}
*/
typedef Math::Matrix2x2<Double> Matrix2x2d;

/**
@brief 3x3 double matrix

Equivalent to GLSL @glsl dmat3x3 @ce. Note that this is different from
@ref Matrix3d, which contains additional functions for transformations in 2D.
@m_keyword{dmat3x3,GLSL dmat3x3,}
*/
typedef Math::Matrix3x3<Double> Matrix3x3d;

/**
@brief 4x4 double matrix

Equivalent to GLSL @glsl dmat4x4 @ce. Note that this is different from
@ref Matrix4d, which contains additional functions for transformations in 3D.
@m_keyword{dmat4x4,GLSL dmat4x4,}
*/
typedef Math::Matrix4x4<Double> Matrix4x4d;

/**
@brief Double matrix with 2 columns and 3 rows

Equivalent to GLSL @glsl dmat2x3 @ce.
@m_keyword{dmat2x3,GLSL dmat2x3,}
*/
typedef Math::Matrix2x3<Double> Matrix2x3d;

/**
@brief Double matrix with 3 columns and 2 rows

Equivalent to GLSL @glsl dmat3x2 @ce.
@m_keyword{dmat3x2,GLSL dmat3x2,}
*/
typedef Math::Matrix3x2<Double> Matrix3x2d;

/**
@brief Double matrix with 2 columns and 4 rows

Equivalent to GLSL @glsl dmat2x4 @ce.
@m_keyword{dmat2x4,GLSL dmat2x4,}
*/
typedef Math::Matrix2x4<Double> Matrix2x4d;

/**
@brief Double matrix with 4 columns and 2 rows

Equivalent to GLSL @glsl dmat4x2 @ce.
@m_keyword{dmat4x2,GLSL dmat4x2,}
*/
typedef Math::Matrix4x2<Double> Matrix4x2d;

/**
@brief Double matrix with 3 columns and 4 rows

Equivalent to GLSL @glsl dmat3x4 @ce.
@m_keyword{dmat3x4,GLSL dmat3x4,}
*/
typedef Math::Matrix3x4<Double> Matrix3x4d;

/**
@brief Double matrix with 4 columns and 3 rows

Equivalent to GLSL @glsl dmat4x3 @ce.
@m_keyword{dmat4x3,GLSL dmat4x3,}
*/
typedef Math::Matrix4x3<Double> Matrix4x3d;

/** @brief Double two-dimensional quadratic Bézier curve */
typedef Math::QuadraticBezier2D<Float> QuadraticBezier2Dd;

/** @brief Double three-dimensional quadratic Bézier curve */
typedef Math::QuadraticBezier3D<Float> QuadraticBezier3Dd;

/** @brief Double two-dimensional cubic Bézier curve */
typedef Math::CubicBezier2D<Float> CubicBezier2Dd;

/** @brief Double three-dimensional cubic Bézier curve */
typedef Math::CubicBezier3D<Float> CubicBezier3Dd;

/** @brief Double scalar cubic Hermite spline point */
typedef Math::CubicHermite1D<Double> CubicHermite1Dd;

/** @brief Double two-dimensional cubic Hermite spline point */
typedef Math::CubicHermite2D<Double> CubicHermite2Dd;

/** @brief Double three-dimensional cubic Hermite spline point */
typedef Math::CubicHermite3D<Double> CubicHermite3Dd;

/** @brief Double cubic Hermite spline complex number */
typedef Math::CubicHermiteComplex<Double> CubicHermiteComplexd;

/** @brief Double cubic Hermite spline quaternion */
typedef Math::CubicHermiteQuaternion<Double> CubicHermiteQuaterniond;

/** @brief Double complex number */
typedef Math::Complex<Double> Complexd;

/** @brief Double dual complex number */
typedef Math::DualComplex<Double> DualComplexd;

/** @brief Double quaternion */
typedef Math::Quaternion<Double> Quaterniond;

/** @brief Double dual quaternion */
typedef Math::DualQuaternion<Double> DualQuaterniond;

/** @brief Double constants */
typedef Math::Constants<Double> Constantsd;

/** @brief Angle in double degrees */
typedef Math::Deg<Double> Degd;

/** @brief Angle in double radians */
typedef Math::Rad<Double> Radd;

/** @brief One-dimensional double range */
typedef Math::Range1D<Double> Range1Dd;

/** @brief Two-dimensional double range */
typedef Math::Range2D<Double> Range2Dd;

/** @brief Three-dimensional double range */
typedef Math::Range3D<Double> Range3Dd;

/** @brief Double frustum */
typedef Math::Frustum<Double> Frustumd;

/* Since 1.8.17, the original short-hand group closing doesn't work anymore.
   FFS. */
/**
 * @}
 */

#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt, class T> class Array;
template<class T> class Array1D;
template<class T> class Array2D;
template<class T> class Array3D;

enum class InputFileCallbackPolicy: UnsignedByte;

template<UnsignedInt> class Image;
typedef Image<1> Image1D;
typedef Image<2> Image2D;
typedef Image<3> Image3D;

template<UnsignedInt> class CompressedImage;
typedef CompressedImage<1> CompressedImage1D;
typedef CompressedImage<2> CompressedImage2D;
typedef CompressedImage<3> CompressedImage3D;

template<UnsignedInt, class> class ImageView;
template<UnsignedInt dimensions> using BasicImageView = ImageView<dimensions, const char>;
typedef BasicImageView<1> ImageView1D;
typedef BasicImageView<2> ImageView2D;
typedef BasicImageView<3> ImageView3D;
template<UnsignedInt dimensions> using BasicMutableImageView = ImageView<dimensions, char>;
typedef BasicMutableImageView<1> MutableImageView1D;
typedef BasicMutableImageView<2> MutableImageView2D;
typedef BasicMutableImageView<3> MutableImageView3D;

template<UnsignedInt, class> class CompressedImageView;
template<UnsignedInt dimensions> using BasicCompressedImageView = CompressedImageView<dimensions, const char>;
typedef BasicCompressedImageView<1> CompressedImageView1D;
typedef BasicCompressedImageView<2> CompressedImageView2D;
typedef BasicCompressedImageView<3> CompressedImageView3D;
template<UnsignedInt dimensions> using BasicMutableCompressedImageView = CompressedImageView<dimensions, char>;
typedef BasicMutableCompressedImageView<1> MutableCompressedImageView1D;
typedef BasicMutableCompressedImageView<2> MutableCompressedImageView2D;
typedef BasicMutableCompressedImageView<3> MutableCompressedImageView3D;

enum class MeshPrimitive: UnsignedInt;
enum class MeshIndexType: UnsignedByte;
enum class VertexFormat: UnsignedInt;

enum class PixelFormat: UnsignedInt;
enum class CompressedPixelFormat: UnsignedInt;

class PixelStorage;
class CompressedPixelStorage;

enum class ResourceState: UnsignedByte;
enum class ResourceDataState: UnsignedByte;
enum class ResourcePolicy: UnsignedByte;
template<class T, class U = T> class Resource;
class ResourceKey;
template<class...> class ResourceManager;

enum class SamplerFilter: UnsignedInt;
enum class SamplerMipmap: UnsignedInt;
enum class SamplerWrapping: UnsignedInt;

class Timeline;
#endif

}

#endif
