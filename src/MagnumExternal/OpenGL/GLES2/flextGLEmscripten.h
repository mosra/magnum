#ifndef _flextgl_h_
#define _flextgl_h_

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

void flextGLInit();

/* Function declaration macros */
#define APIENTRY
#ifndef GLAPI
#define GLAPI extern
#endif

/* Data types */

#include <KHR/khrplatform.h>
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef short GLshort;
typedef int GLint;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef char GLchar;
typedef struct __GLsync *GLsync;
typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef khronos_int8_t GLbyte;
typedef khronos_uint8_t GLubyte;
typedef khronos_float_t GLfloat;
typedef khronos_float_t GLclampf;
typedef khronos_int32_t GLfixed;
typedef khronos_int64_t GLint64;
typedef khronos_uint64_t GLuint64;
typedef khronos_intptr_t GLintptr;
typedef khronos_ssize_t GLsizeiptr;

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

/* GL_ANGLE_framebuffer_blit */

#define GL_READ_FRAMEBUFFER_ANGLE 0x8CA8
#define GL_DRAW_FRAMEBUFFER_ANGLE 0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_ANGLE 0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_ANGLE 0x8CAA

/* GL_ANGLE_framebuffer_multisample */

#define GL_RENDERBUFFER_SAMPLES_ANGLE 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_ANGLE 0x8D56
#define GL_MAX_SAMPLES_ANGLE 0x8D57

/* GL_ANGLE_instanced_arrays */

#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE 0x88FE

/* GL_ANGLE_depth_texture */

#define GL_DEPTH_COMPONENT 0x1902
#define GL_DEPTH_STENCIL_OES 0x84F9
#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_INT 0x1405
#define GL_UNSIGNED_INT_24_8_OES 0x84FA
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT32_OES 0x81A7
#define GL_DEPTH24_STENCIL8_OES 0x88F0

/* GL_APPLE_framebuffer_multisample */

#define GL_RENDERBUFFER_SAMPLES_APPLE 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_APPLE 0x8D56
#define GL_MAX_SAMPLES_APPLE 0x8D57
#define GL_READ_FRAMEBUFFER_APPLE 0x8CA8
#define GL_DRAW_FRAMEBUFFER_APPLE 0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_APPLE 0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_APPLE 0x8CAA

/* GL_APPLE_texture_max_level */

#define GL_TEXTURE_MAX_LEVEL_APPLE 0x813D

/* GL_EXT_texture_type_2_10_10_10_REV */

#define GL_UNSIGNED_INT_2_10_10_10_REV_EXT 0x8368

/* GL_EXT_discard_framebuffer */

#define GL_COLOR_EXT 0x1800
#define GL_DEPTH_EXT 0x1801
#define GL_STENCIL_EXT 0x1802

/* GL_EXT_blend_minmax */

#define GL_MIN_EXT 0x8007
#define GL_MAX_EXT 0x8008
#define GL_FUNC_ADD_EXT 0x8006
#define GL_BLEND_EQUATION_EXT 0x8009

/* GL_EXT_occlusion_query_boolean */

#define GL_ANY_SAMPLES_PASSED_EXT 0x8C2F
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT 0x8D6A
#define GL_CURRENT_QUERY_EXT 0x8865
#define GL_QUERY_RESULT_EXT 0x8866
#define GL_QUERY_RESULT_AVAILABLE_EXT 0x8867

/* GL_EXT_shadow_samplers */

#define GL_TEXTURE_COMPARE_MODE_EXT 0x884C
#define GL_TEXTURE_COMPARE_FUNC_EXT 0x884D
#define GL_COMPARE_REF_TO_TEXTURE_EXT 0x884E
#define GL_SAMPLER_2D_SHADOW_EXT 0x8B62

/* GL_EXT_texture_rg */

#define GL_RED_EXT 0x1903
#define GL_RG_EXT 0x8227
#define GL_R8_EXT 0x8229
#define GL_RG8_EXT 0x822B

/* GL_EXT_texture_storage */

