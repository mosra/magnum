#ifndef Magnum_Text_Script_h
#define Magnum_Text_Script_h
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

/** @file
 * @brief Enum @ref Magnum::Text::Script, function @ref Magnum::Text::script()
 * @m_since_latest
 */

#include <Corrade/Utility/Endianness.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

/**
@brief Script a text is written in
@m_since_latest

The values are [FourCC](https://en.wikipedia.org/wiki/FourCC) codes according
to [ISO 15924](https://en.wikipedia.org/wiki/ISO_15924). Use
@ref script(char, char, char, char) or @ref script(Containers::StringView) for
creating values not listed in the enum.
@see @ref AbstractShaper::setScript(), @ref AbstractShaper::script()
*/
enum class Script: UnsignedInt {
    /* List taken from https://en.wikipedia.org/wiki/ISO_15924, ordered by
       the FourCC except for the Unspecified / zero value and the special
       values which are at the top. Using the Unicode name as the enum value
       and description, not the Wikipedia language name. Entries that don't
       have a 1:1 mapping to Unicode, are not in Unicode or are not in Unicode
       yet are omitted with a comment. The names should match HarfBuzz naming
       where possible and the codes should match always, see the corresponding
       test case in the HarfBuzzFont plugin. */

    /**
     * Unspecified. When set in @ref AbstractShaper::setScript(), makes the
     * shaping rely on script autodetection implemented in a particular
     * @ref AbstractFont plugin (if any). When returned from
     * @ref AbstractShaper::script() after a successful
     * @ref AbstractShaper::shape() call, it means a particular
     * @ref AbstractFont plugin doesn't implement any script-specific behavior.
     * @see @ref Script::Inherited, @ref Script::Common, @ref Script::Unknown
     */
    Unspecified = 0,

    /**
     * Inherited. Characters that may be used with multiple scripts, and that
     * inherit their script from a preceding base character.
     * @see @ref Script::Common, @ref Script::Unknown, @ref Script::Unspecified
     */
    Inherited = Utility::Endianness::fourCC('Z', 'i', 'n', 'h'),

    /** Math symbols */
    Math = Utility::Endianness::fourCC('Z', 'm', 't', 'h'),

    /** @todo Zsye, Zsym? HB has Zmth, but not these */

    /**
     * Undetermined script
     * @see @ref Script::Unknown, @ref Script::Inherited,
     *      @ref Script::Unspecified
     */
    Common = Utility::Endianness::fourCC('Z', 'y', 'y', 'y'),

    /**
     * Unknown script
     * @see @ref Script::Common, @ref Script::Inherited,
     *      @ref Script::Unspecified
     */
    Unknown = Utility::Endianness::fourCC('Z', 'z', 'z', 'z'),

    /** [Adlam](https://en.wikipedia.org/wiki/Adlam_script), Unicode 9.0 */
    Adlam = Utility::Endianness::fourCC('A', 'd', 'l', 'm'),

    /* Afak not in Unicode yet */

    /**
     * [Caucasian Albanian](https://en.wikipedia.org/wiki/Caucasian_Albanian_script),
     * Unicode 7.0
     */
    CaucasianAlbanian = Utility::Endianness::fourCC('A', 'g', 'h', 'b'),

    /** [Ahom](https://en.wikipedia.org/wiki/Ahom_script), Unicode 8.0 */
    Ahom = Utility::Endianness::fourCC('A', 'h', 'o', 'm'),

    /** [Arabic](https://en.wikipedia.org/wiki/Arabic_script), Unicode 1.0 */
    Arabic = Utility::Endianness::fourCC('A', 'r', 'a', 'b'),

    /* Aran treated as a variant of Arab in Unicode */

    /**
     * [Imperial Aramaic](https://en.wikipedia.org/wiki/Aramaic_alphabet),
     * Unicode 5.2
     */
    ImperialAramaic = Utility::Endianness::fourCC('A', 'r', 'm', 'i'),

    /**
     * [Armenian](https://en.wikipedia.org/wiki/Armenian_alphabet), Unicode 1.0
     */
    Armenian = Utility::Endianness::fourCC('A', 'r', 'm', 'n'),

    /**
     * [Avestan](https://en.wikipedia.org/wiki/Avestan_alphabet), Unicode 5.2
     */
    Avestan = Utility::Endianness::fourCC('A', 'v', 's', 't'),

    /**
     * [Balinese](https://en.wikipedia.org/wiki/Balinese_script), Unicode 5.0
     */
    Balinese = Utility::Endianness::fourCC('B', 'a', 'l', 'i'),

    /** [Bamum](https://en.wikipedia.org/wiki/Bamum_script), Unicode 5.2 */
    Bamum = Utility::Endianness::fourCC('B', 'a', 'm', 'u'),

    /**
     * [Bassa Vah](https://en.wikipedia.org/wiki/Bassa_Vah_script), Unicode 7.0
     */
    BassaVah = Utility::Endianness::fourCC('B', 'a', 's', 's'),

    /** [Batak](https://en.wikipedia.org/wiki/Batak_script), Unicode 6.0 */
    Batak = Utility::Endianness::fourCC('B', 'a', 't', 'k'),

    /**
     * [Bengali](https://en.wikipedia.org/wiki/Bengali%E2%80%93Assamese_script),
     * Unicode 1.0
     */
    Bengali = Utility::Endianness::fourCC('B', 'e', 'n', 'g'),

