add_executable(triangleExample main.cpp IdentityShader.cpp Triangle.cpp)
target_link_libraries(triangleExample Magnum ${GLUT_LIBRARY})
