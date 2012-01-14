set(MagnumMeshTools_SRCS
    Tipsify.cpp
)

add_library(MagnumMeshTools SHARED ${MagnumMeshTools_SRCS})
target_link_libraries(MagnumMeshTools ${MAGNUM_LIBRARY})

install(TARGETS MagnumMeshTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
