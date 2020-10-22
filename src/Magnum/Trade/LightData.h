#ifndef Magnum_Trade_LightData_h
#define Magnum_Trade_LightData_h
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

/** @file
 * @brief Class @ref Magnum::Trade::LightData
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Light data

@section Trade-LightData-usage Usage

The class exposes light parameters in a way that makes sense as a whole,
allowing to reduce branching in application code --- e.g., a light defined by
just its range has the quadratic attenuation factor set to one, with constant
and linear attenuation being zero, or spot cone angles are the full circle
everything except spotlights.

@section Trade-LightData-populating Populating an instance

You can choose a constructor overload that matches the subset of input
parameters and let the class set the rest implicitly. For example, a
@ref Type::Point light constructed using a range will have @ref attenuation()
implicitly set to @cpp {1.0f, 0.0f, 1.0f} @ce and cone angles to
@cpp 360.0_degf @ce:

@snippet MagnumTrade.cpp LightData-populating-range

Or, a @ref Type::Spot light constructed from a constant / linear / quadratic
attenuation will have @ref range() implicitly set to @ref Constants::inf():

@snippet MagnumTrade.cpp LightData-populating-attenuation

And a @ref Type::Directional light that doesn't attenuate can be constructed
without either, causing @ref attenuation() to be @cpp {1.0f, 0.0f, 0.0f} @ce
and @ref range() @ref Constants::inf(), cancelling out the attenuation equation:

@snippet MagnumTrade.cpp LightData-populating-none

@section Trade-LightData-attenuation Attenuation calculation

To support all common lighting calculations, the class exposes parameters in a
combined equation containing both constant / linear / quadratic attenuation
@f$ \color{m-success} K_c @f$ / @f$ \color{m-success} K_l @f$ /
@f$ \color{m-success} K_q @f$ and a range parameter @f$ \color{m-info} R @f$
over a distance @f$ d @f$: @f[
    F_{att} = \frac{\operatorname{clamp}(1 - (\frac{d}{\color{m-info} R})^4, 0, 1)^2}{{\color{m-success} K_c} + {\color{m-success} K_l} d + {\color{m-success} K_q} d^2}
@f]

In most cases you'll have the light data using either one or the other
approach. The classic constant/linear/quadratic equation allows for most
control, but because the attenuated intensity never really reaches zero, it
makes light culling optimizations hard to perform. In this case the
@ref range() is set to @ref Constants::inf(): @f[
    F_{att} = \lim_{{\color{m-info} R} \to \infty} \frac{{\color{m-dim} \operatorname{clamp}(}1 \mathbin{\color{m-dim}-} {\color{m-dim} (\frac{d}{R})^4, 0, 1)^2}}{{\color{m-success} K_c} + {\color{m-success} K_l} d + {\color{m-success} K_q} d^2} = \frac{1}{{\color{m-success} K_c} + {\color{m-success} K_l} d + {\color{m-success} K_q} d^2}
@f]

The range-based equation approaches zero when @f$ {\color{m-info} R} = d @f$
and provides a good tradeoff for performance while staying mostly
physically-based. This is modelled after the glTF [KHR_lights_punctual](https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual#range-property)
extension, which in turn is based on the [UE4 implementation](https://github.com/KhronosGroup/glTF/pull/1223#issuecomment-387956919).
In this case, @ref attenuation() is set to
@cpp {1.0f, 0.0f, 1.0f} @ce, the constant factor is present in order to prevent
the function from exploding to infinity when @f$ d \to \infty @f$. @f[
    F_{att} = \frac{\operatorname{clamp}(1 - (\frac{d}{\color{m-info} R})^4, 0, 1)^2}{{\color{m-success} K_c} + {\color{m-dim} K_l d} \mathbin{\color{m-dim} +} {\color{m-success} K_q} d^2} = \frac{\operatorname{clamp}(1 - (\frac{d}{\color{m-info} R})^4, 0, 1)^2}{1 + d^2}
@f]

If @f$ {\color{m-info} R} \to \infty @f$ as well, the equation reduces down to
a simple inverse square: @f[
    F_{att} = \lim_{{\color{m-info} R} \to \infty} \frac{{\color{m-dim} \operatorname{clamp}(} 1 \mathbin{\color{m-dim} -} {\color{m-dim} (\frac{d}{R})^4, 0, 1)^2}}{{\color{m-success} K_c} + {\color{m-dim} K_l d} \mathbin{\color{m-dim} +} {\color{m-success} K_q} d^2} = \frac{1}{1 + d^2}
@f]

As a special case, a @ref Type::Directional light is defined by
@ref attenuation() set to @cpp {1.0f, 0.0f, 0.0f} @ce and @ref range() to
@ref Constants::inf() --- thus without any attenuation: @f[
    F_{att} = \lim_{{\color{m-info} R} \to \infty} \frac{{\color{m-dim} \operatorname{clamp}(} 1 \mathbin{\color{m-dim} -} {\color{m-dim} (\frac{d}{R})^4, 0, 1)^2}}{{\color{m-success} K_c} \mathbin{\color{m-dim} +} {\color{m-dim} K_l d + K_q d^2}} = 1
@f]

@section Trade-LightData-units Units

To follow physically-based principles in lighting calculation, intensity is
assumed to be in in *candela* (lm/sr) for @ref Type::Point and @ref Type::Spot,
and in *lux* (lm/m<sup>2</sup>) for @ref Type::Directional. Distance @f$ d @f$
is in meters.

@see @ref AbstractImporter::light()
*/
class MAGNUM_TRADE_EXPORT LightData {
    public:
        /**
         * @brief Light type
         *
         * @see @ref type()
         */
        enum class Type: UnsignedByte {
            /* Zero reserved for an invalid value */

