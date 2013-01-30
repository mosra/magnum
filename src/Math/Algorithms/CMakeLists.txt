set(MagnumMathAlgorithms_HEADERS
    GaussJordan.h
    GramSchmidt.h
    Svd.h)
install(FILES ${MagnumMathAlgorithms_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Math/Algorithms)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
