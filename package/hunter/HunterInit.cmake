if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/HunterGate.cmake)
  file(
    DOWNLOAD
    https://raw.githubusercontent.com/hunter-packages/gate/c4e5b3e4d1c97fb46ae86af7f59baac95f7f1cf0/cmake/HunterGate.cmake
    ${CMAKE_CURRENT_LIST_DIR}//HunterGate.cmake
  )
endif()

# FIXME : will need to replace this url with the main hunter url !
include("${CMAKE_CURRENT_LIST_DIR}/HunterGate.cmake")
HunterGate(
  URL "https://github.com/pthom/hunter/archive/hunter_magnum_rc4.tar.gz"
  SHA1 "31554df9e988f27f4daa9f4df9529d75007500bf"
)