add_subdirectory(implementations)
add_subdirectory(components)

add_library(strategy STATIC market_data.cpp dispatcher.cpp py_interface.cpp)

target_include_directories(strategy
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/implementations
        ${CMAKE_CURRENT_SOURCE_DIR}/components
        ${CMAKE_SOURCE_DIR}/src/common
        ${CMAKE_SOURCE_DIR}/src/utils
        ${CMAKE_SOURCE_DIR}/src/enums
        ${CMAKE_SOURCE_DIR}/src/config
)

target_link_libraries(strategy PUBLIC implementations components pybind11::embed)
