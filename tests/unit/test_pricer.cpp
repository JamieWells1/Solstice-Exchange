#include <gtest/gtest.h>
#include <order.h>
#include <order_book.h>
#include <pricer.h>

#include <iostream>
#include <memory>

using namespace solstice::pricing;
using namespace solstice;

class PricerTest : public ::testing::Test
{
   protected:
    std::shared_ptr<matching::OrderBook> orderBook;
    std::shared_ptr<Pricer> pricer;

    void SetUp() override
    {
        orderBook = std::make_shared<matching::OrderBook>();
        pricer = std::make_shared<Pricer>(orderBook);

        // Initialize the underlyings pool
        std::vector<Equity> pool = {Equity::AAPL};
        d_underlyingsPool<Equity> = pool;
        d_underlyingsPoolInitialised<Equity> = true;

        orderBook->initialiseBookAtUnderlyings<Equity>();
        pricer->initialisePricerEquities();
    }

    void TearDown() override
    {
        d_underlyingsPool<Equity> = {};
        d_underlyingsPoolInitialised<Equity> = false;
    }
};

TEST_F(PricerTest, InitialSpreadIsSet)
{
    auto& data = pricer->getPriceData(Equity::AAPL);

    std::cout << "Initial state:" << std::endl;
    std::cout << "  lastPrice: " << data.lastPrice() << std::endl;
    std::cout << "  highestBid: " << data.highestBid() << std::endl;
    std::cout << "  lowestAsk: " << data.lowestAsk() << std::endl;
    std::cout << "  demandFactor: " << data.demandFactor() << std::endl;

    EXPECT_GT(data.lastPrice(), 0);
    EXPECT_GE(data.demandFactor(), -1.0);
    EXPECT_LE(data.demandFactor(), 1.0);
}

TEST_F(PricerTest, FirstPriceCalculationInitializesSpread)
{
    auto& data = pricer->getPriceData(Equity::AAPL);

    double price = pricer->calculatePrice(Equity::AAPL, MarketSide::Bid);

    std::cout << "\nAfter first calculatePrice:" << std::endl;
    std::cout << "  price: " << price << std::endl;
    std::cout << "  highestBid: " << data.highestBid() << std::endl;
    std::cout << "  lowestAsk: " << data.lowestAsk() << std::endl;
    std::cout << "  spread: " << (data.lowestAsk() - data.highestBid()) << std::endl;

    EXPECT_GT(price, 0);
    EXPECT_GT(data.highestBid(), 0);
    EXPECT_GT(data.lowestAsk(), 0);
    EXPECT_GT(data.lowestAsk(), data.highestBid());
}

TEST_F(PricerTest, BidPricesAreReasonable)
{
    int pricesAtOne = 0;
    int pricesNegative = 0;
    double minPrice = 1000000;
    double maxPrice = 0;

    for (int i = 0; i < 100; i++)
    {
        double price = pricer->calculatePrice(Equity::AAPL, MarketSide::Bid);

        if (price == 1.0) pricesAtOne++;
        if (price < 0) pricesNegative++;

        minPrice = std::min(minPrice, price);
        maxPrice = std::max(maxPrice, price);
    }

    auto& data = pricer->getPriceData(Equity::AAPL);

    std::cout << "\nBid pricing stats (100 orders):" << std::endl;
    std::cout << "  Prices at 1.0: " << pricesAtOne << "%" << std::endl;
    std::cout << "  Negative prices: " << pricesNegative << std::endl;
    std::cout << "  Min price: " << minPrice << std::endl;
    std::cout << "  Max price: " << maxPrice << std::endl;
    std::cout << "  Current spread: " << data.highestBid() << " - " << data.lowestAsk()
              << std::endl;

    EXPECT_EQ(pricesNegative, 0) << "No prices should be negative";
    EXPECT_LT(pricesAtOne, 50) << "Less than 50% should be at minimum price";
    EXPECT_GT(minPrice, 0) << "All prices should be positive";
}

TEST_F(PricerTest, AskPricesAreReasonable)
{
    int pricesAtOne = 0;
    int pricesNegative = 0;
    double minPrice = 1000000;
    double maxPrice = 0;

    for (int i = 0; i < 100; i++)
    {
        double price = pricer->calculatePrice(Equity::AAPL, MarketSide::Ask);

        if (price == 1.0) pricesAtOne++;
        if (price < 0) pricesNegative++;

        minPrice = std::min(minPrice, price);
        maxPrice = std::max(maxPrice, price);
    }

    auto& data = pricer->getPriceData(Equity::AAPL);

    std::cout << "\nAsk pricing stats (100 orders):" << std::endl;
    std::cout << "  Prices at 1.0: " << pricesAtOne << "%" << std::endl;
    std::cout << "  Negative prices: " << pricesNegative << std::endl;
    std::cout << "  Min price: " << minPrice << std::endl;
    std::cout << "  Max price: " << maxPrice << std::endl;
    std::cout << "  Current spread: " << data.highestBid() << " - " << data.lowestAsk()
              << std::endl;

    EXPECT_EQ(pricesNegative, 0) << "No prices should be negative";
    EXPECT_LT(pricesAtOne, 50) << "Less than 50% should be at minimum price";
    EXPECT_GT(minPrice, 0) << "All prices should be positive";
}

