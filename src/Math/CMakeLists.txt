include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Geometry)

if(BUILD_TESTS)

    # Add Math library for testing purposes
    add_library(MagnumMath STATIC
        Math.cpp
    )

    enable_testing()
    add_subdirectory(Test)
endif()
