set(hunter_gate_file ${CMAKE_BINARY_DIR}/HunterGate.cmake)
if(NOT EXISTS ${hunter_gate_file})
  set(url https://raw.githubusercontent.com/hunter-packages/gate/c4e5b3e4d1c97fb46ae86af7f59baac95f7f1cf0/cmake/HunterGate.cmake)
  message("hunter_gate_file=${hunter_gate_file}")
  set(expected_sha256 "9952fdeb6909855c7802938914d45dffa5e1b87252d4b4a6e9d54421dffe649f")
  file(
    DOWNLOAD ${url}
    ${hunter_gate_file}
    STATUS status
  )
  list(GET status 0 status_code)
  list(GET status 1 status_string)
  if(status_code EQUAL 0)
    file(SHA256 ${hunter_gate_file} computed_sha256)
    if (NOT ${computed_sha256} STREQUAL ${expected_sha256})
      message(FATAL_ERROR "HunterInit.cmake ; error downloading ${url} -> bad signature !")
    endif()
  else()
    message(FATAL_ERROR "HunterInit.cmake : error downloading ${url}
      status : ${status_code}
      status_string : ${status_string}
      ")
  endif()
endif()


# FIXME : will need to replace this url with the main hunter url !
include("${CMAKE_BINARY_DIR}/HunterGate.cmake")
HunterGate(
  URL "https://github.com/pthom/hunter/archive/hunter_magnum_rc4.tar.gz"
  SHA1 "31554df9e988f27f4daa9f4df9529d75007500bf"
)