    /**
     * [Bhaiksuki](https://en.wikipedia.org/wiki/Bhaiksuki_script), Unicode 9.0
     */
    Bhaiksuki = Utility::Endianness::fourCC('B', 'h', 'k', 's'),

    /* Blis not in Unicode yet */

    /** [Bopomofo](https://en.wikipedia.org/wiki/Bopomofo), Unicode 1.0 */
    Bopomofo = Utility::Endianness::fourCC('B', 'o', 'p', 'o'),

    /** [Brahmi](https://en.wikipedia.org/wiki/Brahmi_script), Unicode 6.0 */
    Brahmi = Utility::Endianness::fourCC('B', 'r', 'a', 'h'),

    /** [Braille](https://en.wikipedia.org/wiki/Braille), Unicode 3.0 */
    Braille = Utility::Endianness::fourCC('B', 'r', 'a', 'i'),

    /**
     * [Buginese](https://en.wikipedia.org/wiki/Lontara_script), Unicode 4.1
     */
    Buginese = Utility::Endianness::fourCC('B', 'u', 'g', 'i'),

    /** [Buhid](https://en.wikipedia.org/wiki/Buhid_script), Unicode 3.2 */
    Buhid = Utility::Endianness::fourCC('B', 'u', 'h', 'd'),

    /** [Chakma](https://en.wikipedia.org/wiki/Chakma_script), Unicode 6.1 */
    Chakma = Utility::Endianness::fourCC('C', 'a', 'k', 'm'),

    /**
     * [CanadianAboriginal](https://en.wikipedia.org/wiki/Canadian_Aboriginal_syllabics), Unicode 3.0
     */
    CanadianAboriginal = Utility::Endianness::fourCC('C', 'a', 'n', 's'),

    /**
     * [Carian](https://en.wikipedia.org/wiki/Carian_alphabets), Unicode 5.1
     */
    Carian = Utility::Endianness::fourCC('C', 'a', 'r', 'i'),

    /** [Cham](https://en.wikipedia.org/wiki/Cham_script), Unicode 5.1 */
    Cham = Utility::Endianness::fourCC('C', 'h', 'a', 'm'),

    /**
     * [Cherokee](https://en.wikipedia.org/wiki/Cherokee_syllabary), Unicode
     * 3.0
     */
    Cherokee = Utility::Endianness::fourCC('C', 'h', 'e', 'r'),

    /* Chis not in Unicode yet */

    /**
     * [Chorasmian](https://en.wikipedia.org/wiki/Khwarezmian_language#Writing_system),
     * Unicode 13.0
     */
    Chorasmian = Utility::Endianness::fourCC('C', 'h', 'r', 's'),

    /* Cirt not in Unicode */

    /** [Coptic](https://en.wikipedia.org/wiki/Coptic_alphabet), Unicode 1.0 */
    Coptic = Utility::Endianness::fourCC('C', 'o', 'p', 't'),

    /**
     * [Cypro Minoan](https://en.wikipedia.org/wiki/Cypro-Minoan_syllabary),
     * Unicode 14.0
     */
    CyproMinoan = Utility::Endianness::fourCC('C', 'p', 'm', 'n'),

    /**
     * [Cypriot](https://en.wikipedia.org/wiki/Cypriot_syllabary), Unicode 4.0
     */
    Cypriot = Utility::Endianness::fourCC('C', 'p', 'r', 't'),

    /**
     * [Cyrillic](https://en.wikipedia.org/wiki/Cyrillic_script), Unicode 1.0
     */
    Cyrillic = Utility::Endianness::fourCC('C', 'y', 'r', 'l'),

    /* Cyrs treated as a variant of Cyrl in Unicode */

    /** [Devanagari](https://en.wikipedia.org/wiki/Devanagari), Unicode 1.0 */
    Devanagari = Utility::Endianness::fourCC('D', 'e', 'v', 'a'),

    /**
     * [Dives Akuru](https://en.wikipedia.org/wiki/Dhives_Akuru), Unicode 13.0
     */
    DivesAkuru = Utility::Endianness::fourCC('D', 'i', 'a', 'k'),

    /**
     * [Dogra](https://en.wikipedia.org/wiki/Namem_Dogra_Akkhar), Unicode 11.0
     */
    Dogra = Utility::Endianness::fourCC('D', 'o', 'g', 'r'),

    /**
     * [Deseret](https://en.wikipedia.org/wiki/Deseret_alphabet), Unicode 3.1
     */
    Deseret = Utility::Endianness::fourCC('D', 's', 'r', 't'),

    /**
     * [Duployan](https://en.wikipedia.org/wiki/Duployan_shorthand), Unicode
     * 7.0
     */
    Duployan = Utility::Endianness::fourCC('D', 'u', 'p', 'l'),

    /* Egyd and Egyh not in Unicode */

    /**
     * [Egyptian Hieroglyphs](https://en.wikipedia.org/wiki/Egyptian_hieroglyphs),
     * Unicode 5.2
     */
    EgyptianHieroglyphs = Utility::Endianness::fourCC('E', 'g', 'y', 'p'),

