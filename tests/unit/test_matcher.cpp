#include <gtest/gtest.h>
#include <matcher.h>
#include <orchestrator.h>

namespace solstice::matching
{
class OrderFixture : public ::testing::Test
{
   protected:
    OrderPtr order;
    std::shared_ptr<OrderBook> orderBook;
    Orchestrator orchestrator;

    void SetUp() override
    {
        auto order = Order::createOrder(0, Equity::AAPL, 10, 100, OrderSide::Bid);
        orchestrator->addOrderToBook(order);
    }
};

TEST_F(OrderFixture, ValidPriceWithinPriceRange)
{
    auto orderToMatch = Order::createOrder(0, Equity::AAPL, 10, 100, OrderSide::Ask);
    testing::Matcher.matchOrder(12, order);
}

TEST_F(OrderFixture, InvalidPriceOutsidePriceRange) {}
}  // namespace solstice::matching
