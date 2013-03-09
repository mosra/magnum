corrade_add_resource(MagnumTextureTools_RCS MagnumTextureTools
    DistanceFieldShader.vert DistanceFieldShader.frag
    ../Shaders/compatibility.glsl ALIAS compatibility.glsl)

set(MagnumTextureTools_SRCS
    Atlas.cpp
    DistanceField.cpp
    ${MagnumTextureTools_RCS})

set(MagnumTextureTools_HEADERS
    Atlas.h
    DistanceField.h

    magnumTextureToolsVisibility.h)

add_library(MagnumTextureTools SHARED ${MagnumTextureTools_SRCS})
target_link_libraries(MagnumTextureTools Magnum)

install(TARGETS MagnumTextureTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumTextureTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/TextureTools)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
