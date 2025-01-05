#ifndef Magnum_Shaders_glShaderWrapper_h
#define Magnum_Shaders_glShaderWrapper_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "Magnum/Tags.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders { namespace Implementation {

/* A lightweight alternative to GL::Shader that holds just the type and ID,
   used by CompileState instances of all shaders. There it's used just to
   retrieve error messages in case of a compilation failures, so it doesn't
   make sense to pull in stuff needed to store shader sources such as strings
   and vectors/arrays.

   Might get revisited once GL::Shader gets the deSTLification treatment, but
   even then this class seems significantly lighter weight. */
struct MAGNUM_SHADERS_EXPORT GLShaderWrapper {
    /*implicit*/ GLShaderWrapper(NoCreateT) noexcept: type{}, id{} {}
    /*implicit*/ GLShaderWrapper(GL::Shader&& shader) noexcept;

    GLShaderWrapper(const GLShaderWrapper&) = delete;
    GLShaderWrapper(GLShaderWrapper&& other) noexcept;

    GLShaderWrapper& operator=(const GLShaderWrapper&) = delete;
    GLShaderWrapper& operator=(GLShaderWrapper&& other) noexcept;

    ~GLShaderWrapper();
    /*implicit*/ operator GL::Shader() & noexcept;
    /*implicit*/ operator GL::Shader() && noexcept;

    GLenum type;
    GLuint id;
};

}}}

#endif
