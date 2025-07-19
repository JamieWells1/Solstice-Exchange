add_subdirectory(utils)


add_executable(solstice
    main.cpp
    order.cpp
)

target_include_directories(solstice PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(solstice PRIVATE utils)
