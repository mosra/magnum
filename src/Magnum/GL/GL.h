#ifndef Magnum_GL_h
#define Magnum_GL_h
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
 * @brief Forward declarations for the @ref Magnum::GL namespace
 */

#include "Magnum/Types.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
typedef int GLint; /* Needed for sampler enums */
typedef unsigned int GLenum; /* Needed for *Format and *Type enums */
typedef unsigned int GLuint; /* Needed by Implementation/State.h */
#endif

namespace Magnum { namespace GL {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* FramebufferClear[Mask], FramebufferBlit[Mask], FramebufferBlitFilter,
   FramebufferTarget enums used only directly with framebuffer instance */
class AbstractFramebuffer;

class AbstractQuery;
class AbstractShaderProgram;
class AbstractTexture;

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

enum class MeshPrimitive: GLenum;
enum class MeshIndexType: GLenum;

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

/* ObjectFlag, ObjectFlags are used only in conjunction with *::wrap() function */

#ifndef MAGNUM_TARGET_GLES
class PipelineStatisticsQuery;
#endif
#ifndef MAGNUM_TARGET_GLES2
class PrimitiveQuery;
#endif
#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
class SampleQuery;
#endif
class TimeQuery;

#ifndef MAGNUM_TARGET_GLES
class RectangleTexture;
#endif

class Renderbuffer;
enum class RenderbufferFormat: GLenum;

enum class SamplerFilter: GLint;
enum class SamplerMipmap: GLint;
enum class SamplerWrapping: GLint;
enum class SamplerCompareMode: GLenum;
#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
enum class SamplerCompareFunction: GLenum;
#endif
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
enum class SamplerDepthStencilMode: GLenum;
#endif

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

enum class Version: Int;
#endif

}}

#endif

