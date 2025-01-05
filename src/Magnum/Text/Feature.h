#ifndef Magnum_Text_Feature_h
#define Magnum_Text_Feature_h
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
 * @brief Class @ref Magnum::Text::FeatureRange, enum @ref Magnum::Text::Feature, function @ref Magnum::Text::feature()
 * @m_since_latest
 */

#include <Corrade/Utility/Endianness.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

/**
@brief OpenType feature for a text range
@m_since_latest

@see @ref AbstractShaper::shape()
*/
class FeatureRange {
    public:
        /**
         * @brief Constructor
         * @param feature   Feature to control
         * @param begin     Beginning byte in the input text
         * @param end       (One byte after) the end byte in the input text
         * @param value     Feature value to set
         */
        constexpr /*implicit*/ FeatureRange(Feature feature, UnsignedInt begin, UnsignedInt end, UnsignedInt value = true): _feature{feature}, _value{value}, _begin{begin}, _end{end} {}

        /**
         * @brief Construct for the whole text
         *
         * Equivalent to calling @ref FeatureRange(Feature, UnsignedInt, UnsignedInt, UnsignedInt)
         * with @p begin set to @cpp 0 @ce and @p end to @cpp 0xffffffffu @ce.
         */
        constexpr /*implicit*/ FeatureRange(Feature feature, UnsignedInt value = true): _feature{feature}, _value{value}, _begin{0}, _end{~UnsignedInt{}} {}

        /** @brief Feature to control */
        constexpr Feature feature() const { return _feature; }

        /**
         * @brief Whether to enable the feature
         *
         * Returns @cpp false @ce if @ref value() is @cpp 0 @ce, @cpp true @ce
         * otherwise.
         */
        constexpr bool isEnabled() const { return _value; }

        /** @brief Feature value to set */
        constexpr UnsignedInt value() const { return _value; }

        /**
         * @brief Beginning byte in the input text
         *
         * If the feature is set for the whole text, this is @cpp 0 @ce.
         */
        constexpr UnsignedInt begin() const { return _begin; }

        /**
         * @brief (One byte after) the end byte in the input text
         *
         * If the feature is set for the whole text, this is
         * @cpp 0xffffffffu @ce.
         */
        constexpr UnsignedInt end() const { return _end; }

    private:
        friend AbstractShaper;

        Feature _feature;
        UnsignedInt _value;
        UnsignedInt _begin;
        UnsignedInt _end;
};

/**
@brief OpenType typographic feature
@m_since_latest

The values are [FourCC](https://en.wikipedia.org/wiki/FourCC) codes according
to the [OpenType feature registry](https://learn.microsoft.com/typography/opentype/spec/featurelist).
See also the [List of typographic features](https://en.wikipedia.org/wiki/List_of_typographic_features#OpenType_typographic_features)
on Wikipedia for the values grouped by use case. Use
@ref feature(char, char, char, char) or @ref feature(Containers::StringView)
for creating values not listed in the enum.

Currently, there's no corresponding feature list for [Apple Advanced Typography](https://en.wikipedia.org/wiki/Apple_Advanced_Typography).
Mapping from OpenType features to AAT features is possible but nontrivial, and
is the responsibility of a particular font plugin.
@see @ref FeatureRange, @ref AbstractShaper::shape()
*/
enum class Feature: UnsignedInt {
    /**
     * `aalt`, [Access All Alternates](https://learn.microsoft.com/typography/opentype/spec/features_ae#aalt).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{aalt}
     */
    AccessAllAlternates = Utility::Endianness::fourCC('a', 'a', 'l', 't'),

    /**
     * `abvf`, [Above-base Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#abvf).
     * Intended for South-Asian scripts.
     * @m_keywords{abvf}
     */
    AboveBaseForms = Utility::Endianness::fourCC('a', 'b', 'v', 'f'),

    /**
     * `abvm`, [Above-base Mark Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ae#abvm).
     * Intended for South-Asian scripts.
     * @m_keywords{abvm}
     */
    AboveBaseMarkPositioning = Utility::Endianness::fourCC('a', 'b', 'v', 'm'),

    /**
     * `abvs`, [Above-base Substitutions](https://learn.microsoft.com/typography/opentype/spec/features_ae#abvs).
     * Intended for South-Asian scripts.
     * @m_keywords{abvs}
     */
    AboveBaseSubstitutions = Utility::Endianness::fourCC('a', 'b', 'v', 's'),

    /**
     * `afrc`, [Alternative Fractions](https://learn.microsoft.com/typography/opentype/spec/features_ae#afrc).
     * Intended for digits and math.
     * @m_keywords{afrc}
     */
    AlternativeFractions = Utility::Endianness::fourCC('a', 'f', 'r', 'c'),

    /**
     * `akhn`, [Akhand](https://learn.microsoft.com/typography/opentype/spec/features_ae#akhn).
     * Intended for South-Asian scripts.
     * @m_keywords{akhn}
     */
    Akhand = Utility::Endianness::fourCC('a', 'k', 'h', 'n'),

    /**
     * `apkn`, [Kerning for Alternate Proportional Widths](https://learn.microsoft.com/typography/opentype/spec/features_ae#apkn).
     * Intended for East-Asian scripts.
     * @m_keywords{apkn}
     */
    KerningForAlternateProportionalWidths = Utility::Endianness::fourCC('a', 'p', 'k', 'n'),

    /**
     * `blwf`, [Below-base Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#blwf).
     * Intended for South-Asian scripts.
     * @m_keywords{blwf}
     */
    BelowBaseForms = Utility::Endianness::fourCC('b', 'l', 'w', 'f'),

    /**
     * `blwm`, [Below-base Mark Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ae#blwm).
     * Intended for South-Asian scripts.
     * @m_keywords{blwm}
    */
    BelowBaseMarkPositioning = Utility::Endianness::fourCC('b', 'l', 'w', 'm'),

    /**
     * `blws`, [Below-base Substitutions](https://learn.microsoft.com/typography/opentype/spec/features_ae#blws).
     * Intended for South-Asian scripts.
     * @m_keywords{blws}
     */
    BelowBaseSubstitutions = Utility::Endianness::fourCC('b', 'l', 'w', 's'),

    /**
     * `calt`, [Contextual Alternates](https://learn.microsoft.com/typography/opentype/spec/features_ae#calt).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{calt}
     */
    ContextualAlternates = Utility::Endianness::fourCC('c', 'a', 'l', 't'),

    /**
     * `case`, [Case-Sensitive Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#case).
     * Intended for cased scripts.
     * @m_keywords{case}
     */
    CaseSensitiveForms = Utility::Endianness::fourCC('c', 'a', 's', 'e'),

    /**
     * `ccmp`, [Glyph Composition / Decomposition](https://learn.microsoft.com/typography/opentype/spec/features_ae#ccmp).
     * Positioning feature intended for all scripts.
     * @m_keywords{ccmp}
     */
    GlyphCompositionDecomposition = Utility::Endianness::fourCC('c', 'c', 'm', 'p'),

    /**
     * `cfar`, [Conjunct Form After Ro](https://learn.microsoft.com/typography/opentype/spec/features_ae#cfar).
     * Intended for South-Asian scripts.
     * @m_keywords{cfar}
     */
    ConjunctFormAfterRo = Utility::Endianness::fourCC('c', 'f', 'a', 'r'),