#define GL_TEXTURE_IMMUTABLE_FORMAT_EXT 0x912F
#define GL_ALPHA8_EXT 0x803C
#define GL_LUMINANCE8_EXT 0x8040
#define GL_LUMINANCE8_ALPHA8_EXT 0x8045
#define GL_RGBA32F_EXT 0x8814
#define GL_RGB32F_EXT 0x8815
#define GL_ALPHA32F_EXT 0x8816
#define GL_LUMINANCE32F_EXT 0x8818
#define GL_LUMINANCE_ALPHA32F_EXT 0x8819
#define GL_RGBA16F_EXT 0x881A
#define GL_RGB16F_EXT 0x881B
#define GL_ALPHA16F_EXT 0x881C
#define GL_LUMINANCE16F_EXT 0x881E
#define GL_LUMINANCE_ALPHA16F_EXT 0x881F
#define GL_RGB10_A2_EXT 0x8059
#define GL_RGB10_EXT 0x8052
#define GL_BGRA8_EXT 0x93A1
#define GL_R8_EXT 0x8229
#define GL_RG8_EXT 0x822B
#define GL_R32F_EXT 0x822E
#define GL_RG32F_EXT 0x8230
#define GL_R16F_EXT 0x822D
#define GL_RG16F_EXT 0x822F

/* GL_EXT_map_buffer_range */

#define GL_MAP_READ_BIT_EXT 0x0001
#define GL_MAP_WRITE_BIT_EXT 0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT_EXT 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT_EXT 0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT_EXT 0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT_EXT 0x0020

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

/* GL_EXT_instanced_arrays */

#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_EXT 0x88FE

/* GL_NV_draw_buffers */

#define GL_MAX_DRAW_BUFFERS_NV 0x8824
#define GL_DRAW_BUFFER0_NV 0x8825
#define GL_DRAW_BUFFER1_NV 0x8826
#define GL_DRAW_BUFFER2_NV 0x8827
#define GL_DRAW_BUFFER3_NV 0x8828
#define GL_DRAW_BUFFER4_NV 0x8829
#define GL_DRAW_BUFFER5_NV 0x882A
#define GL_DRAW_BUFFER6_NV 0x882B
#define GL_DRAW_BUFFER7_NV 0x882C
#define GL_DRAW_BUFFER8_NV 0x882D
#define GL_DRAW_BUFFER9_NV 0x882E
#define GL_DRAW_BUFFER10_NV 0x882F
#define GL_DRAW_BUFFER11_NV 0x8830
#define GL_DRAW_BUFFER12_NV 0x8831
#define GL_DRAW_BUFFER13_NV 0x8832
#define GL_DRAW_BUFFER14_NV 0x8833
#define GL_DRAW_BUFFER15_NV 0x8834
#define GL_COLOR_ATTACHMENT0_NV 0x8CE0
#define GL_COLOR_ATTACHMENT1_NV 0x8CE1
#define GL_COLOR_ATTACHMENT2_NV 0x8CE2
#define GL_COLOR_ATTACHMENT3_NV 0x8CE3
#define GL_COLOR_ATTACHMENT4_NV 0x8CE4
#define GL_COLOR_ATTACHMENT5_NV 0x8CE5
#define GL_COLOR_ATTACHMENT6_NV 0x8CE6
#define GL_COLOR_ATTACHMENT7_NV 0x8CE7
#define GL_COLOR_ATTACHMENT8_NV 0x8CE8
#define GL_COLOR_ATTACHMENT9_NV 0x8CE9
#define GL_COLOR_ATTACHMENT10_NV 0x8CEA
#define GL_COLOR_ATTACHMENT11_NV 0x8CEB
#define GL_COLOR_ATTACHMENT12_NV 0x8CEC
#define GL_COLOR_ATTACHMENT13_NV 0x8CED
#define GL_COLOR_ATTACHMENT14_NV 0x8CEE
#define GL_COLOR_ATTACHMENT15_NV 0x8CEF

/* GL_NV_fbo_color_attachments */

#define GL_MAX_COLOR_ATTACHMENTS_NV 0x8CDF
#define GL_COLOR_ATTACHMENT0_NV 0x8CE0
#define GL_COLOR_ATTACHMENT1_NV 0x8CE1
#define GL_COLOR_ATTACHMENT2_NV 0x8CE2
#define GL_COLOR_ATTACHMENT3_NV 0x8CE3
#define GL_COLOR_ATTACHMENT4_NV 0x8CE4
#define GL_COLOR_ATTACHMENT5_NV 0x8CE5
#define GL_COLOR_ATTACHMENT6_NV 0x8CE6
#define GL_COLOR_ATTACHMENT7_NV 0x8CE7
#define GL_COLOR_ATTACHMENT8_NV 0x8CE8
#define GL_COLOR_ATTACHMENT9_NV 0x8CE9
#define GL_COLOR_ATTACHMENT10_NV 0x8CEA
#define GL_COLOR_ATTACHMENT11_NV 0x8CEB
#define GL_COLOR_ATTACHMENT12_NV 0x8CEC
#define GL_COLOR_ATTACHMENT13_NV 0x8CED
#define GL_COLOR_ATTACHMENT14_NV 0x8CEE
#define GL_COLOR_ATTACHMENT15_NV 0x8CEF