    /** [Elbasan](https://en.wikipedia.org/wiki/Elbasan_script), Unicode 7.0 */
    Elbasan = Utility::Endianness::fourCC('E', 'l', 'b', 'a'),

    /** [Elymaic](https://en.wikipedia.org/wiki/Elymaic), Unicode 12.0 */
    Elymaic = Utility::Endianness::fourCC('E', 'l', 'y', 'm'),

    /**
     * [Ethiopic](https://en.wikipedia.org/wiki/Ge%CA%BDez_script), Unicode 3.0
     */
    Ethiopic = Utility::Endianness::fourCC('E', 't', 'h', 'i'),

    /** [Garay](https://en.wikipedia.org/wiki/Garay_alphabet), Unicode 16.0 */
    Garay = Utility::Endianness::fourCC('G', 'a', 'r', 'a'),

    /* Geok treated as a variant of Geor(?) in Unicode */

    /**
     * [Georgian](https://en.wikipedia.org/wiki/Georgian_scripts), Unicode 1.0
     */
    Georgian = Utility::Endianness::fourCC('G', 'e', 'o', 'r'),

    /**
     * [Glagolitic](https://en.wikipedia.org/wiki/Glagolitic_script), Unicode
     * 4.1
     */
    Glagolitic = Utility::Endianness::fourCC('G', 'l', 'a', 'g'),

    /**
     * [Gunjala Gondi](https://en.wikipedia.org/wiki/Gunjala_Gondi_script),
     * Unicode 11.0
     */
    GunjalaGondi = Utility::Endianness::fourCC('G', 'o', 'n', 'g'),

    /**
     * [Masaram Gondi](https://en.wikipedia.org/wiki/Gondi_writing#Masaram),
     * Unicode 10.0
     */
    MasaramGondi = Utility::Endianness::fourCC('G', 'o', 'n', 'm'),

    /** [Gothic](https://en.wikipedia.org/wiki/Gothic_alphabet), Unicode 3.1 */
    Gothic = Utility::Endianness::fourCC('G', 'o', 't', 'h'),

    /** [Grantha](https://en.wikipedia.org/wiki/Grantha_script), Unicode 7.0 */
    Grantha = Utility::Endianness::fourCC('G', 'r', 'a', 'n'),

    /** [Greek](https://en.wikipedia.org/wiki/Greek_alphabet), Unicode 1.0 */
    Greek = Utility::Endianness::fourCC('G', 'r', 'e', 'k'),

    /**
     * [Gujarati](https://en.wikipedia.org/wiki/Gujarati_script), Unicode 1.0
     */
    Gujarati = Utility::Endianness::fourCC('G', 'u', 'j', 'r'),

    /**
     * [Gurung Khema](https://en.wikipedia.org/wiki/Khema_script), Unicode 16.0
     */
    GurungKhema = Utility::Endianness::fourCC('G', 'u', 'k', 'h'),

    /** [Gurmukhi](https://en.wikipedia.org/wiki/Gurmukhi), Unicode 1.0 */
    Gurmukhi = Utility::Endianness::fourCC('G', 'u', 'r', 'u'),

    /* Hanb treated as a mixture of Hani and Bopo in Unicode */

    /** [Hangul](https://en.wikipedia.org/wiki/Hangul), Unicode 1.0 */
    Hangul = Utility::Endianness::fourCC('H', 'a', 'n', 'g'),

    /** [Han](https://en.wikipedia.org/wiki/Chinese_characters), Unicode 1.0 */
    Han = Utility::Endianness::fourCC('H', 'a', 'n', 'i'),

    /** [Hanunoo](https://en.wikipedia.org/wiki/Hanunoo_script), Unicode 3.2 */
    Hanunoo = Utility::Endianness::fourCC('H', 'a', 'n', 'o'),

    /* Hans, Hant treated as a subset of Hani in Unicode */

    /**
     * [Hatran](https://en.wikipedia.org/wiki/Hatran_Aramaic#Alphabet), Unicode
     * 8.0
     */
    Hatran = Utility::Endianness::fourCC('H', 'a', 't', 'r'),

    /** [Hebrew](https://en.wikipedia.org/wiki/Hebrew_alphabet), Unicode 1.0 */
    Hebrew = Utility::Endianness::fourCC('H', 'e', 'b', 'r'),

    /** [Hiragana](https://en.wikipedia.org/wiki/Hiragana), Unicode 1.0 */
    Hiragana = Utility::Endianness::fourCC('H', 'i', 'r', 'a'),

    /**
     * [Anatolian Hieroglyphs](https://en.wikipedia.org/wiki/Anatolian_hieroglyphs),
     * Unicode 8.0
     */
    AnatolianHieroglyphs = Utility::Endianness::fourCC('H', 'l', 'u', 'w'),

    /**
     * [Pahawh Hmong](https://en.wikipedia.org/wiki/Pahawh_Hmong), Unicode 7.0
     */
    PahawhHmong = Utility::Endianness::fourCC('H', 'm', 'n', 'g'),

    /**
     * [Nyiakeng Puachue Hmong](https://en.wikipedia.org/wiki/Nyiakeng_Puachue_Hmong),
     * Unicode 12.0
     */
    NyiakengPuachueHmong = Utility::Endianness::fourCC('H', 'm', 'n', 'p'),

    /* Hrkt treated as an mixture of Hira and Kana in Unicode */

