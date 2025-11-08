#ifndef PRICER_H
#define PRICER_H

#include <asset_class.h>
#include <get_random.h>
#include <market_side.h>
#include <order_book.h>
#include <order_type.h>
#include <time_point.h>

#include <memory>
#include <unordered_map>
#include <variant>

namespace solstice::pricing
{

template <typename PriceData>
void setInitialDemandFactor(PriceData& underlying)  // type
{
    underlying.demandFactor(Random::getRandomDouble(-1, 1));
}

template <typename PriceData>
void setInitialPrice(PriceData& underlying)
{
    underlying.lastPrice(Random::getRandomDouble(10, 200));
}

template <typename PriceData>
void setInitialMovingAverage(PriceData& underlying)
{
    underlying.movingAverage(underlying.lastPrice());
}

struct EquityPriceData
{
   public:
    EquityPriceData(Equity underlying) : d_equity(underlying)
    {
        setInitialDemandFactor(*this);
        setInitialPrice(*this);
        setInitialMovingAverage(*this);
    }

    Equity underlying();
    int maRange();

    double lastPrice();
    double highestBid();
    double lowestAsk();
    double demandFactor();
    double movingAverage();
    int executions();
    double pricesSum();
    double pricesSumSquared();

    void underlying(Equity eq);

    void lastPrice(double newLastPrice);
    void highestBid(double newHighestBid);
    void lowestAsk(double newLowestAsk);
    void demandFactor(double newDemandFactor);
    void movingAverage(double newMovingAverage);
    void incrementExecutions();
    void pricesSum(double newPricesSum);
    void pricesSumSquared(double newPricesSumSquared);

   private:
    static constexpr int d_maRange = 10;

    Equity d_equity;

    double d_lastPrice = 0.0;
    double d_highestBid = 0.0;
    double d_lowestAsk = 0.0;
    double d_demandFactor = 0.0;
    double d_movingAverage = 0.0;

    // used for pricing calculations
    int d_executions = 0;
    double d_pricesSum = 0.0;
    double d_pricesSumSquared = 0.0;
};

struct FuturePriceData
{
   public:
    FuturePriceData(Future underlying) : d_future(underlying)
    {
        setInitialDemandFactor(*this);
        setInitialPrice(*this);
        setInitialMovingAverage(*this);
    }

    Future underlying();
    int maRange();

    double lastPrice();
    double highestBid();
    double lowestAsk();
    double demandFactor();
    double movingAverage();
    int executions();
    double pricesSum();
    double pricesSumSquared();

    void underlying(Future fut);

    void lastPrice(double newLastPrice);
    void highestBid(double newHighestBid);
    void lowestAsk(double newLowestAsk);
    void demandFactor(double newDemandFactor);
    void movingAverage(double newMovingAverage);
    void incrementExecutions();
    void pricesSum(double newPricesSum);
    void pricesSumSquared(double newPricesSumSquared);

   private:
    static constexpr int d_maRange = 10;

    Future d_future;

    double d_lastPrice = 0.0;
    double d_highestBid = 0.0;
    double d_lowestAsk = 0.0;
    double d_demandFactor = 0.0;
    double d_movingAverage = 0.0;

    // used for pricing calculations
    int d_executions = 0;
    double d_pricesSum = 0.0;
    double d_pricesSumSquared = 0.0;
};

struct PricerDepOrderData
{
   public:
    PricerDepOrderData(MarketSide d_marketSide, double d_price, double d_qnty);

    MarketSide marketSide();
    double price();
    double qnty();

   private:
    MarketSide d_marketSide;
    double d_price;
    double d_qnty;
};

double standardDeviation(EquityPriceData& data);
double standardDeviation(FuturePriceData& data);

class Pricer
{
   public:
    Pricer(std::shared_ptr<matching::OrderBook> orderBook);

    void initialisePricerEquities();
    void initialisePricerFutures();

    void update(matching::OrderPtr order);

    template <typename T>
    double updatedDemandFactor(T& priceData)
    {
        double currentDF = priceData.demandFactor();
        double newPrice = priceData.lastPrice();
        double ma = priceData.movingAverage();

        if (priceData.executions() < 2)
        {
            return Random::getRandomDouble(-0.3, 0.3);
        }

        double sigma = standardDeviation(priceData);
        double noise = Random::getRandomDouble(-0.05, 0.05);
        currentDF += noise;

        double priceDeviation = newPrice - ma;
        // price too high
        if (priceDeviation > 1.5 * sigma)
        {
            currentDF -= 0.15;
        }
        // price too low
        else if (priceDeviation < -1.5 * sigma)
        {
            currentDF += 0.15;
        }

        // Mean reversion toward 0 instead of decay
        // Gently pull toward 0, but don't force it there
        currentDF = currentDF * 0.95 + 0.0 * 0.05;
        currentDF = std::max(-1.0, std::min(1.0, currentDF));

        return currentDF;
    }

    template <typename T>
    PricerDepOrderData compute(T& underlying)
    {
        return std::visit(
            [this](auto&& underlying)
            {
                auto marketSide = calculateMarketSide(underlying);
                auto price = calculatePrice(underlying, marketSide);
                auto qnty = calculateQnty(underlying, marketSide, price);

                return PricerDepOrderData(marketSide, price, qnty);
            },
            underlying);
    }

   public:
    EquityPriceData& getPriceData(Equity eq);
    FuturePriceData& getPriceData(Future fut);

    // propogate results from market side calc
    double calculatePrice(Equity eq, MarketSide mktSide);
    double calculatePrice(Future fut, MarketSide mktSide);

    double calculatePriceImpl(MarketSide mktSide, double lowestAsk, double highestBid,
                              double demandFactor);

    // propogate results from market side calc and price calc
    double calculateQnty(Equity eq, MarketSide mktSide, double price);
    double calculateQnty(Future fut, MarketSide mktSide, double price);

   private:
    double generateSeedPrice();

    template <typename Func>
    auto withPriceData(Underlying underlying, Func&& func)
    {
        return std::visit([this, &func](auto asset) { return func(getPriceData(asset)); },
                          underlying);
    }

    template <typename Func>
    auto withPriceData(matching::OrderPtr order, Func&& func)
    {
        return std::visit([this, &func](auto asset) { return func(getPriceData(asset)); },
                          order->underlying());  // Call underlying() to get the variant
    }

    MarketSide calculateMarketSide(Equity eq);
    MarketSide calculateMarketSide(Future fut);

    MarketSide calculateMarketSideImpl(double probability);

    OrderType getOrderType();

    double calculateCarryAdjustment(Future fut);

    std::unordered_map<Equity, EquityPriceData> d_equityDataMap;
    std::unordered_map<Future, FuturePriceData> d_futureDataMap;

    double d_seedPrice;

    std::shared_ptr<matching::OrderBook> d_orderBook;
};
}  // namespace solstice::pricing

#endif  // PRICER_H
