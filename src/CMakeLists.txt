add_subdirectory(utils config)


add_executable(solstice
    main.cpp
    order.cpp
    order_book.cpp
    transaction.cpp
    order_processor.cpp
    matcher.cpp
)

target_include_directories(solstice PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)

# enable/disable logging
target_compile_definitions(solstice PRIVATE ENABLE_LOGGING)

target_link_libraries(solstice PRIVATE utils config)
