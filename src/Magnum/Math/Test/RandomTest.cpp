#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Random.h"

namespace Magnum
{
namespace Math
{

namespace Test
{
namespace
{

struct RandomTest : Corrade::TestSuite::Tester
{
    explicit RandomTest();

    void randScalar();
    void unitVector2();
    void unitVector3();
    void randomRotation();
    void randomDiceChiSquare();

};

typedef Vector<2, Float> Vector2;
typedef Vector<3, Float> Vector3;
typedef Math::Constants<Float> Constants;

RandomTest::RandomTest()
{
    Corrade::TestSuite::Tester::addRepeatedTests(
        {&RandomTest::randScalar,
         &RandomTest::unitVector2,
         &RandomTest::unitVector3,
         &RandomTest::randomRotation},
        /*repeat number*/ 200);
    Corrade::TestSuite::Tester::addTests(
        {&RandomTest::randomDiceChiSquare});
}

void RandomTest::randScalar()
{
    Math::Random::RandomGenerator g;
    CORRADE_COMPARE_AS(Math::Random::randomScalar<Float>(g, -1.0, 1.0), 1.0f, Corrade::TestSuite::Compare::LessOrEqual);
    CORRADE_COMPARE_AS(Math::Random::randomScalar<Float>(g, -1.0, 1.0), -1.0f, Corrade::TestSuite::Compare::GreaterOrEqual);
}

void RandomTest::unitVector2()
{
        Math::Random::RandomGenerator g;
    CORRADE_COMPARE((Math::Random::randomUnitVector2(g)).length(), 1.0f);
}
void RandomTest::unitVector3()
{
        Math::Random::RandomGenerator g;

    CORRADE_COMPARE((Math::Random::randomUnitVector3(g)).length(), 1.0f);
}

void RandomTest::randomRotation()
{
        Math::Random::RandomGenerator g;

    CORRADE_COMPARE(Math::Random::randomRotation(g).length(), 1.0f);
}

void RandomTest::randomDiceChiSquare()
{
    // A step by step explanation
    // https://rpg.stackexchange.com/questions/70802/how-can-i-test-whether-a-die-is-fair
    Math::Random::RandomGenerator g;

    int error_count = 0; // We have 1 chance to over shoot. Thats why no repeated test.

    const Int dice_side = 20;
    const Int expected = 10000;
    const Float thresholdfor100 = 36.191;

    for (auto i = 0; i < 100; i++)
    {

        std::vector<Int> faces(dice_side, 0);
        for (std::size_t i = 0; i < expected * dice_side; i++)
            faces[Math::Random::randomScalar<Int>(g, 0, dice_side - 1)]++;
        std::vector<Int> residual(dice_side, 0);
        for (std::size_t i = 0; i < dice_side; i++)
            residual[i] = Float(pow((faces[i] - expected), 2)) / expected;
        Float chi_square = std::accumulate(residual.begin(), residual.end(), 0.0);
        if (chi_square > thresholdfor100)
            error_count++;
    }
    CORRADE_COMPARE_AS(error_count, 2, Corrade::TestSuite::Compare::Less);
}

} // namespace
} // namespace Test
} // namespace Math
} // namespace Magnum

CORRADE_TEST_MAIN(Magnum::Math::Test::RandomTest)
