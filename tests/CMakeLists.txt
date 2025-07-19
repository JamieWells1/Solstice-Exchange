add_executable(order_tests
    order_tests.cpp
    ${PROJECT_SOURCE_DIR}/src/order.cpp
)

target_link_libraries(order_tests
    PRIVATE
    gtest_main
    utils
    enums
)

target_include_directories(order_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/enums
    ${PROJECT_SOURCE_DIR}/src/utils
)
