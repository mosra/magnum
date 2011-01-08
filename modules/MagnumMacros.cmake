macro(set_parent_scope name)
    set(${name} ${ARGN})

    # Set to parent scope only if parent exists
    if(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
        set(${name} ${${name}} PARENT_SCOPE)
    endif()
endmacro()

if(QT4_FOUND)
function(magnum_add_test test_name moc_header source_file)
    foreach(library ${ARGN})
        set(libraries ${library} ${libraries})
    endforeach()

    qt4_wrap_cpp(${test_name}_MOC ${moc_header})
    add_executable(${test_name} ${source_file} ${${test_name}_MOC})
    target_link_libraries(${test_name} ${libraries} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
    add_test(${test_name} ${test_name})
endfunction()
endif()
