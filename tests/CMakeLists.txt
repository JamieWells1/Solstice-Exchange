add_executable(tests
    test_order.cpp
    ${PROJECT_SOURCE_DIR}/src/order.cpp
)

target_link_libraries(tests
    PRIVATE
    gtest_main
    utils
    enums
)

target_include_directories(tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/enums
    ${PROJECT_SOURCE_DIR}/src/utils
)