    /**
     * [Old Hungarian](https://en.wikipedia.org/wiki/Old_Hungarian_script),
     * Unicode 8.0
     */
    OldHungarian = Utility::Endianness::fourCC('H', 'u', 'n', 'g'),

    /* Inds not in Unicode yet */

    /**
     * [Old Italic](https://en.wikipedia.org/wiki/Old_Italic_scripts), Unicode
     * 3.1
     */
    OldItalic = Utility::Endianness::fourCC('I', 't', 'a', 'l'),

    /* Jamo treated as a subset of Hang in Unicode */

    /**
     * [Javanese](https://en.wikipedia.org/wiki/Javanese_script), Unicode 5.2
     */
    Javanese = Utility::Endianness::fourCC('J', 'a', 'v', 'a'),

    /* Jpan treated as a mixture of Hani, Hira and Kana in Unicode;
       Jurc not in Unicode yet */

    /**
     * [Kayah Li](https://en.wikipedia.org/wiki/Kayah_Li_alphabet), Unicode 5.1
     */
    KayahLi = Utility::Endianness::fourCC('K', 'a', 'l', 'i'),

    /** [Katakana](https://en.wikipedia.org/wiki/Katakana), Unicode 1.0 */
    Katakana = Utility::Endianness::fourCC('K', 'a', 'n', 'a'),

    /** [Kawi](https://en.wikipedia.org/wiki/Kawi_script), Unicode 15.0 */
    Kawi = Utility::Endianness::fourCC('K', 'a', 'w', 'i'),

    /** [Kharoshthi](https://en.wikipedia.org/wiki/Kharosthi), Unicode 4.1 */
    Kharoshthi = Utility::Endianness::fourCC('K', 'h', 'a', 'r'),

    /** [Khmer](https://en.wikipedia.org/wiki/Khmer_script), Unicode 3.0 */
    Khmer = Utility::Endianness::fourCC('K', 'h', 'm', 'r'),

    /** [Khojki](https://en.wikipedia.org/wiki/Khojki_script), Unicode 7.0 */
    Khojki = Utility::Endianness::fourCC('K', 'h', 'o', 'j'),

    /* Kitl not in Unicode */

    /**
     * [Khitan Small Script](https://en.wikipedia.org/wiki/Khitan_small_script),
     * Unicode 13.0
     */
    KhitanSmallScript = Utility::Endianness::fourCC('K', 'i', 't', 's'),

    /** [Kannada](https://en.wikipedia.org/wiki/Kannada_script), Unicode 1.0 */
    Kannada = Utility::Endianness::fourCC('K', 'n', 'd', 'a'),

    /* Kore treated as a mixture of Hani and Hang in Unicode;
       Kpel not in Unicode yet */

    /** [Kirat Rai](https://en.wikipedia.org/wiki/Kirat_Rai), Unicode 16.0 */
    KiratRai = Utility::Endianness::fourCC('K', 'r', 'a', 'i'),

    /** [Kaithi](https://en.wikipedia.org/wiki/Kaithi), Unicode 5.2 */
    Kaithi = Utility::Endianness::fourCC('K', 't', 'h', 'i'),

    /**
     * [Tai Tham](https://en.wikipedia.org/wiki/Tai_Tham_script), Unicode 5.2
     */
    TaiTham = Utility::Endianness::fourCC('L', 'a', 'n', 'a'),

    /** [Lao](https://en.wikipedia.org/wiki/Lao_script), Unicode 1.0 */
    Lao = Utility::Endianness::fourCC('L', 'a', 'o', 'o'),

    /* Latf, Latg treated as a variant of Latn in Unicode */

    /** [Latin](https://en.wikipedia.org/wiki/Latin_script), Unicode 1.0 */
    Latin = Utility::Endianness::fourCC('L', 'a', 't', 'n'),

    /* Leke not in Unicode */

    /** [Lepcha](https://en.wikipedia.org/wiki/Lepcha_script), Unicode 5.1 */
    Lepcha = Utility::Endianness::fourCC('L', 'e', 'p', 'c'),

    /** [Limbu](https://en.wikipedia.org/wiki/Limbu_script), Unicode 4.0 */
    Limbu = Utility::Endianness::fourCC('L', 'i', 'm', 'b'),

    /** [Linear A](https://en.wikipedia.org/wiki/Linear_A), Unicode 7.0 */
    LinearA = Utility::Endianness::fourCC('L', 'i', 'n', 'a'),

    /** [Linear B](https://en.wikipedia.org/wiki/Linear_B), Unicode 4.0 */
    LinearB = Utility::Endianness::fourCC('L', 'i', 'n', 'b'),

    /** [Lisu](https://en.wikipedia.org/wiki/Fraser_script), Unicode 5.2 */
    Lisu = Utility::Endianness::fourCC('L', 'i', 's', 'u'),

    /* Loma not in Unicode yet */

    /** [Lycian](https://en.wikipedia.org/wiki/Lycian_alphabet), Unicode 5.1 */
    Lycian = Utility::Endianness::fourCC('L', 'y', 'c', 'i'),

    /** [Lydian](https://en.wikipedia.org/wiki/Lydian_alphabet), Unicode 5.1 */
    Lydian = Utility::Endianness::fourCC('L', 'y', 'd', 'i'),

