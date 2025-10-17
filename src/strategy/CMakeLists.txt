add_library(strategy STATIC strategy.cpp py_interface.cpp)

target_include_directories(strategy
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/common
        ${CMAKE_SOURCE_DIR}/src/utils
        ${CMAKE_SOURCE_DIR}/src/enums
)

target_link_libraries(strategy PUBLIC pybind11::embed)
