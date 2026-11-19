#ifndef Magnum_Shaders_baseGL_h
#define Magnum_Shaders_baseGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/Containers.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders { namespace Implementation {

/* Used as a base for *GL::Configuration classes, stores a Containers::String
   in a way that doesn't require including it. Inherited privately, the
   extraSource() and setExtraSource() is then exposed and documented separately
   in each. */
class MAGNUM_SHADERS_EXPORT ConfigurationBaseGL {
    public:
        explicit ConfigurationBaseGL();
        ConfigurationBaseGL(const ConfigurationBaseGL& other);
        ConfigurationBaseGL(ConfigurationBaseGL&& other) noexcept;

        ~ConfigurationBaseGL();

        ConfigurationBaseGL& operator=(const ConfigurationBaseGL& other);
        ConfigurationBaseGL& operator=(ConfigurationBaseGL&& other) noexcept;

        Containers::StringView extraSource() const;
        void setExtraSource(Containers::StringView source);
        void setExtraSource(Containers::String&& source);

    private:
        std::size_t _extraSource[3];
};

/* A lightweight alternative to GL::Shader that holds just the type and ID,
   used by CompileState instances of all shaders. There it's used just to
   retrieve error messages in case of a compilation failures, so it doesn't
   make sense to pull in the whole interface including dependent includes. */
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#define _MAGNUM_SHADERS_COMPILESTATE_PUBLICIZED_APIS_ES3                    \
    using AbstractShaderProgram::bindFragmentDataLocationIndexed;           \
    using AbstractShaderProgram::bindFragmentDataLocation;
#else
#define _MAGNUM_SHADERS_COMPILESTATE_PUBLICIZED_APIS_ES3
#endif
#ifndef MAGNUM_TARGET_GLES2
#define _MAGNUM_SHADERS_COMPILESTATE_PUBLICIZED_APIS_ES3_WEBGL2             \
    using AbstractShaderProgram::setTransformFeedbackOutputs;
#else
#define _MAGNUM_SHADERS_COMPILESTATE_PUBLICIZED_APIS_ES3_WEBGL2
#endif

#define _MAGNUM_SHADERS_COMPILESTATE_PUBLICIZED_APIS()                      \
    public:                                                                 \
        using AbstractShaderProgram::bindAttributeLocation;                 \
        _MAGNUM_SHADERS_COMPILESTATE_PUBLICIZED_APIS_ES3                    \
        _MAGNUM_SHADERS_COMPILESTATE_PUBLICIZED_APIS_ES3_WEBGL2             \
        using AbstractShaderProgram::submitLink;                            \
        using AbstractShaderProgram::link;                                  \
    private:

}}}

#endif