    /**
     * `chws`, [Contextual Half-width Spacing](https://learn.microsoft.com/typography/opentype/spec/features_ae#chws).
     * Intended for East-Asian scripts.
     * @m_keywords{chws}
     */
    ContextualHalfWidthSpacing = Utility::Endianness::fourCC('c', 'h', 'w', 's'),

    /**
     * `cjct`, [Conjunct Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#cjct).
     * Intended for South-Asian scripts.
     * @m_keywords{cjct}
     */
    ConjunctForms = Utility::Endianness::fourCC('c', 'j', 'c', 't'),

    /**
     * `clig`, [Contextual Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_ae#clig).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{clig}
     * @see @ref Feature::StandardLigatures,
     *      @relativeref{Feature,DiscretionaryLigatures},
     *      @relativeref{Feature,HistoricalLigatures}
     */
    ContextualLigatures = Utility::Endianness::fourCC('c', 'l', 'i', 'g'),

    /**
     * `cpct`, [Centered CJK Punctuation](https://learn.microsoft.com/typography/opentype/spec/features_ae#cpct).
     * Intended for East-Asian scripts.
     * @m_keywords{cpct}
     */
    CenteredCjkPunctuation = Utility::Endianness::fourCC('c', 'p', 'c', 't'),

    /**
     * `cpsp`, [Capital Spacing](https://learn.microsoft.com/typography/opentype/spec/features_ae#cpsp).
     * Intended for cased scripts.
     * @m_keywords{cpsp}
     */
    CapitalSpacing = Utility::Endianness::fourCC('c', 'p', 's', 'p'),

    /**
     * `cswh`, [Contextual Swash](https://learn.microsoft.com/typography/opentype/spec/features_ae#cswh).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cswh}
     */
    ContextualSwash = Utility::Endianness::fourCC('c', 's', 'w', 'h'),

    /**
     * `curs`, [Cursive Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ae#curs).
     * Intended for West-Asian scripts.
     * @m_keywords{curs}
     */
    CursivePositioning = Utility::Endianness::fourCC('c', 'u', 'r', 's'),

    /**
     * `cv01`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv01}
     */
    CharacterVariants1 = Utility::Endianness::fourCC('c', 'v', '0', '1'),

    /**
     * `cv02`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv02}
     */
    CharacterVariants2 = Utility::Endianness::fourCC('c', 'v', '0', '2'),

    /**
     * `cv03`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv03}
     */
    CharacterVariants3 = Utility::Endianness::fourCC('c', 'v', '0', '3'),

    /**
     * `cv04`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv04}
     */
    CharacterVariants4 = Utility::Endianness::fourCC('c', 'v', '0', '4'),

    /**
     * `cv05`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv05}
     */
    CharacterVariants5 = Utility::Endianness::fourCC('c', 'v', '0', '5'),

    /**
     * `cv06`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv06}
     */
    CharacterVariants6 = Utility::Endianness::fourCC('c', 'v', '0', '6'),

    /**
     * `cv07`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv07}
     */
    CharacterVariants7 = Utility::Endianness::fourCC('c', 'v', '0', '7'),

    /**
     * `cv08`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv08}
     */
    CharacterVariants8 = Utility::Endianness::fourCC('c', 'v', '0', '8'),

    /**
     * `cv09`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv09}
     */
    CharacterVariants9 = Utility::Endianness::fourCC('c', 'v', '0', '9'),

    /**
     * `cv10`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv10}
     */
    CharacterVariants10 = Utility::Endianness::fourCC('c', 'v', '1', '0'),

    /**
     * `cv11`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv11}
     */
    CharacterVariants11 = Utility::Endianness::fourCC('c', 'v', '1', '1'),

    /**
     * `cv12`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv12}
     */
    CharacterVariants12 = Utility::Endianness::fourCC('c', 'v', '1', '2'),

    /**
     * `cv13`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv13}
     */
    CharacterVariants13 = Utility::Endianness::fourCC('c', 'v', '1', '3'),

    /**
     * `cv14`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv14}
     */
    CharacterVariants14 = Utility::Endianness::fourCC('c', 'v', '1', '4'),

    /**
     * `cv15`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv15}
     */
    CharacterVariants15 = Utility::Endianness::fourCC('c', 'v', '1', '5'),

    /**
     * `cv16`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv16}
     */
    CharacterVariants16 = Utility::Endianness::fourCC('c', 'v', '1', '6'),

    /**
     * `cv17`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv17}
     */
    CharacterVariants17 = Utility::Endianness::fourCC('c', 'v', '1', '7'),

    /**
     * `cv18`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv18}
     */
    CharacterVariants18 = Utility::Endianness::fourCC('c', 'v', '1', '8'),

    /**
     * `cv19`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv19}
     */
    CharacterVariants19 = Utility::Endianness::fourCC('c', 'v', '1', '9'),

    /**
     * `cv20`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv20}
     */
    CharacterVariants20 = Utility::Endianness::fourCC('c', 'v', '2', '0'),

    /**
     * `cv21`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv21}
     */
    CharacterVariants21 = Utility::Endianness::fourCC('c', 'v', '2', '1'),

    /**
     * `cv22`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv22}
     */
    CharacterVariants22 = Utility::Endianness::fourCC('c', 'v', '2', '2'),

    /**
     * `cv23`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv23}
     */
    CharacterVariants23 = Utility::Endianness::fourCC('c', 'v', '2', '3'),

    /**
     * `cv24`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv24}
     */
    CharacterVariants24 = Utility::Endianness::fourCC('c', 'v', '2', '4'),

    /**
     * `cv25`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv25}
     */
    CharacterVariants25 = Utility::Endianness::fourCC('c', 'v', '2', '5'),

    /**
     * `cv26`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv26}
     */
    CharacterVariants26 = Utility::Endianness::fourCC('c', 'v', '2', '6'),

    /**
     * `cv27`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv27}
     */
    CharacterVariants27 = Utility::Endianness::fourCC('c', 'v', '2', '7'),

    /**
     * `cv28`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv28}
     */
    CharacterVariants28 = Utility::Endianness::fourCC('c', 'v', '2', '8'),

    /**
     * `cv29`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv29}
     */
    CharacterVariants29 = Utility::Endianness::fourCC('c', 'v', '2', '9'),

    /**
     * `cv30`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv30}
     */
    CharacterVariants30 = Utility::Endianness::fourCC('c', 'v', '3', '0'),

    /**
     * `cv31`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv31}
     */
    CharacterVariants31 = Utility::Endianness::fourCC('c', 'v', '3', '1'),

    /**
     * `cv32`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv32}
     */
    CharacterVariants32 = Utility::Endianness::fourCC('c', 'v', '3', '2'),

    /**
     * `cv33`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv33}
     */
    CharacterVariants33 = Utility::Endianness::fourCC('c', 'v', '3', '3'),

    /**
     * `cv34`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv34}
     */
    CharacterVariants34 = Utility::Endianness::fourCC('c', 'v', '3', '4'),