/* GL_NV_read_buffer */

#define GL_READ_BUFFER_NV 0x0C02

/* GL_NV_framebuffer_blit */

#define GL_READ_FRAMEBUFFER_NV 0x8CA8
#define GL_DRAW_FRAMEBUFFER_NV 0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_NV 0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_NV 0x8CAA

/* GL_NV_framebuffer_multisample */

#define GL_RENDERBUFFER_SAMPLES_NV 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_NV 0x8D56
#define GL_MAX_SAMPLES_NV 0x8D57

/* GL_NV_instanced_arrays */

#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_NV 0x88FE

/* GL_NV_shadow_samplers_array */

#define GL_SAMPLER_2D_ARRAY_SHADOW_NV 0x8DC4

/* GL_NV_shadow_samplers_cube */

#define GL_SAMPLER_CUBE_SHADOW_NV 0x8DC5

/* GL_OES_depth24 */

#define GL_DEPTH_COMPONENT24_OES 0x81A6

/* GL_OES_element_index_uint */

#define GL_UNSIGNED_INT 0x1405

/* GL_OES_rgb8_rgba8 */

#define GL_RGB8_OES 0x8051
#define GL_RGBA8_OES 0x8058

/* GL_OES_texture_3D */

#define GL_TEXTURE_WRAP_R_OES 0x8072
#define GL_TEXTURE_3D_OES 0x806F
#define GL_TEXTURE_BINDING_3D_OES 0x806A
#define GL_MAX_3D_TEXTURE_SIZE_OES 0x8073
#define GL_SAMPLER_3D_OES 0x8B5F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES 0x8CD4

/* GL_OES_texture_half_float */

#define GL_HALF_FLOAT_OES 0x8D61

/* GL_OES_texture_float */

#define GL_FLOAT 0x1406

/* GL_OES_vertex_half_float */

#define GL_HALF_FLOAT_OES 0x8D61

/* GL_OES_packed_depth_stencil */

#define GL_DEPTH_STENCIL_OES 0x84F9
#define GL_UNSIGNED_INT_24_8_OES 0x84FA
#define GL_DEPTH24_STENCIL8_OES 0x88F0

/* GL_OES_depth_texture */

#define GL_DEPTH_COMPONENT 0x1902
#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_INT 0x1405

/* GL_OES_standard_derivatives */

#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES 0x8B8B

/* GL_OES_vertex_array_object */

#define GL_VERTEX_ARRAY_BINDING_OES 0x85B5

/* GL_OES_required_internalformat */

#define GL_ALPHA8_OES 0x803C
#define GL_DEPTH_COMPONENT16_OES 0x81A5
#define GL_DEPTH_COMPONENT24_OES 0x81A6
#define GL_DEPTH24_STENCIL8_OES 0x88F0
#define GL_DEPTH_COMPONENT32_OES 0x81A7
#define GL_LUMINANCE4_ALPHA4_OES 0x8043
#define GL_LUMINANCE8_ALPHA8_OES 0x8045
#define GL_LUMINANCE8_OES 0x8040
#define GL_RGBA4_OES 0x8056
#define GL_RGB5_A1_OES 0x8057
#define GL_RGB565_OES 0x8D62
#define GL_RGB8_OES 0x8051
#define GL_RGBA8_OES 0x8058
#define GL_RGB10_EXT 0x8052
#define GL_RGB10_A2_EXT 0x8059

/* GL_OES_surfaceless_context */

#define GL_FRAMEBUFFER_UNDEFINED_OES 0x8219

/* GL_APPLE_texture_format_BGRA8888 */

#define GL_BGRA_EXT 0x80E1
#define GL_BGRA8_EXT 0x93A1

/* GL_EXT_texture_filter_anisotropic */

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

/* GL_EXT_texture_format_BGRA8888 */

#define GL_BGRA_EXT 0x80E1

/* GL_EXT_read_format_bgra */

