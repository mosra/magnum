set(MagnumTrade_HEADERS
    AbstractImporter.h
    AbstractMaterialData.h
    CameraData.h
    ImageData.h
    LightData.h
    MeshData.h
    MeshObjectData.h
    ObjectData.h
    PhongMaterialData.h
    SceneData.h
    TextureData.h)
install(FILES ${MagnumTrade_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Trade)