    /**
     * `cv35`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv35}
     */
    CharacterVariants35 = Utility::Endianness::fourCC('c', 'v', '3', '5'),

    /**
     * `cv36`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv36}
     */
    CharacterVariants36 = Utility::Endianness::fourCC('c', 'v', '3', '6'),

    /**
     * `cv37`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv37}
     */
    CharacterVariants37 = Utility::Endianness::fourCC('c', 'v', '3', '7'),

    /**
     * `cv38`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv38}
     */
    CharacterVariants38 = Utility::Endianness::fourCC('c', 'v', '3', '8'),

    /**
     * `cv39`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv39}
     */
    CharacterVariants39 = Utility::Endianness::fourCC('c', 'v', '3', '9'),

    /**
     * `cv40`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv40}
     */
    CharacterVariants40 = Utility::Endianness::fourCC('c', 'v', '4', '0'),

    /**
     * `cv41`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv41}
     */
    CharacterVariants41 = Utility::Endianness::fourCC('c', 'v', '4', '1'),

    /**
     * `cv42`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv42}
     */
    CharacterVariants42 = Utility::Endianness::fourCC('c', 'v', '4', '2'),

    /**
     * `cv43`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv43}
     */
    CharacterVariants43 = Utility::Endianness::fourCC('c', 'v', '4', '3'),

    /**
     * `cv44`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv44}
     */
    CharacterVariants44 = Utility::Endianness::fourCC('c', 'v', '4', '4'),

    /**
     * `cv45`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv45}
     */
    CharacterVariants45 = Utility::Endianness::fourCC('c', 'v', '4', '5'),

    /**
     * `cv46`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv46}
     */
    CharacterVariants46 = Utility::Endianness::fourCC('c', 'v', '4', '6'),

    /**
     * `cv47`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv47}
     */
    CharacterVariants47 = Utility::Endianness::fourCC('c', 'v', '4', '7'),

    /**
     * `cv48`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv48}
     */
    CharacterVariants48 = Utility::Endianness::fourCC('c', 'v', '4', '8'),

    /**
     * `cv49`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv49}
     */
    CharacterVariants49 = Utility::Endianness::fourCC('c', 'v', '4', '9'),

    /**
     * `cv50`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv50}
     */
    CharacterVariants50 = Utility::Endianness::fourCC('c', 'v', '5', '0'),

    /**
     * `cv51`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv51}
     */
    CharacterVariants51 = Utility::Endianness::fourCC('c', 'v', '5', '1'),

    /**
     * `cv52`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv52}
     */
    CharacterVariants52 = Utility::Endianness::fourCC('c', 'v', '5', '2'),

    /**
     * `cv53`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv53}
     */
    CharacterVariants53 = Utility::Endianness::fourCC('c', 'v', '5', '3'),

    /**
     * `cv54`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv54}
     */
    CharacterVariants54 = Utility::Endianness::fourCC('c', 'v', '5', '4'),

    /**
     * `cv55`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv55}
     */
    CharacterVariants55 = Utility::Endianness::fourCC('c', 'v', '5', '5'),

    /**
     * `cv56`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv56}
     */
    CharacterVariants56 = Utility::Endianness::fourCC('c', 'v', '5', '6'),

    /**
     * `cv57`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv57}
     */
    CharacterVariants57 = Utility::Endianness::fourCC('c', 'v', '5', '7'),

    /**
     * `cv58`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv58}
     */
    CharacterVariants58 = Utility::Endianness::fourCC('c', 'v', '5', '8'),

    /**
     * `cv59`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv59}
     */
    CharacterVariants59 = Utility::Endianness::fourCC('c', 'v', '5', '9'),

    /**
     * `cv60`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv60}
     */
    CharacterVariants60 = Utility::Endianness::fourCC('c', 'v', '6', '0'),

    /**
     * `cv61`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv61}
     */
    CharacterVariants61 = Utility::Endianness::fourCC('c', 'v', '6', '1'),

    /**
     * `cv62`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv62}
     */
    CharacterVariants62 = Utility::Endianness::fourCC('c', 'v', '6', '2'),

    /**
     * `cv63`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv63}
     */
    CharacterVariants63 = Utility::Endianness::fourCC('c', 'v', '6', '3'),

    /**
     * `cv64`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv64}
     */
    CharacterVariants64 = Utility::Endianness::fourCC('c', 'v', '6', '4'),

    /**
     * `cv65`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv65}
     */
    CharacterVariants65 = Utility::Endianness::fourCC('c', 'v', '6', '5'),

    /**
     * `cv66`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv66}
     */
    CharacterVariants66 = Utility::Endianness::fourCC('c', 'v', '6', '6'),

    /**
     * `cv67`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv67}
     */
    CharacterVariants67 = Utility::Endianness::fourCC('c', 'v', '6', '7'),

    /**
     * `cv68`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv68}
     */
    CharacterVariants68 = Utility::Endianness::fourCC('c', 'v', '6', '8'),

    /**
     * `cv69`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv69}
     */
    CharacterVariants69 = Utility::Endianness::fourCC('c', 'v', '6', '9'),

    /**
     * `cv70`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv70}
     */
    CharacterVariants70 = Utility::Endianness::fourCC('c', 'v', '7', '0'),

    /**
     * `cv71`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv71}
     */
    CharacterVariants71 = Utility::Endianness::fourCC('c', 'v', '7', '1'),

    /**
     * `cv72`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv72}
     */
    CharacterVariants72 = Utility::Endianness::fourCC('c', 'v', '7', '2'),

    /**
     * `cv73`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv73}
     */
    CharacterVariants73 = Utility::Endianness::fourCC('c', 'v', '7', '3'),

    /**
     * `cv74`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv74}
     */
    CharacterVariants74 = Utility::Endianness::fourCC('c', 'v', '7', '4'),

    /**
     * `cv75`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv75}
     */
    CharacterVariants75 = Utility::Endianness::fourCC('c', 'v', '7', '5'),

    /**
     * `cv76`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv76}
     */
    CharacterVariants76 = Utility::Endianness::fourCC('c', 'v', '7', '6'),

    /**
     * `cv77`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv77}
     */
    CharacterVariants77 = Utility::Endianness::fourCC('c', 'v', '7', '7'),

    /**
     * `cv78`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv78}
     */
    CharacterVariants78 = Utility::Endianness::fourCC('c', 'v', '7', '8'),

    /**
     * `cv79`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv79}
     */
    CharacterVariants79 = Utility::Endianness::fourCC('c', 'v', '7', '9'),

    /**
     * `cv80`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv80}
     */
    CharacterVariants80 = Utility::Endianness::fourCC('c', 'v', '8', '0'),

    /**
     * `cv81`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv81}
     */
    CharacterVariants81 = Utility::Endianness::fourCC('c', 'v', '8', '1'),

    /**
     * `cv82`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv82}
     */
    CharacterVariants82 = Utility::Endianness::fourCC('c', 'v', '8', '2'),

    /**
     * `cv83`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv83}
     */
    CharacterVariants83 = Utility::Endianness::fourCC('c', 'v', '8', '3'),

    /**
     * `cv84`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv84}
     */
    CharacterVariants84 = Utility::Endianness::fourCC('c', 'v', '8', '4'),

