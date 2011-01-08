add_executable(texturedTriangleExample main.cpp TexturedIdentityShader.cpp TexturedTriangle.cpp TGATexture.cpp)
target_link_libraries(texturedTriangleExample Magnum ${GLUT_LIBRARY})
