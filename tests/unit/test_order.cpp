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

TEST(OrderTests, OrderHasCorrectUid)
{
    auto result = Order::createOrder(42, Equity::MSFT, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()->uid(), 42);
}

TEST(OrderTests, OrderHasCorrectUnderlying)
{
    auto result = Order::createOrder(0, Equity::GOOGL, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value()->underlying() == Equity::GOOGL);
}

TEST(OrderTests, OrderHasCorrectPrice)
{
    auto result = Order::createOrder(0, Equity::AAPL, 123.45, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()->price(), 123.45);
}

TEST(OrderTests, OrderHasCorrectQuantity)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, 25.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()->qnty(), 25.0);
}

TEST(OrderTests, OrderHasCorrectSide)
{
    auto bidResult = Order::createOrder(0, Equity::AAPL, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(bidResult.has_value());
    EXPECT_EQ(bidResult.value()->orderSide(), OrderSide::Bid);

    auto askResult = Order::createOrder(0, Equity::AAPL, 100.0, 10.0, OrderSide::Ask);
    ASSERT_TRUE(askResult.has_value());
    EXPECT_EQ(askResult.value()->orderSide(), OrderSide::Ask);
}

TEST(OrderTests, OrderInitiallyNotComplete)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value()->orderComplete());
}

TEST(OrderTests, OrderOutstandingQntyInitiallyEqualsQnty)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, 15.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value()->outstandingQnty(), 15.0);
}

TEST(OrderTests, CanUpdateOutstandingQnty)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, 15.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    result.value()->outstandingQnty(5.0);
    EXPECT_EQ(result.value()->outstandingQnty(), 5.0);
}

TEST(OrderTests, CanMarkOrderAsComplete)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    result.value()->orderComplete(true);
    EXPECT_TRUE(result.value()->orderComplete());
}

TEST(OrderTests, TimeOrderFulfilledFailsWhenNotComplete)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
    auto timeResult = result.value()->timeOrderFulfilled();
    ASSERT_FALSE(timeResult.has_value());
}

TEST(OrderTests, OrderSideStringReturnsCorrectValue)
{
    auto bidResult = Order::createOrder(0, Equity::AAPL, 100.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(bidResult.has_value());
    EXPECT_EQ(bidResult.value()->orderSideString(), "Bid");

    auto askResult = Order::createOrder(0, Equity::AAPL, 100.0, 10.0, OrderSide::Ask);
    ASSERT_TRUE(askResult.has_value());
    EXPECT_EQ(askResult.value()->orderSideString(), "ask");
}

TEST(OrderTests, ZeroPriceIsValid)
{
    auto result = Order::createOrder(0, Equity::AAPL, 0.0, 10.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
}

TEST(OrderTests, ZeroQntyIsValid)
{
    auto result = Order::createOrder(0, Equity::AAPL, 100.0, 0.0, OrderSide::Bid);
    ASSERT_TRUE(result.has_value());
}

}  // namespace solstice
