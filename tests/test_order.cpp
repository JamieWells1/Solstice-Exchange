#include <gtest/gtest.h>
#include <order.h>
#include <order_side.h>

#include <chrono>

namespace solstice::matching
{

TEST(OrderTests, ValidOrderSucceeds)
{
    auto now = std::chrono::system_clock::now();
    auto result =
        Order::createOrder(0, Ticker::AAPL, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()->tkr(), Ticker::AAPL);
}

TEST(OrderTests, NegativePriceFails)
{
    auto now = std::chrono::system_clock::now();
    auto result =
        Order::createOrder(0, Ticker::AAPL, -10.0, 10.0, OrderSide::Ask);
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error().find("Invalid price") != std::string::npos);
}
}  // namespace solstice::matching