    /**
     * `cv85`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv85}
     */
    CharacterVariants85 = Utility::Endianness::fourCC('c', 'v', '8', '5'),

    /**
     * `cv86`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv86}
     */
    CharacterVariants86 = Utility::Endianness::fourCC('c', 'v', '8', '6'),

    /**
     * `cv87`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv87}
     */
    CharacterVariants87 = Utility::Endianness::fourCC('c', 'v', '8', '7'),

    /**
     * `cv88`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv88}
     */
    CharacterVariants88 = Utility::Endianness::fourCC('c', 'v', '8', '8'),

    /**
     * `cv89`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv89}
     */
    CharacterVariants89 = Utility::Endianness::fourCC('c', 'v', '8', '9'),

    /**
     * `cv90`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv90}
     */
    CharacterVariants90 = Utility::Endianness::fourCC('c', 'v', '9', '0'),

    /**
     * `cv91`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv91}
     */
    CharacterVariants91 = Utility::Endianness::fourCC('c', 'v', '9', '1'),

    /**
     * `cv92`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv92}
     */
    CharacterVariants92 = Utility::Endianness::fourCC('c', 'v', '9', '2'),

    /**
     * `cv93`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv93}
     */
    CharacterVariants93 = Utility::Endianness::fourCC('c', 'v', '9', '3'),

    /**
     * `cv94`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv94}
     */
    CharacterVariants94 = Utility::Endianness::fourCC('c', 'v', '9', '4'),

    /**
     * `cv95`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv95}
     */
    CharacterVariants95 = Utility::Endianness::fourCC('c', 'v', '9', '5'),

    /**
     * `cv96`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv96}
     */
    CharacterVariants96 = Utility::Endianness::fourCC('c', 'v', '9', '6'),

    /**
     * `cv97`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv97}
     */
    CharacterVariants97 = Utility::Endianness::fourCC('c', 'v', '9', '7'),

    /**
     * `cv98`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv98}
     */
    CharacterVariants98 = Utility::Endianness::fourCC('c', 'v', '9', '8'),

    /**
     * `cv99`, [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{cv99}
     */
    CharacterVariants99 = Utility::Endianness::fourCC('c', 'v', '9', '9'),

    /**
     * `c2pc`, [Petite Capitals From Capitals](https://learn.microsoft.com/typography/opentype/spec/features_ae#c2pc).
     * Intended for cased scripts.
     * @m_keywords{c2pc}
     */
    PetiteCapitalsFromCapitals = Utility::Endianness::fourCC('c', '2', 'p', 'c'),

    /**
     * `c2sc`, [Small Capitals From Capitals](https://learn.microsoft.com/typography/opentype/spec/features_ae#c2sc).
     * Intended for cased scripts.
     * @m_keywords{c2sc}
     */
    SmallCapitalsFromCapitals = Utility::Endianness::fourCC('c', '2', 's', 'c'),

    /**
     * `dist`, [Distances](https://learn.microsoft.com/typography/opentype/spec/features_ae#dist).
     * Intended for South-Asian scripts.
     * @m_keywords{dist}
     */
    Distances = Utility::Endianness::fourCC('d', 'i', 's', 't'),

    /**
     * `dlig`, [Discretionary Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_ae#dlig).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{dlig}
     * @see @ref Feature::StandardLigatures,
     *      @relativeref{Feature,ContextualLigatures},
     *      @relativeref{Feature,HistoricalLigatures}
     */
    DiscretionaryLigatures = Utility::Endianness::fourCC('d', 'l', 'i', 'g'),

    /**
     * `dnom`, [Denominators](https://learn.microsoft.com/typography/opentype/spec/features_ae#dnom).
     * Intended for digits and math.
     * @m_keywords{dnom}
     */
    Denominators = Utility::Endianness::fourCC('d', 'n', 'o', 'm'),

    /**
     * `dtls`, [Dotless Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#dtls).
     * Intended for digits and math.
     * @m_keywords{dtls}
     */
    DotlessForms = Utility::Endianness::fourCC('d', 't', 'l', 's'),

    /**
     * `expt`, [Expert Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#expt).
     * Intended for East-Asian scripts.
     * @m_keywords{expt}
     */
    ExpertForms = Utility::Endianness::fourCC('e', 'x', 'p', 't'),

    /**
     * `falt`, [Final Glyph on Line Alternates](https://learn.microsoft.com/typography/opentype/spec/features_fj#falt).
     * Intended for West-Asian scripts.
     * @m_keywords{falt}
     */
    FinalGlyphOnLineAlternates = Utility::Endianness::fourCC('f', 'a', 'l', 't'),

    /**
     * `fina`, [Terminal Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#fina).
     * Intended for West-Asian scripts.
     * @m_keywords{fina}
     */
    TerminalForms = Utility::Endianness::fourCC('f', 'i', 'n', 'a'),

    /**
     * `fin2`, [Terminal Forms #2](https://learn.microsoft.com/typography/opentype/spec/features_fj#fin2).
     * Intended for West-Asian scripts.
     * @m_keywords{fin2}
     */
    TerminalForms2 = Utility::Endianness::fourCC('f', 'i', 'n', '2'),

    /**
     * `fin3`, [Terminal Forms #3](https://learn.microsoft.com/typography/opentype/spec/features_fj#fin3).
     * Intended for West-Asian scripts.
     * @m_keywords{fin3}
     */
    TerminalForms3 = Utility::Endianness::fourCC('f', 'i', 'n', '3'),

    /**
     * `flac`, [Flattened Accent Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#flac).
     * Intended for digits and math.
     * @m_keywords{flac}
     */
    FlattenedAccentForms = Utility::Endianness::fourCC('f', 'l', 'a', 'c'),

    /**
     * `frac`, [Fractions](https://learn.microsoft.com/typography/opentype/spec/features_fj#frac).
     * Intended for digits and math.
     * @m_keywords{frac}
     */
    Fractions = Utility::Endianness::fourCC('f', 'r', 'a', 'c'),

    /**
     * `fwid`, [Full Widths](https://learn.microsoft.com/typography/opentype/spec/features_fj#fwid).
     * Intended for East-Asian scripts.
     * @m_keywords{fwid}
     */
    FullWidths = Utility::Endianness::fourCC('f', 'w', 'i', 'd'),

    /**
     * `half`, [Half Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#half).
     * Intended for South-Asian scripts.
     * @m_keywords{half}
     */
    HalfForms = Utility::Endianness::fourCC('h', 'a', 'l', 'f'),

    /**
     * `haln`, [Halant Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#haln).
     * Intended for South-Asian scripts.
     * @m_keywords{haln}
     */
    HalantForms = Utility::Endianness::fourCC('h', 'a', 'l', 'n'),

    /**
     * `halt`, [Alternate Half Widths](https://learn.microsoft.com/typography/opentype/spec/features_fj#halt).
     * Intended for East-Asian scripts.
     * @m_keywords{halt}
     */
    AlternateHalfWidths = Utility::Endianness::fourCC('h', 'a', 'l', 't'),

    /**
     * `hist`, [Historical Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#hist).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{hist}
     */
    HistoricalForms = Utility::Endianness::fourCC('h', 'i', 's', 't'),

