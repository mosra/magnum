/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

/*
    Generator for plots in the Easing namespace. Run the executable and it'll
    generate a shitload of files in CWD. Copy these to doc/snippets/, replacing
    the previous, and regenerate the docs using dox2html5.py.

    Note that the SVGs are crafted for inline HTML, so they won't get
    recognized by vector editors. Add the XML preamble

        <?xml version="1.0" encoding="UTF-8" standalone="no"?>

    and the

        xmlns="http://www.w3.org/2000/svg"

    attribute to the <svg> element if you'd ever need that.
*/

#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Animation/Easing.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

namespace {

constexpr Int Points = 96;
constexpr Int PointsOutside = 8;
constexpr Int EndMarkerSize = 12;
constexpr Int ExtraMargin = 32;
constexpr Vector2 Size{224, 128};
constexpr Vector2 Border{16, 16};
constexpr Int CircleRadius = 2;

constexpr Int ThumbPoints = 64;
constexpr Int ThumbEndMarkerSize = 8;
constexpr Vector2 ThumbSize{128, 128};
constexpr Vector2 ThumbBorder{0, 32};

void generateThumb(const std::string& file, Float(*function)(Float)) {
    std::string out;

    Range2D viewBox{{}, ThumbSize};
    Range2D contentBox{ThumbBorder, ThumbSize - ThumbBorder};

    Utility::formatInto(out, out.size(), R"(<svg class="m-image m-nopadb" style="width: {2}px; height: {3}px; margin-top: -16px; margin-bottom: -16px;" viewBox="{0} {1} {2} {3}">
<path fill="none" stroke="#405363" stroke-width="1.0" d="
  M{4} {} L{} {} M{} {} L{} {}
  M{} {} l{} {} M{} {} l{} {}"/>
<path fill="none" stroke="#dcdcdc" stroke-width="1.5" d=")",
        viewBox.left(), viewBox.bottom(), viewBox.sizeX(), viewBox.sizeY(),
        contentBox.left(), contentBox.bottom(), contentBox.right(), contentBox.bottom(),
        contentBox.left(), contentBox.top(), contentBox.right(), contentBox.top(),
        contentBox.left(), contentBox.top() - ThumbEndMarkerSize/2, 0, ThumbEndMarkerSize,
        contentBox.right(), contentBox.bottom() - ThumbEndMarkerSize/2, 0, ThumbEndMarkerSize);

    for(Int i = 0; i != ThumbPoints; ++i) {
        const Vector2 point = Math::lerp(ThumbBorder, ThumbSize - ThumbBorder, Vector2{i/Float(ThumbPoints - 1), function(i/Float(ThumbPoints - 1))});
        Utility::formatInto(out, out.size(), i ? " L{} {}" : "M{} {}", point.x(), ThumbSize.y() - point.y());
    }

    Utility::formatInto(out, out.size(), R"("/>
</svg>
)");

    Utility::Directory::writeString("easings-" + file + "-thumb.svg", out);
}

void generate(const std::string& file, Float(*function)(Float), std::initializer_list<Float(*)(Float)> related = {}, const CubicBezier2D& bezier = {}, const Color3& colorBefore = 0xcd3431_srgbf, const Color3 colorAfter = 0xcd3431_srgbf, bool extraMargin = false) {
    std::string out;

    Vector2 size = Size;
    Vector2 border = Border;
    std::string extraStyle;
    if(extraMargin) {
        size.y() += 2*ExtraMargin;
        border.y() += ExtraMargin;
        extraStyle = " margin-top: -24px; margin-bottom: -8px;";
    }

    Range2D viewBox{{}, size};
    Range2D contentBox{border, size - border};

    /* Background, extrapolated points */
    Utility::formatInto(out, out.size(), R"(<svg class="m-image" style="width: {2}px; height: {3}px;{4}" viewBox="{0} {1} {2} {3}">
<path fill="none" stroke="#405363" stroke-width="1.0" d="
  M{5} {} L{} {} M{} {} L{} {}
  M{} {} l{} {} M{} {} l{} {}"/>
<path fill="none" stroke="#{:.6x}" stroke-width="1.5" d=")",
        viewBox.left(), viewBox.bottom(), viewBox.sizeX(), viewBox.sizeY(),
        extraStyle,
        contentBox.left(), contentBox.bottom(), contentBox.right(), contentBox.bottom(),
        contentBox.left(), contentBox.top(), contentBox.right(), contentBox.top(),
        contentBox.left(), contentBox.top() - EndMarkerSize/2, 0, EndMarkerSize,
        contentBox.right(), contentBox.bottom() - EndMarkerSize/2, 0, EndMarkerSize,
        colorBefore.toSrgbInt());

    for(Int i = 1 - PointsOutside; i <= 0; ++i) {
        const Vector2 point = Math::lerp(border, size - border, Vector2{i/Float(Points - 1), function(i/Float(Points - 1))});
        Utility::formatInto(out, out.size(), i != 1 - PointsOutside ? " L{} {}" : "M{} {}", point.x(), size.y() - point.y());
    }

    Utility::formatInto(out, out.size(), R"("/>
<path fill="none" stroke="#{:.6x}" stroke-width="1.5" d=")",
        colorAfter.toSrgbInt());

    for(Int i = Points - 1; i < Points + PointsOutside - 1; ++i) {
        const Vector2 point = Math::lerp(border, size - border, Vector2{i/Float(Points - 1), function(i/Float(Points - 1))});
        Utility::formatInto(out, out.size(), i != Points - 1 ? " L{} {}" : "M{} {}", point.x(), size.y() - point.y());
    }

    Utility::formatInto(out, out.size(), R"("/>
)");

    /* Related functions */
    auto print = [&](const Color3& color, Float strokeWidth, Float(*function)(Float)) {
        Utility::formatInto(out, out.size(), R"(<path fill="none" stroke="#{:.6x}" stroke-width="{}" d=")", color.toSrgbInt(), strokeWidth);

        for(Int i = 0; i != Points; ++i) {
            const Vector2 point = Math::lerp(border, size - border, Vector2{i/Float(Points - 1), function(i/Float(Points - 1))});
            Utility::formatInto(out, out.size(), i ? " L{} {}" : "M{} {}", point.x(), size.y() - point.y());
        }

        Utility::formatInto(out, out.size(), R"("/>
)");
    };

    for(auto fn: related) print(0x747474_srgbf, 1.0f, fn);

    /* Bezier representation, if any */
    if(!bezier[3].isZero()) {
        CubicBezier2D transformed{bezier[0]*contentBox.size() + border,
                                  bezier[1]*contentBox.size() + border,
                                  bezier[2]*contentBox.size() + border,
                                  bezier[3]*contentBox.size() + border};

        /* Handle end is slightly cut to make the circle nice */
        const Vector2 end1 = transformed[1] - (transformed[1] - transformed[0]).resized(CircleRadius);
        const Vector2 end2 = transformed[2] - (transformed[2] - transformed[3]).resized(CircleRadius);

        /* Handles, handle points and the curve, all smashed togetherrrr so
           we can reuse the data*/
        Utility::formatInto(out, out.size(), R"(<path fill="none" stroke="#2f83cc" stroke-width="1.0" d="M{8} {9} L{0} {1} C{2} {3}, {4} {5}, {6} {7} L{10} {11}"/>
<circle cx="{2}" cy="{3}" r="2" stroke="#2f83cc" stroke-width="1.0" fill="none" />
<circle cx="{4}" cy="{5}" r="2" stroke="#2f83cc" stroke-width="1.0" fill="none" />
)",
            transformed[0][0], size.y() - transformed[0][1],
            transformed[1][0], size.y() - transformed[1][1],
            transformed[2][0], size.y() - transformed[2][1],
            transformed[3][0], size.y() - transformed[3][1],
            end1.x(), size.y() - end1.y(),
            end2.x(), size.y() - end2.y());
    }

    print(0xdcdcdc_srgbf, 1.75f, function);

    Utility::formatInto(out, out.size(), R"(</svg>
)");

    Utility::Directory::writeString("easings-" + file + ".svg", out);

    generateThumb(file, function);
}

const Color3 danger = 0xcd3431_srgbf;
const Color3 success = 0x3bd267_srgbf;

}