            /**
             * Ambient light, without any position, direction or attenuation.
             * Meant to be added to ambient color in Phong workflows, has no
             * use in physically based workflows.
             * @m_since_latest
             */
            Ambient = 1,

            /**
             * Light at a position that is infinitely far away, emitted in a
             * direction of negative Z axis. The rotation is inherited from
             * absolute object transformation; scale and position don't affect
             * the light in any way. Because the light is at infinite distance,
             * it's not attenuated in any way.
             * @m_since_latest
             */
            Directional,

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Directional light.
             * @m_deprecated_since_latest Use @ref Type::Directional instead.
             */
            Infinite CORRADE_DEPRECATED_ENUM("use Type::Directional instead") = Directional,
            #endif

            /**
             * Point light, emitting light in all directions. The position is
             * inherited from absolute object transformation; scale and
             * rotation don't affect the light in any way. Brightness
             * attenuates depending on the @ref range() value.
             */
            Point,

            /**
             * Spot light, emitting light in a cone in direction of local
             * negative Z axis. The position and rotation is inherited from
             * absolute object transformation; scale doesn't affect the light
             * in any way. The angle and falloff of the cone is defined using
             * @ref innerConeAngle() and @ref outerConeAngle() and brightness
             * attenuates depending on the @ref range() value.
             */
            Spot
        };

