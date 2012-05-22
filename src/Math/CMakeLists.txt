include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Geometry)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
