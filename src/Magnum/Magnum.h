#ifndef Magnum_Magnum_h
#define Magnum_Magnum_h
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

/** @file
 * @brief Forward declarations and basic types for the @ref Magnum namespace
 */

#include <Corrade/Utility/Utility.h>

#include "Magnum/Types.h"
#include "Magnum/Math/Math.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/GL.h"
#endif
#endif

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
@brief Multi-threaded build

Defined if the library is built in a way that allows multiple thread-local
Magnum contexts. Enabled by default.
@see @ref building, @ref cmake,
    @ref Magnum::GL::Context::current() "GL::Context::current()"
*/
#define MAGNUM_BUILD_MULTITHREADED
#undef MAGNUM_BUILD_MULTITHREADED

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
#endif

/**
@brief Vulkan interoperability

Defined if the engine is built with Vulkan interoperability enabled --- extra
APIs in various libraries interacting with the @ref Magnum::Vk "Vk" library.
Enabled by default in case the @ref Magnum::Vk "Vk" library is built.
@see @ref building, @ref cmake
*/
#define MAGNUM_TARGET_VK
#undef MAGNUM_TARGET_VK

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

/**
@brief Float (32bit)

Equivalent to GLSL @glsl float @ce.
@m_keyword{float,GLSL float,}
*/
typedef float Float;

/** @brief Half (16bit) */
typedef Math::Half Half;

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

/** @brief Double 1D range */
typedef Math::Range1D<Double> Range1Dd;

/** @brief Double 2D range */
typedef Math::Range2D<Double> Range2Dd;

/** @brief Double 3D range */
typedef Math::Range3D<Double> Range3Dd;

/** @brief Double frustum */
typedef Math::Frustum<Double> Frustumd;

/*@}*/

#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt, class T> class Array;
template<class T> class Array1D;
template<class T> class Array2D;
template<class T> class Array3D;

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

template<UnsignedInt> class CompressedImageView;
typedef CompressedImageView<1> CompressedImageView1D;
typedef CompressedImageView<2> CompressedImageView2D;
typedef CompressedImageView<3> CompressedImageView3D;

enum class MeshPrimitive: UnsignedInt;
enum class MeshIndexType: UnsignedInt;

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

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
typedef CORRADE_DEPRECATED("use GL::AbstractFramebuffer instead") GL::AbstractFramebuffer AbstractFramebuffer;

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
typedef CORRADE_DEPRECATED("use GL::AbstractQuery instead") GL::AbstractQuery AbstractQuery;
#endif
typedef CORRADE_DEPRECATED("use GL::AbstractShaderProgram instead") GL::AbstractShaderProgram AbstractShaderProgram;
typedef CORRADE_DEPRECATED("use GL::AbstractTexture instead") GL::AbstractTexture AbstractTexture;

template<UnsignedInt location, class T> using Attribute CORRADE_DEPRECATED_ALIAS("use GL::Attribute instead") = GL::Attribute<location, T>;

typedef CORRADE_DEPRECATED("use GL::BufferUsage instead") GL::BufferUsage BufferUsage;
typedef CORRADE_DEPRECATED("use GL::Buffer instead") GL::Buffer Buffer;

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> using BufferImage CORRADE_DEPRECATED_ALIAS("use GL::BufferImage instead") = GL::BufferImage<dimensions>;
typedef CORRADE_DEPRECATED("use GL::BufferImage1D instead") GL::BufferImage1D BufferImage1D;
typedef CORRADE_DEPRECATED("use GL::BufferImage2D instead") GL::BufferImage2D BufferImage2D;
typedef CORRADE_DEPRECATED("use GL::BufferImage3D instead") GL::BufferImage3D BufferImage3D;

template<UnsignedInt dimensions> using CompressedBufferImage CORRADE_DEPRECATED_ALIAS("use GL::CompressedBufferImage instead") = GL::BufferImage<dimensions>;
typedef CORRADE_DEPRECATED("use GL::CompressedBufferImage1D instead") GL::CompressedBufferImage1D CompressedBufferImage1D;
typedef CORRADE_DEPRECATED("use GL::CompressedBufferImage2D instead") GL::CompressedBufferImage2D CompressedBufferImage2D;
typedef CORRADE_DEPRECATED("use GL::CompressedBufferImage3D instead") GL::CompressedBufferImage3D CompressedBufferImage3D;
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
typedef CORRADE_DEPRECATED("use GL::BufferTexture instead") GL::BufferTexture BufferTexture;
typedef CORRADE_DEPRECATED("use GL::BufferTextureFormat instead") GL::BufferTextureFormat BufferTextureFormat;
#endif

