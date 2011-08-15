#ifndef Magnum_Examples_TexturedIdentityShader_h
#define Magnum_Examples_TexturedIdentityShader_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "AbstractShaderProgram.h"

namespace Magnum { namespace Examples {

class TexturedIdentityShader: public AbstractShaderProgram {
    public:
        enum Attributes {
            Vertex = 1,
            TextureCoordinates = 2
        };

        TexturedIdentityShader();

        inline void setTextureUniform(GLuint level) {
            setUniform(textureUniform, level);
        }

    private:
        GLint textureUniform;
};

}}

#endif
