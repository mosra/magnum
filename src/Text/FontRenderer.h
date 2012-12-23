#ifndef Magnum_Text_FontRenderer_h
#define Magnum_Text_FontRenderer_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Text::FontRenderer
 */

#include "magnumTextVisibility.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;
#endif

namespace Magnum { namespace Text {

/**
@brief Font renderer

Contains global instance of font renderer used by Font class.
*/
class MAGNUM_TEXT_EXPORT FontRenderer {
    public:
        explicit FontRenderer();

        ~FontRenderer();

        /** @brief FreeType library handle */
        inline FT_Library library() { return _library; }

    private:
        FT_Library _library;
};

}}

#endif