        /**
         * @brief Constructor
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param attenuation       Constant, linear and quadratic light
         *      attenuation factor. Expected to be @cpp {1.0f, 0.0f, 0.0f} @ce
         *      for an @ref Type::Ambient and @ref Type::Directional light.
         * @param range             Light range, after which the intensity is
         *      considered to be zero. Expected to be @ref Constants::inf() for
         *      an @ref Type::Ambient and @ref Type::Directional light.
         * @param innerConeAngle    Inner cone angle. Expected to be greater
         *      than or equal to @cpp 0.0_degf @ce and less than or equal to
         *      @p outerConeAngle for a @ref Type::Spot light,
         *      @cpp 360.0_degf @ce otherwise.
         * @param outerConeAngle    Outer cone angle. Expected to be greater
         *      than or equal to @p innerConeAngle and less than or equal to
         *      @cpp 360.0_degf @ce for a @ref Type::Spot light,
         *      @cpp 360.0_degf @ce otherwise.
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * This is a combined constructor including both attenuation and range
         * parameters. Use @ref LightData(Type, const Color3&, Float, const Vector3&, Rad, Rad, const void*)
         * for light data defined by just attenuation parameters and
         * @ref LightData(Type, const Color3&, Float, Float, Rad, Rad, const void*)
         * for light data defined by a range alone, and
         * @ref LightData(Type, const Color3&, Float, Rad, Rad, const void*)
         * for an implicit inverse square attenuation. See
         * @ref Trade-LightData-attenuation for more information.
         *
         * For lights other than spot it may be more convenient to use
         * @ref LightData(Type, const Color3&, Float, const Vector3&, Float, const void*)
         * and friends instead.
         */
        explicit LightData(Type type, const Color3& color, Float intensity, const Vector3& attenuation, Float range, Rad innerConeAngle, Rad outerConeAngle, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct with implicit cone angles
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param attenuation       Constant, linear and quadratic light
         *      attenuation factor. Expected to be @cpp {1.0f, 0.0f, 0.0f} @ce
         *      for an @ref Type::Ambient and @ref Type::Directional light.
         * @param range             Light range, after which the intensity is
         *      considered to be zero. Expected to be @ref Constants::inf() for
         *      an @ref Type::Ambient and @ref Type::Directional light.
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * This is a combined constructor including both attenuation and range
         * parameters. Use @ref LightData(Type, const Color3&, Float, const Vector3&, const void*)
         * for light data defined by just attenuation parameters and
         * @ref LightData(Type, const Color3&, Float, Float, const void*) for
         * light data defined by a range alone, and
         * @ref LightData(Type, const Color3&, Float, const void*) for an
         * implicit inverse square attenuation. See
         * @ref Trade-LightData-attenuation for more information.
         *
         * For a @ref Type::Spot light, @ref innerConeAngle() is implicitly set
         * to @cpp 0.0_degf @ce and @ref outerConeAngle() to @cpp 90.0_degf @ce,
         * and both are @cpp 360.0_degf @ce otherwise. Use
         * @ref LightData(Type, const Color3&, Float, const Vector3&, Float, Rad, Rad, const void*)
         * in order to specify cone angles as well.
         */
        explicit LightData(Type type, const Color3& color, Float intensity, const Vector3& attenuation, Float range, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct attenuation-based light data
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param attenuation       Constant, linear and quadratic light
         *      attenuation factor. Expected to be @cpp {1.0f, 0.0f, 0.0f} @ce
         *      for an @ref Type::Ambient and @ref Type::Directional light.
         * @param innerConeAngle    Inner cone angle. Expected to be greater
         *      than or equal to @cpp 0.0_degf @ce and less than or equal to
         *      @p outerConeAngle for a @ref Type::Spot light,
         *      @cpp 360.0_degf @ce otherwise.
         * @param outerConeAngle    Inner cone angle. Expected to be greater
         *      than or equal to @p innerConeAngle and less than or equal to
         *      @cpp 360.0_degf @ce for a @ref Type::Spot light,
         *      @cpp 360.0_degf @ce otherwise.
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * The @ref range() is implicitly set to @ref Constants::inf(). See
         * @ref Trade-LightData-attenuation for more information.
         *
         * For lights other than spot it may be more convenient to use
         * @ref LightData(Type, const Color3&, Float, const Vector3&, const void*)
         * instead.
         */
        explicit LightData(Type type, const Color3& color, Float intensity, const Vector3& attenuation, Rad innerConeAngle, Rad outerConeAngle, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct attenuation-based light data with implicit cone angles
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param attenuation       Constant, linear and quadratic light
         *      attenuation factor. Expected to be @cpp {1.0f, 0.0f, 0.0f} @ce
         *      for an @ref Type::Ambient and @ref Type::Directional light.
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * The @ref range() is implicitly set to @ref Constants::inf(). See
         * @ref Trade-LightData-attenuation for more information.
         *
         * For a @ref Type::Spot light, @ref innerConeAngle() is implicitly set
         * to @cpp 0.0_degf @ce and @ref outerConeAngle() to @cpp 90.0_degf @ce,
         * and both are @cpp 360.0_degf @ce otherwise. Use
         * @ref LightData(Type, const Color3&, Float, const Vector3&, Rad, Rad, const void*)
         * in order to specify cone angles as well.
         */
        explicit LightData(Type type, const Color3& color, Float intensity, const Vector3& attenuation, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct range-based light data
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param range             Light range, after which the intensity is
         *      considered to be zero. Expected to be @ref Constants::inf() for
         *      an @ref Type::Ambient and @ref Type::Directional light.
         * @param innerConeAngle    Inner cone angle. Expected to be greater
         *      than or equal to @cpp 0.0_degf @ce and less than or equal to
         *      @p outerConeAngle for a @ref Type::Spot light,
         *      @cpp 360.0_degf @ce otherwise.
         * @param outerConeAngle    Outer cone angle. Expected to be greater
         *      than or equal to @p innerConeAngle and less than or equal to
         *      @cpp 360.0_degf @ce for a @ref Type::Spot light,
         *      @cpp 360.0_degf @ce otherwise.
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * The @ref attenuation() is implicitly set to @cpp {1.0f, 0.0f, 1.0f} @ce
         * for a @ref Type::Point and @ref Type::Spot light and to
         * @cpp {1.0f, 0.0f, 0.0f} @ce for an @ref Type::Ambient and
         * @ref Type::Directional light. See @ref Trade-LightData-attenuation
         * for more information.
         *
         * For lights other than spot it may be more convenient to use
         * @ref LightData(Type, const Color3&, Float, Float, const void*)
         * instead.
         */
        explicit LightData(Type type, const Color3& color, Float intensity, Float range, Rad innerConeAngle, Rad outerConeAngle, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct range-based light data with implicit cone angles
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param range             Light range, after which the intensity is
         *      considered to be zero. Expected to be @ref Constants::inf() for
         *      an @ref Type::Ambient and @ref Type::Directional light.
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * The @ref attenuation() is implicitly set to @cpp {1.0f, 0.0f, 1.0f} @ce
         * for a @ref Type::Point and @ref Type::Spot light and to
         * @cpp {1.0f, 0.0f, 0.0f} @ce for an @ref Type::Ambient and
         * @ref Type::Directional light. See @ref Trade-LightData-attenuation
         * for more information.
         *
         * For a @ref Type::Spot light, @ref innerConeAngle() is implicitly set
         * to @cpp 0.0_degf @ce and @ref outerConeAngle() to @cpp 90.0_degf @ce,
         * and both are @cpp 360.0_degf @ce otherwise. Use
         * @ref LightData(Type, const Color3&, Float, Float, Rad, Rad, const void*)
         * in order to specify cone angles as well.
         */
        explicit LightData(Type type, const Color3& color, Float intensity, Float range, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct light data with implicit attenuation
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param innerConeAngle    Inner cone angle. Expected to be greater
         *      than or equal to @cpp 0.0_degf @ce and less than or equal to
         *      @p outerConeAngle for a @ref Type::Spot light,
         *      @cpp 360.0_degf @ce otherwise.
         * @param outerConeAngle    Outer cone angle. Expected to be greater
         *      than or equal to @p innerConeAngle and less than or equal to
         *      @cpp 360.0_degf @ce for a @ref Type::Spot light,
         *      @cpp 360.0_degf @ce otherwise.
         * @param importerState     Importer-specific state
         * @m_since_latest
         *
         * The @ref attenuation() is implicitly set to @cpp {1.0f, 0.0f, 1.0f} @ce
         * for a @ref Type::Point and @ref Type::Spot light and to
         * @cpp {1.0f, 0.0f, 0.0f} @ce for an @ref Type::Ambient and
         * @ref Type::Directional light; @ref range() is always
         * @ref Constants::inf(). See @ref Trade-LightData-attenuation for more
         * information.
         *
         * For lights other than spot it may be more convenient to use
         * @ref LightData(Type, const Color3&, Float, const void*) instead.
         */
        explicit LightData(Type type, const Color3& color, Float intensity, Rad innerConeAngle, Rad outerConeAngle, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct light data with implicit attenuation and cone angles
         * @param type              Light type
         * @param color             Light color
         * @param intensity         Light intensity
         * @param importerState     Importer-specific state
         *
         * The @ref attenuation() is implicitly set to @cpp {1.0f, 0.0f, 1.0f} @ce
         * for a @ref Type::Point and @ref Type::Spot light and to
         * @cpp {1.0f, 0.0f, 0.0f} @ce for an @ref Type::Ambient and
         * @ref Type::Directional light; @ref range() is always
         * @ref Constants::inf(). See @ref Trade-LightData-attenuation for more
         * information.
         *
         * For a @ref Type::Spot light, @ref innerConeAngle() is implicitly set
         * to @cpp 0.0_degf @ce and @ref outerConeAngle() to @cpp 90.0_degf @ce,
         * and both are @cpp 360.0_degf @ce otherwise. Use
         * @ref LightData(Type, const Color3&, Float, Rad, Rad, const void*) in
         * order to specify cone angles as well.
         */
        explicit LightData(Type type, const Color3& color, Float intensity, const void* importerState = nullptr) noexcept;

        /** @brief Copying is not allowed */
        LightData(const LightData&) = delete;

        /** @brief Move constructor */
        LightData(LightData&&) noexcept = default;

        /** @brief Copying is not allowed */
        LightData& operator=(const LightData&) = delete;

        /** @brief Move assignment */
        LightData& operator=(LightData&&) noexcept = default;

        /** @brief Light type */
        Type type() const { return _type; }

        /** @brief Light color */
        Color3 color() const { return _color; }

        /**
         * @brief Light intensity
         *
         * Defined in *candela* (lm/sr) for @ref Type::Point and
         * @ref Type::Spot, and in *lux* (lm/m<sup>2</sup>) for
         * @ref Type::Directional.
         */
        Float intensity() const { return _intensity; }

        /**
         * @brief Constant, linear and quadratic light attenuation
         * @m_since_latest
         *
         * Values of @f$ \color{m-success} K_c @f$,
         * @f$ \color{m-success} K_l @f$ and @f$ \color{m-success} K_q @f$ in
         * the @ref Trade-LightData-attenuation "attenuation equation". Always
         * @cpp {1.0f, 0.0f, 0.0f} @ce for an @ref Type::Ambient and
         * @ref Type::Directional light, set to @cpp {1.0f, 0.0f, 1.0f} @ce for
         * range-based attenuation --- and if @ref range() is
         * @ref Constants::inf() as well, the attenuation equation is simply
         * @f$ F_{att} = \frac{1}{1 + d^2} @f$.
         */
        Vector3 attenuation() const { return _attenuation; }

        /**
         * @brief Light range
         * @m_since_latest
         *
         * Value of @f$ \color{m-info} R @f$ in
         * the @ref Trade-LightData-attenuation "attenuation equation". If set
         * to @ref Constants::inf(), then:
         *
         * -    if @ref attenuation() is @cpp {1.0f, 0.0f, 1.0f} @ce, the
         *      attenuation equation is @f$ F_{att} = \frac{1}{1 + d^2} @f$;
         * -    if @ref attenuation() is @cpp {1.0f, 0.0f, 0.0f} @ce, the
         *      attenuation equation is @f$ F_{att} = 1 @f$.
         *
         * The latter is always the case for a @ref Type::Directional light.
         */
        Float range() const { return _range; }

        /**
         * @brief Inner cone angle
         * @m_since_latest
         *
         * For a @ref Type::Spot light, it's always less than
         * @ref outerConeAngle(). For a @ref Type::Directional or
         * @ref Type::Point light it's always @cpp 360.0_degf @ce.
         */
        Rad innerConeAngle() const { return _innerConeAngle; }

        /**
         * @brief Outer cone angle
         * @m_since_latest
         *
         * For a @ref Type::Spot light, it's always greater than
         * @ref outerConeAngle() and less than or equal to @cpp 90.0_degf @ce.
         * For a @ref Type::Directional or @ref Type::Point light it's always
         * @cpp 360.0_degf @ce.
         */
        Rad outerConeAngle() const { return _outerConeAngle; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        Type _type;
        Vector3 _color;
        Float _intensity;
        Vector3 _attenuation;
        Float _range;
        Rad _innerConeAngle, _outerConeAngle;
        const void* _importerState;
};

/** @debugoperatorclassenum{LightData,LightData::Type} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, LightData::Type value);

}}

#endif