    /** [Mahajani](https://en.wikipedia.org/wiki/Mahajani), Unicode 7.0 */
    Mahajani = Utility::Endianness::fourCC('M', 'a', 'h', 'j'),

    /**
     * [Makasar](https://en.wikipedia.org/wiki/Makasar_script), Unicode 11.0
     */
    Makasar = Utility::Endianness::fourCC('M', 'a', 'k', 'a'),

    /**
     * [Mandaic](https://en.wikipedia.org/wiki/Mandaic_alphabet), Unicode 6.0
     */
    Mandaic = Utility::Endianness::fourCC('M', 'a', 'n', 'd'),

    /**
     * [Manichaean](https://en.wikipedia.org/wiki/Manichaean_script), Unicode
     * 7.0
     */
    Manichaean = Utility::Endianness::fourCC('M', 'a', 'n', 'i'),

    /** [Marchen](https://en.wikipedia.org/wiki/Marchen_script), Unicode 9.0 */
    Marchen = Utility::Endianness::fourCC('M', 'a', 'r', 'c'),

    /* Maya not in Unicode */

    /**
     * [Medefaidrin](https://en.wikipedia.org/wiki/Medefaidrin), Unicode 11.0
     */
    Medefaidrin = Utility::Endianness::fourCC('M', 'e', 'd', 'f'),

    /**
     * [Mende Kikakui](https://en.wikipedia.org/wiki/Mende_Kikakui_script),
     * Unicode 7.0
     */
    MendeKikakui = Utility::Endianness::fourCC('M', 'e', 'n', 'd'),

    /**
     * [Meroitic Cursive](https://en.wikipedia.org/wiki/Meroitic_script),
     * Unicode 6.1
     */
    MeroiticCursive = Utility::Endianness::fourCC('M', 'e', 'r', 'c'),

    /**
     * [Meroitic Hieroglyphs](https://en.wikipedia.org/wiki/Meroitic_script),
     * Unicode 6.1
     */
    MeroiticHieroglyphs = Utility::Endianness::fourCC('M', 'e', 'r', 'o'),

    /**
     * [Malayalam](https://en.wikipedia.org/wiki/Malayalam_script), Unicode 1.0
     */
    Malayalam = Utility::Endianness::fourCC('M', 'l', 'y', 'm'),

    /** [Modi](https://en.wikipedia.org/wiki/Modi_script), Unicode 7.0 */
    Modi = Utility::Endianness::fourCC('M', 'o', 'd', 'i'),

    /**
     * [Mongolian](https://en.wikipedia.org/wiki/Mongolian_script), Unicode 3.0
     */
    Mongolian = Utility::Endianness::fourCC('M', 'o', 'n', 'g'),

    /* Moon not in Unicode yet */

    /**
     * [Mro](https://en.wikipedia.org/wiki/Mru_language#Script), Unicode 7.0
     */
    Mro = Utility::Endianness::fourCC('M', 'r', 'o', 'o'),

    /**
     * [Meetei Mayek](https://en.wikipedia.org/wiki/Meitei_script), Unicode 5.2
     */
    MeeteiMayek = Utility::Endianness::fourCC('M', 't', 'e', 'i'),

    /** [Multani](https://en.wikipedia.org/wiki/Multani_script), Unicode 8.0 */
    Multani = Utility::Endianness::fourCC('M', 'u', 'l', 't'),

    /**
     * [Myanmar](https://en.wikipedia.org/wiki/Mon%E2%80%93Burmese_script),
     * Unicode 3.0
     */
    Myanmar = Utility::Endianness::fourCC('M', 'y', 'm', 'r'),

    /**
     * [Nag Mundari](https://en.wikipedia.org/wiki/Mundari_Bani), Unicode 15.0
     */
    NagMundari = Utility::Endianness::fourCC('N', 'a', 'g', 'm'),

    /**
     * [Nandinagari](https://en.wikipedia.org/wiki/Nandinagari), Unicode 12.0
     */
    Nandinagari = Utility::Endianness::fourCC('N', 'a', 'n', 'd'),

    /**
     * [Old North Arabian](https://en.wikipedia.org/wiki/Ancient_North_Arabian),
     * Unicode 7.0
     */
    OldNorthArabian = Utility::Endianness::fourCC('N', 'a', 'r', 'b'),

    /**
     * [Nabataean](https://en.wikipedia.org/wiki/Nabataean_alphabet), Unicode
     * 7.0
     */
    Nabataean = Utility::Endianness::fourCC('N', 'b', 'a', 't'),

    /** [Newa](https://en.wikipedia.org/wiki/Pracalit_script), Unicode 9.0 */
    Newa = Utility::Endianness::fourCC('N', 'e', 'w', 'a'),

    /* Nkdb not in Unicode;
       Nkgb not in Unicode yet */

    /** [NKo](https://en.wikipedia.org/wiki/N%27Ko_script), Unicode 5.0 */
    NKo = Utility::Endianness::fourCC('N', 'k', 'o', 'o'),

    /** [Nushu](https://en.wikipedia.org/wiki/N%C3%BCshu), Unicode 10.0 */
    Nushu = Utility::Endianness::fourCC('N', 's', 'h', 'u'),

    /** [Ogham](https://en.wikipedia.org/wiki/Ogham), Unicode 3.0 */
    Ogham = Utility::Endianness::fourCC('O', 'g', 'a', 'm'),

