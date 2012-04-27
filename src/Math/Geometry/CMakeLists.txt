include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()

