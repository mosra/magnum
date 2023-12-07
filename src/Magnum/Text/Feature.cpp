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

#include "Feature.h"

#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Text/Implementation/printFourCC.h"

namespace Magnum { namespace Text {

Debug& operator<<(Debug& debug, const Feature value) {
    debug << "Text::Feature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case Feature::v: return debug << "::" #v;
        _c(AccessAllAlternates)
        _c(AboveBaseForms)
        _c(AboveBaseMarkPositioning)
        _c(AboveBaseSubstitutions)
        _c(AlternativeFractions)
        _c(Akhand)
        _c(BelowBaseForms)
        _c(BelowBaseMarkPositioning)
        _c(BelowBaseSubstitutions)
        _c(ContextualAlternates)
        _c(CaseSensitiveForms)
        _c(GlyphCompositionDecomposition)
        _c(ConjunctFormAfterRo)
        _c(ContextualHalfWidthSpacing)
        _c(ConjunctForms)
        _c(ContextualLigatures)
        _c(CenteredCjkPunctuation)
        _c(CapitalSpacing)
        _c(ContextualSwash)
        _c(CursivePositioning)
        _c(CharacterVariants1)
        _c(CharacterVariants2)
        _c(CharacterVariants3)
        _c(CharacterVariants4)
        _c(CharacterVariants5)
        _c(CharacterVariants6)
        _c(CharacterVariants7)
        _c(CharacterVariants8)
        _c(CharacterVariants9)
        _c(CharacterVariants10)
        _c(CharacterVariants11)
        _c(CharacterVariants12)
        _c(CharacterVariants13)
        _c(CharacterVariants14)
        _c(CharacterVariants15)
        _c(CharacterVariants16)
        _c(CharacterVariants17)
        _c(CharacterVariants18)
        _c(CharacterVariants19)
        _c(CharacterVariants20)
        _c(CharacterVariants21)
        _c(CharacterVariants22)
        _c(CharacterVariants23)
        _c(CharacterVariants24)
        _c(CharacterVariants25)
        _c(CharacterVariants26)
        _c(CharacterVariants27)
        _c(CharacterVariants28)
        _c(CharacterVariants29)
        _c(CharacterVariants30)
        _c(CharacterVariants31)
        _c(CharacterVariants32)
        _c(CharacterVariants33)
        _c(CharacterVariants34)
        _c(CharacterVariants35)
        _c(CharacterVariants36)
        _c(CharacterVariants37)
        _c(CharacterVariants38)
        _c(CharacterVariants39)
        _c(CharacterVariants40)
        _c(CharacterVariants41)
        _c(CharacterVariants42)
        _c(CharacterVariants43)
        _c(CharacterVariants44)
        _c(CharacterVariants45)
        _c(CharacterVariants46)
        _c(CharacterVariants47)
        _c(CharacterVariants48)
        _c(CharacterVariants49)
        _c(CharacterVariants50)
        _c(CharacterVariants51)
        _c(CharacterVariants52)
        _c(CharacterVariants53)
        _c(CharacterVariants54)
        _c(CharacterVariants55)
        _c(CharacterVariants56)
        _c(CharacterVariants57)
        _c(CharacterVariants58)
        _c(CharacterVariants59)
        _c(CharacterVariants60)
        _c(CharacterVariants61)
        _c(CharacterVariants62)
        _c(CharacterVariants63)
        _c(CharacterVariants64)
        _c(CharacterVariants65)
        _c(CharacterVariants66)
        _c(CharacterVariants67)
        _c(CharacterVariants68)
        _c(CharacterVariants69)
        _c(CharacterVariants70)
        _c(CharacterVariants71)
        _c(CharacterVariants72)
        _c(CharacterVariants73)
        _c(CharacterVariants74)
        _c(CharacterVariants75)
        _c(CharacterVariants76)
        _c(CharacterVariants77)
        _c(CharacterVariants78)
        _c(CharacterVariants79)
        _c(CharacterVariants80)
        _c(CharacterVariants81)
        _c(CharacterVariants82)
        _c(CharacterVariants83)
        _c(CharacterVariants84)
        _c(CharacterVariants85)
        _c(CharacterVariants86)
        _c(CharacterVariants87)
        _c(CharacterVariants88)
        _c(CharacterVariants89)
        _c(CharacterVariants90)
        _c(CharacterVariants91)
        _c(CharacterVariants92)
        _c(CharacterVariants93)
        _c(CharacterVariants94)
        _c(CharacterVariants95)
        _c(CharacterVariants96)
        _c(CharacterVariants97)
        _c(CharacterVariants98)
        _c(CharacterVariants99)
        _c(PetiteCapitalsFromCapitals)
        _c(SmallCapitalsFromCapitals)
        _c(Distances)
        _c(DiscretionaryLigatures)
        _c(Denominators)
        _c(DotlessForms)
        _c(ExpertForms)
        _c(FinalGlyphOnLineAlternates)
        _c(TerminalForms)
        _c(TerminalForms2)
        _c(TerminalForms3)
        _c(FlattenedAccentForms)
        _c(Fractions)
        _c(FullWidths)
        _c(HalfForms)
        _c(HalantForms)
        _c(AlternateHalfWidths)
        _c(HistoricalForms)
        _c(HorizontalKanaAlternates)
        _c(HistoricalLigatures)
        _c(Hangul)
        _c(HojoKanjiForms)
        _c(HalfWidths)
        _c(InitialForms)
        _c(IsolatedForms)
        _c(Italics)
        _c(JustificationAlternates)
        _c(Jis78Forms)
        _c(Jis83Forms)
        _c(Jis90Forms)
        _c(Jis2004Forms)
        _c(Kerning)
        _c(LeftBounds)
        _c(StandardLigatures)
        _c(LeadingJamoForms)
        _c(LiningFigures)
        _c(LocalizedForms)
        _c(LeftToRightAlternates)
        _c(LeftToRightMirroredForms)
        _c(MarkPositioning)
        _c(MedialForms)
        _c(MedialForms2)
        _c(MathematicalGreek)
        _c(MarkToMarkPositioning)
        _c(MarkPositioningViaSubstitution)
        _c(AlternateAnnotationForms)
        _c(NlcKanjiForms)
        _c(NuktaForms)
        _c(Numerators)
        _c(OldstyleFigures)
        _c(OpticalBounds)
        _c(Ordinals)
        _c(Ornaments)
        _c(ProportionalAlternateWidths)
        _c(PetiteCapitals)
        _c(ProportionalKana)
        _c(ProportionalFigures)
        _c(PreBaseForms)
        _c(PreBaseSubstitutions)
        _c(PostBaseForms)
        _c(PostBaseSubstitutions)
        _c(ProportionalWidths)
        _c(QuarterWidths)
        _c(Randomize)
        _c(RequiredContextualAlternates)
        _c(RakarForms)
        _c(RequiredLigatures)
        _c(RephForms)
        _c(RightBounds)
        _c(RightToLeftAlternates)
        _c(RightToLeftMirroredForms)
        _c(RubyNotationForms)
        _c(RequiredVariationAlternates)
        _c(StylisticAlternates)
        _c(ScientificInferiors)
        _c(OpticalSize)
        _c(SmallCapitals)
        _c(SimplifiedForms)
        _c(StylisticSet1)
        _c(StylisticSet2)
        _c(StylisticSet3)
        _c(StylisticSet4)
        _c(StylisticSet5)
        _c(StylisticSet6)
        _c(StylisticSet7)
        _c(StylisticSet8)
        _c(StylisticSet9)
        _c(StylisticSet10)
        _c(StylisticSet11)
        _c(StylisticSet12)
        _c(StylisticSet13)
        _c(StylisticSet14)
        _c(StylisticSet15)
        _c(StylisticSet16)
        _c(StylisticSet17)
        _c(StylisticSet18)
        _c(StylisticSet19)
        _c(StylisticSet20)
        _c(MathScriptStyleAlternates)
        _c(StretchingGlyphDecomposition)
        _c(Subscript)
        _c(Superscript)
        _c(Swash)
        _c(Titling)
        _c(TrailingJamoForms)
        _c(TraditionalNameForms)
        _c(TabularFigures)
        _c(TraditionalForms)
        _c(ThirdWidths)
        _c(Unicase)
        _c(AlternateVerticalMetrics)
        _c(VattuVariants)
        _c(VerticalContextualHalfWidthSpacing)
        _c(VerticalWriting)
        _c(AlternateVerticalHalfMetrics)
        _c(VowelJamoForms)
        _c(VerticalKanaAlternates)
        _c(VerticalKerning)
        _c(ProportionalAlternateVerticalMetrics)
        _c(VerticalAlternatesAndRotation)
        _c(VerticalAlternatesForRotation)
        _c(SlashedZero)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return Implementation::printFourCC(debug, UnsignedInt(value));
}

Feature feature(Containers::StringView fourCC) {
    CORRADE_ASSERT(fourCC.size() == 4,
        "Text::feature(): expected a four-character code, got" << fourCC, {});
    return feature(fourCC[0], fourCC[1], fourCC[2], fourCC[3]);
}

}}