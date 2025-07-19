add_library(enums INTERFACE)
target_include_directories(enums INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/enums
)

add_subdirectory(utils)

add_executable(solstice
    main.cpp
    order.cpp
)

target_include_directories(solstice PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(solstice PRIVATE enums utils)
