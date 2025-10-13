#include <asset_class.h>
#include <gtest/gtest.h>
#include <order.h>
#include <order_side.h>

namespace solstice
{

TEST(OrderTests, ValidOrderSucceeds)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()->underlying(), Equity::AAPL);
}

TEST(OrderTests, NegativePriceFails)
{
    auto result = Order::createOrder(0, Equity::AAPL, -10.0, 10.0, OrderSide::Ask);
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error().find("Invalid price") != std::string::npos);
}

TEST(OrderTests, NegativeQntyFails)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, -10.0, OrderSide::Ask);
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error().find("Invalid quantity") != std::string::npos);
}

TEST(OrderTests, FutureOrderCreationTimeFails)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, -10.0, OrderSide::Ask);
    ASSERT_FALSE(result.has_value());
}

}  // namespace solstice
