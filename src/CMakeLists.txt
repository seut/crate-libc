set(DIDIR "${PROJECT_SOURCE_DIR}/src")

file(GLOB FILES_MAIN "${DIDIR}/main.c")
file(GLOB FILES_SRC "${DIDIR}/*.h" "${DIDIR}/*.c")
file(GLOB HEADERS "${DIDIR}/*.h")

add_library (libcrate ${FILES_SRC} ${HEADERS})
add_executable(libcrate_test ${FILES_MAIN})

target_link_libraries(libcrate_test libcrate)

INSTALL(TARGETS libcrate DESTINATION "${_DEFAULT_LIBRARY_INSTALL_DIR}")
INSTALL(FILES ${HEADERS} DESTINATION "${_DEFAULT_INCLUDE_INSTALL_DIR}")
