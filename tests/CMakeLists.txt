include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})

file(GLOB TEST_SOURCES "*.c")

link_directories ("${CHECK_LIB_DIR}")

add_executable(check_libcrate ${TEST_SOURCES})
target_link_libraries(check_libcrate libcrate ${CHECK_LIBRARIES})
