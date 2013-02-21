find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

if(USE_HARFBUZZ)
    find_package(HarfBuzz REQUIRED)
    include_directories(${HARFBUZZ_INCLUDE_DIRS})
endif()

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

target_link_libraries(MagnumText Magnum MagnumTextureTools ${FREETYPE_LIBRARIES})
if(USE_HARFBUZZ)
    target_link_libraries(MagnumText ${HARFBUZZ_LIBRARIES})
endif()

install(TARGETS MagnumText DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumText_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Text)