#define GL_BGRA_EXT 0x80E1
#define GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT 0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT 0x8366

/* GL_EXT_debug_label */

#define GL_PROGRAM_PIPELINE_OBJECT_EXT 0x8A4F
#define GL_PROGRAM_OBJECT_EXT 0x8B40
#define GL_SHADER_OBJECT_EXT 0x8B48
#define GL_BUFFER_OBJECT_EXT 0x9151
#define GL_QUERY_OBJECT_EXT 0x9153
#define GL_VERTEX_ARRAY_OBJECT_EXT 0x9154
#define GL_SAMPLER 0x82E6
#define GL_TRANSFORM_FEEDBACK 0x8E22

/* GL_EXT_disjoint_timer_query */

#define GL_QUERY_COUNTER_BITS_EXT 0x8864
#define GL_CURRENT_QUERY_EXT 0x8865
#define GL_QUERY_RESULT_EXT 0x8866
#define GL_QUERY_RESULT_AVAILABLE_EXT 0x8867
#define GL_TIME_ELAPSED_EXT 0x88BF
#define GL_TIMESTAMP_EXT 0x8E28
#define GL_GPU_DISJOINT_EXT 0x8FBB

/* GL_EXT_texture_sRGB_decode */

#define GL_TEXTURE_SRGB_DECODE_EXT 0x8A48
#define GL_DECODE_EXT 0x8A49
#define GL_SKIP_DECODE_EXT 0x8A4A

/* GL_EXT_separate_shader_objects */

#define GL_ACTIVE_PROGRAM_EXT 0x8259
#define GL_VERTEX_SHADER_BIT_EXT 0x00000001
#define GL_FRAGMENT_SHADER_BIT_EXT 0x00000002
#define GL_ALL_SHADER_BITS_EXT 0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE_EXT 0x8258
#define GL_ACTIVE_PROGRAM_EXT 0x8259
#define GL_PROGRAM_PIPELINE_BINDING_EXT 0x825A

/* GL_EXT_sRGB */

#define GL_SRGB_EXT 0x8C40
#define GL_SRGB_ALPHA_EXT 0x8C42
#define GL_SRGB8_ALPHA8_EXT 0x8C43
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT 0x8210

/* GL_EXT_multisampled_render_to_texture */

#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT 0x8D6C
#define GL_RENDERBUFFER_SAMPLES_EXT 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT 0x8D56
#define GL_MAX_SAMPLES_EXT 0x8D57

/* GL_EXT_robustness */

#define GL_NO_ERROR 0
#define GL_GUILTY_CONTEXT_RESET_EXT 0x8253
#define GL_INNOCENT_CONTEXT_RESET_EXT 0x8254
#define GL_UNKNOWN_CONTEXT_RESET_EXT 0x8255
#define GL_CONTEXT_ROBUST_ACCESS_EXT 0x90F3
#define GL_RESET_NOTIFICATION_STRATEGY_EXT 0x8256
#define GL_LOSE_CONTEXT_ON_RESET_EXT 0x8252
#define GL_NO_RESET_NOTIFICATION_EXT 0x8261

/* GL_KHR_debug */

#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION 0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM 0x8245
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
#define GL_DEBUG_TYPE_POP_GROUP 0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH 0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH 0x826D
#define GL_BUFFER 0x82E0
#define GL_SHADER 0x82E1
#define GL_PROGRAM 0x82E2
#define GL_VERTEX_ARRAY 0x8074
#define GL_QUERY 0x82E3
#define GL_PROGRAM_PIPELINE 0x82E4
#define GL_SAMPLER 0x82E6
#define GL_MAX_LABEL_LENGTH 0x82E8
#define GL_MAX_DEBUG_MESSAGE_LENGTH 0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES 0x9144
#define GL_DEBUG_LOGGED_MESSAGES 0x9145
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR 0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_KHR 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION_KHR 0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM_KHR 0x8245
#define GL_DEBUG_SOURCE_API_KHR 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER_KHR 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY_KHR 0x8249
#define GL_DEBUG_SOURCE_APPLICATION_KHR 0x824A
#define GL_DEBUG_SOURCE_OTHER_KHR 0x824B
#define GL_DEBUG_TYPE_ERROR_KHR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY_KHR 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE_KHR 0x8250
#define GL_DEBUG_TYPE_OTHER_KHR 0x8251
#define GL_DEBUG_TYPE_MARKER_KHR 0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP_KHR 0x8269
#define GL_DEBUG_TYPE_POP_GROUP_KHR 0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION_KHR 0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR 0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH_KHR 0x826D
#define GL_BUFFER_KHR 0x82E0
#define GL_SHADER_KHR 0x82E1
#define GL_PROGRAM_KHR 0x82E2
#define GL_VERTEX_ARRAY_KHR 0x8074
#define GL_QUERY_KHR 0x82E3
#define GL_PROGRAM_PIPELINE_KHR 0x82E4
#define GL_SAMPLER_KHR 0x82E6
#define GL_MAX_LABEL_LENGTH_KHR 0x82E8
#define GL_MAX_DEBUG_MESSAGE_LENGTH_KHR 0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES_KHR 0x9144
#define GL_DEBUG_LOGGED_MESSAGES_KHR 0x9145
#define GL_DEBUG_SEVERITY_HIGH_KHR 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_KHR 0x9147
#define GL_DEBUG_SEVERITY_LOW_KHR 0x9148
#define GL_DEBUG_OUTPUT_KHR 0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT_KHR 0x00000002
#define GL_STACK_OVERFLOW_KHR 0x0503
#define GL_STACK_UNDERFLOW_KHR 0x0504
#define GL_DISPLAY_LIST 0x82E7