/** @todo better bezier approximations for more complex curves, easings.net has it awful */

int main() {
    using namespace Animation::Easing;

    #define _c(name) Utility::String::lowercase(#name), name
    generate(_c(linear), {},
/* [linear] */
CubicBezier2D{Vector2{0.0f}, Vector2{1.0f/3.0f},
              Vector2{2.0f/3.0f}, Vector2{1.0f}}
/* [linear] */
        , success, success);
    generate(_c(step), {}, {}, success, success);
    generate(_c(smoothstep), {smootherstep},
/* [smoothstep] */
CubicBezier2D{Vector2{0.0f}, Vector2{1.0f/3.0f, 0.0f},
              Vector2{2.0f/3.0f, 1.0f}, Vector2{1.0f}}
/* [smoothstep] */
        , success, success);
    generate(_c(smootherstep), {smoothstep}, {}, success, success);
    generate(_c(quadraticIn), {cubicIn, quarticIn, quinticIn},
/* [quadraticIn] */
CubicBezier2D{Vector2{0.0f}, Vector2{1.0f/3.0f, 0.0f},
              Vector2{2.0f/3.0f, 1.0f/3.0f}, Vector2{1.0f}}
/* [quadraticIn] */
        , danger, success);
    generate(_c(quadraticOut), {cubicOut, quarticOut, quinticOut},
/* [quadraticOut] */
CubicBezier2D{Vector2{0.0f}, Vector2{1.0f/3.0f, 2.0f/3.0f},
              Vector2{2.0f/3.0f, 1.0f}, Vector2{1.0f}}
/* [quadraticOut] */
        , success, danger);
    generate(_c(quadraticInOut), {cubicInOut, quarticInOut, quinticInOut},
/* [quadraticInOut] */
CubicBezier2D{Vector2{0.0f}, Vector2{0.455f, 0.0f},
              Vector2{0.545f, 1.0f}, Vector2{1.0f}}
/* [quadraticInOut] */
        );
    generate(_c(cubicIn), {quadraticIn, quarticIn, quinticIn},
/* [cubicIn] */
CubicBezier2D{Vector2{0.0f}, Vector2{1.0f/3.0f, 0.0f},
              Vector2{2.0f/3.0f, 0.0f}, Vector2{1.0f}}
/* [cubicIn] */
        , danger, success);
    generate(_c(cubicOut), {quadraticOut, quarticOut, quinticOut},
/* [cubicOut] */
CubicBezier2D{Vector2{0.0f}, Vector2{1.0f/3.0f, 1.0f},
              Vector2{2.0f/3.0f, 1.0f}, Vector2{1.0f}}
/* [cubicOut] */
        , success, danger);
    generate(_c(cubicInOut), {quadraticInOut, quarticInOut, quinticInOut},
/* [cubicInOut] */
CubicBezier2D{Vector2{0.0f}, Vector2{0.645f, 0.0f},
              Vector2{0.355f, 1.0f}, Vector2{1.0f}}
/* [cubicInOut] */
        );
    generate(_c(quarticIn), {quadraticIn, cubicIn, quinticIn}, {}
//     ,
// /* [quarticIn] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.895f, 0.03f},
//               Vector2{0.685f, 0.22f}, Vector2{1.0f}}
// /* [quarticIn] */
        , danger, success);
    generate(_c(quarticOut), {quadraticOut, cubicOut, quinticOut}, {}
//     ,
// /* [quarticOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.165f, 0.84f},
//               Vector2{0.44f, 1.0f}, Vector2{1.0f}}
// /* [quarticOut] */
        , success, danger);
    generate(_c(quarticInOut), {quadraticInOut, cubicInOut, quinticInOut}
//     ,
// /* [quarticInOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.77f, 0.0f},
//               Vector2{0.175f, 1.0f}, Vector2{1.0f}}
// /* [quarticInOut] */
        );
    generate(_c(quinticIn), {quadraticIn, cubicIn, quarticIn}, {}
//     ,
// /* [quinticIn] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.755f, 0.05f},
//               Vector2{0.855f, 0.06f}, Vector2{1.0f}}
// /* [quinticIn] */
        , danger, success);
    generate(_c(quinticOut), {quadraticOut, cubicOut, quarticOut}, {}
//     ,
// /* [quinticOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.23f, 1.0f},
//               Vector2{0.32f, 1.0f}, Vector2{1.0f}}
// /* [quinticOut] */
        , success, danger);
    generate(_c(quinticInOut), {quadraticInOut, cubicInOut, quarticInOut}
//     ,
// /* [quinticInOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.86f, 0.0f},
//               Vector2{0.07f, 1.0f}, Vector2{1.0f}}
// /* [quinticInOut] */
        );
    generate(_c(sineIn), {circularIn}
//     ,
// /* [sineIn] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.47f, 0.0f},
//               Vector2{0.745f, 0.715f}, Vector2{1.0f}}
// /* [sineIn] */
        );
    generate(_c(sineOut), {circularOut}
//     ,
// /* [sineOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.39f, 0.575f},
//               Vector2{0.565f, 1.0f}, Vector2{1.0f}}
// /* [sineOut] */
        );
    generate(_c(sineInOut), {circularInOut}
//     ,
// /* [sineInOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.445f, 0.05f},
//               Vector2{0.55f, 0.95f}, Vector2{1.0f}}
// /* [sineInOut] */
        );
    generate(_c(circularIn), {sineIn}
//     ,
// /* [circularIn] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.6f, 0.04f},
//               Vector2{0.98f, 0.335f}, Vector2{1.0f}}
// /* [circularIn] */
        );
    generate(_c(circularOut), {sineOut}
//     ,
// /* [circularOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.075f, 0.085f},
//               Vector2{0.165f, 1.0f}, Vector2{1.0f}}
// /* [circularOut] */
        );
    generate(_c(circularInOut), {sineInOut}
//     ,
// /* [circularInOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.785f, 0.135f},
//               Vector2{0.15f, 0.86f}, Vector2{1.0f}}
// /* [circularInOut] */
        );
    generate(_c(exponentialIn), {}, {}
//     ,
// /* [exponentialIn] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.95f, 0.05f},
//               Vector2{0.795f, 0.035f}, Vector2{1.0f}}
// /* [exponentialIn] */
        , success, success);
    generate(_c(exponentialOut), {}, {}
//     ,
// /* [exponentialOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.19f, 1.0f},
//               Vector2{0.22f, 1.0f}, Vector2{1.0f}}
// /* [exponentialOut] */
        , success, success);
    generate(_c(exponentialInOut), {}, {}
//     ,
// /* [exponentialInOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{1.0f, 0.0f},
//               Vector2{0.0f, 1.0f}, Vector2{1.0f}}
// /* [exponentialInOut] */
        , success, success);
    generate(_c(elasticIn), {}, {}, success, danger, true);
    generate(_c(elasticOut), {}, {}, danger, success, true);
    generate(_c(elasticInOut), {}, {}, success, success, true);
    generate(_c(backIn), {}, {}
//     ,
// /* [backIn] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.6f, -0.28f},
//               Vector2{0.735f, 0.045f}, Vector2{1.0f}}
// /* [backIn] */
        , danger, danger, true);
    generate(_c(backOut), {}, {}
//     ,
// /* [backOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.175f, 0.885f},
//               Vector2{0.32f, 1.275f}, Vector2{1.0f}}
// /* [backOut] */
        , danger, danger, true);
    generate(_c(backInOut), {}, {}
//      ,
// /* [backInOut] */
// CubicBezier2D{Vector2{0.0f}, Vector2{0.68f, -0.55f},
//               Vector2{0.265f, 1.55f}, Vector2{1.0f}}
// /* [backInOut] */
        , danger, danger, true);
    generate(_c(bounceIn));
    generate(_c(bounceOut));
    generate(_c(bounceInOut));
    #undef _c
}
