file(READ ${CMAKE_CURRENT_LIST_DIR}/WebApplication.css CONTENT)
string(REPLACE "#canvas" "#canvas2" CONTENT "${CONTENT}")
file(WRITE ${CMAKE_CURRENT_LIST_DIR}/WebApplicationCustomCanvas.css "${CONTENT}")
