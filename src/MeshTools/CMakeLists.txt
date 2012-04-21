set(MagnumMeshTools_SRCS
    FlipNormals.cpp
    GenerateFlatNormals.cpp
    Tipsify.cpp
)

add_library(MagnumMeshTools SHARED ${MagnumMeshTools_SRCS})

if(WIN32)
    set_target_properties(MagnumMeshTools PROPERTIES COMPILE_FLAGS -DMESHTOOLS_EXPORTING)
endif()

target_link_libraries(MagnumMeshTools ${MAGNUM_LIBRARY})

install(TARGETS MagnumMeshTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