    /**
     * `hkna`, [Horizontal Kana Alternates](https://learn.microsoft.com/typography/opentype/spec/features_fj#hkna).
     * Intended for East-Asian scripts.
     * @m_keywords{hkna}
     */
    HorizontalKanaAlternates = Utility::Endianness::fourCC('h', 'k', 'n', 'a'),

    /**
     * `hlig`, [Historical Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_fj#hlig).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{hlig}
     * @see @ref Feature::ContextualLigatures,
     *      @relativeref{Feature,DiscretionaryLigatures},
     *      @relativeref{Feature,StandardLigatures}
     */
    HistoricalLigatures = Utility::Endianness::fourCC('h', 'l', 'i', 'g'),

    /**
     * `hngl`, [Hangul](https://learn.microsoft.com/typography/opentype/spec/features_fj#hngl).
     * Intended for East-Asian scripts.
     * @m_keywords{hngl}
     */
    Hangul = Utility::Endianness::fourCC('h', 'n', 'g', 'l'),

    /**
     * `hojo`, [Hojo Kanji Forms (JIS X 0212-1990 Kanji Forms)](https://learn.microsoft.com/typography/opentype/spec/features_fj#hojo).
     * Intended for East-Asian scripts.
     * @m_keywords{hojo}
     */
    HojoKanjiForms = Utility::Endianness::fourCC('h', 'o', 'j', 'o'),

    /**
     * `hwid`, [Half Widths](https://learn.microsoft.com/typography/opentype/spec/features_fj#hwid).
     * Intended for East-Asian scripts.
     * @m_keywords{hwid}
     */
    HalfWidths = Utility::Endianness::fourCC('h', 'w', 'i', 'd'),

    /**
     * `init`, [Initial Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#init).
     * Intended for West-Asian scripts.
     * @m_keywords{init}
     */
    InitialForms = Utility::Endianness::fourCC('i', 'n', 'i', 't'),

    /**
     * `isol`, [Isolated Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#isol).
     * Intended for West-Asian scripts.
     * @m_keywords{isol}
     */
    IsolatedForms = Utility::Endianness::fourCC('i', 's', 'o', 'l'),

    /**
     * `ital`, [Italics](https://learn.microsoft.com/typography/opentype/spec/features_fj#ital).
     * Intended for cased scripts.
     * @m_keywords{ital}
     */
    Italics = Utility::Endianness::fourCC('i', 't', 'a', 'l'),

    /**
     * `jalt`, [Justification Alternates](https://learn.microsoft.com/typography/opentype/spec/features_fj#jalt).
     * Intended for West-Asian scripts.
     * @m_keywords{jalt}
     */
    JustificationAlternates = Utility::Endianness::fourCC('j', 'a', 'l', 't'),

    /**
     * `jp78`, [JIS78 Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#jp78).
     * Intended for East-Asian scripts.
     * @m_keywords{jp78}
     */
    Jis78Forms = Utility::Endianness::fourCC('j', 'p', '7', '8'),

    /**
     * `jp83`, [JIS83 Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#jp83).
     * Intended for East-Asian scripts.
     * @m_keywords{jp83}
     */
    Jis83Forms = Utility::Endianness::fourCC('j', 'p', '8', '3'),

    /**
     * `jp90`, [JIS90 Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#jp90).
     * Intended for East-Asian scripts.
     * @m_keywords{jp90}
     */
    Jis90Forms = Utility::Endianness::fourCC('j', 'p', '9', '0'),

    /**
     * `jp04`, [JIS2004 Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#jp04).
     * Intended for East-Asian scripts.
     * @m_keywords{jp04}
     */
    Jis2004Forms = Utility::Endianness::fourCC('j', 'p', '0', '4'),

    /**
     * `kern`, [Kerning](https://learn.microsoft.com/typography/opentype/spec/features_ko#kern).
     * Positioning feature intended for all scripts.
     * @m_keywords{kern}
     */
    Kerning = Utility::Endianness::fourCC('k', 'e', 'r', 'n'),

    /**
     * `lfbd`, [Left Bounds](https://learn.microsoft.com/typography/opentype/spec/features_ko#lfbd).
     * Positioning feature intended for all scripts.
     * @m_keywords{lfbd}
     */
    LeftBounds = Utility::Endianness::fourCC('l', 'f', 'b', 'd'),

    /**
     * `liga`, [Standard Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_ko#liga).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{liga}
     * @see @ref Feature::ContextualLigatures,
     *      @relativeref{Feature,DiscretionaryLigatures},
     *      @relativeref{Feature,HistoricalLigatures}
     */
    StandardLigatures = Utility::Endianness::fourCC('l', 'i', 'g', 'a'),

    /**
     * `ljmo`, [Leading Jamo Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#ljmo).
     * Intended for East-Asian scripts.
     * @m_keywords{ljmo}
     */
    LeadingJamoForms = Utility::Endianness::fourCC('l', 'j', 'm', 'o'),

    /**
     * `lnum`, [Lining Figures](https://learn.microsoft.com/typography/opentype/spec/features_ko#lnum).
     * Intended for digits and math.
     * @m_keywords{lnum}
     */
    LiningFigures = Utility::Endianness::fourCC('l', 'n', 'u', 'm'),

    /**
     * `locl`, [Localized Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#locl).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{locl}
     */
    LocalizedForms = Utility::Endianness::fourCC('l', 'o', 'c', 'l'),

    /**
     * `ltra`, [Left-to-right alternates](https://learn.microsoft.com/typography/opentype/spec/features_ko#ltra).
     * Feature depending on writing direction.
     * @m_keywords{ltra}
     */
    LeftToRightAlternates = Utility::Endianness::fourCC('l', 't', 'r', 'a'),

    /**
     * `ltrm`, [Left-to-right mirrored forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#ltrm).
     * Feature depending on writing direction.
     * @m_keywords{ltrm}
     */
    LeftToRightMirroredForms = Utility::Endianness::fourCC('l', 't', 'r', 'm'),

    /**
     * `mark`, [Mark Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ko#mark).
     * Positioning feature intended for all scripts.
     * @m_keywords{mark}
     */
    MarkPositioning = Utility::Endianness::fourCC('m', 'a', 'r', 'k'),

    /**
     * `medi`, [Medial Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#medi).
     * Intended for West-Asian scripts.
     * @m_keywords{medi}
     */
    MedialForms = Utility::Endianness::fourCC('m', 'e', 'd', 'i'),

    /**
     * `med2`, [Medial Forms #2](https://learn.microsoft.com/typography/opentype/spec/features_ko#med2).
     * Intended for West-Asian scripts.
     * @m_keywords{med2}
     */
    MedialForms2 = Utility::Endianness::fourCC('m', 'e', 'd', '2'),

    /**
     * `mgrk`, [Mathematical Greek](https://learn.microsoft.com/typography/opentype/spec/features_ko#mgrk).
     * Intended for digits and math.
     * @m_keywords{mgrk}
     */
    MathematicalGreek = Utility::Endianness::fourCC('m', 'g', 'r', 'k'),

