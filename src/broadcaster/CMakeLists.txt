add_library(broadcaster STATIC broadcaster.cpp)

target_include_directories(broadcaster
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/common
        ${CMAKE_SOURCE_DIR}/src/matching
        ${CMAKE_SOURCE_DIR}/src/pricing
        ${CMAKE_SOURCE_DIR}/src/config
        ${CMAKE_SOURCE_DIR}/src/utils
        ${CMAKE_SOURCE_DIR}/src/enums
)

target_link_libraries(broadcaster
    PUBLIC
        common
        matching
        config
        ${Boost_LIBRARIES}
)