TEST_F(PricerTest, SpreadRemainsValid)
{
    auto& data = pricer->getPriceData(Equity::AAPL);

    for (int i = 0; i < 50; i++)
    {
        pricer->calculatePrice(Equity::AAPL, MarketSide::Bid);
        pricer->calculatePrice(Equity::AAPL, MarketSide::Ask);
    }

    double spread = data.lowestAsk() - data.highestBid();

    std::cout << "\nAfter 100 orders:" << std::endl;
    std::cout << "  highestBid: " << data.highestBid() << std::endl;
    std::cout << "  lowestAsk: " << data.lowestAsk() << std::endl;
    std::cout << "  spread: " << spread << std::endl;

    EXPECT_GT(data.highestBid(), 0);
    EXPECT_GT(data.lowestAsk(), 0);
    EXPECT_GE(spread, 0) << "Spread should never be negative";
}

TEST_F(PricerTest, QuantityCalculationIsValid)
{
    auto& data = pricer->getPriceData(Equity::AAPL);

    for (int i = 0; i < 10; i++)
    {
        pricer->calculatePrice(Equity::AAPL, MarketSide::Bid);
    }

    double price = pricer->calculatePrice(Equity::AAPL, MarketSide::Bid);
    double quantity = pricer->calculateQnty(Equity::AAPL, MarketSide::Bid, price);

    std::cout << "\nQuantity calculation:" << std::endl;
    std::cout << "  price: " << price << std::endl;
    std::cout << "  quantity: " << quantity << std::endl;
    std::cout << "  demandFactor: " << data.demandFactor() << std::endl;

    EXPECT_GT(quantity, 0) << "Quantity should be positive";
    EXPECT_LT(quantity, 1000000) << "Quantity should be reasonable";
}

TEST_F(PricerTest, PriceImplWithKnownValues)
{
    double lowestAsk = 100.0;
    double highestBid = 99.0;
    double demandFactor = 0.5;

    std::cout << "\nDirect calculatePriceImpl test:" << std::endl;
    std::cout << "  Input: bid=" << highestBid << ", ask=" << lowestAsk
              << ", demand=" << demandFactor << std::endl;

    for (int i = 0; i < 10; i++)
    {
        double price =
            pricer->calculatePriceImpl(MarketSide::Bid, lowestAsk, highestBid, demandFactor);
        std::cout << "  Bid price " << i << ": " << price << std::endl;

        EXPECT_GT(price, 0) << "Price should be positive";
        EXPECT_NE(price, 1.0) << "Price shouldn't be clamped to 1.0 with valid spread";
    }

    for (int i = 0; i < 10; i++)
    {
        double price =
            pricer->calculatePriceImpl(MarketSide::Ask, lowestAsk, highestBid, demandFactor);
        std::cout << "  Ask price " << i << ": " << price << std::endl;

        EXPECT_GT(price, 0) << "Price should be positive";
        EXPECT_NE(price, 1.0) << "Price shouldn't be clamped to 1.0 with valid spread";
    }
}

