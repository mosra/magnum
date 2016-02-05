#ifndef MagnumExternal_OpenAL_extensions_h
#define MagnumExternal_OpenAL_extensions_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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

#ifdef __cplusplus
}
#endif

#endif
