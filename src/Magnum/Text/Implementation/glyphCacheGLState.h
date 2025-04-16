#ifndef Magnum_Text_Implementation_glyphCacheGLState_h
#define Magnum_Text_Implementation_glyphCacheGLState_h
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

#include "Magnum/GL/Texture.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/TextureArray.h"
#endif
#include "Magnum/Text/Implementation/abstractGlyphCacheState.h"

namespace Magnum { namespace Text {

struct GlyphCacheGL::State: AbstractGlyphCache::State {
    explicit State(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding);

    GL::Texture2D texture;
};

#ifndef MAGNUM_TARGET_GLES2
struct GlyphCacheArrayGL::State: AbstractGlyphCache::State {
    explicit State(PixelFormat format, const Vector3i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding);

    GL::Texture2DArray texture;
};
#endif

}}

#endif
