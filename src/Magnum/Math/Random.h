#ifndef Magnum_Math_Random_h
#define Magnum_Math_Random_h

// TO DO Licence things.

#include <random>
#include <chrono>
#include "Magnum/Types.h"
#include "Magnum/Math/Constants.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Math/Functions.h"

namespace Magnum
{
namespace Math
{

namespace Random
{
class RandomGenerator
{
public:
    RandomGenerator()
    {
        std::seed_seq seeds{{
            static_cast<std::uintmax_t>(std::random_device{}()),
            static_cast<std::uintmax_t>(std::chrono::steady_clock::now()
                                            .time_since_epoch()
                                            .count()),
        }};
        g = std::mt19937{seeds};
    };
    template <typename T>
    typename std::enable_if<std::is_same<Int, T>::value, T>::type
    generate(T start = -Magnum::Math::Constants<T>::inf(),
             T end = Magnum::Math::Constants<T>::inf())
    {
        return std::uniform_int_distribution<T>{start, end}(g);
    }

    template <typename T>
    typename std::enable_if<std::is_same<Float, T>::value, T>::type
    generate(T start = -Magnum::Math::Constants<T>::inf(),
             T end = Magnum::Math::Constants<T>::inf())
    {
        return std::uniform_real_distribution<T>{start, end}(g);
    }

private:
    // namespace Implementation
    std::mt19937 g;
};

template <class T = Float>
T randomScalar(RandomGenerator &g, T begin = 0.0f, T end = 1.0f)
{

    return g.generate(static_cast<T>(begin),
                      static_cast<T>(end));
}

template <class T = Float>
Vector2<T> randomUnitVector2(RandomGenerator &g)
{
    auto a = g.generate(0.0f, 2 * Math::Constants<T>::pi());
    return {std::cos(a), std::sin(a)};
}

template <class T = Float>
Vector3<T> randomUnitVector3(RandomGenerator &g)
{
    // Better to have it "theta" and "z" than three random numbers.
    // https://mathworld.wolfram.com/SpherePointPicking.html
    auto a = g.generate(0.0f, 2 * Math::Constants<T>::pi());
    auto z = randomScalar(g, -1.0f, -1.0f);
    auto r = sqrt<T>(1 - z * z);
    return {r * std::cos(a), r * std::sin(a), z};
}

template <class T = Float>
Quaternion<T> randomRotation(RandomGenerator &g)
{
    //http://planning.cs.uiuc.edu/node198.html
    auto u = randomScalar(g);
    auto v = 2 * Math::Constants<T>::pi() * randomScalar(g);
    auto w = 2 * Math::Constants<T>::pi() * randomScalar(g);
    return Quaternion<T>({sqrt<T>(1 - u) * std::sin(v),
                          sqrt<T>(1 - u) * std::cos(v),
                          sqrt<T>(u) * std::sin(w)},
                         sqrt<T>(u) * std::cos(w));
}

} // namespace Random

} // namespace Math
} // namespace Magnum

#endif
