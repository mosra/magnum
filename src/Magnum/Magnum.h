#ifndef Magnum_Magnum_h
#define Magnum_Magnum_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Forward declarations for @ref Magnum namespace
 */

#include <Corrade/Utility/Utility.h>

#include "Magnum/Types.h"
#include "Magnum/Math/Math.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
typedef unsigned int GLenum; /* Needed for *Format and *Type enums */
#endif

namespace Magnum {

namespace Math {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    template<class> struct Constants;

    #ifdef MAGNUM_BUILD_DEPRECATED
    namespace Literals {
        constexpr Rad<Double> operator "" _rad(long double);
        constexpr Deg<Double> operator "" _deg(long double);
        constexpr Rad<Float> operator "" _radf(long double);
        constexpr Deg<Float> operator "" _degf(long double);
    }
    #endif
    #endif
}

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
@brief Multi-threaded build

Defined if the library is built in a way that allows multiple thread-local
Magnum contexts. Enabled by default.
@see @ref building, @ref cmake, @ref Magnum::Context::current() "Context::current()"
*/
#define MAGNUM_BUILD_MULTITHREADED
#undef MAGNUM_BUILD_MULTITHREADED

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

#ifndef MAGNUM_TARGET_WEBGL
/**
@brief Unsigned long (64bit)

@requires_gles 64-bit integers are not available in WebGL.
*/
typedef std::uint64_t UnsignedLong;

/**
@brief Signed long (64bit)

@requires_gles 64-bit integers are not available in WebGL.
*/
typedef std::int64_t Long;
#endif

/** @brief Float (32bit) */
typedef float Float;

/** @brief Half (16bit) */
typedef Math::Half Half;

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

/** @brief Three-component (RGB) float color */
typedef Math::Color3<Float> Color3;

/** @brief Four-component (RGBA) float color */
typedef Math::Color4<Float> Color4;

/**
@brief Three-component (RGB) unsigned byte color

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. Use @ref Color3::fromSrgb() and @ref Color3::toSrgb()
    for proper sRGB handling.
*/
typedef Math::Color3<UnsignedByte> Color3ub;

/**
@brief Four-component (RGBA) unsigned byte color

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. Use @ref Color4::fromSrgbAlpha() and
    @ref Color4::toSrgbAlpha() for proper sRGB handling.
*/
typedef Math::Color4<UnsignedByte> Color4ub;

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
typedef Math::Matrix2x2<Float> Matrix2x2;

/**
@brief 3x3 float matrix

Note that this is different from @ref Matrix3, which contains additional
functions for transformations in 2D.
*/
typedef Math::Matrix3x3<Float> Matrix3x3;

/**
@brief 4x4 float matrix

Note that this is different from @ref Matrix4, which contains additional
functions for transformations in 3D.
*/
typedef Math::Matrix4x4<Float> Matrix4x4;

/** @brief Float matrix with 2 columns and 3 rows */
typedef Math::Matrix2x3<Float> Matrix2x3;

/** @brief Float matrix with 3 columns and 2 rows */
typedef Math::Matrix3x2<Float> Matrix3x2;

/** @brief Float matrix with 2 columns and 4 rows */
typedef Math::Matrix2x4<Float> Matrix2x4;

/** @brief Float matrix with 4 columns and 2 rows */
typedef Math::Matrix4x2<Float> Matrix4x2;

/** @brief Float matrix with 3 columns and 4 rows */
typedef Math::Matrix3x4<Float> Matrix3x4;

/** @brief Float matrix with 4 columns and 3 rows */
typedef Math::Matrix4x3<Float> Matrix4x3;

/** @brief Float two-dimensional quadratic Bézier curve */
typedef Math::QuadraticBezier2D<Float> QuadraticBezier2D;

/** @brief Float three-dimensional quadratic Bézier curve */
typedef Math::QuadraticBezier3D<Float> QuadraticBezier3D;

/** @brief Float two-dimensional cubic Bézier curve */
typedef Math::CubicBezier2D<Float> CubicBezier2D;

/** @brief Float three-dimensional cubic Bézier curve */
typedef Math::CubicBezier3D<Float> CubicBezier3D;

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

/** @brief Float 1D range */
typedef Math::Range1D<Float> Range1D;

/** @brief Float 2D range */
typedef Math::Range2D<Float> Range2D;

/** @brief Float 3D range */
typedef Math::Range3D<Float> Range3D;

/** @brief Signed integer 1D range */
typedef Math::Range1D<Int> Range1Di;

/** @brief Signed integer 2D range */
typedef Math::Range2D<Int> Range2Di;

/** @brief Signed integer 3D range */
typedef Math::Range3D<Int> Range3Di;

/** @brief Float frustum */
typedef Math::Frustum<Float> Frustum;

/*@}*/

/** @{ @name Double-precision types

See @ref types for more information.
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
typedef Math::Matrix2x2<Double> Matrix2x2d;

/**
@brief 3x3 double matrix

Note that this is different from @ref Matrix3d, which contains additional
functions for transformations in 2D.
*/
typedef Math::Matrix3x3<Double> Matrix3x3d;

/**
@brief 4x4 double matrix

Note that this is different from @ref Matrix4d, which contains additional
functions for transformations in 3D.
*/
typedef Math::Matrix4x4<Double> Matrix4x4d;

/** @brief Double matrix with 2 columns and 3 rows */
typedef Math::Matrix2x3<Double> Matrix2x3d;

/** @brief Double matrix with 3 columns and 2 rows */
typedef Math::Matrix3x2<Double> Matrix3x2d;

/** @brief Double matrix with 2 columns and 4 rows */
typedef Math::Matrix2x4<Double> Matrix2x4d;

/** @brief Double matrix with 4 columns and 2 rows */
typedef Math::Matrix4x2<Double> Matrix4x2d;

/** @brief Double matrix with 3 columns and 4 rows */
typedef Math::Matrix3x4<Double> Matrix3x4d;

/** @brief Double matrix with 4 columns and 3 rows */
typedef Math::Matrix4x3<Double> Matrix4x3d;

/** @brief Double two-dimensional quadratic Bézier curve */
typedef Math::QuadraticBezier2D<Float> QuadraticBezier2Dd;

/** @brief Double three-dimensional quadratic Bézier curve */
typedef Math::QuadraticBezier3D<Float> QuadraticBezier3Dd;

/** @brief Double two-dimensional cubic Bézier curve */
typedef Math::CubicBezier2D<Float> CubicBezier2Dd;

/** @brief Double three-dimensional cubic Bézier curve */
typedef Math::CubicBezier3D<Float> CubicBezier3Dd;

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

/** @brief Double 1D range */
typedef Math::Range1D<Double> Range1Dd;

/** @brief Double 2D range */
typedef Math::Range2D<Double> Range2Dd;

/** @brief Double 3D range */
typedef Math::Range3D<Double> Range3Dd;

/** @brief Double frustum */
typedef Math::Frustum<Double> Frustumd;

/*@}*/

#ifdef MAGNUM_BUILD_DEPRECATED
using Math::Literals::operator "" _deg;
using Math::Literals::operator "" _rad;
using Math::Literals::operator "" _degf;
using Math::Literals::operator "" _radf;
#endif

/* Forward declarations for all types in root namespace */

#ifndef DOXYGEN_GENERATING_OUTPUT
/* FramebufferClear[Mask], FramebufferBlit[Mask], FramebufferBlitFilter,
   FramebufferTarget enums used only directly with framebuffer instance */
class AbstractFramebuffer;

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
class AbstractQuery;
#endif
class AbstractShaderProgram;
class AbstractTexture;

template<UnsignedInt, class T> class Array;
template<class T> class Array1D;
template<class T> class Array2D;
template<class T> class Array3D;

template<UnsignedInt, class> class Attribute;

enum class BufferUsage: GLenum;
class Buffer;

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt> class BufferImage;
typedef BufferImage<1> BufferImage1D;
typedef BufferImage<2> BufferImage2D;
typedef BufferImage<3> BufferImage3D;

template<UnsignedInt> class CompressedBufferImage;
typedef CompressedBufferImage<1> CompressedBufferImage1D;
typedef CompressedBufferImage<2> CompressedBufferImage2D;
typedef CompressedBufferImage<3> CompressedBufferImage3D;
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
class BufferTexture;
enum class BufferTextureFormat: GLenum;
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
template<class T> using BasicColor3 CORRADE_DEPRECATED_ALIAS("use Math::Color3 instead") = Math::Color3<T>;
template<class T> using BasicColor4 CORRADE_DEPRECATED_ALIAS("use Math::Color4 instead") = Math::Color4<T>;
#endif

class Context;

class CubeMapTexture;
enum class CubeMapCoordinate: GLenum;
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
class CubeMapTextureArray;
#endif

/* DebugOutput, DebugMessage, DebugGroup used only statically */
/* DefaultFramebuffer is available only through global instance */
/* DimensionTraits forward declaration is not needed */

class Extension;
class Framebuffer;

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
enum class ImageFormat: GLenum;
enum class ImageAccess: GLenum;
#endif

template<UnsignedInt> class Image;
typedef Image<1> Image1D;
typedef Image<2> Image2D;
typedef Image<3> Image3D;

template<UnsignedInt> class CompressedImage;
typedef CompressedImage<1> CompressedImage1D;
typedef CompressedImage<2> CompressedImage2D;
typedef CompressedImage<3> CompressedImage3D;

template<UnsignedInt> class ImageView;
typedef ImageView<1> ImageView1D;
typedef ImageView<2> ImageView2D;
typedef ImageView<3> ImageView3D;

#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> using ImageReference CORRADE_DEPRECATED_ALIAS("use ImageView instead") = ImageView<dimensions>;
typedef CORRADE_DEPRECATED("use ImageView1D instead") ImageView1D ImageReference1D;
typedef CORRADE_DEPRECATED("use ImageView2D instead") ImageView2D ImageReference2D;
typedef CORRADE_DEPRECATED("use ImageView3D instead") ImageView3D ImageReference3D;
#endif

template<UnsignedInt> class CompressedImageView;
typedef CompressedImageView<1> CompressedImageView1D;
typedef CompressedImageView<2> CompressedImageView2D;
typedef CompressedImageView<3> CompressedImageView3D;

enum class MeshPrimitive: GLenum;

class Mesh;
class MeshView;

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/* MultisampleTextureSampleLocations enum used only in the function */
template<UnsignedInt> class MultisampleTexture;
typedef MultisampleTexture<2> MultisampleTexture2D;
typedef MultisampleTexture<3> MultisampleTexture2DArray;
#endif

enum class PixelFormat: GLenum;
enum class PixelType: GLenum;
enum class CompressedPixelFormat: GLenum;
#ifdef MAGNUM_BUILD_DEPRECATED
typedef CORRADE_DEPRECATED("use PixelFormat instead") PixelFormat ColorFormat;
typedef CORRADE_DEPRECATED("use PixelType instead") PixelType ColorType;
typedef CORRADE_DEPRECATED("use CompressedPixelFormat instead") CompressedPixelFormat CompressedColorFormat;
#endif

class PixelStorage;
#ifndef MAGNUM_TARGET_GLES
class CompressedPixelStorage;
#endif

/* ObjectFlag, ObjectFlags are used only in conjunction with *::wrap() function */

class PrimitiveQuery;
class SampleQuery;
class TimeQuery;

#ifndef MAGNUM_TARGET_GLES
class RectangleTexture;
#endif

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
#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
typedef Texture<3> Texture3D;
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt> class TextureArray;
#ifndef MAGNUM_TARGET_GLES
typedef TextureArray<1> Texture1DArray;
#endif
typedef TextureArray<2> Texture2DArray;
#endif

enum class TextureFormat: GLenum;

#ifndef MAGNUM_TARGET_GLES2
class TransformFeedback;
#endif
class Timeline;

enum class Version: Int;
#endif

}

#endif
