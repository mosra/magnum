#ifndef _flextgl_h_
#define _flextgl_h_
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

#include "Magnum/GL/GL.h"
#include "MagnumExternal/OpenGL/KHR/khrplatform.h"

/* Defensive include guards */

#if defined(__gl2_h_)
#error Attempt to include auto-generated header after including gl2.h
#endif
#if defined(__gl2ext_h_)
#error Attempt to include auto-generated header after including gl2ext.h
#endif
#if defined(__gl2platform_h_)
#error Attempt to include auto-generated header after including gl2platform.h
#endif

#define __gl2_h_
#define __gl2ext_h_
#define __gl2platform_h_

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
typedef khronos_intptr_t GLintptr;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_int64_t GLint64;
typedef khronos_uint64_t GLuint64;

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

/* GL_ANGLE_instanced_arrays */

#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE 0x88FE

/* GL_EXT_color_buffer_half_float */

#define GL_RGBA16F_EXT 0x881A
#define GL_RGB16F_EXT 0x881B
#define GL_RG16F_EXT 0x822F
#define GL_R16F_EXT 0x822D
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT 0x8211
#define GL_UNSIGNED_NORMALIZED_EXT 0x8C17

/* GL_EXT_sRGB */

#define GL_SRGB_EXT 0x8C40
#define GL_SRGB_ALPHA_EXT 0x8C42
#define GL_SRGB8_ALPHA8_EXT 0x8C43
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT 0x8210

/* GL_EXT_blend_minmax */

#define GL_MIN_EXT 0x8007
#define GL_MAX_EXT 0x8008

/* GL_OES_texture_float */

#define GL_FLOAT 0x1406

/* GL_OES_texture_half_float */

#define GL_HALF_FLOAT_OES 0x8D61

/* GL_OES_standard_derivatives */

#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES 0x8B8B

/* GL_OES_vertex_array_object */

#define GL_VERTEX_ARRAY_BINDING_OES 0x85B5

/* GL_OES_element_index_uint */

#define GL_UNSIGNED_INT 0x1405

/* GL_ANGLE_depth_texture */

#define GL_DEPTH_COMPONENT 0x1902
#define GL_DEPTH_STENCIL_OES 0x84F9
#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_INT 0x1405
#define GL_UNSIGNED_INT_24_8_OES 0x84FA
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT32_OES 0x81A7
#define GL_DEPTH24_STENCIL8_OES 0x88F0

/* GL_EXT_draw_buffers */

#define GL_MAX_COLOR_ATTACHMENTS_EXT 0x8CDF
#define GL_MAX_DRAW_BUFFERS_EXT 0x8824
#define GL_DRAW_BUFFER0_EXT 0x8825
#define GL_DRAW_BUFFER1_EXT 0x8826
#define GL_DRAW_BUFFER2_EXT 0x8827
#define GL_DRAW_BUFFER3_EXT 0x8828
#define GL_DRAW_BUFFER4_EXT 0x8829
#define GL_DRAW_BUFFER5_EXT 0x882A
#define GL_DRAW_BUFFER6_EXT 0x882B
#define GL_DRAW_BUFFER7_EXT 0x882C
#define GL_DRAW_BUFFER8_EXT 0x882D
#define GL_DRAW_BUFFER9_EXT 0x882E
#define GL_DRAW_BUFFER10_EXT 0x882F
#define GL_DRAW_BUFFER11_EXT 0x8830
#define GL_DRAW_BUFFER12_EXT 0x8831
#define GL_DRAW_BUFFER13_EXT 0x8832
#define GL_DRAW_BUFFER14_EXT 0x8833
#define GL_DRAW_BUFFER15_EXT 0x8834
#define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT 0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT 0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT 0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT 0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT 0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT 0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT 0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT 0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT 0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT 0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT 0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT 0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT 0x8CED
#define GL_COLOR_ATTACHMENT14_EXT 0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT 0x8CEF

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

/* GL_EXT_texture_norm16 */

#define GL_R16_EXT 0x822A
#define GL_RG16_EXT 0x822C
#define GL_RGBA16_EXT 0x805B
#define GL_RGB16_EXT 0x8054
#define GL_R16_SNORM_EXT 0x8F98
#define GL_RG16_SNORM_EXT 0x8F99
#define GL_RGB16_SNORM_EXT 0x8F9A
#define GL_RGBA16_SNORM_EXT 0x8F9B

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

/* GL_ANGLE_instanced_arrays */

GLAPI void glDrawArraysInstancedANGLE(GLenum, GLint, GLsizei, GLsizei);
GLAPI void glDrawElementsInstancedANGLE(GLenum, GLsizei, GLenum, const void *, GLsizei);
GLAPI void glVertexAttribDivisorANGLE(GLuint, GLuint);

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

/* GL_EXT_draw_buffers */

GLAPI void glDrawBuffersEXT(GLsizei, const GLenum *);

/* GL_OES_vertex_array_object */

GLAPI void glBindVertexArrayOES(GLuint);
GLAPI void glDeleteVertexArraysOES(GLsizei, const GLuint *);
GLAPI void glGenVertexArraysOES(GLsizei, GLuint *);
GLAPI GLboolean glIsVertexArrayOES(GLuint);

#ifdef __cplusplus
}
#endif

#endif
