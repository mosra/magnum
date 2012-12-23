set(MagnumTextureTools_SRCS
    Atlas.cpp)

set(MagnumTextureTools_HEADERS
    Atlas.h

    magnumTextureToolsVisibility.h)

add_library(MagnumTextureTools SHARED ${MagnumTextureTools_SRCS})

target_link_libraries(MagnumTextureTools Magnum)

install(TARGETS MagnumTextureTools DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumTextureTools_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/TextureTools)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()
