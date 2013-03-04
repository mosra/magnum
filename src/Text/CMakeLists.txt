find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

set(MagnumText_SRCS
    AbstractFont.cpp
    FreeTypeFont.cpp
    TextRenderer.cpp)
set(MagnumText_HEADERS
    AbstractFont.h
    FreeTypeFont.h
    Text.h
    TextRenderer.h

    magnumTextVisibility.h)

if(USE_HARFBUZZ)
    find_package(HarfBuzz REQUIRED)
    include_directories(${HARFBUZZ_INCLUDE_DIRS})

    set(MagnumText_SRCS ${MagnumText_SRCS} HarfBuzzFont.cpp)
    set(MagnumText_HEADERS ${MagnumText_HEADERS} HarfBuzzFont.h)
endif()

add_library(MagnumText SHARED ${MagnumText_SRCS})

target_link_libraries(MagnumText Magnum MagnumTextureTools ${FREETYPE_LIBRARIES})
if(USE_HARFBUZZ)
    target_link_libraries(MagnumText ${HARFBUZZ_LIBRARIES})
endif()

install(TARGETS MagnumText DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumText_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Text)
