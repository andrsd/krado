# Documentation

find_package(Doxygen QUIET)
find_program(SPHINX_BUILD sphinx-build)

if (DOXYGEN_FOUND AND SPHINX_BUILD)
    find_program(SPHINX_BUILD sphinx-build)
    mark_as_advanced(FORCE SPHINX_BUILD)

    configure_file(Doxyfile.in Doxyfile)
    configure_file(conf.py.in conf.py)

    add_custom_target(doc
        DEPENDS ${PROJECT_BINARY_DIR}/html/index.html
        COMMENT "Open ${PROJECT_BINARY_DIR}/docs/html/index.html in your browser to view the documentation."
    )

    file(GLOB_RECURSE RST_FILES ${PROJECT_SOURCE_DIR}/docs/*.rst)
    file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/html/index.html
        COMMAND
            ${SPHINX_BUILD} -c ${PROJECT_BINARY_DIR}/docs -b html -d ${PROJECT_BINARY_DIR}/docs/.doctrees ${PROJECT_SOURCE_DIR}/docs html
        DEPENDS
            ${PROJECT_BINARY_DIR}/docs/conf.py
            ${PROJECT_BINARY_DIR}/docs/xml/index.xml
            ${RST_FILES}
    )

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/docs/xml/index.xml
        COMMAND
            doxygen Doxyfile
        DEPENDS
            ${PROJECT_BINARY_DIR}/docs/Doxyfile
            ${HEADER_FILES}
    )

else()
    set(DOC_FAIL_MSG "Unable to generate documentation:")
    if (NOT DOXYGEN_FOUND)
        string(APPEND DOC_FAIL_MSG "\n- 'doxygen' is not installed.")
    endif()
    if (${SPHINX_BUILD} STREQUAL SPHINX_BUILD-NOTFOUND)
        string(APPEND DOC_FAIL_MSG "\n- 'sphinx-build' is not installed.")
    endif()

    # Echo it all at once using CMake command
    add_custom_target(doc
        COMMAND ${CMAKE_COMMAND} -E echo "${DOC_FAIL_MSG}"
    )
endif()
