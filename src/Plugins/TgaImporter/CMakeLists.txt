corrade_add_plugin(TgaImporter ${MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR} TgaImporter.conf TgaImporter.cpp)
target_link_libraries(TgaImporter ${MAGNUM_LIBRARY})

if(BUILD_TESTS)
    add_library(TgaImporterTestLib TgaImporter.cpp)
    target_link_libraries(TgaImporterTestLib ${MAGNUM_LIBRARY} ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
    enable_testing()
    add_subdirectory(Test)
endif()

if(WIN32)
    target_link_libraries(TgaImporter ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
endif()
