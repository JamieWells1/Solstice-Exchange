add_library(config STATIC
    config.cpp
)

target_include_directories(config PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/enums
)
