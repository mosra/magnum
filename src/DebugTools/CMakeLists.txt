set(MagnumDebugTools_SRCS
    ObjectRenderer.cpp
    Profiler.cpp
    ResourceManager.cpp
    ShapeRenderer.cpp

    Implementation/AbstractBoxRenderer.cpp
    Implementation/AbstractShapeRenderer.cpp
    Implementation/AxisAlignedBoxRenderer.cpp
    Implementation/BoxRenderer.cpp
    Implementation/PointRenderer.cpp)

set(MagnumDebugTools_HEADERS
    DebugTools.h
    ObjectRenderer.h
    Profiler.h
    ResourceManager.h
    ShapeRenderer.h

    magnumDebugToolsVisibility.h)

add_library(MagnumDebugTools SHARED ${MagnumDebugTools_SRCS})

target_link_libraries(MagnumDebugTools Magnum MagnumMeshTools MagnumPhysics MagnumPrimitives MagnumSceneGraph MagnumShaders)

install(TARGETS MagnumDebugTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumDebugTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/DebugTools)