typedef CORRADE_DEPRECATED("use GL::Context instead") GL::Context Context;

typedef CORRADE_DEPRECATED("use GL::CubeMapTexture instead") GL::CubeMapTexture CubeMapTexture;
typedef CORRADE_DEPRECATED("use GL::CubeMapCoordinate instead") GL::CubeMapCoordinate CubeMapCoordinate;
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
typedef CORRADE_DEPRECATED("use GL::CubeMapTextureArray instead") GL::CubeMapTextureArray CubeMapTextureArray;
#endif

typedef CORRADE_DEPRECATED("use GL::Extension instead") GL::Extension Extension;
typedef CORRADE_DEPRECATED("use GL::Framebuffer instead") GL::Framebuffer Framebuffer;

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
typedef CORRADE_DEPRECATED("use GL::ImageAccess instead") GL::ImageAccess ImageAccess;
typedef CORRADE_DEPRECATED("use GL::ImageFormat instead") GL::ImageFormat ImageFormat;
#endif

typedef CORRADE_DEPRECATED("use GL::Mesh instead") GL::Mesh Mesh;
typedef CORRADE_DEPRECATED("use GL::MeshView instead") GL::MeshView MeshView;

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/* MultisampleTextureSampleLocations enum used only in the function */
template<UnsignedInt dimensions> using MultisampleTexture CORRADE_DEPRECATED_ALIAS("use GL::MultisampleTexture instead") = GL::MultisampleTexture<dimensions>;
typedef CORRADE_DEPRECATED("use GL::MultisampleTexture2D instead") GL::MultisampleTexture2D MultisampleTexture2D;
typedef CORRADE_DEPRECATED("use GL::MultisampleTexture2DArray instead") GL::MultisampleTexture2DArray MultisampleTexture2DArray;
#endif

typedef CORRADE_DEPRECATED("use GL::PixelType instead") GL::PixelType PixelType;

typedef CORRADE_DEPRECATED("use GL::PrimitiveQuery instead") GL::PrimitiveQuery PrimitiveQuery;
typedef CORRADE_DEPRECATED("use GL::SampleQuery instead") GL::SampleQuery SampleQuery;
typedef CORRADE_DEPRECATED("use GL::TimeQuery instead") GL::TimeQuery TimeQuery;

#ifndef MAGNUM_TARGET_GLES
typedef CORRADE_DEPRECATED("use GL::RectangleTexture instead") GL::RectangleTexture RectangleTexture;
#endif

typedef CORRADE_DEPRECATED("use GL::Renderbuffer instead") GL::Renderbuffer Renderbuffer;
typedef CORRADE_DEPRECATED("use GL::RenderbufferFormat instead") GL::RenderbufferFormat RenderbufferFormat;

struct CORRADE_DEPRECATED("use GL::Sampler, SamplerFilter, GL::SamplerFilter,  SamplerMipmap, GL::SamplerMipmap, SamplerWrapping, GL::SamplerWrapping, GL::SamplerCompareMode, GL::SamplerCompareFunction or GL::SamplerDepthStencilMode instead") Sampler;
typedef CORRADE_DEPRECATED("use GL::Shader instead") GL::Shader Shader;

template<UnsignedInt dimensions> using Texture CORRADE_DEPRECATED_ALIAS("use GL::Texture instead") = GL::Texture<dimensions>;
#ifndef MAGNUM_TARGET_GLES
typedef CORRADE_DEPRECATED("use GL::Texture1D instead") GL::Texture1D Texture1D;
#endif
typedef CORRADE_DEPRECATED("use GL::Texture2D instead") GL::Texture2D Texture2D;
#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
typedef CORRADE_DEPRECATED("use GL::Texture3D instead") GL::Texture3D Texture3D;
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> using TextureArray CORRADE_DEPRECATED_ALIAS("use GL::TextureArray instead") = GL::TextureArray<dimensions>;
#ifndef MAGNUM_TARGET_GLES
typedef CORRADE_DEPRECATED("use GL::Texture1DArray instead") GL::Texture1DArray Texture1DArray;
#endif
typedef CORRADE_DEPRECATED("use GL::Texture2DArray instead") GL::Texture2DArray Texture2DArray;
#endif

typedef CORRADE_DEPRECATED("use GL::TextureFormat instead") GL::TextureFormat TextureFormat;

#ifndef MAGNUM_TARGET_GLES2
typedef CORRADE_DEPRECATED("use GL::TransformFeedback instead") GL::TransformFeedback TransformFeedback;
#endif

typedef CORRADE_DEPRECATED("use GL::Version instead") GL::Version Version;
#endif
#endif

}

#endif
