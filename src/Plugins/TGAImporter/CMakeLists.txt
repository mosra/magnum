corrade_add_plugin(TGAImporter ${MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR} TGAImporter.conf TGAImporter.cpp)
target_link_libraries(TGAImporter ${MAGNUM_LIBRARY})

if(BUILD_TESTS)
    add_library(TGAImporterTestLib TGAImporter.cpp)
    target_link_libraries(TGAImporterTestLib ${MAGNUM_LIBRARY} ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
    enable_testing()
    add_subdirectory(Test)
endif()

if(WIN32)
    target_link_libraries(TGAImporter ${CORRADE_UTILITY_LIBRARY} ${CORRADE_PLUGINMANAGER_LIBRARY})
endif()
