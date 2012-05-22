set(MagnumMeshTools_SRCS
    FlipNormals.cpp
    GenerateFlatNormals.cpp
    Tipsify.cpp
)

add_library(MagnumMeshTools SHARED ${MagnumMeshTools_SRCS})

target_link_libraries(MagnumMeshTools Magnum)

install(TARGETS MagnumMeshTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()

    # Library with graceful assert for testing
    add_library(MagnumMeshToolsTestLib SHARED ${MagnumMeshTools_SRCS})
    set_target_properties(MagnumMeshToolsTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
    target_link_libraries(MagnumMeshToolsTestLib ${MAGNUM_LIBRARY})

    add_subdirectory(Test)
endif()
