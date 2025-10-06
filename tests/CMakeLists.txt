add_executable(tests
  test_order.cpp
  ${PROJECT_SOURCE_DIR}/src/common/order.cpp
)

target_compile_options(tests PRIVATE -fprofile-arcs -ftest-coverage)
target_link_options(tests PRIVATE -fprofile-arcs -ftest-coverage)
target_link_libraries(tests
  PRIVATE
  gtest_main
  matching
  pricing
  strategy
  common
  utils
  enums
)

target_include_directories(tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/private
    ${PROJECT_SOURCE_DIR}/src/matching
    ${PROJECT_SOURCE_DIR}/src/pricing
    ${PROJECT_SOURCE_DIR}/src/strategy
    ${PROJECT_SOURCE_DIR}/src/common
    ${PROJECT_SOURCE_DIR}/src/enums
    ${PROJECT_SOURCE_DIR}/src/utils
)
