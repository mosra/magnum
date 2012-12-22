include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(MagnumMathGeometry_HEADERS
    Distance.h
    Intersection.h
    Rectangle.h)
install(FILES ${MagnumMathGeometry_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Math/Geometry)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