    /**
     * `mkmk`, [Mark to Mark Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ko#mkmk).
     * Positioning feature intended for all scripts.
     * @m_keywords{mkmk}
     */
    MarkToMarkPositioning = Utility::Endianness::fourCC('m', 'k', 'm', 'k'),

    /**
     * `mset`, [Mark Positioning via Substitution](https://learn.microsoft.com/typography/opentype/spec/features_ko#mset).
     * Intended for West-Asian scripts.
     * @m_keywords{mset}
     */
    MarkPositioningViaSubstitution = Utility::Endianness::fourCC('m', 's', 'e', 't'),

    /**
     * `nalt`, [Alternate Annotation Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#nalt).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{nalt}
     */
    AlternateAnnotationForms = Utility::Endianness::fourCC('n', 'a', 'l', 't'),

    /**
     * `nlck`, [NLC Kanji Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#nlck).
     * Intended for East-Asian scripts.
     * @m_keywords{nlck}
     */
    NlcKanjiForms = Utility::Endianness::fourCC('n', 'l', 'c', 'k'),

    /**
     * `nukt`, [Nukta Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#nukt).
     * Intended for South-Asian scripts.
     * @m_keywords{nukt}
     */
    NuktaForms = Utility::Endianness::fourCC('n', 'u', 'k', 't'),

    /**
     * `numr`, [Numerators](https://learn.microsoft.com/typography/opentype/spec/features_ko#numr).
     * Intended for digits and math.
     * @m_keywords{numr}
     */
    Numerators = Utility::Endianness::fourCC('n', 'u', 'm', 'r'),

    /**
     * `onum`, [Oldstyle Figures](https://learn.microsoft.com/typography/opentype/spec/features_ko#onum).
     * Intended for digits and math.
     * @m_keywords{onum}
     */
    OldstyleFigures = Utility::Endianness::fourCC('o', 'n', 'u', 'm'),

    /**
     * `opbd`, [Optical Bounds](https://learn.microsoft.com/typography/opentype/spec/features_ko#opbd).
     * Positioning feature intended for all scripts.
     * @m_keywords{opbd}
     */
    OpticalBounds = Utility::Endianness::fourCC('o', 'p', 'b', 'd'),

    /**
     * `ordn`, [Ordinals](https://learn.microsoft.com/typography/opentype/spec/features_ko#ordn).
     * Intended for cased scripts.
     * @m_keywords{ordn}
     */
    Ordinals = Utility::Endianness::fourCC('o', 'r', 'd', 'n'),

    /**
     * `ornm`, [Ornaments](https://learn.microsoft.com/typography/opentype/spec/features_ko#ornm).
     * Special feature intended for all scripts.
     * @m_keywords{ornm}
     */
    Ornaments = Utility::Endianness::fourCC('o', 'r', 'n', 'm'),

    /**
     * `palt`, [Proportional Alternate Widths](https://learn.microsoft.com/typography/opentype/spec/features_pt#palt).
     * Intended for East-Asian scripts.
     * @m_keywords{palt}
     */
    ProportionalAlternateWidths = Utility::Endianness::fourCC('p', 'a', 'l', 't'),

    /**
     * `pcap`, [Petite Capitals](https://learn.microsoft.com/typography/opentype/spec/features_pt#pcap).
     * Intended for cased scripts.
     * @m_keywords{pcap}
     */
    PetiteCapitals = Utility::Endianness::fourCC('p', 'c', 'a', 'p'),

    /**
     * `pkna`, [Proportional Kana](https://learn.microsoft.com/typography/opentype/spec/features_pt#pkna).
     * Intended for East-Asian scripts.
     * @m_keywords{pkna}
     */
    ProportionalKana = Utility::Endianness::fourCC('p', 'k', 'n', 'a'),

    /**
     * `pnum`, [Proportional Figures](https://learn.microsoft.com/typography/opentype/spec/features_pt#pnum).
     * Intended for digits and math.
     * @m_keywords{pnum}
     */
    ProportionalFigures = Utility::Endianness::fourCC('p', 'n', 'u', 'm'),

    /**
     * `pref`, [Pre-Base Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#pref).
     * Intended for South-Asian scripts.
     * @m_keywords{pref}
     */
    PreBaseForms = Utility::Endianness::fourCC('p', 'r', 'e', 'f'),

    /**
     * `pres`, [Pre-base Substitutions](https://learn.microsoft.com/typography/opentype/spec/features_pt#pres).
     * Intended for South-Asian scripts.
     * @m_keywords{pres}
     */
    PreBaseSubstitutions = Utility::Endianness::fourCC('p', 'r', 'e', 's'),

    /**
     * `pstf`, [Post-base Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#pstf).
     * Intended for South-Asian scripts.
     * @m_keywords{pstf}
     */
    PostBaseForms = Utility::Endianness::fourCC('p', 's', 't', 'f'),

    /**
     * `psts`, [Post-base Substitutions](https://learn.microsoft.com/typography/opentype/spec/features_pt#psts).
     * Intended for South-Asian scripts.
     * @m_keywords{psts}
     */
    PostBaseSubstitutions = Utility::Endianness::fourCC('p', 's', 't', 's'),

    /**
     * `pwid`, [Proportional Widths](https://learn.microsoft.com/typography/opentype/spec/features_pt#pwid).
     * Intended for East-Asian scripts.
     * @m_keywords{pwid}
     */
    ProportionalWidths = Utility::Endianness::fourCC('p', 'w', 'i', 'd'),

    /**
     * `qwid`, [Quarter Widths](https://learn.microsoft.com/typography/opentype/spec/features_pt#qwid).
     * Intended for East-Asian scripts.
     * @m_keywords{qwid}
     */
    QuarterWidths = Utility::Endianness::fourCC('q', 'w', 'i', 'd'),

    /**
     * `rand`, [Randomize](https://learn.microsoft.com/typography/opentype/spec/features_pt#rand).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{rand}
     */
    Randomize = Utility::Endianness::fourCC('r', 'a', 'n', 'd'),

    /**
     * `rclt`, [Required Contextual Alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#rclt).
     * Intended for West-Asian scripts.
     * @m_keywords{rclt}
     */
    RequiredContextualAlternates = Utility::Endianness::fourCC('r', 'c', 'l', 't'),

    /**
     * `rkrf`, [Rakar Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#rkrf).
     * Intended for South-Asian scripts.
     * @m_keywords{rkrf}
     */
    RakarForms = Utility::Endianness::fourCC('r', 'k', 'r', 'f'),

    /**
     * `rlig`, [Required Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_pt#rlig).
     * Intended for West-Asian scripts.
     * @m_keywords{rlig}
     */
    RequiredLigatures = Utility::Endianness::fourCC('r', 'l', 'i', 'g'),

    /**
     * `rphf`, [Reph Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#rphf).
     * Intended for South-Asian scripts.
     * @m_keywords{rphf}
     */
    RephForms = Utility::Endianness::fourCC('r', 'p', 'h', 'f'),

    /**
     * `rtbd`, [Right Bounds](https://learn.microsoft.com/typography/opentype/spec/features_pt#rtbd).
     * Positioning feature intended for all scripts.
     * @m_keywords{rtbd}
     */
    RightBounds = Utility::Endianness::fourCC('r', 't', 'b', 'd'),

