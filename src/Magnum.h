#ifndef Magnum_Magnum_h
#define Magnum_Magnum_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Basic definitions and forward declarations for Magnum namespace
 */

#include <cstdint>

#include "corradeConfigure.h"
#include "magnumConfigure.h"

#ifndef MAGNUM_TARGET_GLES
#include <GL/glew.h>
#include <GL/glcorearb.h>
#else
#ifndef MAGNUM_TARGET_GLES2
#include <GLES3/gl3.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#endif

/**
 * @todo Link to libGL / libGLES based on which windowcontext is used in app
 *      and whether GLES is enabled or not -- this allows us to use glx with
 *      ES on nvidia/intel. Using libGL and EGL on nvidia is whole another
 *      problem, though. How about windows? It won't allow unlinked DLLs, so
 *      probably always link Magnum itself to GL library there. How about unit
 *      tests not needing any of GL? -- different testing library?
 */

namespace Corrade {
    namespace Utility {
        class Debug;
        class Warning;
        class Error;
    }
}

namespace Magnum {

namespace Math {
    template<class> class Vector2;
    template<class> class Vector3;
    template<class> class Vector4;
    template<class> class Point2D;
    template<class> class Point3D;
    template<class> class Matrix3;
    template<class> class Matrix4;

    template<class T> constexpr T deg(T value);
    template<class T> constexpr T rad(T value);
    template<class T> class Constants;

    namespace Geometry {
        template<class> class Rectangle;
    }
}

/* Bring debugging facility from Corrade::Utility namespace */
using Corrade::Utility::Debug;
using Corrade::Utility::Warning;
using Corrade::Utility::Error;

/** @brief Two-component floating-point vector */
typedef Math::Vector2<GLfloat> Vector2;

/** @brief Three-component floating-point vector */
typedef Math::Vector3<GLfloat> Vector3;

/** @brief Four-component floating-point vector */
typedef Math::Vector4<GLfloat> Vector4;

/** @brief Two-component signed integer vector */
typedef Math::Vector2<GLint> Vector2i;

/** @brief Three-component signed integer vector */
typedef Math::Vector3<GLint> Vector3i;

/** @brief Four-component signed integer vector */
typedef Math::Vector4<GLint> Vector4i;

/** @brief Two-component unsigned integer vector */
typedef Math::Vector2<GLuint> Vector2ui;

/** @brief Three-component unsigned integer vector */
typedef Math::Vector3<GLuint> Vector3ui;

/** @brief Four-component unsigned integer vector */
typedef Math::Vector4<GLuint> Vector4ui;

/** @brief Two-dimensional floating-point homogeneous coordinates */
typedef Math::Point2D<GLfloat> Point2D;

/** @brief Three-dimensional floating-point homogeneous coordinates */
typedef Math::Point3D<GLfloat> Point3D;

/** @brief 3x3 floating-point matrix */
typedef Math::Matrix3<GLfloat> Matrix3;

/** @brief 4x4 floating-point matrix */
typedef Math::Matrix4<GLfloat> Matrix4;

/** @brief Floating-point constants */
/* Using float instead of GLfloat to not break KDevelop autocompletion */
typedef Math::Constants<float> Constants;

/** @brief Floating-point rectangle */
typedef Math::Geometry::Rectangle<GLfloat> Rectangle;

/** @brief Integer rectangle */
typedef Math::Geometry::Rectangle<GLint> Rectanglei;

/* Copying angle converters from Math namespace */
using Math::deg;
using Math::rad;

/* Forward declarations for all types in root namespace */
class AbstractFramebuffer;
class AbstractImage;
class AbstractShaderProgram;
class AbstractTexture;

template<std::uint8_t, class T> class Array;
template<class T> class Array1D;
template<class T> class Array2D;
template<class T> class Array3D;

class Buffer;

#ifndef MAGNUM_TARGET_GLES2
template<std::uint8_t> class BufferImage;
typedef BufferImage<1> BufferImage1D;
typedef BufferImage<2> BufferImage2D;
typedef BufferImage<3> BufferImage3D;
#endif

#ifndef MAGNUM_TARGET_GLES
class BufferTexture;
#endif

template<class T = GLfloat> class Color3;
template<class T = GLfloat> class Color4;

enum class Version: GLint;
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

template<std::uint8_t> class Image;
typedef Image<1> Image1D;
typedef Image<2> Image2D;
typedef Image<3> Image3D;

template<std::uint8_t> class ImageWrapper;
typedef ImageWrapper<1> ImageWrapper1D;
typedef ImageWrapper<2> ImageWrapper2D;
typedef ImageWrapper<3> ImageWrapper3D;

class IndexedMesh;
class Mesh;
class Query;
class Renderbuffer;

enum class ResourceState: std::uint8_t;
enum class ResourceDataState: std::uint8_t;
enum class ResourcePolicy: std::uint8_t;
template<class T, class U = T> class Resource;
class ResourceKey;
template<class...> class ResourceManager;

class Shader;

template<std::uint8_t> class Texture;
#ifndef MAGNUM_TARGET_GLES
typedef Texture<1> Texture1D;
#endif
typedef Texture<2> Texture2D;
typedef Texture<3> Texture3D;

class Timeline;

}

#endif
