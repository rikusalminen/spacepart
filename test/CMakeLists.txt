add_executable(renderq-test renderq-test.c)
target_link_libraries(renderq-test spacepart)

enable_testing()
add_test(renderq-test ${CMAKE_CURRENT_BINARY_DIR}/renderq-test)