    /**
     * `rtla`, [Right-to-left alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#rtla).
     * Feature depending on writing direction.
     * @m_keywords{rtla}
     */
    RightToLeftAlternates = Utility::Endianness::fourCC('r', 't', 'l', 'a'),

    /**
     * `rtlm`, [Right-to-left mirrored forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#rtlm).
     * Feature depending on writing direction.
     * @m_keywords{rtlm}
     */
    RightToLeftMirroredForms = Utility::Endianness::fourCC('r', 't', 'l', 'm'),

    /**
     * `ruby`, [Ruby Notation Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#ruby).
     * Intended for East-Asian scripts.
     * @m_keywords{ruby}
     */
    RubyNotationForms = Utility::Endianness::fourCC('r', 'u', 'b', 'y'),

    /**
     * `rvrn`, [Required Variation Alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#rvrn).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{rvrn}
     */
    RequiredVariationAlternates = Utility::Endianness::fourCC('r', 'v', 'r', 'n'),

    /**
     * `salt`, [Stylistic Alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#salt).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{salt}
     */
    StylisticAlternates = Utility::Endianness::fourCC('s', 'a', 'l', 't'),

    /**
     * `sinf`, [Scientific Inferiors](https://learn.microsoft.com/typography/opentype/spec/features_pt#sinf).
     * Intended for digits and math.
     * @m_keywords{sinf}
     */
    ScientificInferiors = Utility::Endianness::fourCC('s', 'i', 'n', 'f'),

    /**
     * `size`, [Optical Size](https://learn.microsoft.com/typography/opentype/spec/features_pt#size).
     * Special feature intended for all scripts.
     * @m_keywords{size}
     */
    OpticalSize = Utility::Endianness::fourCC('s', 'i', 'z', 'e'),

    /**
     * `smcp`, [Small Capitals](https://learn.microsoft.com/typography/opentype/spec/features_pt#smcp).
     * Intended for cased scripts.
     * @m_keywords{smcp}
     */
    SmallCapitals = Utility::Endianness::fourCC('s', 'm', 'c', 'p'),

    /**
     * `smpl`, [Simplified Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#smpl).
     * Intended for East-Asian scripts.
     * @m_keywords{smpl}
     */
    SimplifiedForms = Utility::Endianness::fourCC('s', 'm', 'p', 'l'),

    /**
     * `ss01`, [Stylistic Set 1](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss01}
     */
    StylisticSet1 = Utility::Endianness::fourCC('s', 's', '0', '1'),

    /**
     * `ss02`, [Stylistic Set 2](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss02}
     */
    StylisticSet2 = Utility::Endianness::fourCC('s', 's', '0', '2'),

    /**
     * `ss03`, [Stylistic Set 3](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss03}
     */
    StylisticSet3 = Utility::Endianness::fourCC('s', 's', '0', '3'),

    /**
     * `ss04`, [Stylistic Set 4](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss04}
     */
    StylisticSet4 = Utility::Endianness::fourCC('s', 's', '0', '4'),

    /**
     * `ss05`, [Stylistic Set 5](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss05}
     */
    StylisticSet5 = Utility::Endianness::fourCC('s', 's', '0', '5'),

    /**
     * `ss06`, [Stylistic Set 6](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss06}
     */
    StylisticSet6 = Utility::Endianness::fourCC('s', 's', '0', '6'),

    /**
     * `ss07`, [Stylistic Set 7](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss07}
     */
    StylisticSet7 = Utility::Endianness::fourCC('s', 's', '0', '7'),

    /**
     * `ss08`, [Stylistic Set 8](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss08}
     */
    StylisticSet8 = Utility::Endianness::fourCC('s', 's', '0', '8'),

    /**
     * `ss09`, [Stylistic Set 9](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss09}
     */
    StylisticSet9 = Utility::Endianness::fourCC('s', 's', '0', '9'),

    /**
     * `ss10`, [Stylistic Set 10](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss10}
     */
    StylisticSet10 = Utility::Endianness::fourCC('s', 's', '1', '0'),

    /**
     * `ss11`, [Stylistic Set 11](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss11}
     */
    StylisticSet11 = Utility::Endianness::fourCC('s', 's', '1', '1'),

    /**
     * `ss12`, [Stylistic Set 12](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss12}
     */
    StylisticSet12 = Utility::Endianness::fourCC('s', 's', '1', '2'),

    /**
     * `ss13`, [Stylistic Set 13](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss13}
     */
    StylisticSet13 = Utility::Endianness::fourCC('s', 's', '1', '3'),

    /**
     * `ss14`, [Stylistic Set 14](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss14}
     */
    StylisticSet14 = Utility::Endianness::fourCC('s', 's', '1', '4'),

    /**
     * `ss15`, [Stylistic Set 15](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss15}
     */
    StylisticSet15 = Utility::Endianness::fourCC('s', 's', '1', '5'),

    /**
     * `ss16`, [Stylistic Set 16](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss16}
     */
    StylisticSet16 = Utility::Endianness::fourCC('s', 's', '1', '6'),

    /**
     * `ss17`, [Stylistic Set 17](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss17}
     */
    StylisticSet17 = Utility::Endianness::fourCC('s', 's', '1', '7'),

    /**
     * `ss18`, [Stylistic Set 18](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss18}
     */
    StylisticSet18 = Utility::Endianness::fourCC('s', 's', '1', '8'),

    /**
     * `ss19`, [Stylistic Set 19](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss19}
     */
    StylisticSet19 = Utility::Endianness::fourCC('s', 's', '1', '9'),

    /**
     * `ss20`, [Stylistic Set 20](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{ss20}
     */
    StylisticSet20 = Utility::Endianness::fourCC('s', 's', '2', '0'),

    /**
     * `ssty`, [Math Script Style Alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssty).
     * Intended for digits and math.
     * @m_keywords{ssty}
     */
    MathScriptStyleAlternates = Utility::Endianness::fourCC('s', 's', 't', 'y'),

    /**
     * `stch`, [Stretching Glyph Decomposition](https://learn.microsoft.com/typography/opentype/spec/features_pt#stch).
     * Intended for West-Asian scripts.
     * @m_keywords{stch}
     */
    StretchingGlyphDecomposition = Utility::Endianness::fourCC('s', 't', 'c', 'h'),

    /**
     * `subs`, [Subscript](https://learn.microsoft.com/typography/opentype/spec/features_pt#subs).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{subs}
     */
    Subscript = Utility::Endianness::fourCC('s', 'u', 'b', 's'),

    /**
     * `sups`, [Superscript](https://learn.microsoft.com/typography/opentype/spec/features_pt#sups).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{sups}
     */
    Superscript = Utility::Endianness::fourCC('s', 'u', 'p', 's'),

    /**
     * `swsh`, [Swash](https://learn.microsoft.com/typography/opentype/spec/features_pt#swsh).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{swsh}
     */
    Swash = Utility::Endianness::fourCC('s', 'w', 's', 'h'),

    /**
     * `titl`, [Titling](https://learn.microsoft.com/typography/opentype/spec/features_pt#titl).
     * Ligation and alternate feature intended for all scripts.
     * @m_keywords{titl}
     */
    Titling = Utility::Endianness::fourCC('t', 'i', 't', 'l'),

