add_library(enums INTERFACE)
target_include_directories(enums INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/enums
)

add_library(utils INTERFACE)
target_include_directories(utils INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

add_executable(solstice
    main.cpp
    order.cpp
)

target_include_directories(solstice PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(solstice PRIVATE enums utils)
