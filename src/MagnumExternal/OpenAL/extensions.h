#ifndef MagnumExternal_OpenAL_extensions_h
#define MagnumExternal_OpenAL_extensions_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Guillaume Jacquemin <williamjcm@users.noreply.github.com>

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

#include <al.h>
#include <alc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* For compatibility with Emscripten that does not define this macro */
#ifndef AL_APIENTRY
#define AL_APIENTRY
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT /* So the macro names are preserved in the docs */

/* AL_EXT_float32 */
#ifndef AL_EXT_float32
#define AL_EXT_float32 1
#define AL_FORMAT_MONO_FLOAT32                   0x10010
#define AL_FORMAT_STEREO_FLOAT32                 0x10011
#endif

/* AL_EXT_double */
#ifndef AL_EXT_double
#define AL_EXT_double 1
#define AL_FORMAT_MONO_DOUBLE_EXT                0x10012
#define AL_FORMAT_STEREO_DOUBLE_EXT              0x10013
#endif

/* AL_EXT_MULAW */
#ifndef AL_EXT_MULAW
#define AL_EXT_MULAW 1
#define AL_FORMAT_MONO_MULAW_EXT                 0x10014
#define AL_FORMAT_STEREO_MULAW_EXT               0x10015
#endif

/* AL_EXT_ALAW */
#ifndef AL_EXT_ALAW
#define AL_EXT_ALAW 1
#define AL_FORMAT_MONO_ALAW_EXT                  0x10016
#define AL_FORMAT_STEREO_ALAW_EXT                0x10017
#endif

/* AL_EXT_MCFORMATS */
#ifndef AL_EXT_MCFORMATS
#define AL_EXT_MCFORMATS 1
#define AL_FORMAT_QUAD8                          0x1204
#define AL_FORMAT_QUAD16                         0x1205
#define AL_FORMAT_QUAD32                         0x1206
#define AL_FORMAT_REAR8                          0x1207
#define AL_FORMAT_REAR16                         0x1208
#define AL_FORMAT_REAR32                         0x1209
#define AL_FORMAT_51CHN8                         0x120A
#define AL_FORMAT_51CHN16                        0x120B
#define AL_FORMAT_51CHN32                        0x120C
#define AL_FORMAT_61CHN8                         0x120D
#define AL_FORMAT_61CHN16                        0x120E
#define AL_FORMAT_61CHN32                        0x120F
#define AL_FORMAT_71CHN8                         0x1210
#define AL_FORMAT_71CHN16                        0x1211
#define AL_FORMAT_71CHN32                        0x1212
#endif

/* AL_SOFT_loop_points */
#ifndef AL_SOFT_loop_points
#define AL_SOFT_loop_points 1
#define AL_LOOP_POINTS_SOFT                      0x2015
#endif

/* ALC_SOFTX_HRTF */
#ifndef ALC_SOFTX_HRTF
#define ALC_SOFTX_HRTF 1
#define ALC_HRTF_SOFT                            0x1992
#endif

/* ALC_SOFT_HRTF */
#ifndef ALC_SOFT_HRTF
#define ALC_SOFT_HRTF 1
#define ALC_HRTF_SOFT                            0x1992

#define ALC_HRTF_ID_SOFT                         0x1996

#define ALC_DONT_CARE_SOFT                       0x0002

#define ALC_HRTF_STATUS_SOFT                     0x1993
#define ALC_NUM_HRTF_SPECIFIER_SOFT              0x1994
#define ALC_HRTF_SPECIFIER_SOFT                  0x1995

#define ALC_HRTF_DISABLED_SOFT                   0x0000
#define ALC_HRTF_ENABLED_SOFT                    0x0001
#define ALC_HRTF_DENIED_SOFT                     0x0002
#define ALC_HRTF_REQUIRED_SOFT                   0x0003
#define ALC_HRTF_HEADPHONES_DETECTED_SOFT        0x0004
#define ALC_HRTF_UNSUPPORTED_FORMAT_SOFT         0x0005

typedef ALCchar* (AL_APIENTRY*LPALGETSTRINGISOFT)(ALCdevice*,ALCenum,ALCsizei);
typedef ALCboolean (AL_APIENTRY*LPALRESETDEVICESOFT)(ALCdevice*,const ALCint*);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
