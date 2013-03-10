/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "PhongShader.h"

#include <Utility/Resource.h>

#include "Extensions.h"
#include "Shader.h"

namespace Magnum { namespace Shaders {

PhongShader::PhongShader(): transformationMatrixUniform(0), projectionMatrixUniform(1), normalMatrixUniform(2), lightUniform(3), diffuseColorUniform(4), ambientColorUniform(5), specularColorUniform(6), lightColorUniform(7), shininessUniform(8) {
    Corrade::Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    Version v = Context::current()->supportedVersion({Version::GL320, Version::GL210});
    #else
    Version v = Context::current()->supportedVersion({Version::GLES300, Version::GLES200});
    #endif

    Shader vertexShader(v, Shader::Type::Vertex);
    vertexShader.addSource(rs.get("compatibility.glsl"));
    vertexShader.addSource(rs.get("PhongShader.vert"));
    attachShader(vertexShader);

    Shader fragmentShader(v, Shader::Type::Fragment);
    fragmentShader.addSource(rs.get("compatibility.glsl"));
    fragmentShader.addSource(rs.get("PhongShader.frag"));
    attachShader(fragmentShader);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>() ||
        Context::current()->version() == Version::GL210) {
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300)) {
    #endif
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(Normal::Location, "normal");
    }

    link();

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>()) {
    #else
    {
    #endif
        transformationMatrixUniform = uniformLocation("transformationMatrix");
        projectionMatrixUniform = uniformLocation("projectionMatrix");
        normalMatrixUniform = uniformLocation("normalMatrix");
        lightUniform = uniformLocation("light");
        diffuseColorUniform = uniformLocation("diffuseColor");
        ambientColorUniform = uniformLocation("ambientColor");
        specularColorUniform = uniformLocation("specularColor");
        lightColorUniform = uniformLocation("lightColor");
        shininessUniform = uniformLocation("shininess");
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setAmbientColor({});
    setSpecularColor(Vector3(1.0f));
    setLightColor(Vector3(1.0f));
    setShininess(80.0f);
    #endif
}

}}
