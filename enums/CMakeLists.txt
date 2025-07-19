add_library(enums STATIC ticker.cpp)

target_include_directories(enums PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