    /**
     * [Ol Chiki](https://en.wikipedia.org/wiki/Ol_Chiki_script), Unicode 5.1
     */
    OlChiki = Utility::Endianness::fourCC('O', 'l', 'c', 'k'),

    /** [Ol Onal](https://en.wikipedia.org/wiki/Ol_Onal), Unicode 16.0 */
    OlOnal = Utility::Endianness::fourCC('O', 'n', 'a', 'o'),

    /**
     * [Old Turkic](https://en.wikipedia.org/wiki/Old_Turkic_script), Unicode
     * 5.2
     */
    OldTurkic = Utility::Endianness::fourCC('O', 'r', 'k', 'h'),

    /** [Oriya](https://en.wikipedia.org/wiki/Odia_script), Unicode 1.0 */
    Oriya = Utility::Endianness::fourCC('O', 'r', 'y', 'a'),

    /** [Osage](https://en.wikipedia.org/wiki/Osage_script), Unicode 9.0 */
    Osage = Utility::Endianness::fourCC('O', 's', 'g', 'e'),

    /** [Osmanya](https://en.wikipedia.org/wiki/Osmanya_script), Unicode 4.0 */
    Osmanya = Utility::Endianness::fourCC('O', 's', 'm', 'a'),

    /**
     * [Old Uyghur](https://en.wikipedia.org/wiki/Old_Uyghur_alphabet), Unicode
     * 14.0
     */
    OldUyghur = Utility::Endianness::fourCC('O', 'u', 'g', 'r'),

    /**
     * [Palmyrene](https://en.wikipedia.org/wiki/Palmyrene_alphabet), Unicode
     * 7.0
     */
    Palmyrene = Utility::Endianness::fourCC('P', 'a', 'l', 'm'),

    /**
     * [Pau Cin Hau](https://en.wikipedia.org/wiki/Pau_Cin_Hau_script), Unicode
     * 7.0
     */
    PauCinHau = Utility::Endianness::fourCC('P', 'a', 'u', 'c'),

    /* Pcun, Pelm not in Unicode */

    /**
     * [Old Permic](https://en.wikipedia.org/wiki/Old_Permic_script), Unicode
     * 7.0
     */
    OldPermic = Utility::Endianness::fourCC('P', 'e', 'r', 'm'),

    /**
     * [Phags-pa](https://en.wikipedia.org/wiki/%CA%BCPhags-pa_script), Unicode
     * 5.0
     */
    PhagsPa = Utility::Endianness::fourCC('P', 'h', 'a', 'g'),

    /**
     * [Inscriptional Pahlavi](https://en.wikipedia.org/wiki/Inscriptional_Pahlavi),
     * Unicode 5.2
     */
    InscriptionalPahlavi = Utility::Endianness::fourCC('P', 'h', 'l', 'i'),

    /**
     * [Psalter Pahlavi](https://en.wikipedia.org/wiki/Psalter_Pahlavi),
     * Unicode 7.0
     */
    PsalterPahlavi = Utility::Endianness::fourCC('P', 'h', 'l', 'p'),

    /* Phlv not in Unicode */

    /**
     * [Phoenician](https://en.wikipedia.org/wiki/Phoenician_alphabet), Unicode
     * 5.0
     */
    Phoenician = Utility::Endianness::fourCC('P', 'h', 'n', 'x'),

    /* Piqd rejected from inclusion in Unicode */

    /** [Miao](https://en.wikipedia.org/wiki/Pollard_script), Unicode 6.1 */
    Miao = Utility::Endianness::fourCC('P', 'l', 'r', 'd'),

    /**
     * [InscriptionalParthian](https://en.wikipedia.org/wiki/Inscriptional_Parthian),
     * Unicode 5.2
     */
    InscriptionalParthian = Utility::Endianness::fourCC('P', 'r', 't', 'i'),

    /* Psin, Ranj not in Unicode */

    /** [Rejang](https://en.wikipedia.org/wiki/Rejang_script), Unicode 5.1 */
    Rejang = Utility::Endianness::fourCC('R', 'j', 'n', 'g'),

    /**
     * [Hanifi Rohingya](https://en.wikipedia.org/wiki/Hanifi_Rohingya_script),
     * Unicode 11.0
     */
    HanifiRohingya = Utility::Endianness::fourCC('R', 'o', 'h', 'g'),

    /* Roro not in Unicode yet */

    /** [Runic](https://en.wikipedia.org/wiki/Runes), Unicode 3.0 */
    Runic = Utility::Endianness::fourCC('R', 'u', 'n', 'r'),

    /**
     * [Samaritan](https://en.wikipedia.org/wiki/Samaritan_script), Unicode 5.2
     */
    Samaritan = Utility::Endianness::fourCC('S', 'a', 'm', 'r'),

    /* Sara not in Unicode */

    /**
     * [Old South Arabian](https://en.wikipedia.org/wiki/Ancient_South_Arabian_script),
     * Unicode 5.2
     */
    OldSouthArabian = Utility::Endianness::fourCC('S', 'a', 'r', 'b'),

    /**
     * [Saurashtra](https://en.wikipedia.org/wiki/Saurashtra_script), Unicode
     * 5.1
     */
    Saurashtra = Utility::Endianness::fourCC('S', 'a', 'u', 'r'),

