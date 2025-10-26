#include <config.h>
#include <gtest/gtest.h>
#include <matcher.h>
#include <orchestrator.h>
#include <order_book.h>

namespace solstice::matching
{

class OrchestratorFixture : public ::testing::Test
{
   protected:
    Config config = Config::instance().value();
    std::shared_ptr<OrderBook> orderBook;
    std::shared_ptr<Matcher> matcher;
    std::unique_ptr<Orchestrator> orchestrator;

    void SetUp() override
    {

        orderBook = std::make_shared<OrderBook>();
        matcher = std::make_shared<Matcher>(orderBook);

        std::vector<Equity> pool = {Equity::AAPL};
        d_underlyingsPool<Equity> = pool;
        d_underlyingsPoolInitialised<Equity> = true;
        orderBook->initialiseBookAtUnderlyings<Equity>();
    }

    void TearDown() override
    {
        d_underlyingsPool<Equity> = {};
        d_underlyingsPoolInitialised<Equity> = false;
    }
};

TEST(OrchestratorTests, StartSucceeds)
{
    auto result = Orchestrator::start();
    ASSERT_TRUE(result.has_value());
}

TEST_F(OrchestratorFixture, ProcessOrderWithMatchSucceeds)
{
    Orchestrator orch{config, orderBook, matcher};

    auto bidOrder = Order::createOrder(1, Equity::AAPL, 100.0, 10.0, MarketSide::Bid);
    ASSERT_TRUE(bidOrder.has_value());
    orch.processOrder(*bidOrder);

    auto askOrder = Order::createOrder(2, Equity::AAPL, 100.0, 10.0, MarketSide::Ask);
    ASSERT_TRUE(askOrder.has_value());

    bool result = orch.processOrder(*askOrder);
    EXPECT_TRUE(result);
}

TEST_F(OrchestratorFixture, ProcessOrderWithoutMatchFails)
{
    Orchestrator orch{config, orderBook, matcher};

    auto bidOrder = Order::createOrder(1, Equity::AAPL, 100.0, 10.0, MarketSide::Bid);
    ASSERT_TRUE(bidOrder.has_value());

    bool result = orch.processOrder(*bidOrder);
    EXPECT_FALSE(result);
}

TEST_F(OrchestratorFixture, ProcessOrderAddsToBook)
{
    Orchestrator orch{config, orderBook, matcher};

    auto bidOrder = Order::createOrder(1, Equity::AAPL, 100.0, 10.0, MarketSide::Bid);
    ASSERT_TRUE(bidOrder.has_value());

    orch.processOrder(*bidOrder);

    auto deque = orderBook->getOrdersDequeAtPrice(*bidOrder);
    ASSERT_TRUE(deque.has_value());
    EXPECT_EQ(deque->get().size(), 1);
}

TEST_F(OrchestratorFixture, ProcessOrderMarksFulfilledOnMatch)
{
    Orchestrator orch{config, orderBook, matcher};

    auto bidOrder = Order::createOrder(1, Equity::AAPL, 100.0, 10.0, MarketSide::Bid);
    ASSERT_TRUE(bidOrder.has_value());
    orch.processOrder(*bidOrder);

    auto askOrder = Order::createOrder(2, Equity::AAPL, 100.0, 10.0, MarketSide::Ask);
    ASSERT_TRUE(askOrder.has_value());

    orch.processOrder(*askOrder);

    EXPECT_TRUE((*askOrder)->orderComplete());
}

}  // namespace solstice::matching
