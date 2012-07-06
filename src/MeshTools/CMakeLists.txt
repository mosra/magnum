# Files shared between main library and unit test library
set(MagnumMeshTools_SRCS
    Tipsify.cpp
)
if(NOT CMAKE_NO_OBJECT_TARGET)
    add_library(MagnumMeshToolsObjects OBJECT ${MagnumMeshTools_SRCS})
endif()

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
if(NOT CMAKE_NO_OBJECT_TARGET)
    set_target_properties(MagnumMeshToolsObjects PROPERTIES COMPILE_FLAGS "-DMagnumMeshToolsObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
endif()

# Files compiled with different flags for main library and unit test library
set(MagnumMeshTools_GracefulAssert_SRCS
    FlipNormals.cpp
    GenerateFlatNormals.cpp
)

# Main library
if(NOT CMAKE_NO_OBJECT_TARGET)
    add_library(MagnumMeshTools SHARED
        $<TARGET_OBJECTS:MagnumMeshToolsObjects>
        ${MagnumMeshTools_GracefulAssert_SRCS}
    )
else()
    add_library(MagnumMeshTools SHARED
        ${MagnumMeshTools_SRCS}
        ${MagnumMeshTools_GracefulAssert_SRCS}
    )
endif()
target_link_libraries(MagnumMeshTools Magnum)

install(TARGETS MagnumMeshTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()

    # Library with graceful assert for testing
    if(NOT CMAKE_NO_OBJECT_TARGET)
        add_library(MagnumMeshToolsTestLib SHARED
            $<TARGET_OBJECTS:MagnumMeshToolsObjects>
            ${MagnumMeshTools_GracefulAssert_SRCS}
        )
    else()
        add_library(MagnumMeshToolsTestLib SHARED
            ${MagnumMeshTools_SRCS}
            ${MagnumMeshTools_GracefulAssert_SRCS}
        )
    endif()
    set_target_properties(MagnumMeshToolsTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
    target_link_libraries(MagnumMeshToolsTestLib Magnum)

    add_subdirectory(Test)
endif()
