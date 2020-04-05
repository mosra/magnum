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
Vector3<T> randomUnitVector2()
{
    auto a = Implementation::RandomGenerator::generate(0.0f, 2 * Math::Constants<T>::pi());
    return {std::cos(a), std::sin(a)};
}

template <class T = Float>
Vector3<T> randomUnitVector3()
{
    auto a = Implementation::RandomGenerator::generate(0.0f, 2 * Math::Constants<T>::pi());
    auto z = randomSignedScalar();
    auto r = sqrt<T>(1 - z * z);
    return {r * std::cos(a), r * std::sin(a), z};
}
template <class T = Float>
Vector2<T> randomPointInACircle() // always length < 1
{
    while (true)
    {
        auto p = Vector2<T>(
            randomSignedScalar(), randomSignedScalar());
        if (p.length() >= 1)
            continue;
        return p;
    }
}

template <class T = Float>
Vector3<T> randomPointInASphere() // always length < 1
{
    auto x = randomSignedScalar();
    auto y = randomSignedScalar();
    while (true)
    {
        auto p = Vector3<T>(x, y, randomSignedScalar());
        if (p.length() >= 1)
            continue;
        return p;
    }
}

template <class T = Float>
Quaternion<T> randomRotation()
{
    return Quaternion<T>({randomSignedScalar<T>(), randomSignedScalar<T>(), randomSignedScalar<T>()}, randomSignedScalar<T>()).normalized();
}

} // namespace Random

} // namespace Math
} // namespace Magnum

#endif
