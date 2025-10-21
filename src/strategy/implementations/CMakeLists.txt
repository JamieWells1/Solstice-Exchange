add_library(implementations STATIC sharp_movements.cpp)

target_include_directories(implementations
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/common
        ${CMAKE_SOURCE_DIR}/src/utils
        ${CMAKE_SOURCE_DIR}/src/enums
        ${CMAKE_SOURCE_DIR}/src/config
        ${CMAKE_SOURCE_DIR}/src/strategy
        ${CMAKE_SOURCE_DIR}/src/strategy/components
)

target_link_libraries(implementations PUBLIC components pybind11::embed)
