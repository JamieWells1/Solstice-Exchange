add_library(enums STATIC order_side.cpp log_level.cpp strategy.cpp)

target_include_directories(enums PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