/* GL_NV_texture_border_clamp */

#define GL_TEXTURE_BORDER_COLOR_NV 0x1004
#define GL_CLAMP_TO_BORDER_NV 0x812D

/* GL_OES_depth32 */

#define GL_DEPTH_COMPONENT32_OES 0x81A7

/* GL_OES_mapbuffer */

#define GL_WRITE_ONLY_OES 0x88B9
#define GL_BUFFER_ACCESS_OES 0x88BB
#define GL_BUFFER_MAPPED_OES 0x88BC
#define GL_BUFFER_MAP_POINTER_OES 0x88BD

/* GL_OES_stencil1 */

#define GL_STENCIL_INDEX1_OES 0x8D46

/* GL_OES_stencil4 */

#define GL_STENCIL_INDEX4_OES 0x8D47

/* Function prototypes */

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
GLAPI void glGenerateMipmap(GLenum);
GLAPI void glGenFramebuffers(GLsizei, GLuint *);
GLAPI void glGenRenderbuffers(GLsizei, GLuint *);
GLAPI void glGenTextures(GLsizei, GLuint *);
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
GLAPI void glGetProgramiv(GLuint, GLenum, GLint *);
GLAPI void glGetProgramInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetRenderbufferParameteriv(GLenum, GLenum, GLint *);
GLAPI void glGetShaderiv(GLuint, GLenum, GLint *);
GLAPI void glGetShaderInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetShaderPrecisionFormat(GLenum, GLenum, GLint *, GLint *);
GLAPI void glGetShaderSource(GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI const GLubyte * glGetString(GLenum);
GLAPI void glGetTexParameterfv(GLenum, GLenum, GLfloat *);
GLAPI void glGetTexParameteriv(GLenum, GLenum, GLint *);
GLAPI void glGetUniformfv(GLuint, GLint, GLfloat *);
GLAPI void glGetUniformiv(GLuint, GLint, GLint *);
GLAPI GLint glGetUniformLocation(GLuint, const GLchar *);
GLAPI void glGetVertexAttribfv(GLuint, GLenum, GLfloat *);
GLAPI void glGetVertexAttribiv(GLuint, GLenum, GLint *);
GLAPI void glGetVertexAttribPointerv(GLuint, GLenum, void **);
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

/* GL_ANGLE_framebuffer_blit */

GLAPI void glBlitFramebufferANGLE(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);

/* GL_ANGLE_framebuffer_multisample */

GLAPI void glRenderbufferStorageMultisampleANGLE(GLenum, GLsizei, GLenum, GLsizei, GLsizei);

/* GL_ANGLE_instanced_arrays */

GLAPI void glDrawArraysInstancedANGLE(GLenum, GLint, GLsizei, GLsizei);
GLAPI void glDrawElementsInstancedANGLE(GLenum, GLsizei, GLenum, const void *, GLsizei);
GLAPI void glVertexAttribDivisorANGLE(GLuint, GLuint);

/* GL_ANGLE_depth_texture */


/* GL_APPLE_framebuffer_multisample */

GLAPI void glRenderbufferStorageMultisampleAPPLE(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI void glResolveMultisampleFramebufferAPPLE(void);

/* GL_APPLE_texture_max_level */


/* GL_ARM_rgba8 */


/* GL_EXT_texture_type_2_10_10_10_REV */


/* GL_EXT_discard_framebuffer */

GLAPI void glDiscardFramebufferEXT(GLenum, GLsizei, const GLenum *);

/* GL_EXT_blend_minmax */

GLAPI void glBlendEquationEXT(GLenum);

/* GL_EXT_shader_texture_lod */


/* GL_EXT_occlusion_query_boolean */

GLAPI void glGenQueriesEXT(GLsizei, GLuint *);
GLAPI void glDeleteQueriesEXT(GLsizei, const GLuint *);
GLAPI GLboolean glIsQueryEXT(GLuint);
GLAPI void glBeginQueryEXT(GLenum, GLuint);
GLAPI void glEndQueryEXT(GLenum);
GLAPI void glGetQueryivEXT(GLenum, GLenum, GLint *);
GLAPI void glGetQueryObjectuivEXT(GLuint, GLenum, GLuint *);

/* GL_EXT_shadow_samplers */


/* GL_EXT_texture_rg */


/* GL_EXT_texture_storage */

GLAPI void glTexStorage1DEXT(GLenum, GLsizei, GLenum, GLsizei);
GLAPI void glTexStorage2DEXT(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI void glTexStorage3DEXT(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
GLAPI void glTextureStorage1DEXT(GLuint, GLenum, GLsizei, GLenum, GLsizei);
GLAPI void glTextureStorage2DEXT(GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI void glTextureStorage3DEXT(GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);

/* GL_EXT_map_buffer_range */

GLAPI void * glMapBufferRangeEXT(GLenum, GLintptr, GLsizeiptr, GLbitfield);
GLAPI void glFlushMappedBufferRangeEXT(GLenum, GLintptr, GLsizeiptr);

/* GL_EXT_draw_buffers */

GLAPI void glDrawBuffersEXT(GLsizei, const GLenum *);

/* GL_EXT_instanced_arrays */

GLAPI void glDrawArraysInstancedEXT(GLenum, GLint, GLsizei, GLsizei);
GLAPI void glDrawElementsInstancedEXT(GLenum, GLsizei, GLenum, const void *, GLsizei);
GLAPI void glVertexAttribDivisorEXT(GLuint, GLuint);

/* GL_EXT_draw_instanced */


/* GL_NV_draw_buffers */

GLAPI void glDrawBuffersNV(GLsizei, const GLenum *);

/* GL_NV_fbo_color_attachments */


/* GL_NV_read_buffer */

GLAPI void glReadBufferNV(GLenum);

/* GL_NV_draw_instanced */

GLAPI void glDrawArraysInstancedNV(GLenum, GLint, GLsizei, GLsizei);
GLAPI void glDrawElementsInstancedNV(GLenum, GLsizei, GLenum, const void *, GLsizei);

/* GL_NV_framebuffer_blit */

GLAPI void glBlitFramebufferNV(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);

/* GL_NV_framebuffer_multisample */

GLAPI void glRenderbufferStorageMultisampleNV(GLenum, GLsizei, GLenum, GLsizei, GLsizei);

/* GL_NV_instanced_arrays */

GLAPI void glVertexAttribDivisorNV(GLuint, GLuint);

/* GL_NV_shadow_samplers_array */


/* GL_NV_shadow_samplers_cube */


/* GL_OES_depth24 */


/* GL_OES_element_index_uint */


/* GL_OES_rgb8_rgba8 */


/* GL_OES_texture_3D */

GLAPI void glTexImage3DOES(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
GLAPI void glTexSubImage3DOES(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *);
GLAPI void glCopyTexSubImage3DOES(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
GLAPI void glCompressedTexImage3DOES(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *);
GLAPI void glCompressedTexSubImage3DOES(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *);
GLAPI void glFramebufferTexture3DOES(GLenum, GLenum, GLenum, GLuint, GLint, GLint);

/* GL_OES_texture_half_float_linear */


/* GL_OES_texture_float_linear */


/* GL_OES_texture_half_float */


/* GL_OES_texture_float */


/* GL_OES_vertex_half_float */


/* GL_OES_packed_depth_stencil */


/* GL_OES_depth_texture */


/* GL_OES_standard_derivatives */


/* GL_OES_vertex_array_object */

GLAPI void glBindVertexArrayOES(GLuint);
GLAPI void glDeleteVertexArraysOES(GLsizei, const GLuint *);
GLAPI void glGenVertexArraysOES(GLsizei, GLuint *);
GLAPI GLboolean glIsVertexArrayOES(GLuint);

/* GL_OES_required_internalformat */


/* GL_OES_surfaceless_context */


/* GL_APPLE_texture_format_BGRA8888 */


/* GL_EXT_texture_filter_anisotropic */


/* GL_EXT_texture_format_BGRA8888 */


/* GL_EXT_read_format_bgra */


/* GL_EXT_multi_draw_arrays */

GLAPI void glMultiDrawArraysEXT(GLenum, const GLint *, const GLsizei *, GLsizei);
GLAPI void glMultiDrawElementsEXT(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei);

/* GL_EXT_debug_label */

GLAPI void glLabelObjectEXT(GLenum, GLuint, GLsizei, const GLchar *);
GLAPI void glGetObjectLabelEXT(GLenum, GLuint, GLsizei, GLsizei *, GLchar *);

/* GL_EXT_debug_marker */

GLAPI void glInsertEventMarkerEXT(GLsizei, const GLchar *);
GLAPI void glPushGroupMarkerEXT(GLsizei, const GLchar *);
GLAPI void glPopGroupMarkerEXT(void);

/* GL_EXT_disjoint_timer_query */

GLAPI void glQueryCounterEXT(GLuint, GLenum);
GLAPI void glGetQueryObjectivEXT(GLuint, GLenum, GLint *);
GLAPI void glGetQueryObjecti64vEXT(GLuint, GLenum, GLint64 *);
GLAPI void glGetQueryObjectui64vEXT(GLuint, GLenum, GLuint64 *);

/* GL_EXT_texture_sRGB_decode */


/* GL_EXT_separate_shader_objects */

GLAPI void glUseShaderProgramEXT(GLenum, GLuint);
GLAPI void glActiveProgramEXT(GLuint);
GLAPI GLuint glCreateShaderProgramEXT(GLenum, const GLchar *);
GLAPI void glActiveShaderProgramEXT(GLuint, GLuint);
GLAPI void glBindProgramPipelineEXT(GLuint);
GLAPI GLuint glCreateShaderProgramvEXT(GLenum, GLsizei, const GLchar **);
GLAPI void glDeleteProgramPipelinesEXT(GLsizei, const GLuint *);
GLAPI void glGenProgramPipelinesEXT(GLsizei, GLuint *);
GLAPI void glGetProgramPipelineInfoLogEXT(GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetProgramPipelineivEXT(GLuint, GLenum, GLint *);
GLAPI GLboolean glIsProgramPipelineEXT(GLuint);
GLAPI void glProgramParameteriEXT(GLuint, GLenum, GLint);
GLAPI void glProgramUniform1fEXT(GLuint, GLint, GLfloat);
GLAPI void glProgramUniform1fvEXT(GLuint, GLint, GLsizei, const GLfloat *);
GLAPI void glProgramUniform1iEXT(GLuint, GLint, GLint);
GLAPI void glProgramUniform1ivEXT(GLuint, GLint, GLsizei, const GLint *);
GLAPI void glProgramUniform2fEXT(GLuint, GLint, GLfloat, GLfloat);
GLAPI void glProgramUniform2fvEXT(GLuint, GLint, GLsizei, const GLfloat *);
GLAPI void glProgramUniform2iEXT(GLuint, GLint, GLint, GLint);
GLAPI void glProgramUniform2ivEXT(GLuint, GLint, GLsizei, const GLint *);
GLAPI void glProgramUniform3fEXT(GLuint, GLint, GLfloat, GLfloat, GLfloat);
GLAPI void glProgramUniform3fvEXT(GLuint, GLint, GLsizei, const GLfloat *);
GLAPI void glProgramUniform3iEXT(GLuint, GLint, GLint, GLint, GLint);
GLAPI void glProgramUniform3ivEXT(GLuint, GLint, GLsizei, const GLint *);
GLAPI void glProgramUniform4fEXT(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI void glProgramUniform4fvEXT(GLuint, GLint, GLsizei, const GLfloat *);
GLAPI void glProgramUniform4iEXT(GLuint, GLint, GLint, GLint, GLint, GLint);
GLAPI void glProgramUniform4ivEXT(GLuint, GLint, GLsizei, const GLint *);
GLAPI void glProgramUniformMatrix2fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glProgramUniformMatrix3fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glProgramUniformMatrix4fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glUseProgramStagesEXT(GLuint, GLbitfield, GLuint);
GLAPI void glValidateProgramPipelineEXT(GLuint);
GLAPI void glProgramUniform1uiEXT(GLuint, GLint, GLuint);
GLAPI void glProgramUniform2uiEXT(GLuint, GLint, GLuint, GLuint);
GLAPI void glProgramUniform3uiEXT(GLuint, GLint, GLuint, GLuint, GLuint);
GLAPI void glProgramUniform4uiEXT(GLuint, GLint, GLuint, GLuint, GLuint, GLuint);
GLAPI void glProgramUniform1uivEXT(GLuint, GLint, GLsizei, const GLuint *);
GLAPI void glProgramUniform2uivEXT(GLuint, GLint, GLsizei, const GLuint *);
GLAPI void glProgramUniform3uivEXT(GLuint, GLint, GLsizei, const GLuint *);
GLAPI void glProgramUniform4uivEXT(GLuint, GLint, GLsizei, const GLuint *);
GLAPI void glProgramUniformMatrix2x3fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glProgramUniformMatrix3x2fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glProgramUniformMatrix2x4fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glProgramUniformMatrix4x2fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glProgramUniformMatrix3x4fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);
GLAPI void glProgramUniformMatrix4x3fvEXT(GLuint, GLint, GLsizei, GLboolean, const GLfloat *);

/* GL_EXT_sRGB */


/* GL_EXT_multisampled_render_to_texture */

GLAPI void glRenderbufferStorageMultisampleEXT(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI void glFramebufferTexture2DMultisampleEXT(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei);

/* GL_EXT_robustness */

GLAPI GLenum glGetGraphicsResetStatusEXT(void);
GLAPI void glReadnPixelsEXT(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *);
GLAPI void glGetnUniformfvEXT(GLuint, GLint, GLsizei, GLfloat *);
GLAPI void glGetnUniformivEXT(GLuint, GLint, GLsizei, GLint *);

/* GL_KHR_debug */

GLAPI void glDebugMessageControl(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean);
GLAPI void glDebugMessageInsert(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *);
GLAPI void glDebugMessageCallback(GLDEBUGPROC, const void *);
GLAPI GLuint glGetDebugMessageLog(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *);
GLAPI void glPushDebugGroup(GLenum, GLuint, GLsizei, const GLchar *);
GLAPI void glPopDebugGroup(void);
GLAPI void glObjectLabel(GLenum, GLuint, GLsizei, const GLchar *);
GLAPI void glGetObjectLabel(GLenum, GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glObjectPtrLabel(const void *, GLsizei, const GLchar *);
GLAPI void glGetObjectPtrLabel(const void *, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetPointerv(GLenum, void **);
GLAPI void glDebugMessageControlKHR(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean);
GLAPI void glDebugMessageInsertKHR(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *);
GLAPI void glDebugMessageCallbackKHR(GLDEBUGPROCKHR, const void *);
GLAPI GLuint glGetDebugMessageLogKHR(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *);
GLAPI void glPushDebugGroupKHR(GLenum, GLuint, GLsizei, const GLchar *);
GLAPI void glPopDebugGroupKHR(void);
GLAPI void glObjectLabelKHR(GLenum, GLuint, GLsizei, const GLchar *);
GLAPI void glGetObjectLabelKHR(GLenum, GLuint, GLsizei, GLsizei *, GLchar *);
GLAPI void glObjectPtrLabelKHR(const void *, GLsizei, const GLchar *);
GLAPI void glGetObjectPtrLabelKHR(const void *, GLsizei, GLsizei *, GLchar *);
GLAPI void glGetPointervKHR(GLenum, void **);

/* GL_NV_read_buffer_front */


/* GL_NV_read_depth */


/* GL_NV_read_stencil */


/* GL_NV_read_depth_stencil */


/* GL_NV_texture_border_clamp */


/* GL_OES_depth32 */


/* GL_OES_mapbuffer */

GLAPI void * glMapBufferOES(GLenum, GLenum);
GLAPI GLboolean glUnmapBufferOES(GLenum);
GLAPI void glGetBufferPointervOES(GLenum, GLenum, void **);

/* GL_OES_stencil1 */


/* GL_OES_stencil4 */


#ifdef __cplusplus
}
#endif

#endif
