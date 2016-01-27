enable_testing()

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(LIBS ${LIBS} ${CHECK_LIBRARIES} crate)

file(GLOB TEST_SOURCES "*.c")

link_directories ("${CHECK_LIB_DIR}")

add_executable(check_streaming ${TEST_SOURCES})
target_link_libraries(check_streaming ${LIBS})
