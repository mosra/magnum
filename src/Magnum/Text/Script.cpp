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

#include "Script.h"

#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Text/Implementation/printFourCC.h"

namespace Magnum { namespace Text {

Debug& operator<<(Debug& debug, const Script value) {
    debug << "Text::Script" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case Script::v: return debug << "::" #v;
        _c(Unspecified)

        _c(Inherited)
        _c(Math)
        _c(Common)
        _c(Unknown)

        _c(Adlam)
        _c(CaucasianAlbanian)
        _c(Ahom)
        _c(Arabic)
        _c(ImperialAramaic)
        _c(Armenian)
        _c(Avestan)
        _c(Balinese)
        _c(Bamum)
        _c(BassaVah)
        _c(Batak)
        _c(Bengali)
        _c(Bhaiksuki)
        _c(Bopomofo)
        _c(Brahmi)
        _c(Braille)
        _c(Buginese)
        _c(Buhid)
        _c(Chakma)
        _c(CanadianAboriginal)
        _c(Carian)
        _c(Cham)
        _c(Cherokee)
        _c(Chorasmian)
        _c(Coptic)
        _c(CyproMinoan)
        _c(Cypriot)
        _c(Cyrillic)
        _c(Devanagari)
        _c(DivesAkuru)
        _c(Dogra)
        _c(Deseret)
        _c(Duployan)
        _c(EgyptianHieroglyphs)
        _c(Elbasan)
        _c(Elymaic)
        _c(Ethiopic)
        _c(Garay)
        _c(Georgian)
        _c(Glagolitic)
        _c(GunjalaGondi)
        _c(MasaramGondi)
        _c(Gothic)
        _c(Grantha)
        _c(Greek)
        _c(Gujarati)
        _c(GurungKhema)
        _c(Gurmukhi)
        _c(Hangul)
        _c(Han)
        _c(Hanunoo)
        _c(Hatran)
        _c(Hebrew)
        _c(Hiragana)
        _c(AnatolianHieroglyphs)
        _c(PahawhHmong)
        _c(NyiakengPuachueHmong)
        _c(OldHungarian)
        _c(OldItalic)
        _c(Javanese)
        _c(KayahLi)
        _c(Katakana)
        _c(Kawi)
        _c(Kharoshthi)
        _c(Khmer)
        _c(Khojki)
        _c(KhitanSmallScript)
        _c(Kannada)
        _c(KiratRai)
        _c(Kaithi)
        _c(TaiTham)
        _c(Lao)
        _c(Latin)
        _c(Lepcha)
        _c(Limbu)
        _c(LinearA)
        _c(LinearB)
        _c(Lisu)
        _c(Lycian)
        _c(Lydian)
        _c(Mahajani)
        _c(Makasar)
        _c(Mandaic)
        _c(Manichaean)
        _c(Marchen)
        _c(Medefaidrin)
        _c(MendeKikakui)
        _c(MeroiticCursive)
        _c(MeroiticHieroglyphs)
        _c(Malayalam)
        _c(Modi)
        _c(Mongolian)
        _c(Mro)
        _c(MeeteiMayek)
        _c(Multani)
        _c(Myanmar)
        _c(NagMundari)
        _c(Nandinagari)
        _c(OldNorthArabian)
        _c(Nabataean)
        _c(Newa)
        _c(NKo)
        _c(Nushu)
        _c(Ogham)
        _c(OlChiki)
        _c(OlOnal)
        _c(OldTurkic)
        _c(Oriya)
        _c(Osage)
        _c(Osmanya)
        _c(OldUyghur)
        _c(Palmyrene)
        _c(PauCinHau)
        _c(OldPermic)
        _c(PhagsPa)
        _c(InscriptionalPahlavi)
        _c(PsalterPahlavi)
        _c(Phoenician)
        _c(Miao)
        _c(InscriptionalParthian)
        _c(Rejang)
        _c(HanifiRohingya)
        _c(Runic)
        _c(Samaritan)
        _c(OldSouthArabian)
        _c(Saurashtra)
        _c(SignWriting)
        _c(Shavian)
        _c(Sharada)
        _c(Siddham)
        _c(Khudawadi)
        _c(Sinhala)
        _c(Sogdian)
        _c(OldSogdian)
        _c(SoraSompeng)
        _c(Soyombo)
        _c(Sundanese)
        _c(Sunuwar)
        _c(SylotiNagri)
        _c(Syriac)
        _c(Tagbanwa)
        _c(Takri)
        _c(TaiLe)
        _c(NewTaiLue)
        _c(Tamil)
        _c(Tangut)
        _c(TaiViet)
        _c(Telugu)
        _c(Tifinagh)
        _c(Tagalog)
        _c(Thaana)
        _c(Thai)
        _c(Tibetan)
        _c(Tirhuta)
        _c(Tangsa)
        _c(Todhri)
        _c(Toto)
        _c(TuluTigalari)
        _c(Ugaritic)
        _c(Vai)
        _c(Vithkuqi)
        _c(WarangCiti)
        _c(Wancho)
        _c(OldPersian)
        _c(Cuneiform)
        _c(Yezidi)
        _c(Yi)
        _c(ZanabazarSquare)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return Implementation::printFourCC(debug, UnsignedInt(value));
}

Script script(Containers::StringView fourCC) {
    CORRADE_ASSERT(fourCC.size() == 4,
        "Text::script(): expected a four-character code, got" << fourCC, {});
    return script(fourCC[0], fourCC[1], fourCC[2], fourCC[3]);
}

}}
