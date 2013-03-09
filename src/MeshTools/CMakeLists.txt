# Files shared between main library and unit test library
set(MagnumMeshTools_SRCS
    CompressIndices.cpp
    Tipsify.cpp)

# Files compiled with different flags for main library and unit test library
set(MagnumMeshTools_GracefulAssert_SRCS
    FlipNormals.cpp
    GenerateFlatNormals.cpp)

set(MagnumMeshTools_HEADERS
    Clean.h
    CombineIndexedArrays.h
    CompressIndices.h
    FlipNormals.h
    GenerateFlatNormals.h
    Interleave.h
    Subdivide.h
    Tipsify.h
    Transform.h

    magnumMeshToolsVisibility.h)

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
add_library(MagnumMeshToolsObjects OBJECT ${MagnumMeshTools_SRCS})
set_target_properties(MagnumMeshToolsObjects PROPERTIES COMPILE_FLAGS "-DMagnumMeshToolsObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")

# Main library
add_library(MagnumMeshTools SHARED
    $<TARGET_OBJECTS:MagnumMeshToolsObjects>
    ${MagnumMeshTools_GracefulAssert_SRCS})
target_link_libraries(MagnumMeshTools Magnum)

install(TARGETS MagnumMeshTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumMeshTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/MeshTools)

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumMeshToolsTestLib SHARED
        $<TARGET_OBJECTS:MagnumMeshToolsObjects>
        ${MagnumMeshTools_GracefulAssert_SRCS})
    set_target_properties(MagnumMeshToolsTestLib PROPERTIES COMPILE_FLAGS "-DCORRADE_GRACEFUL_ASSERT -DMagnumMeshTools_EXPORTS")
    target_link_libraries(MagnumMeshToolsTestLib Magnum)

    add_subdirectory(Test)
endif()
