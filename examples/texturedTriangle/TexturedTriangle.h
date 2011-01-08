#ifndef Magnum_Examples_TexturedTriangle_h
#define Magnum_Examples_TexturedTriangle_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Object.h"
#include "Mesh.h"
#include "TexturedIdentityShader.h"
#include "TGATexture.h"

namespace Magnum { namespace Examples {

class TexturedTriangle: public Object {
    public:
        TexturedTriangle(const std::string& textureFilename, Object* parent = 0);

        virtual void draw(const Matrix4& transformationMatrix, const Matrix4& projectionMatrix);

    private:
        Mesh mesh;
        TexturedIdentityShader shader;
        TGATexture texture;
};

}}

#endif
