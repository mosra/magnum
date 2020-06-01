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

#if (!defined(GL_ES) && __VERSION__ >= 130) || (defined(GL_ES) && __VERSION__ >= 300)
    #define NEW_GLSL
#endif

#if !defined(GL_ES) && defined(GL_ARB_explicit_attrib_location) && !defined(DISABLE_GL_ARB_explicit_attrib_location)
    #extension GL_ARB_explicit_attrib_location: enable
    #define EXPLICIT_ATTRIB_LOCATION
#endif

#if !defined(GL_ES) && defined(GL_ARB_shading_language_420pack) && !defined(DISABLE_GL_ARB_shading_language_420pack)
    #extension GL_ARB_shading_language_420pack: enable
    #define RUNTIME_CONST
    #define EXPLICIT_TEXTURE_LAYER
#endif

#if !defined(GL_ES) && defined(GL_ARB_explicit_uniform_location) && !defined(DISABLE_GL_ARB_explicit_uniform_location)
    #extension GL_ARB_explicit_uniform_location: enable
    #define EXPLICIT_UNIFORM_LOCATION
#endif

#if defined(GL_ES) && __VERSION__ >= 300
    #define EXPLICIT_ATTRIB_LOCATION
    /* EXPLICIT_TEXTURE_LAYER, EXPLICIT_UNIFORM_LOCATION and RUNTIME_CONST is not
       available in OpenGL ES */
#endif

/* Precision qualifiers are not supported in GLSL 1.20 */
#if !defined(GL_ES) && __VERSION__ == 120
    #define highp
    #define mediump
    #define lowp
#endif
