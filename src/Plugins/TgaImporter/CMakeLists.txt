set(TgaImporter_SRCS
    TgaImporter.cpp)

set(TgaImporter_HEADERS
    TgaImporter.h)

add_library(TgaImporterObjects OBJECT ${TgaImporter_SRCS})
set_target_properties(TgaImporterObjects PROPERTIES COMPILE_FLAGS ${CMAKE_SHARED_LIBRARY_CXX_FLAGS})

corrade_add_plugin(TgaImporter ${MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR}
    TgaImporter.conf
    $<TARGET_OBJECTS:TgaImporterObjects>
    pluginRegistrationTgaImporter.cpp)
target_link_libraries(TgaImporter ${MAGNUM_LIBRARIES})

install(FILES ${TgaImporter_HEADERS} DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/TgaImporter)

if(BUILD_TESTS)
    add_library(TgaImporterTestLib $<TARGET_OBJECTS:TgaImporterObjects>)
    target_link_libraries(TgaImporterTestLib ${MAGNUM_LIBRARIES})
    add_subdirectory(Test)
endif()