    /**
     * `tjmo`, [Trailing Jamo Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#tjmo).
     * Intended for East-Asian scripts.
     * @m_keywords{tjmo}
     */
    TrailingJamoForms = Utility::Endianness::fourCC('t', 'j', 'm', 'o'),

    /**
     * `tnam`, [Traditional Name Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#tnam).
     * Intended for East-Asian scripts.
     * @m_keywords{tnam}
     */
    TraditionalNameForms = Utility::Endianness::fourCC('t', 'n', 'a', 'm'),

    /**
     * `tnum`, [Tabular Figures](https://learn.microsoft.com/typography/opentype/spec/features_pt#tnum).
     * Intended for digits and math.
     * @m_keywords{tnum}
     */
    TabularFigures = Utility::Endianness::fourCC('t', 'n', 'u', 'm'),

    /**
     * `trad`, [Traditional Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#trad).
     * Intended for East-Asian scripts.
     * @m_keywords{trad}
     */
    TraditionalForms = Utility::Endianness::fourCC('t', 'r', 'a', 'd'),

    /**
     * `twid`, [Third Widths](https://learn.microsoft.com/typography/opentype/spec/features_pt#twid).
     * Intended for East-Asian scripts.
     * @m_keywords{twid}
     */
    ThirdWidths = Utility::Endianness::fourCC('t', 'w', 'i', 'd'),

    /**
     * `unic`, [Unicase](https://learn.microsoft.com/typography/opentype/spec/features_uz#unic).
     * Intended for cased scripts.
     * @m_keywords{unic}
     */
    Unicase = Utility::Endianness::fourCC('u', 'n', 'i', 'c'),

    /**
     * `valt`, [Alternate Vertical Metrics](https://learn.microsoft.com/typography/opentype/spec/features_uz#valt).
     * Feature depending on writing direction.
     * @m_keywords{valt}
     */
    AlternateVerticalMetrics = Utility::Endianness::fourCC('v', 'a', 'l', 't'),

    /**
     * `vatu`, [Vattu Variants](https://learn.microsoft.com/typography/opentype/spec/features_uz#vatu).
     * Intended for South-Asian scripts.
     * @m_keywords{vatu}
     */
    VattuVariants = Utility::Endianness::fourCC('v', 'a', 't', 'u'),

    /**
     * `vapk`, [Kerning for Alternate Proportional Vertical Metrics](https://learn.microsoft.com/typography/opentype/spec/features_uz#vapk).
     * Intended for East-Asian scripts.
     * @m_keywords{vapk}
     */
    KerningForAlternateProportionalVerticalMetrics = Utility::Endianness::fourCC('v', 'a', 'p', 'k'),

    /**
     * `vchw`, [Vertical Contextual Half-width Spacing](https://learn.microsoft.com/typography/opentype/spec/features_uz#vchw).
     * Intended for East-Asian scripts.
     * @m_keywords{vchw}
     */
    VerticalContextualHalfWidthSpacing = Utility::Endianness::fourCC('v', 'c', 'h', 'w'),

    /**
     * `vert`, [Vertical Writing](https://learn.microsoft.com/typography/opentype/spec/features_uz#vert).
     * Feature depending on writing direction.
     * @m_keywords{vert}
     */
    VerticalWriting = Utility::Endianness::fourCC('v', 'e', 'r', 't'),

    /**
     * `vhal`, [Alternate Vertical Half Metrics](https://learn.microsoft.com/typography/opentype/spec/features_uz#vhal).
     * Feature depending on writing direction.
     * @m_keywords{vhal}
     */
    AlternateVerticalHalfMetrics = Utility::Endianness::fourCC('v', 'h', 'a', 'l'),

    /**
     * `vjmo`, [Vowel Jamo Forms](https://learn.microsoft.com/typography/opentype/spec/features_uz#vjmo).
     * Intended for East-Asian scripts.
     * @m_keywords{vjmo}
     */
    VowelJamoForms = Utility::Endianness::fourCC('v', 'j', 'm', 'o'),

    /**
     * `vkna`, [Vertical Kana Alternates](https://learn.microsoft.com/typography/opentype/spec/features_uz#vkna).
     * Intended for East-Asian scripts.
     * @m_keywords{vkna}
     */
    VerticalKanaAlternates = Utility::Endianness::fourCC('v', 'k', 'n', 'a'),

    /**
     * `vkrn`, [Vertical Kerning](https://learn.microsoft.com/typography/opentype/spec/features_uz#vkrn).
     * Feature depending on writing direction.
     * @m_keywords{vkrn}
     */
    VerticalKerning = Utility::Endianness::fourCC('v', 'k', 'r', 'n'),

    /**
     * `vpal`, [Proportional Alternate Vertical Metrics](https://learn.microsoft.com/typography/opentype/spec/features_uz#vpal).
     * Feature depending on writing direction.
     * @m_keywords{vpal}
     */
    ProportionalAlternateVerticalMetrics = Utility::Endianness::fourCC('v', 'p', 'a', 'l'),

    /**
     * `vrt2`, [Vertical Alternates and Rotation](https://learn.microsoft.com/typography/opentype/spec/features_uz#vrt2).
     * Feature depending on writing direction.
     * @m_keywords{vrt2}
     */
    VerticalAlternatesAndRotation = Utility::Endianness::fourCC('v', 'r', 't', '2'),

    /**
     * `vrtr`, [Vertical Alternates for Rotation](https://learn.microsoft.com/typography/opentype/spec/features_uz#vrtr).
     * Feature depending on writing direction.
     * @m_keywords{vrtr}
     */
    VerticalAlternatesForRotation = Utility::Endianness::fourCC('v', 'r', 't', 'r'),

    /**
     * `zero`, [Slashed Zero](https://learn.microsoft.com/typography/opentype/spec/features_uz#zero).
     * Intended for digits and math.
     * @m_keywords{zero}
     */
    SlashedZero = Utility::Endianness::fourCC('z', 'e', 'r', 'o'),
};

/** @debugoperatorenum{Feature} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, Feature value);

/**
@brief Create a @ref Feature value from a four-character code
@m_since_latest

Delegates to @relativeref{Corrade,Utility::Endianness::fourCC()}. Doesn't
perform any validity check on the input, i.e. it's possible to create a code
that isn't listed in the [OpenType feature registry](https://learn.microsoft.com/typography/opentype/spec/featurelist)
or even a code with non-ASCII characters.
@see @ref feature(Containers::StringView)
*/
constexpr Feature feature(char a, char b, char c, char d) {
    return Feature(Utility::Endianness::fourCC(a, b, c, d));
}

/**
@brief Create a @ref Feature value from a string
@m_since_latest

Expects that the string has exactly four bytes. Other than that doesn't perform
any validity check on the input, i.e. it's possible to create a code that isn't
listed in the [OpenType feature registry](https://learn.microsoft.com/typography/opentype/spec/featurelist)
or even a code with non-ASCII characters.
@see @ref feature(char, char, char, char)
*/
MAGNUM_TEXT_EXPORT Feature feature(Containers::StringView fourCC);

}}

#endif
