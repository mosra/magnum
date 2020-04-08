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

    void signedScalar();
    void unitVector2();
    void unitVector3();
    void randomRotation();
};

typedef Vector<2, Float> Vector2;
typedef Vector<3, Float> Vector3;
typedef Math::Constants<Float> Constants;

RandomTest::RandomTest()
{
    Corrade::TestSuite::Tester::addRepeatedTests(
        {&RandomTest::signedScalar,
         &RandomTest::unitVector2,
         &RandomTest::unitVector3,
         &RandomTest::randomRotation},
        /*repeat number*/200);
}

void RandomTest::signedScalar()
{
        CORRADE_COMPARE_AS(Math::Random::randomSignedScalar<Float>(), 1.0f, Corrade::TestSuite::Compare::LessOrEqual);
        CORRADE_COMPARE_AS(Math::Random::randomSignedScalar<Float>(), -1.0f, Corrade::TestSuite::Compare::GreaterOrEqual);
}

void RandomTest::unitVector2()
{
        CORRADE_COMPARE((Math::Random::randomUnitVector2()).length(), 1.0f);
}
void RandomTest::unitVector3()
{
        CORRADE_COMPARE((Math::Random::randomUnitVector3()).length(), 1.0f);
}

void RandomTest::randomRotation()
{
        CORRADE_COMPARE(Math::Random::randomRotation().length(), 1.0f);
}
} // namespace
} // namespace Test
} // namespace Math
} // namespace Magnum

CORRADE_TEST_MAIN(Magnum::Math::Test::RandomTest)
