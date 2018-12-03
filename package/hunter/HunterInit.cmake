file(
  DOWNLOAD
  https://raw.githubusercontent.com/hunter-packages/gate/master/cmake/HunterGate.cmake
  ${CMAKE_CURRENT_LIST_DIR}//HunterGate.cmake
)

# FIXME : will need to replace this url with the main hunter url !
include("${CMAKE_CURRENT_LIST_DIR}/HunterGate.cmake")
HunterGate(
  URL "https://github.com/pthom/magnum/archive/hunter_rc3.tar.gz"
  SHA1 "990c49b8bb085c395eebe8a46686552767cffb3f"
)
