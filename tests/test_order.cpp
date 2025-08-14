#include <gtest/gtest.h>
#include <order.h>

#include <chrono>

using namespace solstice;

TEST(OrderTests, ValidOrderSucceeds) {
    auto now = std::chrono::system_clock::now();
    auto result = Order::createOrder(Ticker::AAPL, 100.0, 10.0, true, now);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()->tkr(), Ticker::AAPL);
}

TEST(OrderTests, NegativePriceFails) {
    auto now = std::chrono::system_clock::now();
    auto result = Order::createOrder(Ticker::AAPL, -10.0, 10.0, true, now);
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error().find("Invalid price") != std::string::npos);
}

TEST(OrderTests, FutureTimeFails) {
    auto future = std::chrono::system_clock::now() + std::chrono::seconds(5);
    auto result = Order::createOrder(Ticker::AAPL, 10.0, 10.0, true, future);
    ASSERT_FALSE(result.has_value());
}