    /**
     * [SignWriting](https://en.wikipedia.org/wiki/SignWriting), Unicode 8.0
     */
    SignWriting = Utility::Endianness::fourCC('S', 'g', 'n', 'w'),

    /**
     * [Shavian](https://en.wikipedia.org/wiki/Shavian_alphabet), Unicode 4.0
     */
    Shavian = Utility::Endianness::fourCC('S', 'h', 'a', 'w'),

    /** [Sharada](https://en.wikipedia.org/wiki/Sharada_script), Unicode 6.1 */
    Sharada = Utility::Endianness::fourCC('S', 'h', 'r', 'd'),

    /* Shui not in Unicode */

    /**
     * [Siddham](https://en.wikipedia.org/wiki/Siddha%E1%B9%83_script), Unicode
     * 7.0
     */
    Siddham = Utility::Endianness::fourCC('S', 'i', 'd', 'd'),

    /* Sidt not in Unicode yet */

    /**
     * [Khudawadi](https://en.wikipedia.org/wiki/Khudabadi_script), Unicode 7.0
     */
    Khudawadi = Utility::Endianness::fourCC('S', 'i', 'n', 'd'),

    /** [Sinhala](https://en.wikipedia.org/wiki/Sinhala_script), Unicode 3.0 */
    Sinhala = Utility::Endianness::fourCC('S', 'i', 'n', 'h'),

    /**
     * [Sogdian](https://en.wikipedia.org/wiki/Sogdian_alphabet), Unicode 11.0
     */
    Sogdian = Utility::Endianness::fourCC('S', 'o', 'g', 'd'),

    /**
     * [Old Sogdian](https://en.wikipedia.org/wiki/Sogdian_alphabet), Unicode
     * 11.0
     */
    OldSogdian = Utility::Endianness::fourCC('S', 'o', 'g', 'o'),

    /**
     * [Sora Sompeng](https://en.wikipedia.org/wiki/Sorang_Sompeng_script),
     * Unicode 6.1
     */
    SoraSompeng = Utility::Endianness::fourCC('S', 'o', 'r', 'a'),

    /**
     * [Soyombo](https://en.wikipedia.org/wiki/Soyombo_script), Unicode 10.0
     */
    Soyombo = Utility::Endianness::fourCC('S', 'o', 'y', 'o'),

    /**
     * [Sundanese](https://en.wikipedia.org/wiki/Sundanese_script), Unicode 5.1
     */
    Sundanese = Utility::Endianness::fourCC('S', 'u', 'n', 'd'),

    /**
     * [Sunuwar](https://en.wikipedia.org/wiki/Sunuwar_script), Unicode 16.0
     */
    Sunuwar = Utility::Endianness::fourCC('S', 'u', 'n', 'u'),

    /**
     * [Syloti Nagri](https://en.wikipedia.org/wiki/Sylheti_Nagri), Unicode 4.1
     */
    SylotiNagri = Utility::Endianness::fourCC('S', 'y', 'l', 'o'),

    /** [Syriac](https://en.wikipedia.org/wiki/Syriac_alphabet), Unicode 3.0 */
    Syriac = Utility::Endianness::fourCC('S', 'y', 'r', 'c'),

    /* Syre, Syrj, Syrn treated as a variant of Syrc in Unicode */

    /**
     * [Tagbanwa](https://en.wikipedia.org/wiki/Tagbanwa_script), Unicode 3.2
     */
    Tagbanwa = Utility::Endianness::fourCC('T', 'a', 'g', 'b'),

    /** [Takri](https://en.wikipedia.org/wiki/Takri_script), Unicode 6.1 */
    Takri = Utility::Endianness::fourCC('T', 'a', 'k', 'r'),

    /** [Tai Le](https://en.wikipedia.org/wiki/Tai_Le_script), Unicode 4.0 */
    TaiLe = Utility::Endianness::fourCC('T', 'a', 'l', 'e'),

    /**
     * [New Tai Lue](https://en.wikipedia.org/wiki/New_Tai_Lue_alphabet),
     * Unicode 4.1
     */
    NewTaiLue = Utility::Endianness::fourCC('T', 'a', 'l', 'u'),

    /** [Tamil](https://en.wikipedia.org/wiki/Tamil_script), Unicode 1.0 */
    Tamil = Utility::Endianness::fourCC('T', 'a', 'm', 'l'),

    /** [Tangut](https://en.wikipedia.org/wiki/Tangut_script), Unicode 9.0 */
    Tangut = Utility::Endianness::fourCC('T', 'a', 'n', 'g'),

    /**
     * [Tai Viet](https://en.wikipedia.org/wiki/Tai_Viet_script), Unicode 5.2
     */
    TaiViet = Utility::Endianness::fourCC('T', 'a', 'v', 't'),

    /* Tayo not in Unicode yet */

    /** [Telugu](https://en.wikipedia.org/wiki/Telugu_script), Unicode 1.0 */
    Telugu = Utility::Endianness::fourCC('T', 'e', 'l', 'u'),

    /* Teng not in Unicode */

    /** [Tifinagh](https://en.wikipedia.org/wiki/Tifinagh), Unicode 4.1 */
    Tifinagh = Utility::Endianness::fourCC('T', 'f', 'n', 'g'),

