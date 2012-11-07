set(MagnumSceneGraph_SRCS
    Camera.cpp)
set(MagnumSceneGraph_HEADERS
    AbstractFeature.h
    AbstractGroupedFeature.h
    AbstractObject.h
    AbstractTransformation.h
    AbstractTranslationRotation2D.h
    AbstractTranslationRotation3D.h
    AbstractTranslationRotationScaling2D.h
    AbstractTranslationRotationScaling3D.h
    Camera.h
    Camera.hpp
    Drawable.h
    MatrixTransformation2D.h
    MatrixTransformation3D.h
    Object.h
    Object.hpp
    Scene.h

    magnumSceneGraphVisibility.h)
add_library(MagnumSceneGraphObjects OBJECT ${MagnumSceneGraph_SRCS})

# Files compiled with different flags for main library and unit test library
set(MagnumSceneGraph_GracefulAssert_SRCS
    MatrixTransformation2D.cpp
    MatrixTransformation3D.cpp)

# Set shared library flags for the objects, as they will be part of shared lib
# TODO: fix when CMake sets target_EXPORTS for OBJECT targets as well
set_target_properties(MagnumSceneGraphObjects PROPERTIES COMPILE_FLAGS "-DMagnumSceneGraphObjects_EXPORTS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")

# SceneGraph library
add_library(MagnumSceneGraph SHARED
    $<TARGET_OBJECTS:MagnumSceneGraphObjects>
    ${MagnumSceneGraph_GracefulAssert_SRCS})
target_link_libraries(MagnumSceneGraph Magnum)

install(TARGETS MagnumSceneGraph DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumSceneGraph_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/SceneGraph)

if(BUILD_TESTS)
    enable_testing()

    # Library with graceful assert for testing
    add_library(MagnumSceneGraphTestLib SHARED
        $<TARGET_OBJECTS:MagnumSceneGraphObjects>
        ${MagnumSceneGraph_GracefulAssert_SRCS})
    set_target_properties(MagnumSceneGraphTestLib PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)
    target_link_libraries(MagnumSceneGraphTestLib Magnum)

    add_subdirectory(Test)
endif()
