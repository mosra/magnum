corrade_add_plugin(TGAImporter ${MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR} TGAImporter.conf TGAImporter.cpp)
target_link_libraries(TGAImporter ${MAGNUM_LIBRARY})
