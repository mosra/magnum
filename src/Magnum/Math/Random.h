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

namespace Implementation
{
static std::seed_seq seeds{{
    static_cast<std::uintmax_t>(std::random_device{}()),
    static_cast<std::uintmax_t>(std::chrono::steady_clock::now()
                                    .time_since_epoch()
                                    .count()),
}};

class RandomGenerator
{
public:
    RandomGenerator() = delete;

    template <typename T>
    static typename std::enable_if<std::is_same<Int, T>::value, T>::type
    generate(T start = -Magnum::Math::Constants<T>::inf(),
             T end = Magnum::Math::Constants<T>::inf())
    {
        return std::uniform_int_distribution<T>{start, end}(generator());
    }

    template <typename T>
    static typename std::enable_if<std::is_same<Float, T>::value, T>::type
    generate(T start = -Magnum::Math::Constants<T>::inf(),
             T end = Magnum::Math::Constants<T>::inf())
    {
        return std::uniform_real_distribution<T>{start, end}(generator());
    }

public:
    static std::mt19937 &generator()
    {
        static std::mt19937 g{seeds};
        return g;
    }
};
} // namespace Implementation
namespace Random
{
template <class T = Float>
T randomUnsignedScalar() // range [0, 1]
{
    return Implementation::RandomGenerator::generate<T>(static_cast<T>(0.0f),
                                                        static_cast<T>(1.0f));
}
template <class T = Float>
T randomSignedScalar() // range [-1, 1]
{
    return Implementation::RandomGenerator::generate(static_cast<T>(-1.0f), static_cast<T>(1.0f));
}

template <class T = Float>
Vector2<T> randomUnitVector2()
{
    auto a = Implementation::RandomGenerator::generate(0.0f, 2 * Math::Constants<T>::pi());
    return {std::cos(a), std::sin(a)};
}

template <class T = Float>
Vector3<T> randomUnitVector3()
{
    // Better to have it "theta" and "z" than three random numbers.
    // https://mathworld.wolfram.com/SpherePointPicking.html
    auto a = Implementation::RandomGenerator::generate(0.0f, 2 * Math::Constants<T>::pi());
    auto z = randomSignedScalar();
    auto r = sqrt<T>(1 - z * z);
    return {r * std::cos(a), r * std::sin(a), z};
}

template <class T = Float>
Quaternion<T> randomRotation()
{
    //http://planning.cs.uiuc.edu/node198.html
    auto u{randomUnsignedScalar()};
    auto v{2 * Math::Constants<T>::pi() * randomUnsignedScalar()};
    auto w{2 * Math::Constants<T>::pi() * randomUnsignedScalar()};
    return Quaternion<T>({sqrt<T>(1 - u) * std::sin(v),
                          sqrt<T>(1 - u) * std::cos(v),
                          sqrt<T>(u) * std::sin(w)},
                         sqrt<T>(u) * std::cos(w));
}

} // namespace Random

} // namespace Math
} // namespace Magnum

#endif
