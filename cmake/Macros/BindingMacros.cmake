macro(CREATE_BINDING_MODULE MODULE_NAME MODULE_INCLUDE_DIR SRCS INC_DIRS LINK_LIBS)
    STRING(TOLOWER ${MODULE_NAME} LOWER_MODULE_NAME)

    add_library(${MODULE_NAME} MODULE ${${SRCS}})
    set_property(TARGET ${MODULE_NAME} PROPERTY PREFIX "")

    include_directories(${MODULE_NAME} ${${INC_DIRS}})
    target_link_libraries(${MODULE_NAME} ${${LINK_LIBS}})

    add_custom_command(OUTPUT ${${SRCS}}
                       COMMAND ${GENERATORRUNNER_BINARY} ${GENERATOR_EXTRA_FLAGS}
                       ${CMAKE_SOURCE_DIR}/${MODULE_NAME}/global.h
                       --include-paths=${QT_INCLUDE_DIR}:${MODULE_INCLUDE_DIR}
                       --typesystem-paths=${PYSIDE_TYPESYSTEMS}
                       --output-directory=${CMAKE_CURRENT_BINARY_DIR}
                       --license-file=${CMAKE_SOURCE_DIR}/cmake/Macros/licensecomment.txt
                       ${CMAKE_CURRENT_SOURCE_DIR}/typesystem_${LOWER_MODULE_NAME}.xml
                       --api-version=${BINDING_MAJOR_VERSION}.${BINDING_MINOR_VERSION}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                       COMMENT "Running generator for ${MODULE_NAME}..."
                      )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}${CMAKE_DEBUG_POSTFIX}.so
            DESTINATION "${SITE_PACKAGE}")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}/${LOWER_MODULE_NAME}_python.h
            DESTINATION include/python-${MODULE_NAME}/)
endmacro(CREATE_BINDING_MODULE)


macro(UNINSTALL_TARGET)
configure_file("${CMAKE_SOURCE_DIR}/cmake/Macros/cmake_uninstall.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}"
                  -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endmacro(UNINSTALL_TARGET)


macro(FIND_PYTHON_SITE_PACKAGE_DIR)
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; \\
        print sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}')"
    OUTPUT_VARIABLE SITE_PACKAGE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if (NOT SITE_PACKAGE)
    message(FATAL_ERROR "Could not detect Python module installation directory.")
endif()
endmacro(FIND_PYTHON_SITE_PACKAGE_DIR)


macro(PYTHON_COMPILED_FOR_DEBUG)
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; \\
        print sysconfig.get_config_var('Py_DEBUG')"
    OUTPUT_VARIABLE PY_DEBUG
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endmacro(PYTHON_COMPILED_FOR_DEBUG)


macro(SET_BINDING_ARCHIVE ARCHIVE_NAME)
set(ARCHIVE_NAME ${ARCHIVE_NAME}-${BINDING_API_VERSION})
add_custom_target(dist
    COMMAND mkdir -p "${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}" &&
            git log > "${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}/ChangeLog" &&
            git archive --prefix=${ARCHIVE_NAME}/ HEAD --format=tar --output="${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar" &&
            tar -C "${CMAKE_BINARY_DIR}" --owner=root --group=root -r "${ARCHIVE_NAME}/ChangeLog" -f "${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar" &&
            bzip2 -f9 "${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar" &&
            echo "Source package created at ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2.\n"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endmacro(SET_BINDING_ARCHIVE)


