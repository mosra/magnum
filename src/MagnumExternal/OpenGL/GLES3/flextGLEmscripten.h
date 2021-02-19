#ifndef _flextgl_h_
#define _flextgl_h_
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/GL/GL.h"
#include "MagnumExternal/OpenGL/KHR/khrplatform.h"

/* Defensive include guards */

#if defined(__gl_h_) || defined(__gl2_h_) || defined(__gl3_h_) || defined(__gl31_h_) || defined(__gl32_h_)
#error Attempt to include auto-generated header after including gl3.h
#endif
#if defined(__gl2ext_h_) || defined(__gl3ext_h_)
#error Attempt to include auto-generated header after including gl3ext.h
#endif
#if defined(__gl2platform_h_) || defined(__gl3platform_h_)
#error Attempt to include auto-generated header after including gl3platform.h
#endif

#define __gl_h_
#define __gl2_h_
#define __gl3_h_
#define __gl31_h_
#define __gl32_h_
#define __gl2ext_h_
#define __gl3ext_h_
#define __gl2platform_h_
#define __gl3platform_h_

#ifdef __cplusplus
extern "C" {
#endif

void flextGLInit(Magnum::GL::Context& context);

/* Function declaration macros */
#define APIENTRY
#ifndef GLAPI
#define GLAPI extern
#endif

/* GL version defines. Needs to be done to avoid conflicting definitions with
   other GL headers (such as qopengl.h in Qt, which has different definition
   for GLsizei and GLintptr on Windows) */
#define GL_ES_VERSION_2_0 1
#define GL_ES_VERSION_3_0 1

/* Data types */

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef khronos_uint8_t GLubyte;
typedef int GLint;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef khronos_float_t GLfloat;
typedef char GLchar;
typedef khronos_uint16_t GLhalf;
typedef khronos_intptr_t GLintptr;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_int64_t GLint64;
typedef khronos_uint64_t GLuint64;
typedef struct __GLsync *GLsync;

/* Enums */

/* GL_ES_VERSION_2_0 */

#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_FUNC_ADD 0x8006
#define GL_BLEND_EQUATION 0x8009
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_BLEND_COLOR 0x8005
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_STREAM_DRAW 0x88E0
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_FRONT_AND_BACK 0x0408
#define GL_TEXTURE_2D 0x0DE1
#define GL_CULL_FACE 0x0B44
#define GL_BLEND 0x0BE2
#define GL_DITHER 0x0BD0
#define GL_STENCIL_TEST 0x0B90
#define GL_DEPTH_TEST 0x0B71
#define GL_SCISSOR_TEST 0x0C11
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_SAMPLE_COVERAGE 0x80A0
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_LINE_WIDTH 0x0B21
#define GL_ALIASED_POINT_SIZE_RANGE 0x846D
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_DEPTH_RANGE 0x0B70
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_FUNC 0x0B74
#define GL_STENCIL_CLEAR_VALUE 0x0B91
#define GL_STENCIL_FUNC 0x0B92
#define GL_STENCIL_FAIL 0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
#define GL_STENCIL_REF 0x0B97
#define GL_STENCIL_VALUE_MASK 0x0B93
#define GL_STENCIL_WRITEMASK 0x0B98
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_VIEWPORT 0x0BA2
#define GL_SCISSOR_BOX 0x0C10
#define GL_COLOR_CLEAR_VALUE 0x0C22
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_SUBPIXEL_BITS 0x0D50
#define GL_RED_BITS 0x0D52
#define GL_GREEN_BITS 0x0D53
#define GL_BLUE_BITS 0x0D54
#define GL_ALPHA_BITS 0x0D55
#define GL_DEPTH_BITS 0x0D56
#define GL_STENCIL_BITS 0x0D57
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_SAMPLE_BUFFERS 0x80A8
#define GL_SAMPLES 0x80A9
#define GL_SAMPLE_COVERAGE_VALUE 0x80AA
#define GL_SAMPLE_COVERAGE_INVERT 0x80AB
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_DONT_CARE 0x1100
#define GL_FASTEST 0x1101
#define GL_NICEST 0x1102
#define GL_GENERATE_MIPMAP_HINT 0x8192
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_FIXED 0x140C
#define GL_DEPTH_COMPONENT 0x1902
#define GL_ALPHA 0x1906
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#define GL_MAX_VARYING_VECTORS 0x8DFC
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD
#define GL_SHADER_TYPE 0x8B4F
#define GL_DELETE_STATUS 0x8B80
#define GL_LINK_STATUS 0x8B82
#define GL_VALIDATE_STATUS 0x8B83
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_INCR 0x1E02
#define GL_DECR 0x1E03
#define GL_INVERT 0x150A
#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE 0x1702
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MIRRORED_REPEAT 0x8370
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_CUBE 0x8B60
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_COMPILE_STATUS 0x8B81
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_SHADER_COMPILER 0x8DFA
#define GL_SHADER_BINARY_FORMATS 0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS 0x8DF9
#define GL_LOW_FLOAT 0x8DF0
#define GL_MEDIUM_FLOAT 0x8DF1
#define GL_HIGH_FLOAT 0x8DF2
#define GL_LOW_INT 0x8DF3
#define GL_MEDIUM_INT 0x8DF4
#define GL_HIGH_INT 0x8DF5
#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGB565 0x8D62
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_STENCIL_INDEX8 0x8D48
#define GL_RENDERBUFFER_WIDTH 0x8D42
#define GL_RENDERBUFFER_HEIGHT 0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
#define GL_RENDERBUFFER_RED_SIZE 0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_NONE 0
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_RENDERBUFFER_BINDING 0x8CA7
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506

/* GL_ES_VERSION_3_0 */

#define GL_READ_BUFFER 0x0C02
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_UNPACK_SKIP_ROWS 0x0CF3
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_PACK_ROW_LENGTH 0x0D02
#define GL_PACK_SKIP_ROWS 0x0D03
#define GL_PACK_SKIP_PIXELS 0x0D04
#define GL_COLOR 0x1800
#define GL_DEPTH 0x1801
#define GL_STENCIL 0x1802
#define GL_RED 0x1903
#define GL_RGB8 0x8051
#define GL_RGBA8 0x8058
#define GL_RGB10_A2 0x8059
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_UNPACK_SKIP_IMAGES 0x806D
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_MAX_TEXTURE_LOD_BIAS 0x84FD
#define GL_TEXTURE_COMPARE_MODE 0x884C
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#define GL_CURRENT_QUERY 0x8865
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_BUFFER_MAPPED 0x88BC
#define GL_BUFFER_MAP_POINTER 0x88BD
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_DRAW_BUFFER0 0x8825
#define GL_DRAW_BUFFER1 0x8826
#define GL_DRAW_BUFFER2 0x8827
#define GL_DRAW_BUFFER3 0x8828
#define GL_DRAW_BUFFER4 0x8829
#define GL_DRAW_BUFFER5 0x882A
#define GL_DRAW_BUFFER6 0x882B
#define GL_DRAW_BUFFER7 0x882C
#define GL_DRAW_BUFFER8 0x882D
#define GL_DRAW_BUFFER9 0x882E
#define GL_DRAW_BUFFER10 0x882F
#define GL_DRAW_BUFFER11 0x8830
#define GL_DRAW_BUFFER12 0x8831
#define GL_DRAW_BUFFER13 0x8832
#define GL_DRAW_BUFFER14 0x8833
#define GL_DRAW_BUFFER15 0x8834
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
#define GL_SAMPLER_3D 0x8B5F
#define GL_SAMPLER_2D_SHADOW 0x8B62
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING 0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING 0x88EF
#define GL_FLOAT_MAT2x3 0x8B65
#define GL_FLOAT_MAT2x4 0x8B66
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_FLOAT_MAT3x4 0x8B68
#define GL_FLOAT_MAT4x2 0x8B69
#define GL_FLOAT_MAT4x3 0x8B6A
#define GL_SRGB 0x8C40
#define GL_SRGB8 0x8C41
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_COMPARE_REF_TO_TEXTURE 0x884E
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_NUM_EXTENSIONS 0x821D
#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815
#define GL_RGBA16F 0x881A
#define GL_RGB16F 0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER 0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET 0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET 0x8905
#define GL_MAX_VARYING_COMPONENTS 0x8B4B
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#define GL_RGB9_E5 0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV 0x8C3E
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS 0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_RASTERIZER_DISCARD 0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_INTERLEAVED_ATTRIBS 0x8C8C
#define GL_SEPARATE_ATTRIBS 0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RGBA32UI 0x8D70
#define GL_RGB32UI 0x8D71
#define GL_RGBA16UI 0x8D76
#define GL_RGB16UI 0x8D77
#define GL_RGBA8UI 0x8D7C
#define GL_RGB8UI 0x8D7D
#define GL_RGBA32I 0x8D82
#define GL_RGB32I 0x8D83
#define GL_RGBA16I 0x8D88
#define GL_RGB16I 0x8D89
#define GL_RGBA8I 0x8D8E
#define GL_RGB8I 0x8D8F
#define GL_RED_INTEGER 0x8D94
#define GL_RGB_INTEGER 0x8D98
#define GL_RGBA_INTEGER 0x8D99
#define GL_SAMPLER_2D_ARRAY 0x8DC1
#define GL_SAMPLER_2D_ARRAY_SHADOW 0x8DC4
#define GL_SAMPLER_CUBE_SHADOW 0x8DC5
#define GL_UNSIGNED_INT_VEC2 0x8DC6
#define GL_UNSIGNED_INT_VEC3 0x8DC7
#define GL_UNSIGNED_INT_VEC4 0x8DC8
#define GL_INT_SAMPLER_2D 0x8DCA
#define GL_INT_SAMPLER_3D 0x8DCB
#define GL_INT_SAMPLER_CUBE 0x8DCC
#define GL_INT_SAMPLER_2D_ARRAY 0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_2D 0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D 0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE 0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY 0x8DD7
#define GL_BUFFER_ACCESS_FLAGS 0x911F
#define GL_BUFFER_MAP_LENGTH 0x9120
#define GL_BUFFER_MAP_OFFSET 0x9121
#define GL_DEPTH_COMPONENT32F 0x8CAC
#define GL_DEPTH32F_STENCIL8 0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT 0x8218
#define GL_FRAMEBUFFER_UNDEFINED 0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_DEPTH_STENCIL 0x84F9
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_UNSIGNED_NORMALIZED 0x8C17
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_RENDERBUFFER_SAMPLES 0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_COLOR_ATTACHMENT16 0x8CF0
#define GL_COLOR_ATTACHMENT17 0x8CF1
#define GL_COLOR_ATTACHMENT18 0x8CF2
#define GL_COLOR_ATTACHMENT19 0x8CF3
#define GL_COLOR_ATTACHMENT20 0x8CF4
#define GL_COLOR_ATTACHMENT21 0x8CF5
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_COLOR_ATTACHMENT23 0x8CF7
#define GL_COLOR_ATTACHMENT24 0x8CF8
#define GL_COLOR_ATTACHMENT25 0x8CF9
#define GL_COLOR_ATTACHMENT26 0x8CFA
#define GL_COLOR_ATTACHMENT27 0x8CFB
#define GL_COLOR_ATTACHMENT28 0x8CFC
#define GL_COLOR_ATTACHMENT29 0x8CFD
#define GL_COLOR_ATTACHMENT30 0x8CFE
#define GL_COLOR_ATTACHMENT31 0x8CFF
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_MAX_SAMPLES 0x8D57
#define GL_HALF_FLOAT 0x140B
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT 0x0020
#define GL_RG 0x8227
#define GL_RG_INTEGER 0x8228
#define GL_R8 0x8229
#define GL_RG8 0x822B
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C
#define GL_VERTEX_ARRAY_BINDING 0x85B5
#define GL_R8_SNORM 0x8F94
#define GL_RG8_SNORM 0x8F95
#define GL_RGB8_SNORM 0x8F96
#define GL_RGBA8_SNORM 0x8F97
#define GL_SIGNED_NORMALIZED 0x8F9C
#define GL_PRIMITIVE_RESTART_FIXED_INDEX 0x8D69
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_COPY_READ_BUFFER_BINDING 0x8F36
#define GL_COPY_WRITE_BUFFER_BINDING 0x8F37
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_UNIFORM_BUFFER_BINDING 0x8A28
#define GL_UNIFORM_BUFFER_START 0x8A29
#define GL_UNIFORM_BUFFER_SIZE 0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS 0x8A2B
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS 0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS 0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS 0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE 0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36
#define GL_UNIFORM_TYPE 0x8A37
#define GL_UNIFORM_SIZE 0x8A38
#define GL_UNIFORM_NAME_LENGTH 0x8A39
#define GL_UNIFORM_BLOCK_INDEX 0x8A3A
#define GL_UNIFORM_OFFSET 0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE 0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE 0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR 0x8A3E
#define GL_UNIFORM_BLOCK_BINDING 0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH 0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS 0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX 0xFFFFFFFFu
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS 0x9122
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS 0x9125
#define GL_MAX_SERVER_WAIT_TIMEOUT 0x9111
#define GL_OBJECT_TYPE 0x9112
#define GL_SYNC_CONDITION 0x9113
#define GL_SYNC_STATUS 0x9114
#define GL_SYNC_FLAGS 0x9115
#define GL_SYNC_FENCE 0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_UNSIGNALED 0x9118
#define GL_SIGNALED 0x9119
#define GL_ALREADY_SIGNALED 0x911A
#define GL_TIMEOUT_EXPIRED 0x911B
#define GL_CONDITION_SATISFIED 0x911C
#define GL_WAIT_FAILED 0x911D
#define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001
#define GL_TIMEOUT_IGNORED 0xFFFFFFFFFFFFFFFFull
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR 0x88FE
#define GL_ANY_SAMPLES_PASSED 0x8C2F
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE 0x8D6A
#define GL_SAMPLER_BINDING 0x8919
#define GL_RGB10_A2UI 0x906F
#define GL_TEXTURE_SWIZZLE_R 0x8E42
#define GL_TEXTURE_SWIZZLE_G 0x8E43
#define GL_TEXTURE_SWIZZLE_B 0x8E44
#define GL_TEXTURE_SWIZZLE_A 0x8E45
#define GL_GREEN 0x1904
#define GL_BLUE 0x1905
#define GL_INT_2_10_10_10_REV 0x8D9F
#define GL_TRANSFORM_FEEDBACK 0x8E22
#define GL_TRANSFORM_FEEDBACK_PAUSED 0x8E23
#define GL_TRANSFORM_FEEDBACK_ACTIVE 0x8E24
#define GL_TRANSFORM_FEEDBACK_BINDING 0x8E25
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
#define GL_PROGRAM_BINARY_LENGTH 0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS 0x87FE
#define GL_PROGRAM_BINARY_FORMATS 0x87FF
#define GL_COMPRESSED_R11_EAC 0x9270
#define GL_COMPRESSED_SIGNED_R11_EAC 0x9271
#define GL_COMPRESSED_RG11_EAC 0x9272
#define GL_COMPRESSED_SIGNED_RG11_EAC 0x9273
#define GL_COMPRESSED_RGB8_ETC2 0x9274
#define GL_COMPRESSED_SRGB8_ETC2 0x9275
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#define GL_TEXTURE_IMMUTABLE_FORMAT 0x912F
#define GL_MAX_ELEMENT_INDEX 0x8D6B
#define GL_NUM_SAMPLE_COUNTS 0x9380
#define GL_TEXTURE_IMMUTABLE_LEVELS 0x82DF

/* GL_EXT_texture_filter_anisotropic */

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

/* GL_EXT_disjoint_timer_query */

#define GL_QUERY_COUNTER_BITS_EXT 0x8864
#define GL_CURRENT_QUERY_EXT 0x8865
#define GL_QUERY_RESULT_EXT 0x8866
#define GL_QUERY_RESULT_AVAILABLE_EXT 0x8867
#define GL_TIME_ELAPSED_EXT 0x88BF
#define GL_TIMESTAMP_EXT 0x8E28
#define GL_GPU_DISJOINT_EXT 0x8FBB

/* GL_EXT_texture_compression_rgtc */

#define GL_COMPRESSED_RED_RGTC1_EXT 0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1_EXT 0x8DBC
#define GL_COMPRESSED_RED_GREEN_RGTC2_EXT 0x8DBD
#define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT 0x8DBE

/* GL_EXT_texture_compression_bptc */

#define GL_COMPRESSED_RGBA_BPTC_UNORM_EXT 0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT 0x8E8F

/* GL_EXT_clip_cull_distance */

#define GL_MAX_CLIP_DISTANCES_EXT 0x0D32
#define GL_MAX_CULL_DISTANCES_EXT 0x82F9
#define GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES_EXT 0x82FA
#define GL_CLIP_DISTANCE0_EXT 0x3000
#define GL_CLIP_DISTANCE1_EXT 0x3001
#define GL_CLIP_DISTANCE2_EXT 0x3002
#define GL_CLIP_DISTANCE3_EXT 0x3003
#define GL_CLIP_DISTANCE4_EXT 0x3004
#define GL_CLIP_DISTANCE5_EXT 0x3005
#define GL_CLIP_DISTANCE6_EXT 0x3006
#define GL_CLIP_DISTANCE7_EXT 0x3007

/* GL_EXT_texture_norm16 */

#define GL_R16_EXT 0x822A
#define GL_RG16_EXT 0x822C
#define GL_RGBA16_EXT 0x805B
#define GL_RGB16_EXT 0x8054
#define GL_R16_SNORM_EXT 0x8F98
#define GL_RG16_SNORM_EXT 0x8F99
#define GL_RGB16_SNORM_EXT 0x8F9A
#define GL_RGBA16_SNORM_EXT 0x8F9B

/* GL_EXT_draw_buffers_indexed */

#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_BLEND 0x0BE2
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_SRC_ALPHA_SATURATE 0x0308

/* GL_OVR_multiview */

#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR 0x9630
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR 0x9632
#define GL_MAX_VIEWS_OVR 0x9631
#define GL_FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR 0x9633

/* GL_EXT_texture_compression_s3tc */

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

/* GL_EXT_texture_compression_s3tc_srgb */

#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT 0x8C4C
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F

/* GL_IMG_texture_compression_pvrtc */

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03

/* GL_KHR_texture_compression_astc_ldr */

#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR 0x93B0
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR 0x93B1
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR 0x93B2
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR 0x93B3
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR 0x93B4
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR 0x93B5
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR 0x93B6
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR 0x93B7
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR 0x93B8
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR 0x93B9
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR 0x93BA
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD

/* GL_KHR_blend_equation_advanced */

#define GL_MULTIPLY_KHR 0x9294
#define GL_SCREEN_KHR 0x9295
#define GL_OVERLAY_KHR 0x9296
#define GL_DARKEN_KHR 0x9297
#define GL_LIGHTEN_KHR 0x9298
#define GL_COLORDODGE_KHR 0x9299
#define GL_COLORBURN_KHR 0x929A
#define GL_HARDLIGHT_KHR 0x929B
#define GL_SOFTLIGHT_KHR 0x929C
#define GL_DIFFERENCE_KHR 0x929E
#define GL_EXCLUSION_KHR 0x92A0
#define GL_HSL_HUE_KHR 0x92AD
#define GL_HSL_SATURATION_KHR 0x92AE
#define GL_HSL_COLOR_KHR 0x92AF
#define GL_HSL_LUMINOSITY_KHR 0x92B0

/* Function prototypes */

/* GL_ANGLE_base_vertex_base_instance */

GLAPI void glDrawArraysInstancedBaseInstanceANGLE(GLenum, GLint, GLsizei, GLsizei, GLuint);
GLAPI void glDrawElementsInstancedBaseVertexBaseInstanceANGLE(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei, GLint, GLuint);
GLAPI void glMultiDrawArraysInstancedBaseInstanceANGLE(GLenum, const GLint *, const GLsizei *, const GLsizei *, const GLuint *, GLsizei);
GLAPI void glMultiDrawElementsInstancedBaseVertexBaseInstanceANGLE(GLenum, const GLsizei *, GLenum, const GLvoid *const*, const GLsizei *, const GLint *, const GLuint *, GLsizei);

/* GL_ANGLE_multi_draw */

GLAPI void glMultiDrawArraysANGLE(GLenum, const GLint *, const GLsizei *, GLsizei);
GLAPI void glMultiDrawArraysInstancedANGLE(GLenum, const GLint *, const GLsizei *, const GLsizei *, GLsizei);
GLAPI void glMultiDrawElementsANGLE(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei);
GLAPI void glMultiDrawElementsInstancedANGLE(GLenum, const GLsizei *, GLenum, const GLvoid *const*, const GLsizei*, GLsizei);

/* GL_ES_VERSION_2_0 */

GLAPI void glActiveTexture(GLenum);
GLAPI void glAttachShader(GLuint, GLuint);
GLAPI void glBindAttribLocation(GLuint, GLuint, const GLchar *);
GLAPI void glBindBuffer(GLenum, GLuint);
GLAPI void glBindFramebuffer(GLenum, GLuint);
GLAPI void glBindRenderbuffer(GLenum, GLuint);
GLAPI void glBindTexture(GLenum, GLuint);
GLAPI void glBlendColor(GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI void glBlendEquation(GLenum);
GLAPI void glBlendEquationSeparate(GLenum, GLenum);
GLAPI void glBlendFunc(GLenum, GLenum);
GLAPI void glBlendFuncSeparate(GLenum, GLenum, GLenum, GLenum);
GLAPI void glBufferData(GLenum, GLsizeiptr, const void *, GLenum);
GLAPI void glBufferSubData(GLenum, GLintptr, GLsizeiptr, const void *);
GLAPI GLenum glCheckFramebufferStatus(GLenum);
GLAPI void glClear(GLbitfield);
GLAPI void glClearColor(GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI void glClearDepthf(GLfloat);
GLAPI void glClearStencil(GLint);
GLAPI void glColorMask(GLboolean, GLboolean, GLboolean, GLboolean);
GLAPI void glCompileShader(GLuint);
GLAPI void glCompressedTexImage2D(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *);
GLAPI void glCompressedTexSubImage2D(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *);
GLAPI void glCopyTexImage2D(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint);
GLAPI void glCopyTexSubImage2D(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
GLAPI GLuint glCreateProgram(void);
GLAPI GLuint glCreateShader(GLenum);
GLAPI void glCullFace(GLenum);
GLAPI void glDeleteBuffers(GLsizei, const GLuint *);
GLAPI void glDeleteFramebuffers(GLsizei, const GLuint *);
GLAPI void glDeleteProgram(GLuint);
GLAPI void glDeleteRenderbuffers(GLsizei, const GLuint *);
GLAPI void glDeleteShader(GLuint);
GLAPI void glDeleteTextures(GLsizei, const GLuint *);
GLAPI void glDepthFunc(GLenum);
GLAPI void glDepthMask(GLboolean);
GLAPI void glDepthRangef(GLfloat, GLfloat);
GLAPI void glDetachShader(GLuint, GLuint);
GLAPI void glDisable(GLenum);
GLAPI void glDisableVertexAttribArray(GLuint);
GLAPI void glDrawArrays(GLenum, GLint, GLsizei);
GLAPI void glDrawElements(GLenum, GLsizei, GLenum, const void *);
GLAPI void glEnable(GLenum);
GLAPI void glEnableVertexAttribArray(GLuint);
GLAPI void glFinish(void);
GLAPI void glFlush(void);
GLAPI void glFramebufferRenderbuffer(GLenum, GLenum, GLenum, GLuint);
GLAPI void glFramebufferTexture2D(GLenum, GLenum, GLenum, GLuint, GLint);
GLAPI void glFrontFace(GLenum);
GLAPI void glGenBuffers(GLsizei, GLuint *);
GLAPI void glGenFramebuffers(GLsizei, GLuint *);
GLAPI void glGenRenderbuffers(GLsizei, GLuint *);
GLAPI void glGenTextures(GLsizei, GLuint *);
GLAPI void glGenerateMipmap(GLenum);
GLAPI void glGetActiveAttrib(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
GLAPI void glGetActiveUniform(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
GLAPI void glGetAttachedShaders(GLuint, GLsizei, GLsizei *, GLuint *);
GLAPI GLint glGetAttribLocation(GLuint, const GLchar *);
GLAPI void glGetBooleanv(GLenum, GLboolean *);
GLAPI void glGetBufferParameteriv(GLenum, GLenum, GLint *);
GLAPI GLenum glGetError(void);
GLAPI void glGetFloatv(GLenum, GLfloat *);
GLAPI void glGetFramebufferAttachmentParameteriv(GLenum, GLenum, GLenum, GLint *);
GLAPI void glGetIntegerv(GLenum, GLint *);
GLAPI void glGetProgramInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetProgramiv(GLuint, GLenum, GLint *);
GLAPI void glGetRenderbufferParameteriv(GLenum, GLenum, GLint *);
GLAPI void glGetShaderInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetShaderPrecisionFormat(GLenum, GLenum, GLint *, GLint *);
GLAPI void glGetShaderSource(GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetShaderiv(GLuint, GLenum, GLint *);
GLAPI const GLubyte * glGetString(GLenum);
GLAPI void glGetTexParameterfv(GLenum, GLenum, GLfloat *);
GLAPI void glGetTexParameteriv(GLenum, GLenum, GLint *);
GLAPI GLint glGetUniformLocation(GLuint, const GLchar *);
GLAPI void glGetUniformfv(GLuint, GLint, GLfloat *);
GLAPI void glGetUniformiv(GLuint, GLint, GLint *);
GLAPI void glGetVertexAttribPointerv(GLuint, GLenum, void **);
GLAPI void glGetVertexAttribfv(GLuint, GLenum, GLfloat *);
GLAPI void glGetVertexAttribiv(GLuint, GLenum, GLint *);
GLAPI void glHint(GLenum, GLenum);
GLAPI GLboolean glIsBuffer(GLuint);
GLAPI GLboolean glIsEnabled(GLenum);
GLAPI GLboolean glIsFramebuffer(GLuint);
GLAPI GLboolean glIsProgram(GLuint);
GLAPI GLboolean glIsRenderbuffer(GLuint);
GLAPI GLboolean glIsShader(GLuint);
GLAPI GLboolean glIsTexture(GLuint);
GLAPI void glLineWidth(GLfloat);
GLAPI void glLinkProgram(GLuint);
GLAPI void glPixelStorei(GLenum, GLint);
GLAPI void glPolygonOffset(GLfloat, GLfloat);
GLAPI void glReadPixels(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *);
GLAPI void glReleaseShaderCompiler(void);
GLAPI void glRenderbufferStorage(GLenum, GLenum, GLsizei, GLsizei);
GLAPI void glSampleCoverage(GLfloat, GLboolean);
GLAPI void glScissor(GLint, GLint, GLsizei, GLsizei);
GLAPI void glShaderBinary(GLsizei, const GLuint *, GLenum, const void *, GLsizei);
GLAPI void glShaderSource(GLuint, GLsizei, const GLchar *const*, const GLint *);
GLAPI void glStencilFunc(GLenum, GLint, GLuint);
GLAPI void glStencilFuncSeparate(GLenum, GLenum, GLint, GLuint);
GLAPI void glStencilMask(GLuint);
GLAPI void glStencilMaskSeparate(GLenum, GLuint);
GLAPI void glStencilOp(GLenum, GLenum, GLenum);
GLAPI void glStencilOpSeparate(GLenum, GLenum, GLenum, GLenum);
GLAPI void glTexImage2D(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
GLAPI void glTexParameterf(GLenum, GLenum, GLfloat);
GLAPI void glTexParameterfv(GLenum, GLenum, const GLfloat *);
GLAPI void glTexParameteri(GLenum, GLenum, GLint);
GLAPI void glTexParameteriv(GLenum, GLenum, const GLint *);
GLAPI void glTexSubImage2D(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *);
GLAPI void glUniform1f(GLint, GLfloat);
GLAPI void glUniform1fv(GLint, GLsizei, const GLfloat *);
GLAPI void glUniform1i(GLint, GLint);
GLAPI void glUniform1iv(GLint, GLsizei, const GLint *);
GLAPI void glUniform2f(GLint, GLfloat, GLfloat);
GLAPI void glUniform2fv(GLint, GLsizei, const GLfloat *);
GLAPI void glUniform2i(GLint, GLint, GLint);
GLAPI void glUniform2iv(GLint, GLsizei, const GLint *);
GLAPI void glUniform3f(GLint, GLfloat, GLfloat, GLfloat);
GLAPI void glUniform3fv(GLint, GLsizei, const GLfloat *);
GLAPI void glUniform3i(GLint, GLint, GLint, GLint);
GLAPI void glUniform3iv(GLint, GLsizei, const GLint *);
GLAPI void glUniform4f(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI void glUniform4fv(GLint, GLsizei, const GLfloat *);
GLAPI void glUniform4i(GLint, GLint, GLint, GLint, GLint);
GLAPI void glUniform4iv(GLint, GLsizei, const GLint *);
GLAPI void glUniformMatrix2fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUniformMatrix3fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUniformMatrix4fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUseProgram(GLuint);
GLAPI void glValidateProgram(GLuint);
GLAPI void glVertexAttrib1f(GLuint, GLfloat);
GLAPI void glVertexAttrib1fv(GLuint, const GLfloat *);
GLAPI void glVertexAttrib2f(GLuint, GLfloat, GLfloat);
GLAPI void glVertexAttrib2fv(GLuint, const GLfloat *);
GLAPI void glVertexAttrib3f(GLuint, GLfloat, GLfloat, GLfloat);
GLAPI void glVertexAttrib3fv(GLuint, const GLfloat *);
GLAPI void glVertexAttrib4f(GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI void glVertexAttrib4fv(GLuint, const GLfloat *);
GLAPI void glVertexAttribPointer(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *);
GLAPI void glViewport(GLint, GLint, GLsizei, GLsizei);

/* GL_ES_VERSION_3_0 */

GLAPI void glBeginQuery(GLenum, GLuint);
GLAPI void glBeginTransformFeedback(GLenum);
GLAPI void glBindBufferBase(GLenum, GLuint, GLuint);
GLAPI void glBindBufferRange(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr);
GLAPI void glBindSampler(GLuint, GLuint);
GLAPI void glBindTransformFeedback(GLenum, GLuint);
GLAPI void glBindVertexArray(GLuint);
GLAPI void glBlitFramebuffer(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
GLAPI void glClearBufferfi(GLenum, GLint, GLfloat, GLint);
GLAPI void glClearBufferfv(GLenum, GLint, const GLfloat *);
GLAPI void glClearBufferiv(GLenum, GLint, const GLint *);
GLAPI void glClearBufferuiv(GLenum, GLint, const GLuint *);
GLAPI GLenum glClientWaitSync(GLsync, GLbitfield, GLuint64);
GLAPI void glCompressedTexImage3D(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *);
GLAPI void glCompressedTexSubImage3D(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *);
GLAPI void glCopyBufferSubData(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr);
GLAPI void glCopyTexSubImage3D(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
GLAPI void glDeleteQueries(GLsizei, const GLuint *);
GLAPI void glDeleteSamplers(GLsizei, const GLuint *);
GLAPI void glDeleteSync(GLsync);
GLAPI void glDeleteTransformFeedbacks(GLsizei, const GLuint *);
GLAPI void glDeleteVertexArrays(GLsizei, const GLuint *);
GLAPI void glDrawArraysInstanced(GLenum, GLint, GLsizei, GLsizei);
GLAPI void glDrawBuffers(GLsizei, const GLenum *);
GLAPI void glDrawElementsInstanced(GLenum, GLsizei, GLenum, const void *, GLsizei);
GLAPI void glDrawRangeElements(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *);
GLAPI void glEndQuery(GLenum);
GLAPI void glEndTransformFeedback(void);
GLAPI GLsync glFenceSync(GLenum, GLbitfield);
GLAPI void glFlushMappedBufferRange(GLenum, GLintptr, GLsizeiptr);
GLAPI void glFramebufferTextureLayer(GLenum, GLenum, GLuint, GLint, GLint);
GLAPI void glGenQueries(GLsizei, GLuint *);
GLAPI void glGenSamplers(GLsizei, GLuint *);
GLAPI void glGenTransformFeedbacks(GLsizei, GLuint *);
GLAPI void glGenVertexArrays(GLsizei, GLuint *);
GLAPI void glGetActiveUniformBlockName(GLuint, GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetActiveUniformBlockiv(GLuint, GLuint, GLenum, GLint *);
GLAPI void glGetActiveUniformsiv(GLuint, GLsizei, const GLuint *, GLenum, GLint *);
GLAPI void glGetBufferParameteri64v(GLenum, GLenum, GLint64 *);
GLAPI void glGetBufferPointerv(GLenum, GLenum, void **);
GLAPI GLint glGetFragDataLocation(GLuint, const GLchar *);
GLAPI void glGetInteger64i_v(GLenum, GLuint, GLint64 *);
GLAPI void glGetInteger64v(GLenum, GLint64 *);
GLAPI void glGetIntegeri_v(GLenum, GLuint, GLint *);
GLAPI void glGetInternalformativ(GLenum, GLenum, GLenum, GLsizei, GLint *);
GLAPI void glGetProgramBinary(GLuint, GLsizei, GLsizei *, GLenum *, void *);
GLAPI void glGetQueryObjectuiv(GLuint, GLenum, GLuint *);
GLAPI void glGetQueryiv(GLenum, GLenum, GLint *);
GLAPI void glGetSamplerParameterfv(GLuint, GLenum, GLfloat *);
GLAPI void glGetSamplerParameteriv(GLuint, GLenum, GLint *);
GLAPI const GLubyte * glGetStringi(GLenum, GLuint);
GLAPI void glGetSynciv(GLsync, GLenum, GLsizei, GLsizei *, GLint *);
GLAPI void glGetTransformFeedbackVarying(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *);
GLAPI GLuint glGetUniformBlockIndex(GLuint, const GLchar *);
GLAPI void glGetUniformIndices(GLuint, GLsizei, const GLchar *const*, GLuint *);
GLAPI void glGetUniformuiv(GLuint, GLint, GLuint *);
GLAPI void glGetVertexAttribIiv(GLuint, GLenum, GLint *);
GLAPI void glGetVertexAttribIuiv(GLuint, GLenum, GLuint *);
GLAPI void glInvalidateFramebuffer(GLenum, GLsizei, const GLenum *);
GLAPI void glInvalidateSubFramebuffer(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei);
GLAPI GLboolean glIsQuery(GLuint);
GLAPI GLboolean glIsSampler(GLuint);
GLAPI GLboolean glIsSync(GLsync);
GLAPI GLboolean glIsTransformFeedback(GLuint);
GLAPI GLboolean glIsVertexArray(GLuint);
GLAPI void * glMapBufferRange(GLenum, GLintptr, GLsizeiptr, GLbitfield);
GLAPI void glPauseTransformFeedback(void);
GLAPI void glProgramBinary(GLuint, GLenum, const void *, GLsizei);
GLAPI void glProgramParameteri(GLuint, GLenum, GLint);
GLAPI void glReadBuffer(GLenum);
GLAPI void glRenderbufferStorageMultisample(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI void glResumeTransformFeedback(void);
GLAPI void glSamplerParameterf(GLuint, GLenum, GLfloat);
GLAPI void glSamplerParameterfv(GLuint, GLenum, const GLfloat *);
GLAPI void glSamplerParameteri(GLuint, GLenum, GLint);
GLAPI void glSamplerParameteriv(GLuint, GLenum, const GLint *);
GLAPI void glTexImage3D(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
GLAPI void glTexStorage2D(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI void glTexStorage3D(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
GLAPI void glTexSubImage3D(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *);
GLAPI void glTransformFeedbackVaryings(GLuint, GLsizei, const GLchar *const*, GLenum);
GLAPI void glUniform1ui(GLint, GLuint);
GLAPI void glUniform1uiv(GLint, GLsizei, const GLuint *);
GLAPI void glUniform2ui(GLint, GLuint, GLuint);
GLAPI void glUniform2uiv(GLint, GLsizei, const GLuint *);
GLAPI void glUniform3ui(GLint, GLuint, GLuint, GLuint);
GLAPI void glUniform3uiv(GLint, GLsizei, const GLuint *);
GLAPI void glUniform4ui(GLint, GLuint, GLuint, GLuint, GLuint);
GLAPI void glUniform4uiv(GLint, GLsizei, const GLuint *);
GLAPI void glUniformBlockBinding(GLuint, GLuint, GLuint);
GLAPI void glUniformMatrix2x3fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUniformMatrix2x4fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUniformMatrix3x2fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUniformMatrix3x4fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUniformMatrix4x2fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUniformMatrix4x3fv(GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI GLboolean glUnmapBuffer(GLenum);
GLAPI void glVertexAttribDivisor(GLuint, GLuint);
GLAPI void glVertexAttribI4i(GLuint, GLint, GLint, GLint, GLint);
GLAPI void glVertexAttribI4iv(GLuint, const GLint *);
GLAPI void glVertexAttribI4ui(GLuint, GLuint, GLuint, GLuint, GLuint);
GLAPI void glVertexAttribI4uiv(GLuint, const GLuint *);
GLAPI void glVertexAttribIPointer(GLuint, GLint, GLenum, GLsizei, const void *);
GLAPI void glWaitSync(GLsync, GLbitfield, GLuint64);

/* GL_EXT_disjoint_timer_query */

GLAPI void glBeginQueryEXT(GLenum, GLuint);
GLAPI void glDeleteQueriesEXT(GLsizei, const GLuint *);
GLAPI void glEndQueryEXT(GLenum);
GLAPI void glGenQueriesEXT(GLsizei, GLuint *);
GLAPI void glGetInteger64vEXT(GLenum, GLint64 *);
GLAPI void glGetQueryObjecti64vEXT(GLuint, GLenum, GLint64 *);
GLAPI void glGetQueryObjectivEXT(GLuint, GLenum, GLint *);
GLAPI void glGetQueryObjectui64vEXT(GLuint, GLenum, GLuint64 *);
GLAPI void glGetQueryObjectuivEXT(GLuint, GLenum, GLuint *);
GLAPI void glGetQueryivEXT(GLenum, GLenum, GLint *);
GLAPI GLboolean glIsQueryEXT(GLuint);
GLAPI void glQueryCounterEXT(GLuint, GLenum);

/* GL_EXT_draw_buffers_indexed */

GLAPI void glBlendEquationSeparateiEXT(GLuint, GLenum, GLenum);
GLAPI void glBlendEquationiEXT(GLuint, GLenum);
GLAPI void glBlendFuncSeparateiEXT(GLuint, GLenum, GLenum, GLenum, GLenum);
GLAPI void glBlendFunciEXT(GLuint, GLenum, GLenum);
GLAPI void glColorMaskiEXT(GLuint, GLboolean, GLboolean, GLboolean, GLboolean);
GLAPI void glDisableiEXT(GLenum, GLuint);
GLAPI void glEnableiEXT(GLenum, GLuint);
GLAPI GLboolean glIsEnablediEXT(GLenum, GLuint);

/* GL_OVR_multiview */

GLAPI void glFramebufferTextureMultiviewOVR(GLenum, GLenum, GLuint, GLint, GLint, GLsizei);

#ifdef __cplusplus
}
#endif

#endif