    /** [Tagalog](https://en.wikipedia.org/wiki/Baybayin), Unicode 3.2 */
    Tagalog = Utility::Endianness::fourCC('T', 'g', 'l', 'g'),

    /** [Thaana](https://en.wikipedia.org/wiki/Thaana), Unicode 3.0 */
    Thaana = Utility::Endianness::fourCC('T', 'h', 'a', 'a'),

    /** [Thai](https://en.wikipedia.org/wiki/Thai_script), Unicode 1.0 */
    Thai = Utility::Endianness::fourCC('T', 'h', 'a', 'i'),

    /** [Tibetan](https://en.wikipedia.org/wiki/Tibetan_script), Unicode 2.0 */
    Tibetan = Utility::Endianness::fourCC('T', 'i', 'b', 't'),

    /** [Tirhuta](https://en.wikipedia.org/wiki/Tirhuta_script), Unicode 7.0 */
    Tirhuta = Utility::Endianness::fourCC('T', 'i', 'r', 'h'),

    /**
     * [Tangsa](https://en.wikipedia.org/wiki/Tangsa_language#Orthography),
     * Unicode 14.0
     */
    Tangsa = Utility::Endianness::fourCC('T', 'n', 's', 'a'),

    /**
     * [Todhri](https://en.wikipedia.org/wiki/Todhri_alphabet), Unicode 16.0
     */
    Todhri = Utility::Endianness::fourCC('T', 'o', 'd', 'r'),

    /* Tols not in Unicode yet */

    /**
     * [Toto](https://en.wikipedia.org/wiki/Toto_language#Writing_system),
     * Unicode 14.0
     */
    Toto = Utility::Endianness::fourCC('T', 'o', 't', 'o'),

    /**
     * [Tulu Tigalari](https://en.wikipedia.org/wiki/Tigalari_script), Unicode
     * 16.0
     */
    TuluTigalari = Utility::Endianness::fourCC('T', 'u', 't', 'g'),

    /**
     * [Ugaritic](https://en.wikipedia.org/wiki/Ugaritic_alphabet), Unicode 4.0
     */
    Ugaritic = Utility::Endianness::fourCC('U', 'g', 'a', 'r'),

    /** [Vai](https://en.wikipedia.org/wiki/Vai_syllabary), Unicode 5.1 */
    Vai = Utility::Endianness::fourCC('V', 'a', 'i', 'i'),

    /* Visp not in Unicode */

    /**
     * [Vithkuqi](https://en.wikipedia.org/wiki/Vithkuqi_script), Unicode 14.0
     */
    Vithkuqi = Utility::Endianness::fourCC('V', 'i', 't', 'h'),

    /**
     * [Warang Citi](https://en.wikipedia.org/wiki/Warang_Citi), Unicode 7.0
     */
    WarangCiti = Utility::Endianness::fourCC('W', 'a', 'r', 'a'),

    /** [Wancho](https://en.wikipedia.org/wiki/Wancho_script), Unicode 12.0 */
    Wancho = Utility::Endianness::fourCC('W', 'c', 'h', 'o'),

    /* Wole not in Unicode yet */

    /**
     * [Old Persian](https://en.wikipedia.org/wiki/Old_Persian_cuneiform),
     * Unicode 4.1
     */
    OldPersian = Utility::Endianness::fourCC('X', 'p', 'e', 'o'),

    /** [Cuneiform](https://en.wikipedia.org/wiki/Cuneiform), Unicode 5.0 */
    Cuneiform = Utility::Endianness::fourCC('X', 's', 'u', 'x'),

    /**
     * [Yezidi](https://en.wikipedia.org/wiki/Kurdish_alphabets#Yezidi),
     * Unicode 13.0
     */
    Yezidi = Utility::Endianness::fourCC('Y', 'e', 'z', 'i'),

    /** [Yi](https://en.wikipedia.org/wiki/Yi_script), Unicode 3.0 */
    Yi = Utility::Endianness::fourCC('Y', 'i', 'i', 'i'),

    /**
     * [Zanabazar Square](https://en.wikipedia.org/wiki/Zanabazar_square_script),
     * Unicode 10.0
     */
    ZanabazarSquare = Utility::Endianness::fourCC('Z', 'a', 'n', 'b'),
};

/** @debugoperatorenum{Script} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, Script value);

/**
@brief Create a @ref Script value from a four-character code
@m_since_latest

Delegates to @relativeref{Corrade,Utility::Endianness::fourCC()}. Doesn't
perform any validity check on the input, i.e. it's possible to create a code
that isn't listed in [ISO 15924](https://en.wikipedia.org/wiki/ISO_15924) or
even a code with non-ASCII characters.
@see @ref script(Containers::StringView)
*/
constexpr Script script(char a, char b, char c, char d) {
    return Script(Utility::Endianness::fourCC(a, b, c, d));
}

/**
@brief Create a @ref Script value from a string
@m_since_latest

Expects that the string has exactly four bytes. Other than that doesn't perform
any validity check on the input, i.e. it's possible to create a code that isn't
listed in [ISO 15924](https://en.wikipedia.org/wiki/ISO_15924) or even a code
with non-ASCII characters.
@see @ref script(char, char, char, char)
*/
MAGNUM_TEXT_EXPORT Script script(Containers::StringView fourCC);

}}

#endif
