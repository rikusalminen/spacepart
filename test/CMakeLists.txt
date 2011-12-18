add_executable(renderq-test renderq-test.c)
target_link_libraries(renderq-test spacepart)

add_executable(octree-test octree-test.c)
target_link_libraries(octree-test spacepart)

add_executable(frustum-test frustum-test.c)
target_link_libraries(frustum-test spacepart)

enable_testing()
add_test(renderq-test ${CMAKE_CURRENT_BINARY_DIR}/renderq-test)
add_test(octree-test ${CMAKE_CURRENT_BINARY_DIR}/octree-test)
add_test(frustum-test ${CMAKE_CURRENT_BINARY_DIR}/frustum-test)
