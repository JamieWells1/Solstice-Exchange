#include <gtest/gtest.h>
#include <order.h>

#include <chrono>

namespace solstice
{

TEST(OrderTests, ValidOrderSucceeds)
{
    auto now = std::chrono::system_clock::now();
    auto result = Order::createOrder(Ticker::AAPL, 100.0, 10.0, true);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()->tkr(), Ticker::AAPL);
}

TEST(OrderTests, NegativePriceFails)
{
    auto now = std::chrono::system_clock::now();
    auto result = Order::createOrder(Ticker::AAPL, -10.0, 10.0, true);
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error().find("Invalid price") != std::string::npos);
}
}  // namespace solstice