TEST_F(PricerTest, QuantityDistribution)
{
    auto& data = pricer->getPriceData(Equity::AAPL);

    // Test with different scenarios
    std::cout << "\n=== Quantity Distribution Analysis ===" << std::endl;

    // Scenario 1: Initial state (no executions, sigma = 0)
    std::cout << "\nScenario 1 - Initial (executions=0, sigma=0):" << std::endl;
    std::cout << "  demandFactor: " << data.demandFactor() << std::endl;
    std::cout << "  executions: " << data.executions() << std::endl;

    int qtyAtOne = 0;
    int minQty = 1000000;
    int maxQty = 0;
    int sumQty = 0;

    for (int i = 0; i < 100; i++)
    {
        double price = 50.0;  // typical price
        int qty = pricer->calculateQnty(Equity::AAPL, MarketSide::Bid, price);

        if (qty == 1.0) qtyAtOne++;
        minQty = std::min(minQty, qty);
        maxQty = std::max(maxQty, qty);
        sumQty += qty;
    }

    std::cout << "  Quantities at 1.0: " << qtyAtOne << "%" << std::endl;
    std::cout << "  Min quantity: " << minQty << std::endl;
    std::cout << "  Max quantity: " << maxQty << std::endl;
    std::cout << "  Avg quantity: " << (sumQty / 100) << std::endl;

    // Scenario 2: After simulating some executions
    std::cout << "\nScenario 2 - After simulating executions:" << std::endl;

    // Manually set up some execution history
    data.incrementExecutions();
    data.incrementExecutions();
    data.pricesSum(100.0);
    data.pricesSumSquared(5050.0);  // sqrt(variance) ≈ small sigma

    std::cout << "  demandFactor: " << data.demandFactor() << std::endl;
    std::cout << "  executions: " << data.executions() << std::endl;

    qtyAtOne = 0;
    minQty = 1000000;
    maxQty = 0;
    sumQty = 0;

    for (int i = 0; i < 100; i++)
    {
        double price = 50.0;
        int qty = pricer->calculateQnty(Equity::AAPL, MarketSide::Bid, price);

        if (qty == 1.0) qtyAtOne++;
        minQty = std::min(minQty, qty);
        maxQty = std::max(maxQty, qty);
        sumQty += qty;
    }

    std::cout << "  Quantities at 1.0: " << qtyAtOne << "%" << std::endl;
    std::cout << "  Min quantity: " << minQty << std::endl;
    std::cout << "  Max quantity: " << maxQty << std::endl;
    std::cout << "  Avg quantity: " << (sumQty / 100) << std::endl;

    // Scenario 3: Different price points
    std::cout << "\nScenario 3 - Different price points (demandFactor=" << data.demandFactor() << "):" << std::endl;

    for (double price : {10.0, 50.0, 100.0, 200.0})
    {
        double qty = pricer->calculateQnty(Equity::AAPL, MarketSide::Bid, price);
        double maxQtyFormula = 10000.0 * std::abs(data.demandFactor()) / price;
        std::cout << "  Price " << price << ": qty=" << qty
                  << ", maxQty=" << maxQtyFormula << std::endl;
    }

    // Scenario 4: Very low demandFactor
    std::cout << "\nScenario 4 - Low demandFactor:" << std::endl;
    data.demandFactor(0.05);  // Very low
    std::cout << "  demandFactor: " << data.demandFactor() << std::endl;

    qtyAtOne = 0;
    for (int i = 0; i < 100; i++)
    {
        double price = 50.0;
        double qty = pricer->calculateQnty(Equity::AAPL, MarketSide::Bid, price);
        if (qty == 1.0) qtyAtOne++;
    }
    std::cout << "  Quantities at 1.0: " << qtyAtOne << "%" << std::endl;

    double maxQtyLowDF = 10000.0 * 0.05 / 50.0;
    std::cout << "  Expected maxQty: " << maxQtyLowDF << std::endl;
}

TEST_F(PricerTest, PricesFluctuateOverTime)
{
    auto& data = pricer->getPriceData(Equity::AAPL);

    std::cout << "\n=== Price Fluctuation Test ===" << std::endl;
    std::cout << "Initial state:" << std::endl;
    std::cout << "  lastPrice: " << data.lastPrice() << std::endl;
    std::cout << "  demandFactor: " << data.demandFactor() << std::endl;

    // Generate 50 prices and track unique values
    std::set<double> uniqueBidPrices;
    std::set<double> uniqueAskPrices;

    for (int i = 0; i < 50; i++)
    {
        double bidPrice = pricer->calculatePrice(Equity::AAPL, MarketSide::Bid);
        double askPrice = pricer->calculatePrice(Equity::AAPL, MarketSide::Ask);

        uniqueBidPrices.insert(bidPrice);
        uniqueAskPrices.insert(askPrice);

        if (i < 10)
        {
            std::cout << "Order " << i << ": Bid=$" << bidPrice << ", Ask=$" << askPrice
                      << ", Spread: " << (data.lowestAsk() - data.highestBid()) << std::endl;
        }
    }

    std::cout << "\nAfter 50 orders:" << std::endl;
    std::cout << "  Unique bid prices: " << uniqueBidPrices.size() << std::endl;
    std::cout << "  Unique ask prices: " << uniqueAskPrices.size() << std::endl;
    std::cout << "  highestBid: " << data.highestBid() << std::endl;
    std::cout << "  lowestAsk: " << data.lowestAsk() << std::endl;
    std::cout << "  Spread: " << (data.lowestAsk() - data.highestBid()) << std::endl;

    // Prices should vary, not be stuck at 2 values
    EXPECT_GT(uniqueBidPrices.size(), 5) << "Bid prices should have more variety";
    EXPECT_GT(uniqueAskPrices.size(), 5) << "Ask prices should have more variety";
}
