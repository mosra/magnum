file(
  DOWNLOAD
  https://raw.githubusercontent.com/hunter-packages/gate/master/cmake/HunterGate.cmake
  ${CMAKE_CURRENT_LIST_DIR}//HunterGate.cmake
)

# FIXME : will need to replace this url with the main hunter url !
include("${CMAKE_CURRENT_LIST_DIR}/HunterGate.cmake")
HunterGate(
  URL "https://github.com/pthom/hunter/archive/test-corrade-v2018.10.tar.gz"
  SHA1 "3415f425f0ff1aa1754b8a2b57c77ba20f5b7bbc"
)
