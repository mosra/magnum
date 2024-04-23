#ifndef Magnum_Text_Feature_h
#define Magnum_Text_Feature_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Enum @ref Magnum::Text::Feature, function @ref Magnum::Text::feature()
 * @m_since_latest
 */

#include <Corrade/Utility/Endianness.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

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
     * [Access All Alternates](https://learn.microsoft.com/typography/opentype/spec/features_ae#aalt).
     * Ligation and alternate feature intended for all scripts.
    */
    AccessAllAlternates = Utility::Endianness::fourCC('a', 'a', 'l', 't'),

    /**
     * [Above-base Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#abvf).
     * Intended for South-Asian scripts.
    */
    AboveBaseForms = Utility::Endianness::fourCC('a', 'b', 'v', 'f'),

    /**
     * [Above-base Mark Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ae#abvm).
     * Intended for South-Asian scripts.
    */
    AboveBaseMarkPositioning = Utility::Endianness::fourCC('a', 'b', 'v', 'm'),

    /**
     * [Above-base Substitutions](https://learn.microsoft.com/typography/opentype/spec/features_ae#abvs).
     * Intended for South-Asian scripts.
    */
    AboveBaseSubstitutions = Utility::Endianness::fourCC('a', 'b', 'v', 's'),

    /**
     * [Alternative Fractions](https://learn.microsoft.com/typography/opentype/spec/features_ae#afrc).
     * Intended for digits and math.
    */
    AlternativeFractions = Utility::Endianness::fourCC('a', 'f', 'r', 'c'),

    /**
     * [Akhand](https://learn.microsoft.com/typography/opentype/spec/features_ae#akhn).
     * Intended for South-Asian scripts.
    */
    Akhand = Utility::Endianness::fourCC('a', 'k', 'h', 'n'),

    /**
     * [Below-base Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#blwf).
     * Intended for South-Asian scripts.
    */
    BelowBaseForms = Utility::Endianness::fourCC('b', 'l', 'w', 'f'),

    /**
     * [Below-base Mark Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ae#blwm).
     * Intended for South-Asian scripts.
    */
    BelowBaseMarkPositioning = Utility::Endianness::fourCC('b', 'l', 'w', 'm'),

    /**
     * [Below-base Substitutions](https://learn.microsoft.com/typography/opentype/spec/features_ae#blws).
     * Intended for South-Asian scripts.
    */
    BelowBaseSubstitutions = Utility::Endianness::fourCC('b', 'l', 'w', 's'),

    /**
     * [Contextual Alternates](https://learn.microsoft.com/typography/opentype/spec/features_ae#calt).
     * Ligation and alternate feature intended for all scripts.
    */
    ContextualAlternates = Utility::Endianness::fourCC('c', 'a', 'l', 't'),

    /**
     * [Case-Sensitive Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#case).
     * Intended for cased scripts.
    */
    CaseSensitiveForms = Utility::Endianness::fourCC('c', 'a', 's', 'e'),

    /**
     * [Glyph Composition / Decomposition](https://learn.microsoft.com/typography/opentype/spec/features_ae#ccmp).
     * Positioning feature intended for all scripts.
    */
    GlyphCompositionDecomposition = Utility::Endianness::fourCC('c', 'c', 'm', 'p'),

    /**
     * [Conjunct Form After Ro](https://learn.microsoft.com/typography/opentype/spec/features_ae#cfar).
     * Intended for South-Asian scripts.
    */
    ConjunctFormAfterRo = Utility::Endianness::fourCC('c', 'f', 'a', 'r'),

    /**
     * [Contextual Half-width Spacing](https://learn.microsoft.com/typography/opentype/spec/features_ae#chws).
     * Intended for East-Asian scripts.
    */
    ContextualHalfWidthSpacing = Utility::Endianness::fourCC('c', 'h', 'w', 's'),

    /**
     * [Conjunct Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#cjct).
     * Intended for South-Asian scripts.
    */
    ConjunctForms = Utility::Endianness::fourCC('c', 'j', 'c', 't'),

    /**
     * [Contextual Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_ae#clig).
     * Ligation and alternate feature intended for all scripts.
    */
    ContextualLigatures = Utility::Endianness::fourCC('c', 'l', 'i', 'g'),

    /**
     * [Centered CJK Punctuation](https://learn.microsoft.com/typography/opentype/spec/features_ae#cpct).
     * Intended for East-Asian scripts.
    */
    CenteredCjkPunctuation = Utility::Endianness::fourCC('c', 'p', 'c', 't'),

    /**
     * [Capital Spacing](https://learn.microsoft.com/typography/opentype/spec/features_ae#cpsp).
     * Intended for cased scripts.
    */
    CapitalSpacing = Utility::Endianness::fourCC('c', 'p', 's', 'p'),

    /**
     * [Contextual Swash](https://learn.microsoft.com/typography/opentype/spec/features_ae#cswh).
     * Ligation and alternate feature intended for all scripts.
    */
    ContextualSwash = Utility::Endianness::fourCC('c', 's', 'w', 'h'),

    /**
     * [Cursive Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ae#curs).
     * Intended for West-Asian scripts.
    */
    CursivePositioning = Utility::Endianness::fourCC('c', 'u', 'r', 's'),

    /**
     * [Character Variants](https://learn.microsoft.com/typography/opentype/spec/features_ae#cv01-cv99).
     * Ligation and alternate feature intended for all scripts.
    */
    CharacterVariants1 = Utility::Endianness::fourCC('c', 'v', '0', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants2 = Utility::Endianness::fourCC('c', 'v', '0', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants3 = Utility::Endianness::fourCC('c', 'v', '0', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants4 = Utility::Endianness::fourCC('c', 'v', '0', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants5 = Utility::Endianness::fourCC('c', 'v', '0', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants6 = Utility::Endianness::fourCC('c', 'v', '0', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants7 = Utility::Endianness::fourCC('c', 'v', '0', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants8 = Utility::Endianness::fourCC('c', 'v', '0', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants9 = Utility::Endianness::fourCC('c', 'v', '0', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants10 = Utility::Endianness::fourCC('c', 'v', '1', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants11 = Utility::Endianness::fourCC('c', 'v', '1', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants12 = Utility::Endianness::fourCC('c', 'v', '1', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants13 = Utility::Endianness::fourCC('c', 'v', '1', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants14 = Utility::Endianness::fourCC('c', 'v', '1', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants15 = Utility::Endianness::fourCC('c', 'v', '1', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants16 = Utility::Endianness::fourCC('c', 'v', '1', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants17 = Utility::Endianness::fourCC('c', 'v', '1', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants18 = Utility::Endianness::fourCC('c', 'v', '1', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants19 = Utility::Endianness::fourCC('c', 'v', '1', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants20 = Utility::Endianness::fourCC('c', 'v', '2', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants21 = Utility::Endianness::fourCC('c', 'v', '2', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants22 = Utility::Endianness::fourCC('c', 'v', '2', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants23 = Utility::Endianness::fourCC('c', 'v', '2', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants24 = Utility::Endianness::fourCC('c', 'v', '2', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants25 = Utility::Endianness::fourCC('c', 'v', '2', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants26 = Utility::Endianness::fourCC('c', 'v', '2', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants27 = Utility::Endianness::fourCC('c', 'v', '2', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants28 = Utility::Endianness::fourCC('c', 'v', '2', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants29 = Utility::Endianness::fourCC('c', 'v', '2', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants30 = Utility::Endianness::fourCC('c', 'v', '3', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants31 = Utility::Endianness::fourCC('c', 'v', '3', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants32 = Utility::Endianness::fourCC('c', 'v', '3', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants33 = Utility::Endianness::fourCC('c', 'v', '3', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants34 = Utility::Endianness::fourCC('c', 'v', '3', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants35 = Utility::Endianness::fourCC('c', 'v', '3', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants36 = Utility::Endianness::fourCC('c', 'v', '3', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants37 = Utility::Endianness::fourCC('c', 'v', '3', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants38 = Utility::Endianness::fourCC('c', 'v', '3', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants39 = Utility::Endianness::fourCC('c', 'v', '3', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants40 = Utility::Endianness::fourCC('c', 'v', '4', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants41 = Utility::Endianness::fourCC('c', 'v', '4', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants42 = Utility::Endianness::fourCC('c', 'v', '4', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants43 = Utility::Endianness::fourCC('c', 'v', '4', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants44 = Utility::Endianness::fourCC('c', 'v', '4', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants45 = Utility::Endianness::fourCC('c', 'v', '4', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants46 = Utility::Endianness::fourCC('c', 'v', '4', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants47 = Utility::Endianness::fourCC('c', 'v', '4', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants48 = Utility::Endianness::fourCC('c', 'v', '4', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants49 = Utility::Endianness::fourCC('c', 'v', '4', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants50 = Utility::Endianness::fourCC('c', 'v', '5', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants51 = Utility::Endianness::fourCC('c', 'v', '5', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants52 = Utility::Endianness::fourCC('c', 'v', '5', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants53 = Utility::Endianness::fourCC('c', 'v', '5', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants54 = Utility::Endianness::fourCC('c', 'v', '5', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants55 = Utility::Endianness::fourCC('c', 'v', '5', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants56 = Utility::Endianness::fourCC('c', 'v', '5', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants57 = Utility::Endianness::fourCC('c', 'v', '5', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants58 = Utility::Endianness::fourCC('c', 'v', '5', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants59 = Utility::Endianness::fourCC('c', 'v', '5', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants60 = Utility::Endianness::fourCC('c', 'v', '6', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants61 = Utility::Endianness::fourCC('c', 'v', '6', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants62 = Utility::Endianness::fourCC('c', 'v', '6', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants63 = Utility::Endianness::fourCC('c', 'v', '6', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants64 = Utility::Endianness::fourCC('c', 'v', '6', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants65 = Utility::Endianness::fourCC('c', 'v', '6', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants66 = Utility::Endianness::fourCC('c', 'v', '6', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants67 = Utility::Endianness::fourCC('c', 'v', '6', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants68 = Utility::Endianness::fourCC('c', 'v', '6', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants69 = Utility::Endianness::fourCC('c', 'v', '6', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants70 = Utility::Endianness::fourCC('c', 'v', '7', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants71 = Utility::Endianness::fourCC('c', 'v', '7', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants72 = Utility::Endianness::fourCC('c', 'v', '7', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants73 = Utility::Endianness::fourCC('c', 'v', '7', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants74 = Utility::Endianness::fourCC('c', 'v', '7', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants75 = Utility::Endianness::fourCC('c', 'v', '7', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants76 = Utility::Endianness::fourCC('c', 'v', '7', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants77 = Utility::Endianness::fourCC('c', 'v', '7', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants78 = Utility::Endianness::fourCC('c', 'v', '7', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants79 = Utility::Endianness::fourCC('c', 'v', '7', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants80 = Utility::Endianness::fourCC('c', 'v', '8', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants81 = Utility::Endianness::fourCC('c', 'v', '8', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants82 = Utility::Endianness::fourCC('c', 'v', '8', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants83 = Utility::Endianness::fourCC('c', 'v', '8', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants84 = Utility::Endianness::fourCC('c', 'v', '8', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants85 = Utility::Endianness::fourCC('c', 'v', '8', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants86 = Utility::Endianness::fourCC('c', 'v', '8', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants87 = Utility::Endianness::fourCC('c', 'v', '8', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants88 = Utility::Endianness::fourCC('c', 'v', '8', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants89 = Utility::Endianness::fourCC('c', 'v', '8', '9'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants90 = Utility::Endianness::fourCC('c', 'v', '9', '0'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants91 = Utility::Endianness::fourCC('c', 'v', '9', '1'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants92 = Utility::Endianness::fourCC('c', 'v', '9', '2'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants93 = Utility::Endianness::fourCC('c', 'v', '9', '3'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants94 = Utility::Endianness::fourCC('c', 'v', '9', '4'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants95 = Utility::Endianness::fourCC('c', 'v', '9', '5'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants96 = Utility::Endianness::fourCC('c', 'v', '9', '6'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants97 = Utility::Endianness::fourCC('c', 'v', '9', '7'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants98 = Utility::Endianness::fourCC('c', 'v', '9', '8'),
    /** @copydoc Feature::CharacterVariants1 */
    CharacterVariants99 = Utility::Endianness::fourCC('c', 'v', '9', '9'),

    /**
     * [Petite Capitals From Capitals](https://learn.microsoft.com/typography/opentype/spec/features_ae#c2pc).
     * Intended for cased scripts.
    */
    PetiteCapitalsFromCapitals = Utility::Endianness::fourCC('c', '2', 'p', 'c'),

    /**
     * [Small Capitals From Capitals](https://learn.microsoft.com/typography/opentype/spec/features_ae#c2sc).
     * Intended for cased scripts.
    */
    SmallCapitalsFromCapitals = Utility::Endianness::fourCC('c', '2', 's', 'c'),

    /**
     * [Distances](https://learn.microsoft.com/typography/opentype/spec/features_ae#dist).
     * Intended for South-Asian scripts.
    */
    Distances = Utility::Endianness::fourCC('d', 'i', 's', 't'),

    /**
     * [Discretionary Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_ae#dlig).
     * Ligation and alternate feature intended for all scripts.
    */
    DiscretionaryLigatures = Utility::Endianness::fourCC('d', 'l', 'i', 'g'),

    /**
     * [Denominators](https://learn.microsoft.com/typography/opentype/spec/features_ae#dnom).
     * Intended for digits and math.
    */
    Denominators = Utility::Endianness::fourCC('d', 'n', 'o', 'm'),

    /**
     * [Dotless Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#dtls).
     * Intended for digits and math.
    */
    DotlessForms = Utility::Endianness::fourCC('d', 't', 'l', 's'),

    /**
     * [Expert Forms](https://learn.microsoft.com/typography/opentype/spec/features_ae#expt).
     * Intended for East-Asian scripts.
    */
    ExpertForms = Utility::Endianness::fourCC('e', 'x', 'p', 't'),

    /**
     * [Final Glyph on Line Alternates](https://learn.microsoft.com/typography/opentype/spec/features_fj#falt).
     * Intended for West-Asian scripts.
    */
    FinalGlyphOnLineAlternates = Utility::Endianness::fourCC('f', 'a', 'l', 't'),

    /**
     * [Terminal Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#fina).
     * Intended for West-Asian scripts.
    */
    TerminalForms = Utility::Endianness::fourCC('f', 'i', 'n', 'a'),

    /**
     * [Terminal Forms #2](https://learn.microsoft.com/typography/opentype/spec/features_fj#fin2).
     * Intended for West-Asian scripts.
    */
    TerminalForms2 = Utility::Endianness::fourCC('f', 'i', 'n', '2'),

    /**
     * [Terminal Forms #3](https://learn.microsoft.com/typography/opentype/spec/features_fj#fin3).
     * Intended for West-Asian scripts.
    */
    TerminalForms3 = Utility::Endianness::fourCC('f', 'i', 'n', '3'),

    /**
     * [Flattened accent forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#flac).
     * Intended for digits and math.
    */
    FlattenedAccentForms = Utility::Endianness::fourCC('f', 'l', 'a', 'c'),

    /**
     * [Fractions](https://learn.microsoft.com/typography/opentype/spec/features_fj#frac).
     * Intended for digits and math.
    */
    Fractions = Utility::Endianness::fourCC('f', 'r', 'a', 'c'),

    /**
     * [Full Widths](https://learn.microsoft.com/typography/opentype/spec/features_fj#fwid).
     * Intended for East-Asian scripts.
    */
    FullWidths = Utility::Endianness::fourCC('f', 'w', 'i', 'd'),

    /**
     * [Half Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#half).
     * Intended for South-Asian scripts.
    */
    HalfForms = Utility::Endianness::fourCC('h', 'a', 'l', 'f'),

    /**
     * [Halant Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#haln).
     * Intended for South-Asian scripts.
    */
    HalantForms = Utility::Endianness::fourCC('h', 'a', 'l', 'n'),

    /**
     * [Alternate Half Widths](https://learn.microsoft.com/typography/opentype/spec/features_fj#halt).
     * Intended for East-Asian scripts.
    */
    AlternateHalfWidths = Utility::Endianness::fourCC('h', 'a', 'l', 't'),

    /**
     * [Historical Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#hist).
     * Ligation and alternate feature intended for all scripts.
    */
    HistoricalForms = Utility::Endianness::fourCC('h', 'i', 's', 't'),

    /**
     * [Horizontal Kana Alternates](https://learn.microsoft.com/typography/opentype/spec/features_fj#hkna).
     * Intended for East-Asian scripts.
    */
    HorizontalKanaAlternates = Utility::Endianness::fourCC('h', 'k', 'n', 'a'),

    /**
     * [Historical Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_fj#hlig).
     * Ligation and alternate feature intended for all scripts.
    */
    HistoricalLigatures = Utility::Endianness::fourCC('h', 'l', 'i', 'g'),

    /**
     * [Hangul](https://learn.microsoft.com/typography/opentype/spec/features_fj#hngl).
     * Intended for East-Asian scripts.
    */
    Hangul = Utility::Endianness::fourCC('h', 'n', 'g', 'l'),

    /**
     * [Hojo Kanji Forms (JIS X 0212-1990 Kanji Forms)](https://learn.microsoft.com/typography/opentype/spec/features_fj#hojo).
     * Intended for East-Asian scripts.
    */
    HojoKanjiForms = Utility::Endianness::fourCC('h', 'o', 'j', 'o'),

    /**
     * [Half Widths](https://learn.microsoft.com/typography/opentype/spec/features_fj#hwid).
     * Intended for East-Asian scripts.
    */
    HalfWidths = Utility::Endianness::fourCC('h', 'w', 'i', 'd'),

    /**
     * [Initial Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#init).
     * Intended for West-Asian scripts.
    */
    InitialForms = Utility::Endianness::fourCC('i', 'n', 'i', 't'),

    /**
     * [Isolated Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#isol).
     * Intended for West-Asian scripts.
    */
    IsolatedForms = Utility::Endianness::fourCC('i', 's', 'o', 'l'),

    /**
     * [Italics](https://learn.microsoft.com/typography/opentype/spec/features_fj#ital).
     * Intended for cased scripts.
    */
    Italics = Utility::Endianness::fourCC('i', 't', 'a', 'l'),

    /**
     * [Justification Alternates](https://learn.microsoft.com/typography/opentype/spec/features_fj#jalt).
     * Intended for West-Asian scripts.
    */
    JustificationAlternates = Utility::Endianness::fourCC('j', 'a', 'l', 't'),

    /**
     * [JIS78 Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#jp78).
     * Intended for East-Asian scripts.
    */
    Jis78Forms = Utility::Endianness::fourCC('j', 'p', '7', '8'),

    /**
     * [JIS83 Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#jp83).
     * Intended for East-Asian scripts.
    */
    Jis83Forms = Utility::Endianness::fourCC('j', 'p', '8', '3'),

    /**
     * [JIS90 Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#jp90).
     * Intended for East-Asian scripts.
    */
    Jis90Forms = Utility::Endianness::fourCC('j', 'p', '9', '0'),

    /**
     * [JIS2004 Forms](https://learn.microsoft.com/typography/opentype/spec/features_fj#jp04).
     * Intended for East-Asian scripts.
    */
    Jis2004Forms = Utility::Endianness::fourCC('j', 'p', '0', '4'),

    /**
     * [Kerning](https://learn.microsoft.com/typography/opentype/spec/features_ko#kern).
     * Positioning feature intended for all scripts.
    */
    Kerning = Utility::Endianness::fourCC('k', 'e', 'r', 'n'),

    /**
     * [Left Bounds](https://learn.microsoft.com/typography/opentype/spec/features_ko#lfbd).
     * Positioning feature intended for all scripts.
    */
    LeftBounds = Utility::Endianness::fourCC('l', 'f', 'b', 'd'),

    /**
     * [Standard Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_ko#liga).
     * Ligation and alternate feature intended for all scripts.
    */
    StandardLigatures = Utility::Endianness::fourCC('l', 'i', 'g', 'a'),

    /**
     * [Leading Jamo Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#ljmo).
     * Intended for East-Asian scripts.
    */
    LeadingJamoForms = Utility::Endianness::fourCC('l', 'j', 'm', 'o'),

    /**
     * [Lining Figures](https://learn.microsoft.com/typography/opentype/spec/features_ko#lnum).
     * Intended for digits and math.
    */
    LiningFigures = Utility::Endianness::fourCC('l', 'n', 'u', 'm'),

    /**
     * [Localized Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#locl).
     * Ligation and alternate feature intended for all scripts.
    */
    LocalizedForms = Utility::Endianness::fourCC('l', 'o', 'c', 'l'),

    /**
     * [Left-to-right alternates](https://learn.microsoft.com/typography/opentype/spec/features_ko#ltra).
     * Feature depending on writing direction.
    */
    LeftToRightAlternates = Utility::Endianness::fourCC('l', 't', 'r', 'a'),

    /**
     * [Left-to-right mirrored forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#ltrm).
     * Feature depending on writing direction.
    */
    LeftToRightMirroredForms = Utility::Endianness::fourCC('l', 't', 'r', 'm'),

    /**
     * [Mark Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ko#mark).
     * Positioning feature intended for all scripts.
    */
    MarkPositioning = Utility::Endianness::fourCC('m', 'a', 'r', 'k'),

    /**
     * [Medial Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#medi).
     * Intended for West-Asian scripts.
    */
    MedialForms = Utility::Endianness::fourCC('m', 'e', 'd', 'i'),

    /**
     * [Medial Forms #2](https://learn.microsoft.com/typography/opentype/spec/features_ko#med2).
     * Intended for West-Asian scripts.
    */
    MedialForms2 = Utility::Endianness::fourCC('m', 'e', 'd', '2'),

    /**
     * [Mathematical Greek](https://learn.microsoft.com/typography/opentype/spec/features_ko#mgrk).
     * Intended for digits and math.
    */
    MathematicalGreek = Utility::Endianness::fourCC('m', 'g', 'r', 'k'),

    /**
     * [Mark to Mark Positioning](https://learn.microsoft.com/typography/opentype/spec/features_ko#mkmk).
     * Positioning feature intended for all scripts.
    */
    MarkToMarkPositioning = Utility::Endianness::fourCC('m', 'k', 'm', 'k'),

    /**
     * [Mark Positioning via Substitution](https://learn.microsoft.com/typography/opentype/spec/features_ko#mset).
     * Intended for West-Asian scripts.
    */
    MarkPositioningViaSubstitution = Utility::Endianness::fourCC('m', 's', 'e', 't'),

    /**
     * [Alternate Annotation Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#nalt).
     * Ligation and alternate feature intended for all scripts.
    */
    AlternateAnnotationForms = Utility::Endianness::fourCC('n', 'a', 'l', 't'),

    /**
     * [NLC Kanji Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#nlck).
     * Intended for East-Asian scripts.
    */
    NlcKanjiForms = Utility::Endianness::fourCC('n', 'l', 'c', 'k'),

    /**
     * [Nukta Forms](https://learn.microsoft.com/typography/opentype/spec/features_ko#nukt).
     * Intended for South-Asian scripts.
    */
    NuktaForms = Utility::Endianness::fourCC('n', 'u', 'k', 't'),

    /**
     * [Numerators](https://learn.microsoft.com/typography/opentype/spec/features_ko#numr).
     * Intended for digits and math.
    */
    Numerators = Utility::Endianness::fourCC('n', 'u', 'm', 'r'),

    /**
     * [Oldstyle Figures](https://learn.microsoft.com/typography/opentype/spec/features_ko#onum).
     * Intended for digits and math.
    */
    OldstyleFigures = Utility::Endianness::fourCC('o', 'n', 'u', 'm'),

    /**
     * [Optical Bounds](https://learn.microsoft.com/typography/opentype/spec/features_ko#opbd).
     * Positioning feature intended for all scripts.
    */
    OpticalBounds = Utility::Endianness::fourCC('o', 'p', 'b', 'd'),

    /**
     * [Ordinals](https://learn.microsoft.com/typography/opentype/spec/features_ko#ordn).
     * Intended for cased scripts.
    */
    Ordinals = Utility::Endianness::fourCC('o', 'r', 'd', 'n'),

    /**
     * [Ornaments](https://learn.microsoft.com/typography/opentype/spec/features_ko#ornm).
     * Special feature intended for all scripts.
    */
    Ornaments = Utility::Endianness::fourCC('o', 'r', 'n', 'm'),

    /**
     * [Proportional Alternate Widths](https://learn.microsoft.com/typography/opentype/spec/features_pt#palt).
     * Intended for East-Asian scripts.
    */
    ProportionalAlternateWidths = Utility::Endianness::fourCC('p', 'a', 'l', 't'),

    /**
     * [Petite Capitals](https://learn.microsoft.com/typography/opentype/spec/features_pt#pcap).
     * Intended for cased scripts.
    */
    PetiteCapitals = Utility::Endianness::fourCC('p', 'c', 'a', 'p'),

    /**
     * [Proportional Kana](https://learn.microsoft.com/typography/opentype/spec/features_pt#pkna).
     * Intended for East-Asian scripts.
    */
    ProportionalKana = Utility::Endianness::fourCC('p', 'k', 'n', 'a'),

    /**
     * [Proportional Figures](https://learn.microsoft.com/typography/opentype/spec/features_pt#pnum).
     * Intended for digits and math.
    */
    ProportionalFigures = Utility::Endianness::fourCC('p', 'n', 'u', 'm'),

    /**
     * [Pre-Base Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#pref).
     * Intended for South-Asian scripts.
    */
    PreBaseForms = Utility::Endianness::fourCC('p', 'r', 'e', 'f'),

    /**
     * [Pre-base Substitutions](https://learn.microsoft.com/typography/opentype/spec/features_pt#pres).
     * Intended for South-Asian scripts.
    */
    PreBaseSubstitutions = Utility::Endianness::fourCC('p', 'r', 'e', 's'),

    /**
     * [Post-base Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#pstf).
     * Intended for South-Asian scripts.
    */
    PostBaseForms = Utility::Endianness::fourCC('p', 's', 't', 'f'),

    /**
     * [Post-base Substitutions](https://learn.microsoft.com/typography/opentype/spec/features_pt#psts).
     * Intended for South-Asian scripts.
    */
    PostBaseSubstitutions = Utility::Endianness::fourCC('p', 's', 't', 's'),

    /**
     * [Proportional Widths](https://learn.microsoft.com/typography/opentype/spec/features_pt#pwid).
     * Intended for East-Asian scripts.
    */
    ProportionalWidths = Utility::Endianness::fourCC('p', 'w', 'i', 'd'),

    /**
     * [Quarter Widths](https://learn.microsoft.com/typography/opentype/spec/features_pt#qwid).
     * Intended for East-Asian scripts.
    */
    QuarterWidths = Utility::Endianness::fourCC('q', 'w', 'i', 'd'),

    /**
     * [Randomize](https://learn.microsoft.com/typography/opentype/spec/features_pt#rand).
     * Ligation and alternate feature intended for all scripts.
    */
    Randomize = Utility::Endianness::fourCC('r', 'a', 'n', 'd'),

    /**
     * [Required Contextual Alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#rclt).
     * Intended for West-Asian scripts.
    */
    RequiredContextualAlternates = Utility::Endianness::fourCC('r', 'c', 'l', 't'),

    /**
     * [Rakar Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#rkrf).
     * Intended for South-Asian scripts.
    */
    RakarForms = Utility::Endianness::fourCC('r', 'k', 'r', 'f'),

    /**
     * [Required Ligatures](https://learn.microsoft.com/typography/opentype/spec/features_pt#rlig).
     * Intended for West-Asian scripts.
    */
    RequiredLigatures = Utility::Endianness::fourCC('r', 'l', 'i', 'g'),

    /**
     * [Reph Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#rphf).
     * Intended for South-Asian scripts.
    */
    RephForms = Utility::Endianness::fourCC('r', 'p', 'h', 'f'),

    /**
     * [Right Bounds](https://learn.microsoft.com/typography/opentype/spec/features_pt#rtbd).
     * Positioning feature intended for all scripts.
    */
    RightBounds = Utility::Endianness::fourCC('r', 't', 'b', 'd'),

    /**
     * [Right-to-left alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#rtla).
     * Feature depending on writing direction.
    */
    RightToLeftAlternates = Utility::Endianness::fourCC('r', 't', 'l', 'a'),

    /**
     * [Right-to-left mirrored forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#rtlm).
     * Feature depending on writing direction.
    */
    RightToLeftMirroredForms = Utility::Endianness::fourCC('r', 't', 'l', 'm'),

    /**
     * [Ruby Notation Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#ruby).
     * Intended for East-Asian scripts.
    */
    RubyNotationForms = Utility::Endianness::fourCC('r', 'u', 'b', 'y'),

    /**
     * [Required Variation Alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#rvrn).
     * Ligation and alternate feature intended for all scripts.
    */
    RequiredVariationAlternates = Utility::Endianness::fourCC('r', 'v', 'r', 'n'),

    /**
     * [Stylistic Alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#salt).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticAlternates = Utility::Endianness::fourCC('s', 'a', 'l', 't'),

    /**
     * [Scientific Inferiors](https://learn.microsoft.com/typography/opentype/spec/features_pt#sinf).
     * Intended for digits and math.
    */
    ScientificInferiors = Utility::Endianness::fourCC('s', 'i', 'n', 'f'),

    /**
     * [Optical Size](https://learn.microsoft.com/typography/opentype/spec/features_pt#size).
     * Special feature intended for all scripts.
    */
    OpticalSize = Utility::Endianness::fourCC('s', 'i', 'z', 'e'),

    /**
     * [Small Capitals](https://learn.microsoft.com/typography/opentype/spec/features_pt#smcp).
     * Intended for cased scripts.
    */
    SmallCapitals = Utility::Endianness::fourCC('s', 'm', 'c', 'p'),

    /**
     * [Simplified Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#smpl).
     * Intended for East-Asian scripts.
    */
    SimplifiedForms = Utility::Endianness::fourCC('s', 'm', 'p', 'l'),

    /**
     * [Stylistic Set 1](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet1 = Utility::Endianness::fourCC('s', 's', '0', '1'),

    /**
     * [Stylistic Set 2](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet2 = Utility::Endianness::fourCC('s', 's', '0', '2'),

    /**
     * [Stylistic Set 3](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet3 = Utility::Endianness::fourCC('s', 's', '0', '3'),

    /**
     * [Stylistic Set 4](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet4 = Utility::Endianness::fourCC('s', 's', '0', '4'),

    /**
     * [Stylistic Set 5](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet5 = Utility::Endianness::fourCC('s', 's', '0', '5'),

    /**
     * [Stylistic Set 6](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet6 = Utility::Endianness::fourCC('s', 's', '0', '6'),

    /**
     * [Stylistic Set 7](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet7 = Utility::Endianness::fourCC('s', 's', '0', '7'),

    /**
     * [Stylistic Set 8](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet8 = Utility::Endianness::fourCC('s', 's', '0', '8'),

    /**
     * [Stylistic Set 9](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet9 = Utility::Endianness::fourCC('s', 's', '0', '9'),

    /**
     * [Stylistic Set 10](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet10 = Utility::Endianness::fourCC('s', 's', '1', '0'),

    /**
     * [Stylistic Set 11](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet11 = Utility::Endianness::fourCC('s', 's', '1', '1'),

    /**
     * [Stylistic Set 12](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet12 = Utility::Endianness::fourCC('s', 's', '1', '2'),

    /**
     * [Stylistic Set 13](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet13 = Utility::Endianness::fourCC('s', 's', '1', '3'),

    /**
     * [Stylistic Set 14](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet14 = Utility::Endianness::fourCC('s', 's', '1', '4'),

    /**
     * [Stylistic Set 15](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet15 = Utility::Endianness::fourCC('s', 's', '1', '5'),

    /**
     * [Stylistic Set 16](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet16 = Utility::Endianness::fourCC('s', 's', '1', '6'),

    /**
     * [Stylistic Set 17](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet17 = Utility::Endianness::fourCC('s', 's', '1', '7'),

    /**
     * [Stylistic Set 18](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet18 = Utility::Endianness::fourCC('s', 's', '1', '8'),

    /**
     * [Stylistic Set 19](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet19 = Utility::Endianness::fourCC('s', 's', '1', '9'),

    /**
     * [Stylistic Set 20](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssxx).
     * Ligation and alternate feature intended for all scripts.
    */
    StylisticSet20 = Utility::Endianness::fourCC('s', 's', '2', '0'),

    /**
     * [Math Script Style Alternates](https://learn.microsoft.com/typography/opentype/spec/features_pt#ssty).
     * Intended for digits and math.
    */
    MathScriptStyleAlternates = Utility::Endianness::fourCC('s', 's', 't', 'y'),

    /**
     * [Stretching Glyph Decomposition](https://learn.microsoft.com/typography/opentype/spec/features_pt#stch).
     * Intended for West-Asian scripts.
    */
    StretchingGlyphDecomposition = Utility::Endianness::fourCC('s', 't', 'c', 'h'),

    /**
     * [Subscript](https://learn.microsoft.com/typography/opentype/spec/features_pt#subs).
     * Ligation and alternate feature intended for all scripts.
    */
    Subscript = Utility::Endianness::fourCC('s', 'u', 'b', 's'),

    /**
     * [Superscript](https://learn.microsoft.com/typography/opentype/spec/features_pt#sups).
     * Ligation and alternate feature intended for all scripts.
    */
    Superscript = Utility::Endianness::fourCC('s', 'u', 'p', 's'),

    /**
     * [Swash](https://learn.microsoft.com/typography/opentype/spec/features_pt#swsh).
     * Ligation and alternate feature intended for all scripts.
    */
    Swash = Utility::Endianness::fourCC('s', 'w', 's', 'h'),

    /**
     * [Titling](https://learn.microsoft.com/typography/opentype/spec/features_pt#titl).
     * Ligation and alternate feature intended for all scripts.
    */
    Titling = Utility::Endianness::fourCC('t', 'i', 't', 'l'),

    /**
     * [Trailing Jamo Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#tjmo).
     * Intended for East-Asian scripts.
    */
    TrailingJamoForms = Utility::Endianness::fourCC('t', 'j', 'm', 'o'),

    /**
     * [Traditional Name Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#tnam).
     * Intended for East-Asian scripts.
    */
    TraditionalNameForms = Utility::Endianness::fourCC('t', 'n', 'a', 'm'),

    /**
     * [Tabular Figures](https://learn.microsoft.com/typography/opentype/spec/features_pt#tnum).
     * Intended for digits and math.
    */
    TabularFigures = Utility::Endianness::fourCC('t', 'n', 'u', 'm'),

    /**
     * [Traditional Forms](https://learn.microsoft.com/typography/opentype/spec/features_pt#trad).
     * Intended for East-Asian scripts.
    */
    TraditionalForms = Utility::Endianness::fourCC('t', 'r', 'a', 'd'),

    /**
     * [Third Widths](https://learn.microsoft.com/typography/opentype/spec/features_pt#twid).
     * Intended for East-Asian scripts.
    */
    ThirdWidths = Utility::Endianness::fourCC('t', 'w', 'i', 'd'),

    /**
     * [Unicase](https://learn.microsoft.com/typography/opentype/spec/features_uz#unic).
     * Intended for cased scripts.
    */
    Unicase = Utility::Endianness::fourCC('u', 'n', 'i', 'c'),

    /**
     * [Alternate Vertical Metrics](https://learn.microsoft.com/typography/opentype/spec/features_uz#valt).
     * Feature depending on writing direction.
    */
    AlternateVerticalMetrics = Utility::Endianness::fourCC('v', 'a', 'l', 't'),

    /**
     * [Vattu Variants](https://learn.microsoft.com/typography/opentype/spec/features_uz#vatu).
     * Intended for South-Asian scripts.
    */
    VattuVariants = Utility::Endianness::fourCC('v', 'a', 't', 'u'),

    /**
     * [Vertical Contextual Half-width Spacing](https://learn.microsoft.com/typography/opentype/spec/features_uz#vchw).
     * Intended for East-Asian scripts.
    */
    VerticalContextualHalfWidthSpacing = Utility::Endianness::fourCC('v', 'c', 'h', 'w'),

    /**
     * [Vertical Writing](https://learn.microsoft.com/typography/opentype/spec/features_uz#vert).
     * Feature depending on writing direction.
    */
    VerticalWriting = Utility::Endianness::fourCC('v', 'e', 'r', 't'),

    /**
     * [Alternate Vertical Half Metrics](https://learn.microsoft.com/typography/opentype/spec/features_uz#vhal).
     * Feature depending on writing direction.
    */
    AlternateVerticalHalfMetrics = Utility::Endianness::fourCC('v', 'h', 'a', 'l'),

    /**
     * [Vowel Jamo Forms](https://learn.microsoft.com/typography/opentype/spec/features_uz#vjmo).
     * Intended for East-Asian scripts.
    */
    VowelJamoForms = Utility::Endianness::fourCC('v', 'j', 'm', 'o'),

    /**
     * [Vertical Kana Alternates](https://learn.microsoft.com/typography/opentype/spec/features_uz#vkna).
     * Intended for East-Asian scripts.
    */
    VerticalKanaAlternates = Utility::Endianness::fourCC('v', 'k', 'n', 'a'),

    /**
     * [Vertical Kerning](https://learn.microsoft.com/typography/opentype/spec/features_uz#vkrn).
     * Feature depending on writing direction.
    */
    VerticalKerning = Utility::Endianness::fourCC('v', 'k', 'r', 'n'),

    /**
     * [Proportional Alternate Vertical Metrics](https://learn.microsoft.com/typography/opentype/spec/features_uz#vpal).
     * Feature depending on writing direction.
    */
    ProportionalAlternateVerticalMetrics = Utility::Endianness::fourCC('v', 'p', 'a', 'l'),

    /**
     * [Vertical Alternates and Rotation](https://learn.microsoft.com/typography/opentype/spec/features_uz#vrt2).
     * Feature depending on writing direction.
    */
    VerticalAlternatesAndRotation = Utility::Endianness::fourCC('v', 'r', 't', '2'),

    /**
     * [Vertical Alternates for Rotation](https://learn.microsoft.com/typography/opentype/spec/features_uz#vrtr).
     * Feature depending on writing direction.
    */
    VerticalAlternatesForRotation = Utility::Endianness::fourCC('v', 'r', 't', 'r'),

    /**
     * [Slashed Zero](https://learn.microsoft.com/typography/opentype/spec/features_uz#zero).
     * Intended for digits and math.
    */
    SlashedZero = Utility::Endianness::fourCC('z', 'e', 'r', 'o'),
};

/** @debugoperatorenum{Feature} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, Feature value);

/**
@brief Create a @ref Feature value from a four-character code
@m_since_latest

Delegates to @ref Utility::Endianness::fourCC(). Doesn't perform any validity
check on the input, i.e. it's possible to create a code that isn't listed in
the [OpenType feature registry](https://learn.microsoft.com/typography/opentype/spec/featurelist)
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
