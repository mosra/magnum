#include <Corrade/TestSuite/Tester.h>
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

    void unsignedScalar();
    void signedScalar();
    void unitVector2();
    void unitVector3();
    void pointInACircle();
    void pointInASphere();
    void randomRotation();
};

typedef Vector<2, Float> Vector2;
typedef Vector<3, Float> Vector3;
typedef Math::Constants<Float> Constants;

RandomTest::RandomTest()
{
    addTests({&RandomTest::unsignedScalar,
              &RandomTest::signedScalar,
              &RandomTest::unitVector2,
              &RandomTest::unitVector3,
              &RandomTest::pointInACircle,
              &RandomTest::pointInASphere,
              &RandomTest::randomRotation});
}

// void RandomTest::construct() {
//     constexpr Matrix4x4 a = {Vector4(3.0f,  5.0f, 8.0f, -3.0f),
//                              Vector4(4.5f,  4.0f, 7.0f,  2.0f),
//                              Vector4(1.0f,  2.0f, 3.0f, -1.0f),
//                              Vector4(7.9f, -1.0f, 8.0f, -1.5f)};
//     CORRADE_COMPARE(a, Matrix4x4(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
//                                  Vector4(4.5f,  4.0f, 7.0f,  2.0f),
//                                  Vector4(1.0f,  2.0f, 3.0f, -1.0f),
//                                  Vector4(7.9f, -1.0f, 8.0f, -1.5f)));

//     CORRADE_VERIFY((std::is_nothrow_constructible<Matrix4x4, Vector4, Vector4, Vector4, Vector4>::value));
// }
void RandomTest::unsignedScalar()
{
    for (std::size_t i = 0; i < 10; i++)
    {
        CORRADE_VERIFY(Math::Random::randomUnsignedScalar<Float>() < 1.000000001);
        CORRADE_VERIFY(Math::Random::randomUnsignedScalar<Float>() > -.000000001);
    }
}

void RandomTest::signedScalar()
{
    for (std::size_t i = 0; i < 10; i++)
    {
        CORRADE_VERIFY(Math::Random::randomUnsignedScalar<Float>() < 1.000000001);
        CORRADE_VERIFY(Math::Random::randomUnsignedScalar<Float>() > -1.000000001);
    }
}

void RandomTest::unitVector2()
{
    for (std::size_t i = 0; i < 10; i++)
    {
        CORRADE_COMPARE((Math::Random::randomUnitVector2()).length(),1.0f);
    }
}
void RandomTest::unitVector3()
{
    for (std::size_t i = 0; i < 10; i++)
    {
        CORRADE_COMPARE((Math::Random::randomUnitVector3()).length(),1.0f);
    }
}

void RandomTest::pointInACircle(){
    for (std::size_t i = 0; i < 10; i++)
    {
        CORRADE_VERIFY((Math::Random::randomPointInACircle()).length()<1.0f);
    }
}
void RandomTest::pointInASphere(){
    for (std::size_t i = 0; i < 10; i++)
    {
        CORRADE_VERIFY((Math::Random::randomPointInASphere()).length()<1.0f);
    }
}
void RandomTest::randomRotation(){
    for (std::size_t i = 0; i < 10; i++)
    {
        CORRADE_COMPARE(Math::Random::randomRotation().length(), 1.0f);
    }
}
} // namespace
} // namespace Test
} // namespace Math
} // namespace Magnum

CORRADE_TEST_MAIN(Magnum::Math::Test::RandomTest)
