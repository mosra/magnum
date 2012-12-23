find_package(Freetype REQUIRED)
find_package(HarfBuzz REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS})

set(MagnumText_SRCS
    Font.cpp
    FontRenderer.cpp
    TextRenderer.cpp)
set(MagnumText_HEADERS
    Font.h
    FontRenderer.h
    Text.h
    TextRenderer.h

    magnumTextVisibility.h)

add_library(MagnumText SHARED ${MagnumText_SRCS})

target_link_libraries(MagnumText Magnum MagnumMeshTools MagnumTextureTools ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES})

install(TARGETS MagnumText DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumText_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Text)
