corrade_add_test(CleanTest CleanTest.h CleanTest.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(CombineIndexedArraysTest CombineIndexedArraysTest.h CombineIndexedArraysTest.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(CompressIndicesTest CompressIndicesTest.h CompressIndicesTest.cpp ${CORRADE_UTILITY_LIBRARY} ${MAGNUM_LIBRARY})
corrade_add_test(FlipNormalsTest FlipNormalsTest.h FlipNormalsTest.cpp MagnumMeshToolsTestLib)
corrade_add_test(GenerateFlatNormalsTest GenerateFlatNormalsTest.h GenerateFlatNormalsTest.cpp MagnumMeshToolsTestLib)
corrade_add_test(InterleaveTest InterleaveTest.h InterleaveTest.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(SubdivideTest SubdivideTest.h SubdivideTest.cpp ${CORRADE_UTILITY_LIBRARY})
corrade_add_test(SubdivideCleanBenchmark SubdivideCleanBenchmark.h SubdivideCleanBenchmark.cpp ${CORRADE_UTILITY_LIBRARY} ${MAGNUM_PRIMITIVES_LIBRARY})
corrade_add_test(TipsifyTest TipsifyTest.h TipsifyTest.cpp ${MAGNUM_MESHTOOLS_LIBRARY})

# Graceful assert for testing
set_target_properties(CombineIndexedArraysTest InterleaveTest SubdivideTest PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
