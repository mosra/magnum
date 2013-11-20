#ifndef Magnum_Magnum_h
#define Magnum_Magnum_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Forward declarations for Magnum namespace
 */

#include <Utility/Utility.h>

#include "Math/Math.h"
#include "Types.h"
#include "magnumConfigure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
typedef unsigned int GLenum; /* Needed for *Format and *Type enums */
#endif

namespace Magnum {

namespace Math {
    template<class T> struct Constants;

    /** @todoc Remove `ifndef` when Doxygen is able to handle operator"" */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #ifndef CORRADE_GCC46_COMPATIBILITY
    #ifndef MAGNUM_TARGET_GLES
    constexpr Rad<Double> operator "" _rad(long double);
    constexpr Deg<Double> operator "" _deg(long double);
    #endif
    constexpr Rad<Float> operator "" _radf(long double);
    constexpr Deg<Float> operator "" _degf(long double);
    #endif
    #endif
}

/* Bring whole Corrade namespace */
using namespace Corrade;

/* Bring debugging facility from Corrade::Utility namespace */
using Corrade::Utility::Debug;
using Corrade::Utility::Warning;
using Corrade::Utility::Error;

#ifdef DOXYGEN_GENERATING_OUTPUT
/**
@brief Build with deprecated API included

Defined if the library contains deprecated API (which will be removed in the
future). To preserve backward compatibility, %Magnum is by default built with
deprecated API included.
@see @ref building
*/
#define MAGNUM_BUILD_DEPRECATED
/* (enabled by default) */

/**
@brief Static library build

Defined if built as static libraries. Default are shared libraries.
@see @ref building
*/
#define MAGNUM_BUILD_STATIC
#undef MAGNUM_BUILD_STATIC

/**
@brief OpenGL ES target

Defined if the engine is built for OpenGL ES 3.0 or OpenGL ES 2.0.
@see @ref MAGNUM_TARGET_GLES2, @ref MAGNUM_TARGET_GLES3,
    @ref MAGNUM_TARGET_DESKTOP_GLES, @ref building
*/
#define MAGNUM_TARGET_GLES
#undef MAGNUM_TARGET_GLES

/**
@brief OpenGL ES 2.0 target.

Defined if the engine is built for OpenGL ES 2.0. Implies also
@ref MAGNUM_TARGET_GLES.
@see @ref MAGNUM_TARGET_GLES3, @ref MAGNUM_TARGET_DESKTOP_GLES, @ref building
*/
#define MAGNUM_TARGET_GLES2
#undef MAGNUM_TARGET_GLES2

/**
@brief OpenGL ES 3.0 target.

Defined if the engine is built for OpenGL ES 3.0. Implies also
@ref MAGNUM_TARGET_GLES.
@see @ref MAGNUM_TARGET_GLES2, @ref MAGNUM_TARGET_DESKTOP_GLES, @ref building
*/
#define MAGNUM_TARGET_GLES3
#undef MAGNUM_TARGET_GLES3

/**
@brief Desktop emulation of OpenGL ES target

Defined if the engine is built for OpenGL ES 3.0 or OpenGL ES 2.0 emulated
within standard desktop OpenGL. Implies also @ref MAGNUM_TARGET_GLES.
@see @ref MAGNUM_TARGET_GLES2, @ref building
*/
#define MAGNUM_TARGET_DESKTOP_GLES
#undef MAGNUM_TARGET_DESKTOP_GLES
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

/** @brief Unsigned int (32bit) */
typedef std::uint32_t UnsignedInt;

/** @brief Signed int (32bit) */
typedef std::int32_t Int;

/** @brief Unsigned long (64bit) */
typedef std::uint64_t UnsignedLong;

/** @brief Signed long (64bit) */
typedef std::int64_t Long;

/** @brief Float (32bit) */
typedef float Float;

/** @brief Two-component float vector */
typedef Math::Vector2<Float> Vector2;

/** @brief Three-component float vector */
typedef Math::Vector3<Float> Vector3;

/** @brief Four-component float vector */
typedef Math::Vector4<Float> Vector4;

/** @brief Two-component unsigned integer vector */
typedef Math::Vector2<UnsignedInt> Vector2ui;

/** @brief Three-component unsigned integer vector */
typedef Math::Vector3<UnsignedInt> Vector3ui;

/** @brief Four-component unsigned integer vector */
typedef Math::Vector4<UnsignedInt> Vector4ui;

/** @brief Two-component signed integer vector */
typedef Math::Vector2<Int> Vector2i;

/** @brief Three-component signed integer vector */
typedef Math::Vector3<Int> Vector3i;

/** @brief Four-component signed integer vector */
typedef Math::Vector4<Int> Vector4i;

/**
@brief 3x3 float transformation matrix

@see @ref Matrix3x3
*/
typedef Math::Matrix3<Float> Matrix3;

/**
@brief 4x4 float transformation matrix

@see @ref Matrix4x4
*/
typedef Math::Matrix4<Float> Matrix4;

/** @brief 2x2 float matrix */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix2x2<Float> Matrix2x2;
#else
typedef Math::Matrix<2, Float> Matrix2x2;
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@copybrief Matrix2x2
@deprecated Use @ref Magnum::Matrix2x2 "Matrix2x2" instead.
*/
typedef Math::Matrix<2, Float> Matrix2;
#endif

/**
@brief 3x3 float matrix

Note that this is different from @ref Matrix3, which contains additional
functions for transformations in 2D.
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix3x3<Float> Matrix3x3;
#else
typedef Math::Matrix<3, Float> Matrix3x3;
#endif

/**
@brief 4x4 float matrix

Note that this is different from @ref Matrix4, which contains additional
functions for transformations in 3D.
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix4x4<Float> Matrix4x4;
#else
typedef Math::Matrix<4, Float> Matrix4x4;
#endif

/** @brief Float matrix with 2 columns and 3 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix2x3<Float> Matrix2x3;
#else
typedef Math::RectangularMatrix<2, 3, Float> Matrix2x3;
#endif

/** @brief Float matrix with 3 columns and 2 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix3x2<Float> Matrix3x2;
#else
typedef Math::RectangularMatrix<3, 2, Float> Matrix3x2;
#endif

/** @brief Float matrix with 2 columns and 4 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix2x4<Float> Matrix2x4;
#else
typedef Math::RectangularMatrix<2, 4, Float> Matrix2x4;
#endif

/** @brief Float matrix with 4 columns and 2 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix4x2<Float> Matrix4x2;
#else
typedef Math::RectangularMatrix<4, 2, Float> Matrix4x2;
#endif

/** @brief Float matrix with 3 columns and 4 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix3x4<Float> Matrix3x4;
#else
typedef Math::RectangularMatrix<3, 4, Float> Matrix3x4;
#endif

/** @brief Float matrix with 4 columns and 3 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix4x3<Float> Matrix4x3;
#else
typedef Math::RectangularMatrix<4, 3, Float> Matrix4x3;
#endif

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

/** @brief Float rectangle */
typedef Math::Geometry::Rectangle<Float> Rectangle;

/** @brief Signed integer rectangle */
typedef Math::Geometry::Rectangle<Int> Rectanglei;

/*@}*/

#ifndef MAGNUM_TARGET_GLES
/** @{ @name Double-precision types

See @ref types for more information.
@requires_gl Only single-precision types are available in OpenGL ES.
*/

/** @brief Double (64bit) */
typedef double Double;

/** @brief Two-component double vector */
typedef Math::Vector2<Double> Vector2d;

/** @brief Three-component double vector */
typedef Math::Vector3<Double> Vector3d;

/** @brief Four-component double vector */
typedef Math::Vector4<Double> Vector4d;

/**
@brief 3x3 double transformation matrix

@see @ref Matrix3x3d
*/
typedef Math::Matrix3<Double> Matrix3d;

/**
@brief 4x4 double transformation matrix

@see @ref Matrix4x4d
*/
typedef Math::Matrix4<Double> Matrix4d;

/** @brief 2x2 double matrix */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix2x2<Double> Matrix2x2d;
#else
typedef Math::Matrix<2, Double> Matrix2x2d;
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@copybrief Matrix2x2d
@deprecated Use @ref Magnum::Matrix2x2d "Matrix2x2d" instead.
*/
typedef Math::Matrix<2, Double> Matrix2d;
#endif

/**
@brief 3x3 double matrix

Note that this is different from @ref Matrix3d, which contains additional
functions for transformations in 2D.
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix3x3<Double> Matrix3x3d;
#else
typedef Math::Matrix<3, Double> Matrix3x3d;
#endif

/**
@brief 4x4 double matrix

Note that this is different from @ref Matrix4d, which contains additional
functions for transformations in 3D.
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix4x4<Double> Matrix4x4d;
#else
typedef Math::Matrix<4, Double> Matrix4x4d;
#endif

/** @brief Double matrix with 2 columns and 3 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix2x3<Double> Matrix2x3d;
#else
typedef Math::RectangularMatrix<2, 3, Double> Matrix2x3d;
#endif

/** @brief Double matrix with 3 columns and 2 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix3x2<Double> Matrix3x2d;
#else
typedef Math::RectangularMatrix<3, 2, Double> Matrix3x2d;
#endif

/** @brief Double matrix with 2 columns and 4 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix2x4<Double> Matrix2x4d;
#else
typedef Math::RectangularMatrix<2, 4, Double> Matrix2x4d;
#endif

/** @brief Double matrix with 4 columns and 2 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix4x2<Double> Matrix4x2d;
#else
typedef Math::RectangularMatrix<4, 2, Double> Matrix4x2d;
#endif

/** @brief Double matrix with 3 columns and 4 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix3x4<Double> Matrix3x4d;
#else
typedef Math::RectangularMatrix<3, 4, Double> Matrix3x4d;
#endif

/** @brief Double matrix with 4 columns and 3 rows */
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Matrix4x3<Double> Matrix4x3d;
#else
typedef Math::RectangularMatrix<4, 3, Double> Matrix4x3d;
#endif

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

/** @brief Double rectangle */
typedef Math::Geometry::Rectangle<Double> Rectangled;

/*@}*/
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/* Using angle literals from Math namespace */
#ifndef MAGNUM_TARGET_GLES
using Math::operator "" _deg;
using Math::operator "" _rad;
#endif
using Math::operator "" _degf;
using Math::operator "" _radf;
#endif

/* Forward declarations for all types in root namespace */

/* FramebufferClear[Mask], FramebufferBlit[Mask], FramebufferBlitFilter,
   FramebufferTarget enums used only directly with framebuffer instance */
class AbstractFramebuffer;

class AbstractImage;
class AbstractShaderProgram;
class AbstractTexture;

template<UnsignedInt, class T> class Array;
template<class T> class Array1D;
template<class T> class Array2D;
template<class T> class Array3D;

enum class BufferUsage: GLenum;
class Buffer;

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt> class BufferImage;
typedef BufferImage<1> BufferImage1D;
typedef BufferImage<2> BufferImage2D;
typedef BufferImage<3> BufferImage3D;
#endif

#ifndef MAGNUM_TARGET_GLES
class BufferTexture;
enum class BufferTextureFormat: GLenum;
#endif

template<class> class BasicColor3;
template<class> class BasicColor4;
typedef BasicColor3<Float> Color3;
typedef BasicColor4<Float> Color4;

enum class ColorFormat: GLenum;
enum class ColorType: GLenum;
/** @todo Remove this when dropping backward compatibility */
typedef ColorFormat ImageFormat;
typedef ColorType ColorType;

enum class Version: Int;
class Context;
class CubeMapTexture;

#ifndef MAGNUM_TARGET_GLES
class CubeMapTextureArray;
#endif

/* DebugMarker forward declaration is not needed */
/* DefaultFramebuffer is available only through global instance */
/* DimensionTraits forward declaration is not needed */

class Extension;
class Framebuffer;

template<UnsignedInt> class Image;
typedef Image<1> Image1D;
typedef Image<2> Image2D;
typedef Image<3> Image3D;

template<UnsignedInt> class ImageReference;
typedef ImageReference<1> ImageReference1D;
typedef ImageReference<2> ImageReference2D;
typedef ImageReference<3> ImageReference3D;

class Mesh;
class MeshView;

/* AbstractQuery is not used directly */
class PrimitiveQuery;
class SampleQuery;
class TimeQuery;

class Renderbuffer;
enum class RenderbufferFormat: GLenum;

enum class ResourceState: UnsignedByte;
enum class ResourceDataState: UnsignedByte;
enum class ResourcePolicy: UnsignedByte;
template<class T, class U = T> class Resource;
class ResourceKey;
template<class...> class ResourceManager;

class Sampler;
class Shader;

template<UnsignedInt> class Texture;
#ifndef MAGNUM_TARGET_GLES
typedef Texture<1> Texture1D;
#endif
typedef Texture<2> Texture2D;
typedef Texture<3> Texture3D;

enum class TextureFormat: GLenum;

class Timeline;

}

#endif
