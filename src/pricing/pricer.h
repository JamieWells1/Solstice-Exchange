#ifndef PRICER_H
#define PRICER_H

#include <asset_class.h>
#include <get_random.h>
#include <market_side.h>
#include <order_book.h>

#include <memory>
#include <unordered_map>

namespace solstice::pricing
{

template <typename T>
void setInitialDemandFactor(T& underlying)
{
    underlying.demandFactor(Random::getRandomDouble(-1, 1));
}

struct EquityPriceData
{
   public:
    double lastPrice();
    double highestBid();
    double lowestAsk();
    double demandFactor();
    double movingAverage();

    void lastPrice(int newLastPrice);
    void highestBid(int newHighestBid);
    void lowestAsk(int newLowestAsk);
    void demandFactor(int newDemandFactor);
    void movingAverage(double newMovingAverage);

   private:
    static constexpr int movingAverageRange = 10;

    double d_lastPrice;
    double d_highestBid;
    double d_lowestAsk;
    double d_demandFactor;
    double d_movingAverage;
};

struct FuturePriceData
{
   public:
    double lastPrice();
    double highestBid();
    double lowestAsk();
    double demandFactor();
    double movingAverage();

    void lastPrice(int newLastPrice);
    void highestBid(int newHighestBid);
    void lowestAsk(int newLowestAsk);
    void demandFactor(int newDemandFactor);
    void movingAverage(double newMovingAverage);

   private:
    static constexpr int movingAverageRange = 10;

    double d_lastPrice;
    double d_highestBid;
    double d_lowestAsk;
    double d_demandFactor;
    double d_movingAverage;
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

class Pricer
{
   public:
    Pricer(std::shared_ptr<matching::OrderBook> orderBook);

    template <typename T>
    void initialisePricerEquities()
    {
        for (const auto& underlying : underlyingsPool<Equity>())
        {
            d_equityDataMap[underlying];
            setInitialDemandFactor(d_equityDataMap[underlying]);
        }
    }

    template <typename T>
    void initialisePricerFutures()
    {
        for (const auto& underlying : underlyingsPool<Future>())
        {
            d_futureDataMap[underlying];
            setInitialDemandFactor(d_futureDataMap[underlying]);
        }
    }

    void update(matching::OrderPtr order, bool orderMatched);

    template <typename T>
    PricerDepOrderData compute(T underlying);

   private:
    double generateSeedPrice();

    EquityPriceData& getPriceData(Equity eq);
    FuturePriceData& getPriceData(Future fut);

    MarketSide calculateMarketSide(Equity underlying);
    MarketSide calculateMarketSide(Future underlying);

    MarketSide calculateMarketSideImpl(double probability);

    double calculatePrice(Equity underlying);
    double calculatePrice(Future underlying);

    double calculateQnty(Equity underlying);
    double calculateQnty(Future underlying);

    std::unordered_map<Equity, EquityPriceData> d_equityDataMap;
    std::unordered_map<Future, FuturePriceData> d_futureDataMap;

    double d_seedPrice;

    std::shared_ptr<matching::OrderBook> d_orderBook;
};
}  // namespace solstice::pricing

#endif  // PRICER_H
