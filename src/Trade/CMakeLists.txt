set(MagnumTrade_HEADERS
    AbstractImporter.h
    AbstractMaterialData.h
    CameraData.h
    ImageData.h
    LightData.h
    MeshData2D.h
    MeshData3D.h
    MeshObjectData2D.h
    MeshObjectData3D.h
    ObjectData2D.h
    ObjectData3D.h
    PhongMaterialData.h
    SceneData.h
    TextureData.h
    Trade.h)

install(FILES ${MagnumTrade_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Trade)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
