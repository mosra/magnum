#ifndef Magnum_Math_Test_Cpp14Vector
#define Magnum_Math_Test_Cpp14Vector

#define CE
#ifdef TESTING_CONSTEXPR
#if __cpp_constexpr >= 201304
#undef CE
#define CE constexpr
#else
#define SKIP_TESTING
#endif
#endif

#endif
