#ifndef Magnum_Shaders_Implementation_CreateCompatibilityShader_h
#define Magnum_Shaders_Implementation_CreateCompatibilityShader_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/Utility/Resource.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Shader.h"

/* Enable only when compiling Shaders library and thus work around
   "static symbol not used" warning when using this file for TextureTools */
#if defined(MAGNUM_BUILD_STATIC) && defined(MAGNUM_SHADERS_EXPORT)
static void importShaderResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumShaders_RCS)
}
#endif

namespace Magnum { namespace Shaders { namespace Implementation {

inline Shader createCompatibilityShader(const Utility::Resource& rs, Version version, Shader::Type type) {
    Shader shader(version, type);

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionDisabled<Extensions::GL::ARB::explicit_attrib_location>(version))
        shader.addSource("#define DISABLE_GL_ARB_explicit_attrib_location\n");
    if(Context::current().isExtensionDisabled<Extensions::GL::ARB::shading_language_420pack>(version))
        shader.addSource("#define DISABLE_GL_ARB_shading_language_420pack\n");
    if(Context::current().isExtensionDisabled<Extensions::GL::ARB::explicit_uniform_location>(version))
        shader.addSource("#define DISABLE_GL_ARB_explicit_uniform_location\n");
    #endif

    /* My Android emulator (running on NVidia) doesn't define GL_ES
       preprocessor macro, thus *all* the stock shaders fail to compile */
    /** @todo remove this when Android emulator is sane */
    #ifdef CORRADE_TARGET_ANDROID
    shader.addSource("#ifndef GL_ES\n#define GL_ES 1\n#endif\n");
    #endif

    shader.addSource(rs.get("compatibility.glsl"));
    return shader;
}

}}}

#endif
